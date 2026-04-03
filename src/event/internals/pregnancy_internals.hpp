////////////////////////////////////////////////////////////////////////////////
// File: pregnancy_internals.hpp                                              //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-03                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_PREGNANCYINTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_PREGNANCYINTERNALS_HPP_

#include <sstream>

#include <hepce/utils/formatting.hpp>

#include "base_event_internals.hpp"

namespace hepce {
namespace event {
class Pregnancy : public virtual EventBase {
public:
    struct pregnancy_probabilities {
        double stillbirth = 0.0;
        double pregnant = 0.0;
    };
    using pregnancymap_t =
        std::unordered_map<int, struct pregnancy_probabilities>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    Pregnancy(const data::Inputs &inputs, const std::string &log)
        : EventBase("pregnancy", inputs, log),
          _multiple_delivery_probability(utils::GetDoubleFromConfig(
              "pregnancy.multiple_delivery_probability", inputs)),
          _infant_hcv_tested_probability(utils::GetDoubleFromConfig(
              "pregnancy.infant_hcv_tested_probability", inputs)),
          _vertical_hcv_transition_probability(utils::GetDoubleFromConfig(
              "pregnancy.vertical_hcv_transition_probability", inputs)) {
        LoadData();
    }
    ~Pregnancy() = default;
    void Execute(model::Person &person, const model::Sampler &sampler) override;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<Pregnancy>(GetInputs(), GetLogName());
    }

private:
    pregnancymap_t _pregnancy_data;
    const double _multiple_delivery_probability;
    const double _infant_hcv_tested_probability;
    const double _vertical_hcv_transition_probability;

    void LoadData();

    void ProgressPostpartum(model::Person &person) const;

    inline bool CheckOldAge(const model::Person &person) {
        bool too_old = person.GetAge() > 540;
        auto ps = person.GetPregnancyDetails().pregnancy_state;
        bool pregnant = (ps == data::PregnancyState::kPregnant);
        bool rpostpartum =
            (ps == data::PregnancyState::kRestrictedPostpartum) ||
            (ps == data::PregnancyState::kYearOnePostpartum) ||
            (ps == data::PregnancyState::kYearTwoPostpartum);
        ;
        return (too_old && (!pregnant && !rpostpartum));
    }

    inline bool CheckPostpartumTime(const model::Person &person) {
        bool postpartum = person.GetPregnancyDetails().pregnancy_state ==
                          data::PregnancyState::kRestrictedPostpartum;
        bool valid_time =
            GetTimeSince(
                person, person.GetPregnancyDetails().time_of_pregnancy_change) <
            3;
        return (postpartum && valid_time);
    }

    inline const std::string PregnancySQL() const {
        return "SELECT age_years, stillbirth, pregnancy_probability FROM "
               "pregnancy;";
    }

    inline bool CheckStillbirth(const model::Person &person,
                                const model::Sampler &sampler) {
        int age = static_cast<int>(person.GetAge() / 12.0);
        std::vector<double> probs = {_pregnancy_data[age].stillbirth,
                                     1 - _pregnancy_data[age].stillbirth};
        return !sampler.GetDecision(probs);
    }

    inline int const GetNumberOfBirths(const model::Person &person,
                                       const model::Sampler &sampler) const {

        std::vector<double> result = {_multiple_delivery_probability,
                                      1 - _multiple_delivery_probability};
        // Currently only deciding between single birth or twins
        return (sampler.GetDecision(result) == 0) ? 2 : 1;
    }

    inline bool DoChildrenGetTested(const model::Sampler &sampler) {

        std::vector<double> result = {_infant_hcv_tested_probability,
                                      1 - _infant_hcv_tested_probability};
        return (sampler.GetDecision(result) == 0) ? true : false;
    }

    inline bool DrawChildInfection(const model::Sampler &sampler) {
        std::vector<double> result = {_vertical_hcv_transition_probability,
                                      1 - _vertical_hcv_transition_probability};
        return (sampler.GetDecision(result) == 0) ? true : false;
    }

    void AttemptHaveChild(model::Person &person, const model::Sampler &sampler);

    data::Child MakeChild(const data::HCV &hcv, const bool &test);
};
} // namespace event
} // namespace hepce

#endif