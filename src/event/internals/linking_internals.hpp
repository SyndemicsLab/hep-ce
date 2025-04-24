////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
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
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
class LinkingBase : public EventBase {
public:
    // typing
    // First is background, second is intervention
    using linkmap_t =
        std::unordered_map<utils::tuple_4i, std::pair<double, double>,
                           utils::key_hash_4i, utils::key_equal_4i>;

    virtual ~LinkingBase() = default;

    data::LinkageType GetLinkageType() const { return _linkage_type; }
    bool GetLinkingStratifiedByPregnancy() const {
        return _stratify_by_pregnancy;
    }
    double GetInterventionCost() const { return _intervention_cost; }
    double GetFalsePositiveCost() const { return _false_positive_cost; }

    virtual data::InfectionType GetInfectionType() const = 0;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override {
        SetLinkageType(person.GetLinkageType(GetInfectionType()));
        bool is_linked = (person.GetLinkState(GetInfectionType()) ==
                          data::LinkageState::LINKED);
        bool is_not_identified =
            (!person.IsIdentifiedAsInfected(GetInfectionType()));
        if (is_linked || is_not_identified) {
            return;
        }

        if (FalsePositive(person)) {
            return;
        }

        double prob = GetLinkProbability(person);
        // check if the person was recently screened, for multiplier
        bool recently_screened =
            (person.GetTimeSinceLastScreening(GetInfectionType()) <=
             _recent_screen_cutoff);
        // apply the multiplier to recently screened persons
        if (recently_screened && (prob < 1)) {
            prob = ApplyMultiplier(prob, _recent_screen_multiplier);
        }

        // draw from link probability
        if (sampler.GetDecision({prob}) == 0) {
            data::LinkageType lt = person.GetLinkageType(GetInfectionType());
            person.Link(lt, GetInfectionType());
            if (lt == data::LinkageType::INTERVENTION) {
                SetCost(GetInterventionCost());
                AddEventCost(person);
            }
        }
    }

protected:
    static int CallbackLink(void *storage, int count, char **data,
                            char **columns) {
        utils::tuple_4i tup =
            std::make_tuple(std::stoi(data[0]), std::stoi(data[1]),
                            std::stoi(data[2]), std::stoi(data[3]));
        (*((linkmap_t *)storage))[tup] = {utils::SToDPositive(data[4]),
                                          utils::SToDPositive(data[5])};
        return 0;
    }

    void SetLinkageType(const data::LinkageType &type) { _linkage_type = type; }
    void SetLinkingStratifiedByPregnancy(bool stratify) {
        _stratify_by_pregnancy = stratify;
    }
    void SetInterventionCost(double cost) { _intervention_cost = cost; }
    void SetFalsePositiveCost(double cost) { _false_positive_cost = cost; }

    void SetRecentScreenCutoff(int cutoff) { _recent_screen_cutoff = cutoff; }
    void SetRecentScreenMultiplier(double mult) {
        _recent_screen_multiplier = mult;
    }

    void SetLinkData(const linkmap_t &link_data) { _link_data = link_data; }

    linkmap_t &GetLinkData() { return _link_data; }

    virtual bool FalsePositive(model::Person &person) = 0;
    virtual void
    LoadLinkingData(std::shared_ptr<datamanagement::DataManagerBase> dm) = 0;

    // FYI This is just a straight up SQL Injection Vulnerability Waiting to Happen
    inline const std::string LinkSQL(const std::string &table) const {
        std::stringstream ss;
        ss << "SELECT age_years, gender, drug_behavior, "
           << ((GetLinkingStratifiedByPregnancy()) ? "pregnancy, " : "-1, ")
           << "background_link_probability, intervention_link_probability "
           << "FROM " << table << ";";
        return ss.str();
    }

    double GetLinkProbability(model::Person &person) {
        int age_years = static_cast<int>((person.GetAge() / 12.0));
        int gender = static_cast<int>(person.GetSex());
        int drug_behavior = static_cast<int>(person.GetBehavior());
        int pregnancy = static_cast<int>(person.GetPregnancyState());
        utils::tuple_4i tup =
            std::make_tuple(age_years, gender, drug_behavior, pregnancy);
        if (GetLinkageType() == data::LinkageType::BACKGROUND) {
            return _link_data[tup].first;
        } else if (GetLinkageType() == data::LinkageType::INTERVENTION) {
            return _link_data[tup].second;
        }
        return 0.0;
    }
    void AddFalsePositiveCost(model::Person &person,
                              const model::CostCategory &category) {
        double discounted_cost =
            utils::Discount(GetFalsePositiveCost(), GetDiscount(),
                            person.GetCurrentTimestep(), false);
        person.AddCost(GetFalsePositiveCost(), discounted_cost, category);
    }
    double ApplyMultiplier(double prob, double mult) {
        return utils::RateToProbability(utils::ProbabilityToRate(prob) * mult);
    }
    bool CheckForPregnancyEvent(
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::vector<std::string> event_list = utils::SplitToVecT<std::string>(
            utils::GetStringFromConfig("simulation.events", dm), ',');

        return (std::find(event_list.begin(), event_list.end(), "pregnancy") !=
                event_list.end());
    }

private:
    // properties
    data::LinkageType _linkage_type = data::LinkageType::BACKGROUND;
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