////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
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
    virtual ~LinkingBase() = default;

    // typing
    // First is background, second is intervention
    using linkmap_t =
        std::unordered_map<utils::tuple_4i, std::pair<double, double>,
                           utils::key_hash_4i, utils::key_equal_4i>;

    void SetLinkageType(const data::LinkageType &type) {
        this->linkage_type = type;
    }
    void SetLinkingStratifiedByPregnancy(bool stratify) {
        this->stratify_by_pregnancy = stratify;
    }
    void SetInterventionCost(double cost) { this->intervention_cost = cost; }
    void SetFalsePositiveCost(double cost) { this->false_positive_cost = cost; }

    data::LinkageType GetLinkageType() const { return linkage_type; }
    bool GetLinkingStratifiedByPregnancy() const {
        return stratify_by_pregnancy;
    }
    double GetInterventionCost() const { return intervention_cost; }
    double GetFalsePositiveCost() const { return false_positive_cost; }

protected:
    linkmap_t link_data;

    virtual bool FalsePositive(model::Person &person) = 0;
    virtual void
    LoadLinkingData(std::shared_ptr<datamanagement::DataManagerBase> dm) = 0;

    static int callback_link(void *storage, int count, char **data,
                             char **columns) {
        utils::tuple_4i tup =
            std::make_tuple(std::stoi(data[0]), std::stoi(data[1]),
                            std::stoi(data[2]), std::stoi(data[3]));
        (*((linkmap_t *)storage))[tup] = {utils::SToDPositive(data[4]),
                                          utils::SToDPositive(data[5])};
        return 0;
    }

    // FYI This is just a straight up SQL Injection Vulnerability Waiting to Happen
    const std::string LinkSQL(const std::string &table) const {
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
            return this->link_data[tup].first;
        } else if (GetLinkageType() == data::LinkageType::INTERVENTION) {
            return this->link_data[tup].second;
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
    data::LinkageType linkage_type = data::LinkageType::BACKGROUND;
    bool stratify_by_pregnancy = false;
    double intervention_cost = 0.0;
    double false_positive_cost = 0.0;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_LINKINGINTERNALS_HPP_