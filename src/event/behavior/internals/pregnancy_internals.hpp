////////////////////////////////////////////////////////////////////////////////
// File: pregnancy_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_PREGNANCYINTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_PREGNANCYINTERNALS_HPP_

#include <hepce/event/behavior/pregnancy.hpp>

#include <sstream>

#include "internals/event_internals.hpp"
#include <hepce/utils/formatting.hpp>

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

    PregnancyImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name = "console");
    ~PregnancyImpl() = default;
    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    pregnancymap_t _pregnancy_data;
    double _multiple_delivery_probability;
    double _infant_hcv_tested_probability;
    double _vertical_hcv_transition_probability;

    static int Callback(void *storage, int count, char **data, char **columns) {
        struct pregnancy_probabilities d = {utils::SToDPositive(data[1]),
                                            utils::SToDPositive(data[2])};
        (*((pregnancymap_t *)storage))[std::stoi(data[0])] = d;
        return 0;
    }
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

    int LoadPregnancyData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string error;
        int rc = dm->SelectCustomCallback(PregnancySQL(), Callback,
                                          &_pregnancy_data, error);
        if (rc != 0) {
            // spdlog::get("main")->error(
            //     "No probabilities avaliable for Pregnancy!");
            return rc;
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