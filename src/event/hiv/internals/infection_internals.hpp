////////////////////////////////////////////////////////////////////////////////
// File: infection_internals.hpp                                              //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_
#define HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_

// Header File
#include <hepce/event/hiv/infection.hpp>

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
class InfectionImpl : public virtual Infection, public EventBase {
public:
    using hivincidencemap_t =
        std::unordered_map<utils::tuple_3i, double, utils::key_hash_3i,
                           utils::key_equal_3i>;
    InfectionImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");

    ~InfectionImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    hivincidencemap_t _infection_data;
    static void Callback(std::any &storage, const SQLite::Statement &stmt) {
        utils::tuple_3i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt(),
                                              stmt.getColumn(2).getInt());
        std::any_cast<hivincidencemap_t>(storage)[tup] =
            stmt.getColumn(3).getDouble();
    }

    inline const std::string HIVIncidenceSQL() const {
        return "SELECT age_years, gender, drug_behavior, incidence FROM "
               "hiv_incidence;";
    }

    std::vector<double> GetInfectionProbability(const model::Person &person) {
        if (_infection_data.empty()) {
            hepce::utils::LogWarning(
                GetLogName(),
                "HIV Infection Probability is Empty. Returning 0.0...");
            return {0.0};
        }

        int age_years = static_cast<int>(person.GetAge() / 12.0);
        int gender = static_cast<int>(person.GetSex());
        int drug_behavior =
            static_cast<int>(person.GetBehaviorDetails().behavior);
        utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
        double incidence = _infection_data[tup];

        return {incidence};
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_