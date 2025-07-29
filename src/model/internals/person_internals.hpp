////////////////////////////////////////////////////////////////////////////////
// File: person_internals.hpp                                                 //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-23                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_PERSONINTERNALS_HPP_
#define HEPCE_MODEL_PERSONINTERNALS_HPP_

// File Header
#include <hepce/model/person.hpp>

// STL Includes
#include <algorithm>
#include <execution>
#include <numeric>

// Library Includes
#include <hepce/data/types.hpp>
#include <hepce/model/costing.hpp>
#include <hepce/utils/math.hpp>

namespace hepce {
namespace model {
class PersonImpl : public Person {
public:
    PersonImpl(const std::string &log_name = "console");
    ~PersonImpl() = default;

    // Implementation Separated Functions
    void InfectHCV() override;
    void InitiateTreatment(data::InfectionType it) override;
    void SetBehavior(data::Behavior) override;
    bool IsCirrhotic() const override;
    void Birth(const data::Child &child) override;
    void TransitionMOUD() override;
    void DevelopHCC(data::HCCState state) override;
    std::string MakePopulationRow() const override;
    void SetMoudState(data::MOUD moud) override;

    void SetPersonDetails(const data::PersonSelect &select) override;

    // Section Getters
    inline data::HCVDetails GetHCVDetails() const override {
        return _hcv_details;
    }
    inline data::ScreeningDetails
    GetScreeningDetails(data::InfectionType it) const override {
        return _screening_details.at(it);
    }
    inline data::LinkageDetails
    GetLinkageDetails(data::InfectionType it) const override {
        return _linkage_details.at(it);
    }
    inline data::TreatmentDetails
    GetTreatmentDetails(data::InfectionType it) const override {
        return _treatment_details.at(it);
    }
    inline data::HIVDetails GetHIVDetails() const override {
        return _hiv_details;
    }
    inline data::HCCDetails GetHCCDetails() const override {
        return _hcc_details;
    }
    inline data::StagingDetails GetFibrosisStagingDetails() const override {
        return _staging_details;
    }
    inline data::PregnancyDetails GetPregnancyDetails() const override {
        return _pregnancy_details;
    }
    inline data::MOUDDetails GetMoudDetails() const override {
        return _moud_details;
    }

    // Inline Functions
    inline void Grow() override {
        UpdateTimers();
        _age++;
        _life_span++;
    }
    inline void Die(data::DeathReason death_reason =
                        data::DeathReason::kBackground) override {
        _is_alive = false;
        SetDeathReason(death_reason);
    }
    inline void ClearHCV(bool is_acute = false) override {
        _hcv_details.hcv = data::HCV::kNone;
        _hcv_details.time_changed = _current_time;
        if (is_acute) {
            AddAcuteHCVClearance();
        }
    }
    inline void SetHCV(data::HCV hcv) override {
        _hcv_details.hcv = hcv;
        _hcv_details.time_changed = _current_time;
    }
    inline void Diagnose(data::InfectionType it) override {
        _screening_details[it].identified = true;
        _screening_details[it].time_identified = _current_time;
        _screening_details[it].times_identified++;
        _screening_details[it].ab_positive = true;
    }
    inline void ClearDiagnosis(data::InfectionType it) override {
        _screening_details[it].identified = false;
    }
    inline void FalsePositive(data::InfectionType it) override {
        ClearDiagnosis(it);
        _screening_details[it].times_identified--;
        if (_screening_details[it].times_identified == 0) {
            _screening_details[it].time_identified = -1;
            _screening_details[it].ab_positive = false;
        }
    }
    inline void AddFalseNegative(data::InfectionType it) override {
        _screening_details[it].num_false_negatives++;
    }

    void Screen(data::InfectionType it, data::ScreeningTest test,
                data::ScreeningType type) override {
        _screening_details[it].time_of_last_screening = _current_time;
        if (test == data::ScreeningTest::kAb) {
            _screening_details[it].num_ab_tests++;
        } else {
            _screening_details[it].num_rna_tests++;
        }
        _screening_details[it].screen_type = type;
    }
    inline void GiveSecondStagingTest() override {
        _staging_details.had_second_test = true;
    }

    // Linking
    inline void Unlink(data::InfectionType it) override {
        _linkage_details[it].link_state = data::LinkageState::kUnlinked;
        _linkage_details[it].time_link_change = _current_time;
    }
    inline void Link(data::InfectionType it) override {
        _linkage_details[it].link_state = data::LinkageState::kLinked;
        _linkage_details[it].time_link_change = _current_time;
        _linkage_details[it].link_count++;
    }

    // Treatment
    inline void AddWithdrawal(data::InfectionType it) override {
        _treatment_details[it].num_withdrawals++;
    }
    inline void AddToxicReaction(data::InfectionType it) override {
        _treatment_details[it].num_toxic_reactions++;
    }
    inline void AddCompletedTreatment(data::InfectionType it) override {
        _treatment_details[it].num_completed++;
    }
    inline void AddSVR() override { _hcv_details.svrs++; }
    inline void EndTreatment(data::InfectionType it) override {
        _treatment_details[it].initiated_treatment = false;
        _treatment_details[it].in_salvage_treatment = false;
    }

    inline data::BehaviorDetails GetBehaviorDetails() const override {
        return _behavior_details;
    }

    inline void ToggleOverdose() override {
        _currently_overdosing = !_currently_overdosing;
        if (_currently_overdosing) {
            _num_overdoses++;
        }
    }
    inline bool GetCurrentlyOverdosing() const override {
        return _currently_overdosing;
    }

    inline int GetNumberOfOverdoses() const override { return _num_overdoses; }
    inline void DiagnoseFibrosis(data::MeasuredFibrosisState data) override {
        _staging_details.measured_fibrosis_state = data;
        _staging_details.time_of_last_staging = _current_time;
    }
    inline void SetFibrosis(data::FibrosisState state) override {
        _hcv_details.time_fibrosis_state_changed = _current_time;
        _hcv_details.fibrosis_state = state;
    }

    // Cost Effectiveness
    inline void AddCost(double base_cost, double discount_cost,
                        model::CostCategory category) override {
        _costs->AddCost(base_cost, discount_cost, category);
    }

    // Life, Quality of Life
    inline data::LifetimeUtility GetTotalUtility() const override {
        return _life_utilites;
    }
    inline void AccumulateTotalUtility(double discount) override {
        _life_utilites.min_util += GetMinimizedUtility();
        _life_utilites.mult_util += GetMultipliedUtility();
        _life_utilites.discount_min_util += utils::Discount(
            GetMinimizedUtility(), discount, GetCurrentTimestep());
        _life_utilites.discount_mult_util += utils::Discount(
            GetMultipliedUtility(), discount, GetCurrentTimestep());
    }
    inline std::unordered_map<model::UtilityCategory, double>
    GetUtilities() const override {
        return _utilities;
    }
    inline void SetUtility(double util,
                           model::UtilityCategory category) override {
        _utilities[category] = util;
    }
    inline int GetLifeSpan() const override { return _life_span; }
    inline double GetDiscountedLifeSpan() const override {
        return _discounted_life_span;
    }
    inline void AddDiscountedLifeSpan(double discounted_life) override {
        _discounted_life_span += discounted_life;
    }

    // General Data Handling
    inline bool IsAlive() const override { return _is_alive; }
    inline void SetGenotypeThree(bool genotype) override {
        _hcv_details.is_genotype_three = genotype;
    }
    inline bool IsBoomer() const override { return _boomer_classification; }
    inline void SetDeathReason(data::DeathReason death_reason) override {
        _death_reason = death_reason;
    }
    inline data::DeathReason GetDeathReason() const override {
        return _death_reason;
    }
    inline int GetAge() const override { return _age; }

    inline int GetCurrentTimestep() const override { return _current_time; }
    inline data::Sex GetSex() const override { return _sex; }
    inline std::unordered_map<model::CostCategory, std::pair<double, double>>
    GetCosts() const override {
        return _costs->GetCosts();
    }
    inline std::pair<double, double> GetCostTotals() const override {
        return _costs->GetTotals();
    }

    inline void DiagnoseHCC() override { _hcc_details.hcc_diagnosed = true; }

    inline void SetHIV(data::HIV hiv) override { _hiv_details.hiv = hiv; }
    inline void InfectHIV() override {
        if (_hiv_details.hiv != data::HIV::kNone) {
            return;
        }
        _hiv_details.hiv = data::HIV::kHiUn;
        _hiv_details.time_changed = _current_time;
    }

    inline void Stillbirth() override {
        _pregnancy_details.num_stillbirths++;
        _pregnancy_details.time_of_pregnancy_change = _current_time;
        _pregnancy_details.pregnancy_state =
            data::PregnancyState::kRestrictedPostpartum;
    }
    inline void EndPostpartum() override {
        _pregnancy_details.time_of_pregnancy_change = _current_time;
        _pregnancy_details.pregnancy_state = data::PregnancyState::kNone;
    }
    inline void Impregnate() override {
        _pregnancy_details.count++;
        _pregnancy_details.time_of_pregnancy_change = _current_time;
        _pregnancy_details.pregnancy_state = data::PregnancyState::kPregnant;
    }
    inline void AddInfantExposure() override {
        _pregnancy_details.num_hcv_exposures++;
    }
    inline void SetPregnancyState(data::PregnancyState state) override {
        _pregnancy_details.time_of_pregnancy_change = _current_time;
        _pregnancy_details.pregnancy_state = state;
    }

private:
    const std::string _log_name;

    size_t _id;
    int _current_time = 0;

    data::Sex _sex = data::Sex::kMale;
    int _age = 0;
    bool _is_alive = true;
    bool _boomer_classification = false;
    data::DeathReason _death_reason = data::DeathReason::kNa;
    data::BehaviorDetails _behavior_details;
    data::HCVDetails _hcv_details;
    data::HIVDetails _hiv_details;
    data::HCCDetails _hcc_details;
    bool _currently_overdosing = false;
    int _num_overdoses = 0;
    data::MOUDDetails _moud_details;
    data::PregnancyDetails _pregnancy_details;
    data::StagingDetails _staging_details;
    std::unordered_map<data::InfectionType, data::LinkageDetails>
        _linkage_details;
    std::unordered_map<data::InfectionType, data::ScreeningDetails>
        _screening_details;
    std::unordered_map<data::InfectionType, data::TreatmentDetails>
        _treatment_details;
    // utility
    std::unordered_map<model::UtilityCategory, double> _utilities;
    data::LifetimeUtility _life_utilites;
    // life span tracking
    int _life_span = 0;
    double _discounted_life_span = 0;
    // cost
    std::unique_ptr<model::Costs> _costs;

    void UpdateTimers();

    inline void AddAcuteHCVClearance() { _hcv_details.times_acute_cleared++; }

    inline void SetInfectionDefaults(const data::InfectionType &infection) {
        _linkage_details[infection] = data::LinkageDetails{};
        _screening_details[infection] = data::ScreeningDetails{};
        _treatment_details[infection] = data::TreatmentDetails{};
    }

    inline double GetMinimizedUtility() const {
        double min = 1;
        for (const auto &kv : _utilities) {
            if (kv.second < min) {
                min = kv.second;
            }
        }
        return min;
    }

    inline double GetMultipliedUtility() const {
        double mult = 1;
        for (const auto &kv : _utilities) {
            mult *= kv.second;
        }
        return mult;
    }
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_PERSONINTERNALS_HPP_
