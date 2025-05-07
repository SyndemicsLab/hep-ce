////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_LINKINGINTERNALS_HPP_

#include "event_internals.hpp"

#include <tuple>

#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
class LinkingBase : public EventBase {
public:
    using linkmap_t =
        std::unordered_map<utils::tuple_4i, std::pair<double, double>,
                           utils::key_hash_4i, utils::key_equal_4i>;

    LinkingBase(datamanagement::ModelData &model_data,
                const std::string &log_name)
        : EventBase(model_data, log_name) {}

    virtual ~LinkingBase() = default;

    virtual data::InfectionType GetInfectionType() const = 0;

    void Execute(model::Person &person, model::Sampler &sampler) override {
        bool is_linked =
            (person.GetLinkageDetails(GetInfectionType()).link_state ==
             data::LinkageState::kLinked);
        bool is_not_identified =
            (!person.GetScreeningDetails(GetInfectionType()).identified);
        if (is_linked || is_not_identified) {
            return;
        }

        if (FalsePositive(person)) {
            return;
        }

        double prob = GetLinkProbability(person);
        // check if the person was recently screened, for multiplier
        bool recently_screened =
            (GetTimeSince(person, person.GetScreeningDetails(GetInfectionType())
                                      .time_of_last_screening) <=
             _recent_screen_cutoff);
        // apply the multiplier to recently screened persons
        if (recently_screened && (prob < 1)) {
            prob = ApplyMultiplier(prob, _recent_screen_multiplier);
        }

        // draw from link probability
        if (sampler.GetDecision({prob}) == 0) {
            data::LinkageType lt =
                person.GetLinkageDetails(GetInfectionType()).link_type;
            person.Link(lt, GetInfectionType());
            if (lt == data::LinkageType::kIntervention) {
                AddEventCost(person, GetInterventionCost());
            }
        }
    }

protected:
    inline bool GetLinkingStratifiedByPregnancy() const {
        return _stratify_by_pregnancy;
    }
    inline double GetInterventionCost() const { return _intervention_cost; }
    inline double GetFalsePositiveCost() const { return _false_positive_cost; }

    static void CallbackLink(std::any &storage, const SQLite::Statement &stmt) {
        linkmap_t *temp = std::any_cast<linkmap_t>(&storage);
        utils::tuple_4i tup = std::make_tuple(
            stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt(),
            stmt.getColumn(2).getInt(), stmt.getColumn(3).getInt());
        (*temp)[tup] = {stmt.getColumn(4).getDouble(),
                        stmt.getColumn(5).getDouble()};
    }

    inline void LoadLinkingData(datamanagement::ModelData &model_data) {
        std::any storage = linkmap_t{};
        try {
            model_data.GetDBSource("inputs").Select(LinkSQL(), CallbackLink,
                                                    storage);
        } catch (std::exception &e) {
            std::stringstream msg;
            msg << "Error getting " << GetInfectionType()
                << " Linking Data: " << e.what();
            hepce::utils::LogError(GetLogName(), msg.str());
            return;
        }
        SetLinkData(std::any_cast<linkmap_t>(storage));
        if (GetLinkData().empty()) {
            std::stringstream s;
            s << GetInfectionType() << " Linking Data is Empty...";
            hepce::utils::LogWarning(GetLogName(), s.str());
        }
    }

    inline void SetLinkingStratifiedByPregnancy(bool stratify) {
        _stratify_by_pregnancy = stratify;
    }
    inline void SetInterventionCost(double cost) { _intervention_cost = cost; }
    inline void SetFalsePositiveCost(double cost) {
        _false_positive_cost = cost;
    }

    inline void SetRecentScreenCutoff(int cutoff) {
        _recent_screen_cutoff = cutoff;
    }
    inline void SetRecentScreenMultiplier(double mult) {
        _recent_screen_multiplier = mult;
    }

    inline void SetLinkData(const linkmap_t &link_data) {
        _link_data = link_data;
    }

    inline linkmap_t GetLinkData() { return _link_data; }

    virtual bool FalsePositive(model::Person &person) = 0;

    virtual const std::string TableName() const = 0;

    inline const std::string LinkSQL() const {
        std::stringstream ss;
        ss << "SELECT age_years, gender, drug_behavior, "
           << ((GetLinkingStratifiedByPregnancy()) ? "pregnancy, " : "-1, ")
           << "background_link_probability, intervention_link_probability "
           << "FROM " << TableName() << ";";
        return ss.str();
    }

    inline double GetLinkProbability(model::Person &person) {
        int age_years = static_cast<int>((person.GetAge() / 12.0));
        int gender = static_cast<int>(person.GetSex());
        int drug_behavior =
            static_cast<int>(person.GetBehaviorDetails().behavior);
        int pregnancy =
            static_cast<int>(person.GetPregnancyDetails().pregnancy_state);
        utils::tuple_4i tup =
            std::make_tuple(age_years, gender, drug_behavior, pregnancy);
        auto t = person.GetLinkageDetails(GetInfectionType()).link_type;
        if (t == data::LinkageType::kBackground) {
            return _link_data[tup].first;
        } else if (t == data::LinkageType::kIntervention) {
            return _link_data[tup].second;
        }
        return 0.0;
    }
    inline void AddFalsePositiveCost(model::Person &person,
                                     const model::CostCategory &category) {
        double discounted_cost =
            utils::Discount(GetFalsePositiveCost(), GetEventDiscount(),
                            person.GetCurrentTimestep(), false);
        person.AddCost(GetFalsePositiveCost(), discounted_cost, category);
    }
    inline double ApplyMultiplier(double prob, double mult) {
        return utils::RateToProbability(utils::ProbabilityToRate(prob) * mult);
    }
    inline bool CheckForPregnancyEvent(datamanagement::ModelData &model_data) {
        std::vector<std::string> event_list = utils::SplitToVecT<std::string>(
            utils::GetStringFromConfig("simulation.events", model_data), ',');

        return (std::find(event_list.begin(), event_list.end(), "pregnancy") !=
                event_list.end());
    }

private:
    // properties
    linkmap_t _link_data;
    int _recent_screen_cutoff = -1;
    double _recent_screen_multiplier = 1.0;
    bool _stratify_by_pregnancy = false;
    double _intervention_cost = 0.0;
    double _false_positive_cost = 0.0;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_LINKINGINTERNALS_HPP_