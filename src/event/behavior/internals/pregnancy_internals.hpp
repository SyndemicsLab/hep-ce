////////////////////////////////////////////////////////////////////////////////
// File: pregnancy_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
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
    PregnancyImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name = "console");
    ~PregnancyImpl() = default;
    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    struct pregnancy_probabilities {
        double miscarriage = 0.0;
        double pregnant = 0.0;
    };
    using pregnancymap_t =
        std::unordered_map<int, struct pregnancy_probabilities>;
    pregnancymap_t pregnancy_data;
    double multiple_delivery_probability;
    double infant_hcv_tested_probability;
    double vertical_hcv_transition_probability;

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
        std::vector<double> probs = {pregnancy_data[age].miscarriage,
                                     1 - pregnancy_data[age].miscarriage};
        return !sampler.GetDecision(probs);
    }

    int LoadPregnancyData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string error;
        int rc = dm->SelectCustomCallback(PregnancySQL(), Callback,
                                          &pregnancy_data, error);
        if (rc != 0) {
            // spdlog::get("main")->error(
            //     "No probabilities avaliable for Pregnancy!");
            return rc;
        }
        return 0;
    }

    int const GetNumberOfBirths(const model::Person &person,
                                model::Sampler &sampler) const {

        std::vector<double> result = {multiple_delivery_probability,
                                      1 - multiple_delivery_probability};
        // Currently only deciding between single birth or twins
        return (sampler.GetDecision(result) == 0) ? 2 : 1;
    }

    bool DoChildrenGetTested(model::Sampler &sampler) {

        std::vector<double> result = {infant_hcv_tested_probability,
                                      1 - infant_hcv_tested_probability};
        return (sampler.GetDecision(result) == 0) ? true : false;
    }

    bool DrawChildInfection(model::Sampler &sampler) {
        std::vector<double> result = {vertical_hcv_transition_probability,
                                      1 - vertical_hcv_transition_probability};
        return (sampler.GetDecision(result) == 0) ? true : false;
    }

    void AttemptHaveChild(model::Person &person, model::Sampler &sampler) {
        if (CheckMiscarriage(person, sampler)) {
            person.Stillbirth();
            return;
        }

        int numberOfBirths = GetNumberOfBirths(person, sampler);

        if (person.GetHCV() != data::HCV::CHRONIC) {
            for (int child = 0; child < numberOfBirths; ++child) {
                person.AddChild(data::HCV::NONE, false);
            }
            return;
        }

        bool tested = DoChildrenGetTested(sampler);
        for (int child = 0; child < numberOfBirths; ++child) {
            person.ExposeInfant();
            data::HCV hcv = (DrawChildInfection(sampler)) ? data::HCV::CHRONIC
                                                          : data::HCV::NONE;
            person.AddChild(hcv, tested);
        }
    }

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