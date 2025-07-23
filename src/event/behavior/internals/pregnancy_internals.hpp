////////////////////////////////////////////////////////////////////////////////
// File: pregnancy_internals.hpp                                              //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-12                                                  //
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
        double stillbirth = 0.0;
        double pregnant = 0.0;
    };
    using pregnancymap_t =
        std::unordered_map<int, struct pregnancy_probabilities>;

    PregnancyImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");
    ~PregnancyImpl() = default;
    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    pregnancymap_t _pregnancy_data;
    const double _multiple_delivery_probability;
    const double _infant_hcv_tested_probability;
    const double _vertical_hcv_transition_probability;

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
                                model::Sampler &sampler) {
        int age = static_cast<int>(person.GetAge() / 12.0);
        std::vector<double> probs = {_pregnancy_data[age].stillbirth,
                                     1 - _pregnancy_data[age].stillbirth};
        return !sampler.GetDecision(probs);
    }

    inline int const GetNumberOfBirths(const model::Person &person,
                                       model::Sampler &sampler) const {

        std::vector<double> result = {_multiple_delivery_probability,
                                      1 - _multiple_delivery_probability};
        // Currently only deciding between single birth or twins
        return (sampler.GetDecision(result) == 0) ? 2 : 1;
    }

    inline bool DoChildrenGetTested(model::Sampler &sampler) {

        std::vector<double> result = {_infant_hcv_tested_probability,
                                      1 - _infant_hcv_tested_probability};
        return (sampler.GetDecision(result) == 0) ? true : false;
    }

    inline bool DrawChildInfection(model::Sampler &sampler) {
        std::vector<double> result = {_vertical_hcv_transition_probability,
                                      1 - _vertical_hcv_transition_probability};
        return (sampler.GetDecision(result) == 0) ? true : false;
    }

    void AttemptHaveChild(model::Person &person, model::Sampler &sampler);

    data::Child MakeChild(data::HCV hcv, bool test);
};
} // namespace behavior
} // namespace event
} // namespace hepce

#endif