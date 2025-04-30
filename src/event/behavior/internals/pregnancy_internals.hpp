////////////////////////////////////////////////////////////////////////////////
// File: pregnancy_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_PREGNANCYINTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_PREGNANCYINTERNALS_HPP_

#include <hepce/event/behavior/pregnancy.hpp>

#include <sstream>

#include <hepce/utils/formatting.hpp>

#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {
class PregnancyImpl : public virtual Pregnancy, public EventBase {
public:
    struct pregnancy_probabilities {
        double miscarriage = 0.0;
        double pregnant = 0.0;
    };
    using pregnancymap_t =
        std::unordered_map<int, struct pregnancy_probabilities>;

    PregnancyImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");
    ~PregnancyImpl() = default;
    void Execute(model::Person &person, model::Sampler &sampler) override;

private:
    pregnancymap_t _pregnancy_data;
    double _multiple_delivery_probability;
    double _infant_hcv_tested_probability;
    double _vertical_hcv_transition_probability;

    inline const std::string PregnancySQL() const {
        return "SELECT age_years, miscarriage, pregnancy_probability FROM "
               "pregnancy;";
    }

    inline bool CheckMiscarriage(const model::Person &person,
                                 model::Sampler &sampler) {
        int age = static_cast<int>(person.GetAge());
        std::vector<double> probs = {_pregnancy_data[age].miscarriage,
                                     1 - _pregnancy_data[age].miscarriage};
        return !sampler.GetDecision(probs);
    }

    int LoadPregnancyData(datamanagement::ModelData &model_data) {
        std::any storage = _pregnancy_data;

        model_data.GetDBSource("inputs").Select(
            PregnancySQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                struct pregnancy_probabilities d = {
                    stmt.getColumn(1).getDouble(),
                    stmt.getColumn(2).getDouble()};
                std::any_cast<pregnancymap_t>(
                    storage)[stmt.getColumn(0).getInt()] = d;
            },
            storage);

        _pregnancy_data = std::any_cast<pregnancymap_t>(storage);

        if (_pregnancy_data.empty()) {
            // Warn Empty
        }
        return 0;
    }

    int const GetNumberOfBirths(const model::Person &person,
                                model::Sampler &sampler) const {

        std::vector<double> result = {_multiple_delivery_probability,
                                      1 - _multiple_delivery_probability};
        // Currently only deciding between single birth or twins
        return (sampler.GetDecision(result) == 0) ? 2 : 1;
    }

    bool DoChildrenGetTested(model::Sampler &sampler) {

        std::vector<double> result = {_infant_hcv_tested_probability,
                                      1 - _infant_hcv_tested_probability};
        return (sampler.GetDecision(result) == 0) ? true : false;
    }

    bool DrawChildInfection(model::Sampler &sampler) {
        std::vector<double> result = {_vertical_hcv_transition_probability,
                                      1 - _vertical_hcv_transition_probability};
        return (sampler.GetDecision(result) == 0) ? true : false;
    }

    void AttemptHaveChild(model::Person &person, model::Sampler &sampler);

    data::Child MakeChild(data::HCV hcv, bool test);

    void AttemptHealthyMonth(model::Person &person, model::Sampler &sampler) {
        if (CheckMiscarriage(person, sampler)) {
            person.Miscarry();
        }
    }
};
} // namespace behavior
} // namespace event
} // namespace hepce

#endif