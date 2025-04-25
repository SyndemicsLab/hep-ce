////////////////////////////////////////////////////////////////////////////////
// File: infection_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_
#define HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_

#include <hepce/event/hiv/infection.hpp>

#include "internals/event_internals.hpp"
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
namespace hiv {
class InfectionImpl : public virtual hiv::Infection, public EventBase {
public:
    using hivincidencemap_t =
        std::unordered_map<utils::tuple_3i, double, utils::key_hash_3i,
                           utils::key_equal_3i>;
    InfectionImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name = "console");

    ~InfectionImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    hivincidencemap_t _infection_data;
    static int Callback(void *storage, int count, char **data, char **columns) {
        utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((hivincidencemap_t *)storage))[tup] = utils::SToDPositive(data[3]);
        return 0;
    }

    inline const std::string HIVIncidenceSQL() const {
        return "SELECT age_years, gender, drug_behavior, incidence FROM "
               "hiv_incidence;";
    }

    std::vector<double> GetInfectionProbability(
        const model::Person &person,
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        if (_infection_data.empty()) {
            // Warn Empty
            return {0.0};
        }

        int age_years = static_cast<int>(person.GetAge() / 12.0);
        int gender = static_cast<int>(person.GetSex());
        int drug_behavior = static_cast<int>(person.GetBehavior());
        utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
        double incidence = _infection_data[tup];

        return {incidence};
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_