////////////////////////////////////////////////////////////////////////////////
// File: infection_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_INFECTIONINTERNALS_HPP_
#define HEPCE_EVENT_HCV_INFECTIONINTERNALS_HPP_

// File Header
#include <hepce/event/hcv/infection.hpp>

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "../../internals/event_internals.hpp"
namespace hepce {
namespace event {
namespace hcv {
class InfectionImpl : public virtual hcv::Infection, public EventBase {
public:
    using incidencemap_t =
        std::unordered_map<utils::tuple_3i, double, utils::key_hash_3i,
                           utils::key_equal_3i>;

    InfectionImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");

    ~InfectionImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    incidencemap_t _infection_data;
    double _gt3_prob = 0;

    static void CallbackInfection(std::any &storage,
                                  const SQLite::Statement &stmt) {
        utils::tuple_3i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt(),
                                              stmt.getColumn(2).getInt());
        std::any_cast<incidencemap_t>(storage)[tup] =
            stmt.getColumn(3).getDouble();
    }

    inline const std::string IncidenceSQL() const {
        return "SELECT age_years, gender, drug_behavior, incidence FROM "
               "incidence;";
    }

    std::vector<double> GetInfectionProbability(const model::Person &person);
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif