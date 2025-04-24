////////////////////////////////////////////////////////////////////////////////
// File: infection_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_INFECTIONINTERNALS_HPP_
#define HEPCE_EVENT_HCV_INFECTIONINTERNALS_HPP_

#include <hepce/event/hcv/infection.hpp>

#include "internals/event_internals.hpp"
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>
namespace hepce {
namespace event {
namespace hcv {
class InfectionImpl : public virtual Infection, public EventBase {
public:
    using incidencemap_t =
        std::unordered_map<utils::tuple_3i, double, utils::key_hash_3i,
                           utils::key_equal_3i>;

    InfectionImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name = "console");

    ~InfectionImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    incidencemap_t _infection_data;
    double _gt3_prob = 0;

    static int CallbackInfection(void *storage, int count, char **data,
                                 char **columns) {
        utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((incidencemap_t *)storage))[tup] = utils::SToDPositive(data[3]);
        return 0;
    }

    std::string IncidenceSQL() {
        return "SELECT age_years, gender, drug_behavior, incidence FROM "
               "incidence;";
    }

    std::vector<double> GetInfectionProbability(
        model::Person &person,
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

    int LoadIncidenceData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string error;
        int rc = dm->SelectCustomCallback(IncidenceSQL(), CallbackInfection,
                                          &_infection_data, error);
        if (rc != 0) {
            // Log Error
        }
        if (_infection_data.empty()) {
            // Warn Empty
        }
        return rc;
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif