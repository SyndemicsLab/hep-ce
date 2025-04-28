////////////////////////////////////////////////////////////////////////////////
// File: Person.cpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-02                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Person.hpp"
#include "ConfigUtils.hpp"
#include "PersonTableOperations.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cstdint>
#include <datamanagement/datamanagement.hpp>
#include <map>
#include <string>
#include <vector>

namespace person {
int count = 0;

class Person::PersonIMPL {
private:
    size_t id = count;
    size_t _current_time = 0;

    Sex sex = Sex::kMale;
    int age = 0;
    bool is_alive = true;
    bool boomer_classification = false;
    DeathReason death_reason = DeathReason::kNa;
    BehaviorDetails behavior_details;
    HCVDetails hcv_details;
    HIVDetails hiv_details;
    HCCDetails hcc_details;
    bool currently_overdosing = false;
    int num_overdoses = 0;
    MOUDDetails moud_details;
    PregnancyDetails pregnancy_details;
    StagingDetails staging_details;
    std::unordered_map<InfectionType, LinkageDetails> linkage_details;
    std::unordered_map<InfectionType, ScreeningDetails> screening_details;
    std::unordered_map<InfectionType, TreatmentDetails> treatment_details;
    // utility
    utility::UtilityTracker utility_tracker;
    LifetimeUtility lifetime_utility;
    // life span tracking
    int life_span = 0;
    double discounted_life_span = 0;
    // cost
    cost::CostTracker costs;

    int UpdateTimers() {
        this->_current_time++;
        if (GetBehavior() == person::Behavior::kNoninjection ||
            GetBehavior() == person::Behavior::kInjection) {
            this->behavior_details.time_last_active = this->_current_time;
        }
        if (GetMoudState() == person::MOUD::kCurrent) {
            this->moud_details.total_moud_months++;
        }
        if (GetHIV() == person::HIV::LoUn || GetHIV() == person::HIV::LoSu) {
            this->hiv_details.low_cd4_months_count++;
        }
        this->moud_details.current_state_concurrent_months++;
        return 0;
    }

    /// @brief Setter for MOUD State
    /// @param moud PersonIMPL's new MOUD state
    void SetMoudState(MOUD moud) {
        if (moud == person::MOUD::kCurrent) {
            this->moud_details.time_started = this->_current_time;
        }
        this->moud_details.current_state_concurrent_months = 0;
        this->moud_details.moud_state = moud;
    }

    int CalculateTimeSince(int lasttime) const {
        if (lasttime <= -1) {
            return GetCurrentTimestep();
        }
        return GetCurrentTimestep() - lasttime;
    }

public:
    PersonIMPL() {
        for (InfectionType i = ::kHcv; i < ::kCount; ++i) {
            this->linkage_details[i] = LinkageDetails();
            this->screening_details[i] = ScreeningDetails();
            this->treatment_details[i] = TreatmentDetails();
            count++;
        }
    }

    int CreatePersonFromTable(int id, datamanagement::ModelData &model_data) {
        this->id = id;
        if (dm == nullptr) {
            // Let default values stay
            return 0;
        }

        std::stringstream query;
        std::vector<std::string> events = Utils::split2vecT<std::string>(
            Utils::GetStringFromConfig("simulation.events", dm), ',');

        // this is a stopgap with plans to make Event-scoped CheckFor<X>Event
        // functions that are static / usable throughout the model.
        bool pregnancy =
            Utils::FindInVector<std::string>(events, {"pregnancy"});
        bool hcc = Utils::FindInVector<std::string>(events, {"HCCScreening"});
        bool overdose =
            Utils::FindInVector<std::string>(events, {"BehaviorChanges"});
        bool hiv = Utils::FindInVector<std::string>(
            events,
            {"HIVInfections", "HIVLinking", "HIVScreening", "HIVTreatment"});
        bool moud = Utils::FindInVector<std::string>(events, {"MOUD"});

        query << "SELECT "
              << person::POPULATION_HEADERS(pregnancy, hcc, overdose, hiv,
                                            moud);
        query << "FROM population ";
        query << "WHERE id = " << std::to_string(id);

        struct person_select storage;
        std::string error;
        int rc = dm->SelectCustomCallback(query.str(), person_callback,
                                          &storage, error);
        if (rc != 0) {
#ifdef NDEBUG
            // only log on main because during debug we don't always expect
            // tables
            spdlog::get("main")->error(
                "Issue Selecting person from population table! Error "
                "Message: {}",
                error);
#endif
            return rc;
        }

        // basic characteristics
        this->sex = storage.sex;
        SetAge(storage.age);
        this->is_alive = storage.is_alive;
        this->boomer_classification = storage.boomer_classification;
        this->death_reason = storage.death_reason;

        // BehaviorDetails
        this->behavior_details.behavior = storage.drug_behavior;
        this->behavior_details.time_last_active =
            storage.time_last_active_drug_use;

        // HCVDetails
        this->hcv_details.hcv = storage.hcv;
        this->hcv_details.fibrosis_state = storage.fibrosis_state;
        this->hcv_details.is_genotype_three = storage.is_genotype_three;
        this->hcv_details.seropositive = storage.seropositive;
        this->hcv_details.time_changed = storage.time_hcv_changed;
        this->hcv_details.time_fibrosis_state_changed =
            storage.time_fibrosis_state_changed;
        this->hcv_details.times_infected = storage.times_hcv_infected;
        this->hcv_details.times_acute_cleared = storage.times_acute_cleared;
        this->hcv_details.svrs = storage.svrs;

        // HIVDetails
        this->hiv_details.hiv = storage.hiv;
        this->hiv_details.time_changed = storage.time_hiv_changed;
        this->hiv_details.low_cd4_months_count = storage.low_cd4_months_count;

        // HCCDetails
        this->hcc_details.hcc_state = storage.hcc_state;
        this->hcc_details.hcc_diagnosed = storage.hcc_diagnosed;

        // Overdoses
        this->num_overdoses = storage.num_overdoses;
        this->currently_overdosing = storage.currently_overdosing;

        // MOUDDetails
        this->moud_details.moud_state = storage.moud_state;
        this->moud_details.time_started = storage.time_started_moud;
        this->moud_details.current_state_concurrent_months =
            storage.current_moud_concurrent_months;
        this->moud_details.total_moud_months = storage.total_moud_months;

        // PregnancyDetails
        this->pregnancy_details.pregnancy_state = storage.pregnancy_state;
        this->pregnancy_details.time_of_pregnancy_change =
            storage.time_of_pregnancy_change;
        this->pregnancy_details.count = storage.pregnancy_count;
        this->pregnancy_details.num_infants = storage.num_infants;
        this->pregnancy_details.num_miscarriages = storage.num_miscarriages;
        this->pregnancy_details.num_hcv_exposures =
            storage.num_infant_hcv_exposures;
        this->pregnancy_details.num_hcv_infections =
            storage.num_infant_hcv_infections;
        this->pregnancy_details.num_hcv_tests = storage.num_infant_hcv_tests;

        // StagingDetails
        this->staging_details.measured_fibrosis_state =
            storage.measured_fibrosis_state;
        this->staging_details.had_second_test = storage.had_second_test;
        this->staging_details.time_of_last_staging =
            storage.time_of_last_staging;

        // HCV
        InfectionType it = ::kHcv;

        // LinkageDetails
        this->linkage_details[it].link_state = storage.hcv_link_state;
        this->linkage_details[it].time_of_link_change =
            storage.time_of_hcv_link_change;
        this->linkage_details[it].link_type = storage.hcv_link_type;
        this->linkage_details[it].link_count = storage.hcv_link_count;

        // ScreeningDetails
        this->screening_details[it].time_of_last_screening =
            storage.time_of_last_hcv_screening;
        this->screening_details[it].num_ab_tests = storage.num_hcv_ab_tests;
        this->screening_details[it].num_rna_tests = storage.num_hcv_rna_tests;
        this->screening_details[it].antibody_positive =
            storage.hcv_antibody_positive;
        this->screening_details[it].identified = storage.hcv_identified;
        this->screening_details[it].time_identified =
            storage.time_hcv_identified;

        // TreatmentDetails
        this->treatment_details[it].initiated_treatment =
            storage.initiated_hcv_treatment;
        this->treatment_details[it].time_of_treatment_initiation =
            storage.time_of_hcv_treatment_initiation;
        this->treatment_details[it].num_starts =
            storage.num_hcv_treatment_starts;
        this->treatment_details[it].num_withdrawals =
            storage.num_hcv_treatment_withdrawals;
        this->treatment_details[it].num_toxic_reactions =
            storage.num_hcv_treatment_toxic_reactions;
        this->treatment_details[it].num_completed =
            storage.num_completed_hcv_treatments;
        this->treatment_details[it].num_retreatments =
            storage.num_hcv_retreatments;
        this->treatment_details[it].retreatment = storage.in_hcv_retreatment;

        // HIV
        it = ::kHiv;

        // LinkageDetails
        this->linkage_details[it].link_state = storage.hiv_link_state;
        this->linkage_details[it].time_of_link_change =
            storage.time_of_hiv_link_change;
        this->linkage_details[it].link_type = storage.hiv_link_type;
        this->linkage_details[it].link_count = storage.hiv_link_count;

        // ScreeningDetails
        this->screening_details[it].time_of_last_screening =
            storage.time_of_last_hiv_screening;
        this->screening_details[it].num_ab_tests = storage.num_hiv_ab_tests;
        this->screening_details[it].num_rna_tests = storage.num_hiv_rna_tests;
        this->screening_details[it].antibody_positive =
            storage.hiv_antibody_positive;
        this->screening_details[it].identified = storage.hiv_identified;
        this->screening_details[it].time_identified =
            storage.time_hiv_identified;

        // TreatmentDetails
        this->treatment_details[it].initiated_treatment =
            storage.initiated_hiv_treatment;
        this->treatment_details[it].time_of_treatment_initiation =
            storage.time_of_hiv_treatment_initiation;
        this->treatment_details[it].num_starts =
            storage.num_hiv_treatment_starts;
        this->treatment_details[it].num_withdrawals =
            storage.num_hiv_treatment_withdrawals;
        this->treatment_details[it].num_toxic_reactions =
            storage.num_hiv_treatment_toxic_reactions;

        // UtilityTracker
        this->utility_tracker.SetUtility(storage.behavior_utility,
                                         utility::UtilityCategory::BEHAVIOR);
        this->utility_tracker.SetUtility(storage.liver_utility,
                                         utility::UtilityCategory::LIVER);
        this->utility_tracker.SetUtility(storage.treatment_utility,
                                         utility::UtilityCategory::TREATMENT);
        this->utility_tracker.SetUtility(storage.background_utility,
                                         utility::UtilityCategory::kBackground);
        this->utility_tracker.SetUtility(storage.hiv_utility,
                                         utility::UtilityCategory::HIV);

        return 0;
    }

    std::string MakePopulationRow() const {
        std::stringstream population_row;
        // clang-format off
        // basic characteristics
        population_row << this->id << ","
                       << this->sex << ","
                       << this->age << ","
                       << std::boolalpha << this->is_alive << ","
                       << std::boolalpha << this->boomer_classification << ","
                       << this->death_reason << ",";
        // BehaviorDetails
        const auto &bd = this->behavior_details;
        population_row << bd.behavior << ","
                       << bd.time_last_active << ",";
        // HCVDetails
        const auto &hcv = this->hcv_details;
        population_row << hcv.hcv << ","
                       << hcv.fibrosis_state << ","
                       << std::boolalpha << hcv.is_genotype_three << ","
                       << std::boolalpha << hcv.seropositive << ","
                       << hcv.time_changed << ","
                       << hcv.time_fibrosis_state_changed << ","
                       << hcv.times_infected << ","
                       << hcv.times_acute_cleared << ","
                       << hcv.svrs << ",";
        // HIVDetails
        const auto &hiv = this->hiv_details;
        population_row << hiv.hiv << ","
                       << hiv.time_changed << ","
                       << hiv.low_cd4_months_count << ",";
        // HCCDetails
        const auto &hcc = this->hcc_details;
        population_row << hcc.hcc_state << ","
                       << std::boolalpha << hcc.hcc_diagnosed << ",";
        // overdose characteristics
        population_row << std::boolalpha << this->currently_overdosing << ","
                       << this->num_overdoses << ",";
        // MOUDDetails
        const auto &moud = this->moud_details;
        population_row << moud.moud_state << ","
                       << moud.time_started << ","
                       << moud.current_state_concurrent_months << ","
                       << moud.total_moud_months << ",";
        // PregnancyDetails
        const auto &pd = this->pregnancy_details;
        population_row << pd.pregnancy_state << ","
                       << pd.time_of_pregnancy_change << ","
                       << pd.count << ","
                       << pd.num_infants << ","
                       << pd.num_miscarriages << ","
                       << pd.num_hcv_exposures << ","
                       << pd.num_hcv_infections << ","
                       << pd.num_hcv_tests << ",";
        // StagingDetails
        const auto &sd = this->staging_details;
        population_row << sd.measured_fibrosis_state << ","
                       << std::boolalpha << sd.had_second_test << ","
                       << sd.time_of_last_staging << ",";
        // LinkageDetails
        const auto &hcvld = this->linkage_details.at(::kHcv);
        population_row << hcvld.link_state << ","
                       << hcvld.time_of_link_change << ","
                       << hcvld.link_type << ","
                       << hcvld.link_count << ",";
        const auto &hivld = this->linkage_details.at(::kHiv);
        population_row << hivld.link_state << ","
                       << hivld.time_of_link_change << ","
                       << hivld.link_type << ","
                       << hivld.link_count << ",";
        // ScreeningDetails
        const auto &hcvsd = this->screening_details.at(::kHcv);
        population_row << hcvsd.time_of_last_screening << ","
                       << hcvsd.num_ab_tests << ","
                       << hcvsd.num_rna_tests << ","
                       << std::boolalpha << hcvsd.antibody_positive << ","
                       << std::boolalpha << hcvsd.identified << ","
                       << hcvsd.time_identified << ",";
        const auto &hivsd = this->screening_details.at(::kHiv);
        population_row << hivsd.time_of_last_screening << ","
                       << hivsd.num_ab_tests << ","
                       << hivsd.num_rna_tests << ","
                       << std::boolalpha << hivsd.antibody_positive << ","
                       << std::boolalpha << hivsd.identified << ","
                       << hivsd.time_identified << ",";
        const auto &hcvtd = this->treatment_details.at(::kHcv);
        population_row << std::boolalpha << hcvtd.initiated_treatment << ","
                       << hcvtd.time_of_treatment_initiation << ","
                       << hcvtd.num_starts << ","
                       << hcvtd.num_withdrawals << ","
                       << hcvtd.num_toxic_reactions << ","
                       << hcvtd.num_completed << ","
                       << hcvtd.num_retreatments << ","
                       << std::boolalpha << hcvtd.retreatment << ",";
        const auto &hivtd = this->treatment_details.at(::kHiv);
        population_row << std::boolalpha << hivtd.initiated_treatment << ","
                       << hivtd.time_of_treatment_initiation << ","
                       << hivtd.num_starts << ","
                       << hivtd.num_withdrawals << ","
                       << hivtd.num_toxic_reactions << ",";
        // Utilities
        // current utilities
        const auto &cu = this->GetCurrentUtilities();
        population_row << cu.at(utility::UtilityCategory::BEHAVIOR) << ","
                       << cu.at(utility::UtilityCategory::LIVER) << ","
                       << cu.at(utility::UtilityCategory::TREATMENT) << ","
                       << cu.at(utility::UtilityCategory::kBackground) << ","
                       << cu.at(utility::UtilityCategory::HIV) << ",";
        // total/lifetime utilities
        const auto &tu = this->GetTotalUtility();
        population_row << tu.min_util << ","
                       << tu.mult_util << ","
                       << tu.discount_min_util << ","
                       << tu.discount_mult_util << ",";
        // lifespan
        population_row << this->life_span << ","
                       << this->discounted_life_span << ",";

        // Cost Totals
        const auto &ct = this->GetCostTotals();
        population_row << ct.first << ","
                       << ct.second << std::endl;
        // clang-format on
        return population_row.str();
    }

    /// @brief End a PersonIMPL's life and set final Age
    void Die(DeathReason death_reason) {
        this->is_alive = false;
        SetDeathReason(death_reason);
    }

    /// @brief increase a person's Age
    void Grow() {
        UpdateTimers();
        SetAge(GetAge() + 1);
        this->life_span++;
    }

    /// @brief Infect the person
    void InfectHCV() {
        // cannot be multiply infected
        if (this->hcv_details.hcv != HCV::kNone) {
            return;
        }
        this->hcv_details.hcv = HCV::kAcute;
        this->hcv_details.time_changed = this->_current_time;
        this->hcv_details.seropositive = true;
        this->hcv_details.times_infected++;

        // once infected, immediately enter F0
        if (this->hcv_details.fibrosis_state == FibrosisState::kNone) {
            UpdateTrueFibrosis(FibrosisState::kF0);
        }
    }

    /// @brief Clear of HCV infection
    void ClearHCV(bool acute) {
        this->hcv_details.hcv = HCV::kNone;
        this->hcv_details.time_changed = this->_current_time;
        if (acute) {
            this->AddAcuteHCVClearance();
        }
    }

    /// @brief infect the person with HIV
    void InfectHIV() {
        // cannot be multiply infected
        if (this->hiv_details.hiv != HIV::kNone) {
            return;
        }
        // infected start as high CD4, unsuppressed infection
        this->hiv_details.hiv = HIV::kHiUn;
        this->hiv_details.time_changed = this->_current_time;
    }

    int LoadICValues(int id, std::vector<std::string> icValues) {
        this->id = id;
        SetAge(std::stoi(icValues[1]));
        this->sex = static_cast<person::Sex>(std::stoi(icValues[2]));
        SetBehavior(static_cast<person::Behavior>(std::stoi(icValues[3])));
        behavior_details.time_last_active = std::stoi(icValues[4]);
        bool sero = (std::stoi(icValues[5]) == 1) ? true : false;
        SetSeropositivity(sero);
        bool geno = (std::stoi(icValues[6]) == 1) ? true : false;
        SetGenotypeThree(geno);
        this->hcv_details.fibrosis_state =
            static_cast<person::FibrosisState>(std::stoi(icValues[7]));
        if (std::stoi(icValues[8]) == 1) {
            this->screening_details[::kHcv].identified = true;
            this->screening_details[::kHcv].antibody_positive = true;
        }
        linkage_details[::kHcv].link_state =
            static_cast<person::LinkageState>(std::stoi(icValues[9]));
        hcv_details.hcv = static_cast<person::HCV>(std::stoi(icValues[10]));

        if (icValues.size() == 11) {
            return 0;
        }

        pregnancy_details.pregnancy_state =
            static_cast<person::PregnancyState>(std::stoi(icValues[11]));
        if (pregnancy_details.pregnancy_state ==
            person::PregnancyState::kPregnant) {
            pregnancy_details.time_of_pregnancy_change = 0;
        }
        return 0;
    }

    /// @brief Update Opioid Use Behavior Classification
    /// @param bc The intended resultant Behavior
    /// @param timestep Current simulation timestep
    void SetBehavior(const Behavior &bc) {
        // nothing to do -- cannot go back to kNever
        if (bc == this->behavior_details.behavior || bc == Behavior::kNever) {
            return;
        }
        if ((bc == person::Behavior::kNoninjection ||
             bc == person::Behavior::kInjection) &&
            (this->behavior_details.behavior == Behavior::kNever ||
             this->behavior_details.behavior == Behavior::kFormerInjection ||
             this->behavior_details.behavior ==
                 Behavior::kFormerNoninjection)) {
            this->behavior_details.time_last_active = this->_current_time;
        }
        this->behavior_details.behavior = bc;
    }

    /// @brief Diagnose somebody's fibrosis
    /// @return Fibrosis state that is diagnosed
    MeasuredFibrosisState DiagnoseFibrosis(MeasuredFibrosisState &data) {
        // need to add functionality here
        this->staging_details.measured_fibrosis_state = data;
        this->staging_details.time_of_last_staging = this->_current_time;
        return this->staging_details.measured_fibrosis_state;
    }

    /// @brief Add an acute clearance to the running count
    void AddAcuteHCVClearance() { this->hcv_details.times_acute_cleared++; };

    void AddWithdrawal(InfectionType it) {
        this->treatment_details[it].num_withdrawals++;
    }

    void AddToxicReaction(InfectionType it) {
        this->treatment_details[it].num_toxic_reactions++;
    }

    void AddCompletedTreatment(InfectionType it) {
        this->treatment_details[it].num_completed++;
    }

    void AddSVR() { this->hcv_details.svrs++; }

    /// @brief Mark somebody as having been screened this timestep
    void MarkScreened(InfectionType it) {
        this->screening_details[it].time_of_last_screening =
            this->_current_time;
    }

    /// @brief
    void AddAbScreen(InfectionType it) {
        this->screening_details[it].time_of_last_screening =
            this->_current_time;
        this->screening_details[it].num_ab_tests++;
    }

    /// @brief
    void AddRnaScreen(InfectionType it) {
        this->screening_details[it].time_of_last_screening =
            this->_current_time;
        this->screening_details[it].num_rna_tests++;
    }

    /// @brief Reset a PersonIMPL's Link State to Unlinked
    /// @param timestep Timestep during which the PersonIMPL is Unlinked
    void Unlink(InfectionType it) {
        this->linkage_details[it].link_state = LinkageState::kUnlinked;
        this->linkage_details[it].time_of_link_change = this->_current_time;
    }

    /// @brief Reset a PersonIMPL's Link State to Linked
    /// @param timestep Timestep during which the PersonIMPL is Linked
    /// @param link_type The linkage type the PersonIMPL recieves
    void Link(LinkageType link_type, InfectionType it) {
        this->linkage_details[it].link_state = LinkageState::kLinked;
        this->linkage_details[it].time_of_link_change = this->_current_time;
        this->linkage_details[it].link_type = link_type;
        this->linkage_details[it].link_count++;
    }

    void EndTreatment(InfectionType it) {
        this->treatment_details[it].initiated_treatment = false;
        this->treatment_details[it].retreatment = false;
    }

    void Diagnose(InfectionType it) {
        this->screening_details[it].identified = true;
        this->screening_details[it].time_identified = this->_current_time;
        this->screening_details[it].antibody_positive = true;
    }

    void ClearDiagnosis(InfectionType it) {
        this->screening_details[it].identified = false;
    }

    /// @brief Add a cost to the person's CostTracker object
    /// @param cost The cost to be added
    /// @param timestep The timestep during which the cost was accrued
    void AddCost(double base_cost, double discount_cost,
                 cost::CostCategory category) {
        this->costs.AddCost(base_cost, discount_cost, category);
    }

    /// @brief Flips the person's overdose state
    void ToggleOverdose() {
        this->currently_overdosing = !this->currently_overdosing;
        if (this->currently_overdosing) {
            this->num_overdoses++;
        }
    }

    /// @brief Increment the tracker for number of infant HCV exposures
    void AddInfantExposure() { this->pregnancy_details.num_hcv_exposures++; }

    /// @brief Increment the tracker for number of miscarriages and end person's
    /// pregnancy
    void Miscarry() {
        this->pregnancy_details.num_miscarriages++;
        this->pregnancy_details.time_of_pregnancy_change = this->_current_time;
        this->pregnancy_details.pregnancy_state = person::PregnancyState::kNone;
    }

    void EndPostpartum() {
        this->pregnancy_details.time_of_pregnancy_change = this->_current_time;
        this->pregnancy_details.pregnancy_state = person::PregnancyState::kNone;
    }

    void Impregnate() {
        if (GetSex() == person::Sex::kMale) {
            return;
        }
        this->pregnancy_details.count++;
        this->pregnancy_details.time_of_pregnancy_change = this->_current_time;
        this->pregnancy_details.pregnancy_state =
            person::PregnancyState::kPregnant;
    }

    void Stillbirth() {
        if (GetSex() == person::Sex::kMale) {
            return;
        }
        this->pregnancy_details.time_of_pregnancy_change = this->_current_time;
        this->pregnancy_details.pregnancy_state =
            person::PregnancyState::kPostpartum;
        this->pregnancy_details.num_miscarriages++;
    }

    int GetInfantHCVExposures() const {
        return this->pregnancy_details.num_hcv_exposures;
    }

    int GetInfantHCVInfections() const {
        return this->pregnancy_details.num_hcv_infections;
    }

    int GetInfantHCVTests() const {
        return this->pregnancy_details.num_hcv_tests;
    }

    int GetPregnancyCount() const { return this->pregnancy_details.count; }

    void AddChild(HCV hcv, bool test) {
        person::Child child;
        child.hcv = hcv;
        if (hcv != HCV::kNone) {
            this->pregnancy_details.num_hcv_infections++;
        }
        child.tested = test;
        if (test) {
            this->pregnancy_details.num_hcv_tests++;
        }
        this->pregnancy_details.children.push_back(child);
        this->pregnancy_details.num_infants++;
    }

    void TransitionMOUD() {
        if (GetBehavior() == person::Behavior::kNever) {
            return;
        }
        person::MOUD current = GetMoudState();
        if (current == person::MOUD::kCurrent) {
            SetMoudState(person::MOUD::kPost);
        } else if (current == person::MOUD::kPost) {
            SetMoudState(person::MOUD::kNone);
        } else if (current == person::MOUD::kNone) {
            SetMoudState(person::MOUD::kCurrent);
        }
    }

    ////////////// CHECKS /////////////////

    bool IsAlive() const { return this->is_alive; }

    /// @brief Getter for whether PersonIMPL experienced fibtest two this
    /// cycle
    /// @return value of had_second_test
    bool HadSecondScreeningTest() const {
        return this->staging_details.had_second_test;
    }

    /// @brief Getter for Identification Status
    /// @return Boolean Describing Indentified as Positive Status
    bool IsIdentifiedAsInfected(InfectionType it) const {
        return this->screening_details.at(it).identified;
    }

    /// @brief Getter for whether PersonIMPL has initiated treatment
    /// @return Boolean true if PersonIMPL has initiated treatment, false
    /// otherwise
    bool HasInitiatedTreatment(InfectionType it) const {
        return this->treatment_details.at(it).initiated_treatment;
    }

    bool IsInRetreatment(InfectionType it) const {
        return this->treatment_details.at(it).retreatment;
    }
    /// @brief Getter for whether PersonIMPL is genotype three
    /// @return True if genotype three, false otherwise
    bool IsGenotypeThree() const { return this->hcv_details.is_genotype_three; }
    bool IsBoomer() const { return this->boomer_classification; }

    bool IsCirrhotic() const {
        if (this->GetTrueFibrosisState() == FibrosisState::kF4 ||
            this->GetTrueFibrosisState() == FibrosisState::kDecomp) {
            return true;
        }
        return false;
    }
    ////////////// CHECKS /////////////////
    ////////////// GETTERS ////////////////
    int GetID() const { return this->id; }

    int GetCurrentTimestep() const { return this->_current_time; }

    int GetAge() const { return this->age; }

    /// @brief Getter for the number of HCV infections experienced by
    /// PersonIMPL
    /// @return Number of HCV infections experienced by PersonIMPL
    int GetTimesHCVInfected() const { return this->hcv_details.times_infected; }

    /// @brief Get the running total of clearances for PersonIMPL
    int GetAcuteHCVClearances() const {
        return this->hcv_details.times_acute_cleared;
    };

    int GetWithdrawals(InfectionType it) const {
        return this->treatment_details.at(it).num_withdrawals;
    }

    int GetToxicReactions(InfectionType it) const {
        return this->treatment_details.at(it).num_toxic_reactions;
    }

    int GetCompletedTreatments(InfectionType it) const {
        return this->treatment_details.at(it).num_completed;
    }

    int GetRetreatments(InfectionType it) const {
        return this->treatment_details.at(it).num_retreatments;
    }

    int GetSVRs() const { return this->hcv_details.svrs; }

    /// @brief
    int GetNumberOfABTests(InfectionType it) const {
        return this->screening_details.at(it).num_ab_tests;
    }

    /// @brief
    int GetNumberOfRNATests(InfectionType it) const {
        return this->screening_details.at(it).num_rna_tests;
    }

    /// @brief Getter for the Time Since the Last Screening
    /// @return The Time Since the Last Screening
    int GetTimeOfLastScreening(InfectionType it) const {
        return this->screening_details.at(it).time_of_last_screening;
    }

    /// @brief Getter for the person's overdose state
    /// @return Boolean representing overdose or not
    bool GetCurrentlyOverdosing() const { return this->currently_overdosing; }

    int GetNumberOfOverdoses() const { return this->num_overdoses; }

    DeathReason GetDeathReason() const { return this->death_reason; }

    /// @brief Getter for the Fibrosis State
    /// @return The Current Fibrosis State
    FibrosisState GetTrueFibrosisState() const {
        return this->hcv_details.fibrosis_state;
    }

    /// @brief Getter for the HCV State
    /// @return HCV State
    HCV GetHCV() const { return this->hcv_details.hcv; }

    /// @brief Getter for Behavior Classification
    /// @return Behavior Classification
    Behavior GetBehavior() const { return this->behavior_details.behavior; }

    /// @brief Getter for time since active drug use
    /// @return Time since the person left an active drug use state
    int GetTimeBehaviorChange() const {
        return this->behavior_details.time_last_active;
    }

    /// @brief Getter for timestep in which HCV last changed
    /// @return Timestep that HCV last changed
    int GetTimeHCVChanged() const { return this->hcv_details.time_changed; }

    /// @brief Getter for timestep in which HIV last changed
    /// @return Timestep that HIV last changed
    int GetTimeHIVChanged() const { return this->hiv_details.time_changed; }

    /// @brief Getter for Time since Fibrosis State Change
    /// @return Time Since Fibrosis State Change
    int GetTimeTrueFibrosisStateChanged() const {
        return this->hcv_details.time_fibrosis_state_changed;
    }

    /// @brief Getter for Seropositive
    /// @return Seropositive
    bool GetSeropositivity() const { return this->hcv_details.seropositive; }

    int GetTimeHCVIdentified() const {
        return this->screening_details.at(InfectionType::kHcv).time_identified;
    }

    /// @brief Getter for Link State
    /// @return Link State
    LinkageState GetLinkState(InfectionType it) const {
        return this->linkage_details.at(it).link_state;
    }

    /// @brief Getter for Timestep in which linkage changed
    /// @return Time Link Change
    int GetTimeOfLinkChange(InfectionType it) const {
        return this->linkage_details.at(it).time_of_link_change;
    }

    /// @brief Getter for link count
    /// @return Number of times PersonIMPL has linked to care
    int GetLinkCount(InfectionType it) const {
        return this->linkage_details.at(it).link_count;
    }

    void SetLinkageType(LinkageType link_type, InfectionType it) {
        this->linkage_details[it].link_type = link_type;
    }

    /// @brief Getter for Linkage Type
    /// @return Linkage Type
    LinkageType GetLinkageType(InfectionType it) const {
        return this->linkage_details.at(it).link_type;
    }

    /// @brief Getter for the number of timesteps PersonIMPL has been in
    /// treatment
    /// @return Integer number of timesteps spent in treatment
    int GetTimeOfTreatmentInitiation(InfectionType it) const {
        return this->treatment_details.at(it).time_of_treatment_initiation;
    }

    /// @brief Getter for pregnancy status
    /// @return Pregnancy State
    PregnancyState GetPregnancyState() const {
        return this->pregnancy_details.pregnancy_state;
    }

    /// @brief Getter for timestep in which last pregnancy change happened
    /// @return Time pregnancy state last changed
    int GetTimeOfPregnancyChange() const {
        return this->pregnancy_details.time_of_pregnancy_change;
    }

    int GetTimeSincePregnancyChange() const {
        return CalculateTimeSince(GetTimeOfPregnancyChange());
    }

    int GetTimeSinceLastScreening(InfectionType it) const {
        return CalculateTimeSince(GetTimeOfLastScreening(it));
    }

    int GetTimeSinceHCVChanged() const {
        return CalculateTimeSince(GetTimeHCVChanged());
    }

    int GetTimeSinceHIVChanged() const {
        return CalculateTimeSince(GetTimeHIVChanged());
    }

    int GetTimeSinceLinkChange(InfectionType it) const {
        return CalculateTimeSince(GetTimeOfLinkChange(it));
    }

    int GetTimeSinceTreatmentInitiation(InfectionType it) const {
        return CalculateTimeSince(GetTimeOfTreatmentInitiation(it));
    }

    int GetTimeSinceFibrosisStaging() const {
        return CalculateTimeSince(GetTimeOfFibrosisStaging());
    }

    std::vector<Child> GetChildren() const {
        return this->pregnancy_details.children;
    }

    /// @brief Getter for number of miscarriages
    /// @return Number of miscarriages this person has experienced
    int GetNumMiscarriages() const {
        return this->pregnancy_details.num_miscarriages;
    }

    /// @brief Getter for timestep in which the last fibrosis
    /// staging test happened
    /// @return Timestep of person's last fibrosis staging
    int GetTimeOfFibrosisStaging() const {
        return this->staging_details.time_of_last_staging;
    }

    /// @brief Getter for MOUD State
    /// @return MOUD State
    MOUD GetMoudState() const { return this->moud_details.moud_state; }

    /// @brief Getter for measured fibrosis state
    /// @return Measured Fibrosis State
    MeasuredFibrosisState GetMeasuredFibrosisState() const {
        return this->staging_details.measured_fibrosis_state;
    }
    /// @brief Getter for timestep in which MOUD was started
    /// @return Time spent on MOUD
    int GetTimeStartedMoud() const { return this->moud_details.time_started; }

    /// @brief Get Person's undiscounted life span
    /// @return Undiscounted life span, in months
    int GetLifeSpan() const { return this->life_span; }

    /// @brief Get Person's discounted life span
    /// @return Discounted life span, in months
    double GetDiscountedLifeSpan() const { return this->discounted_life_span; }

    /// @brief Add discounted life span to person
    /// @param
    void AddDiscountedLifeSpan(double discounted_life) {
        this->discounted_life_span += discounted_life;
    }

    /// @brief Getter for the person's sex
    /// @return PersonIMPL's sex
    Sex GetSex() const { return this->sex; }

    /// @brief Getter for the person's stratified utilities
    /// @return PersonIMPL's stratified utilities
    LifetimeUtility GetTotalUtility() const { return this->lifetime_utility; }

    /// @brief Getter for the person's utilities broken down by category
    /// @return PersonIMPL's category-specific utilities
    std::unordered_map<utility::UtilityCategory, double>
    GetCurrentUtilities() const {
        return this->utility_tracker.GetRawUtilities();
    }

    /// @brief Getter for the PersonIMPL's costs
    /// @return cost::CostTracker containing this person's costs
    std::unordered_map<cost::CostCategory, std::pair<double, double>>
    GetCosts() const {
        return this->costs.GetCosts();
    }

    std::pair<double, double> GetCostTotals() const {
        return this->costs.GetTotals();
    }

    HCVDetails GetHCVDetails() const { return this->hcv_details; }
    HIVDetails GetHIVDetails() const { return this->hiv_details; }
    HCCDetails GetHCCDetails() const { return this->hcc_details; }

    BehaviorDetails GetBehaviorDetails() const {
        return this->behavior_details;
    }

    LinkageDetails GetLinkStatus(InfectionType it) const {
        return this->linkage_details.at(it);
    }

    MOUDDetails GetMOUDDetails() const { return this->moud_details; }

    HIV GetHIV() const { return this->hiv_details.hiv; }

    PregnancyDetails GetPregnancyDetails() const {
        return this->pregnancy_details;
    }

    StagingDetails GetFibrosisStagingDetails() const {
        return this->staging_details;
    }

    ScreeningDetails GetScreeningDetails(InfectionType it) const {
        return this->screening_details.at(it);
    }

    bool CheckAntibodyPositive(InfectionType it) const {
        return this->screening_details.at(it).antibody_positive;
    }

    void SetAntibodyPositive(bool result, InfectionType it) {
        this->screening_details[it].antibody_positive = result;
    }

    TreatmentDetails GetTreatmentDetails(InfectionType it) const {
        return this->treatment_details.at(it);
    }
    int GetNumberOfTreatmentStarts(InfectionType it) const {
        return this->treatment_details.at(it).num_starts;
    }

    std::string GetPersonDataString() const {
        std::stringstream data;
        data << GetAge() << "," << GetSex() << "," << GetBehavior() << ","
             << GetTimeBehaviorChange() << "," << GetSeropositivity() << ","
             << IsGenotypeThree() << "," << GetTrueFibrosisState() << ","
             << IsIdentifiedAsInfected(::kHcv) << "," << GetLinkState(::kHcv)
             << "," << IsAlive() << "," << GetDeathReason() << ","
             << GetTimeHCVIdentified() << "," << GetHCV() << ","
             << GetTimeHCVChanged() << "," << GetTimeTrueFibrosisStateChanged()
             << "," << GetTimeBehaviorChange() << ","
             << GetTimeOfLinkChange(::kHcv) << "," << GetLinkageType(::kHcv)
             << "," << GetLinkCount(::kHcv) << "," << GetMeasuredFibrosisState()
             << "," << GetTimeOfFibrosisStaging() << ","
             << GetTimeOfLastScreening(::kHcv) << ","
             << GetNumberOfABTests(::kHcv) << "," << GetNumberOfRNATests(::kHcv)
             << "," << GetTimesHCVInfected() << "," << GetAcuteHCVClearances()
             << "," << std::boolalpha
             << GetTreatmentDetails(::kHcv).initiated_treatment << ","
             << GetTimeOfTreatmentInitiation(::kHcv) << ","
             << std::to_string(GetTotalUtility().min_util) << ","
             << std::to_string(GetTotalUtility().mult_util);
        return data.str();
    }

    //////////////////// GETTERS ////////////////////
    //////////////////// SETTERS ////////////////////

    void SetAge(int age) { this->age = age; }

    /// @brief Set whether the person experienced fibtest two this cycle
    /// @param state New value of had_second_test
    void GiveSecondScreeningTest(bool state) {
        this->staging_details.had_second_test = state;
    }

    /// @brief Set the Seropositive value
    /// @param seropositive Seropositive status to set
    void SetSeropositivity(bool seropositive) {
        this->hcv_details.seropositive = seropositive;
    }

    void SetDeathReason(DeathReason death_reason) {
        this->death_reason = death_reason;
    }

    /// @brief Set HEPC State -- used to change to chronic infection
    /// @param New HEPC State
    void SetHCV(HCV hcv) {
        this->hcv_details.hcv = hcv;
        this->hcv_details.time_changed = this->_current_time;
    }

    /// @brief Set HIV infection state -- used to change between states of
    /// suppression and CD4 count
    /// @param New HIV infection state
    void SetHIV(HIV hiv) {
        if (hiv == this->hiv_details.hiv) {
            return;
        }
        this->hiv_details.hiv = hiv;
        this->hiv_details.time_changed = this->_current_time;
    }

    int GetLowCD4MonthCount() const {
        return this->hiv_details.low_cd4_months_count;
    }

    /// @brief Setter for PersonIMPL's treatment initiation state
    /// @param incompleteTreatment Boolean value for initiated treatment
    /// state to be set
    void InitiateTreatment(InfectionType it) {
        TreatmentDetails &td = this->treatment_details[it];
        // cannot continue being treated if already in retreatment
        if (td.initiated_treatment && td.retreatment) {
            return;
        } else if (td.initiated_treatment) {
            td.retreatment = true;
            td.num_retreatments++;
        } else {
            td.initiated_treatment = true;
        }
        // treatment starts counts treatment regimens
        td.num_starts++;
        td.time_of_treatment_initiation = this->_current_time;
    }

    /// @brief Setter for Pregnancy State
    /// @param state PersonIMPL's pregnancy State
    void SetPregnancyState(PregnancyState state) {
        this->pregnancy_details.time_of_pregnancy_change = this->_current_time;
        this->pregnancy_details.pregnancy_state = state;
    }

    /// @brief Add miscarriages to the count
    /// @param miscarriages Number of miscarriages to add
    void SetNumMiscarriages(int miscarriages) {
        this->pregnancy_details.num_miscarriages += miscarriages;
    }

    /// @brief Set PersonIMPL's measured fibrosis state
    /// @param state
    void UpdateTrueFibrosis(FibrosisState state) {
        this->hcv_details.time_fibrosis_state_changed = this->_current_time;
        this->hcv_details.fibrosis_state = state;
    }

    /// @brief Setter for whether PersonIMPL is genotype three
    /// @param genotype True if infection is genotype three, false
    /// otherwise
    void SetGenotypeThree(bool genotype) {
        this->hcv_details.is_genotype_three = genotype;
    }

    void AccumulateTotalUtility(std::pair<double, double> util,
                                std::pair<double, double> discount_util) {
        this->lifetime_utility.min_util += util.first;
        this->lifetime_utility.mult_util += util.second;
        this->lifetime_utility.discount_min_util += discount_util.first;
        this->lifetime_utility.discount_mult_util += discount_util.second;
    }

    /// @brief Get Person's utility values
    /// @return min, mult utility
    std::pair<double, double> GetUtility() const {
        return this->utility_tracker.GetUtilities();
    }

    /// @brief Set a value for a person's utility
    /// @param util The value of the utility to be updated, bounded by
    /// 0, 1
    /// @param category The category of the utility to be updated
    void SetUtility(double util, utility::UtilityCategory category) {
        this->utility_tracker.SetUtility(util, category);
    }

    void SetBoomer(bool status) { this->boomer_classification = status; }

    void DevelopHCC(HCCState state) {
        HCCState current = this->hcc_details.hcc_state;
        switch (current) {
        case HCCState::kNone:
            this->hcc_details.hcc_state = HCCState::kEarly;
            break;
        case HCCState::kEarly:
            this->hcc_details.hcc_state = HCCState::kLate;
            break;
        default:
            break;
        }
    }
    HCCState GetHCCState() const { return this->hcc_details.hcc_state; }
    void DiagnoseHCC() { hcc_details.hcc_diagnosed = true; }
    bool IsDiagnosedWithHCC() const { return hcc_details.hcc_diagnosed; }

    //////////////////// SETTERS ////////////////////
};

std::ostream &operator<<(std::ostream &os, const Person &person) {
    os << "sex: " << person.GetSex() << std::endl;
    os << "alive: " << std::boolalpha << person.IsAlive()
       << "from: " << person.GetDeathReason() << std::endl;
    os << person.GetHCVDetails() << std::endl;
    os << person.GetBehaviorDetails() << std::endl;
    os << person.GetLinkStatus(::kHcv) << std::endl;
    os << "Overdoses: " << person.GetNumberOfOverdoses() << std::endl;
    os << person.GetMOUDDetails() << std::endl;
    os << person.GetPregnancyDetails() << std::endl;
    os << person.GetFibrosisStagingDetails() << std::endl;
    os << person.GetScreeningDetails(::kHcv) << std::endl;
    os << person.GetTreatmentDetails(::kHcv) << std::endl;
    os << person.GetTotalUtility() << std::endl;
    // os << person.getCosts() << std::endl;
    os << "Is a Baby Boomer: " << std::boolalpha << person.IsBoomer()
       << std::endl;
    os << "Age: " << person.GetAge() << std::endl;
    return os;
};

/// Base Defined Pass Through Wrappers
Person::Person() {
    pImplPERSON = std::make_unique<person::Person::PersonIMPL>();
}

Person::~Person() = default;

Person::Person(Person &&p) noexcept = default;
Person &Person::operator=(Person &&) noexcept = default;

int Person::CreatePersonFromTable(int id,
                                  datamanagement::ModelData &model_data) {
    return pImplPERSON->CreatePersonFromTable(id, dm);
}

int Person::Grow() {
    pImplPERSON->Grow();
    return 0;
}
int Person::Die(DeathReason death_reason) {
    pImplPERSON->Die(death_reason);
    return 0;
}
int Person::InfectHCV() {
    pImplPERSON->InfectHCV();
    return 0;
}
int Person::ClearHCV(bool acute) {
    pImplPERSON->ClearHCV(acute);
    return 0;
}
int Person::SetBehavior(Behavior bc) {
    pImplPERSON->SetBehavior(bc);
    return 0;
}
int Person::DiagnoseFibrosis(MeasuredFibrosisState data) {
    data = pImplPERSON->DiagnoseFibrosis(data);
    return 0;
}
int Person::LoadICValues(int id, std::vector<std::string> icValues) {
    pImplPERSON->LoadICValues(id, icValues);
    return 0;
}
int Person::AddAcuteHCVClearance() {
    pImplPERSON->AddAcuteHCVClearance();
    return 0;
}
int Person::AddWithdrawal(InfectionType it) {
    pImplPERSON->AddWithdrawal(it);
    return 0;
}
int Person::AddToxicReaction(InfectionType it) {
    pImplPERSON->AddToxicReaction(it);
    return 0;
}
int Person::AddCompletedTreatment(InfectionType it) {
    pImplPERSON->AddCompletedTreatment(it);
    return 0;
}
int Person::AddSVR() {
    pImplPERSON->AddSVR();
    return 0;
}
int Person::MarkScreened(InfectionType it) {
    pImplPERSON->MarkScreened(it);
    return 0;
}
int Person::AddAbScreen(InfectionType it) {
    pImplPERSON->AddAbScreen(it);
    return 0;
}
int Person::AddRnaScreen(InfectionType it) {
    pImplPERSON->AddRnaScreen(it);
    return 0;
}
int Person::Unlink(InfectionType it) {
    pImplPERSON->Unlink(it);
    return 0;
}
int Person::Link(LinkageType link_type, InfectionType it) {
    pImplPERSON->Link(link_type, it);
    return 0;
}
int Person::EndTreatment(InfectionType it) {
    pImplPERSON->EndTreatment(it);
    return 0;
}
void Person::ClearDiagnosis(InfectionType it) {
    return pImplPERSON->ClearDiagnosis(it);
}
int Person::AddCost(double base_cost, double discount_cost,
                    cost::CostCategory category) {
    pImplPERSON->AddCost(base_cost, discount_cost, category);
    return 0;
}
int Person::ToggleOverdose() {
    pImplPERSON->ToggleOverdose();
    return 0;
}
int Person::Miscarry() {
    pImplPERSON->Miscarry();
    return 0;
}
int Person::EndPostpartum() {
    pImplPERSON->EndPostpartum();
    return 0;
}
int Person::Impregnate() {
    pImplPERSON->Impregnate();
    return 0;
}
int Person::Stillbirth() {
    pImplPERSON->Stillbirth();
    return 0;
}
int Person::AddChild(HCV hcv, bool test) {
    pImplPERSON->AddChild(hcv, test);
    return 0;
}
// Checks
bool Person::IsAlive() const { return pImplPERSON->IsAlive(); }
bool Person::HadSecondScreeningTest() const {
    return pImplPERSON->HadSecondScreeningTest();
}
bool Person::IsIdentifiedAsInfected(InfectionType it) const {
    return pImplPERSON->IsIdentifiedAsInfected(it);
}
bool Person::HasInitiatedTreatment(InfectionType it) const {
    return pImplPERSON->HasInitiatedTreatment(it);
}
bool Person::IsInRetreatment(InfectionType it) const {
    return pImplPERSON->IsInRetreatment(it);
}
bool Person::IsGenotypeThree() const { return pImplPERSON->IsGenotypeThree(); }
bool Person::IsBoomer() const { return pImplPERSON->IsBoomer(); }
bool Person::IsCirrhotic() { return pImplPERSON->IsCirrhotic(); }
// Getters
int Person::GetID() const { return pImplPERSON->GetID(); }
int Person::GetCurrentTimestep() const {
    return pImplPERSON->GetCurrentTimestep();
}
int Person::GetAge() const { return pImplPERSON->GetAge(); }
int Person::GetTimesHCVInfected() const {
    return pImplPERSON->GetTimesHCVInfected();
}
int Person::GetAcuteHCVClearances() const {
    return pImplPERSON->GetAcuteHCVClearances();
}
int Person::GetWithdrawals(InfectionType it) const {
    return pImplPERSON->GetWithdrawals(it);
}
int Person::GetToxicReactions(InfectionType it) const {
    return pImplPERSON->GetToxicReactions(it);
}
int Person::GetCompletedTreatments(InfectionType it) const {
    return pImplPERSON->GetCompletedTreatments(it);
}
int Person::GetRetreatments(InfectionType it) const {
    return pImplPERSON->GetRetreatments(it);
}
int Person::GetSVRs() const { return pImplPERSON->GetSVRs(); }
int Person::GetNumberOfABTests(InfectionType it) const {
    return pImplPERSON->GetNumberOfABTests(it);
}
int Person::GetNumberOfRNATests(InfectionType it) const {
    return pImplPERSON->GetNumberOfRNATests(it);
}
int Person::GetTimeOfLastScreening(InfectionType it) const {
    return pImplPERSON->GetTimeOfLastScreening(it);
}
int Person::GetTimeSinceLastScreening(InfectionType it) const {
    return pImplPERSON->GetTimeSinceLastScreening(it);
}
bool Person::GetCurrentlyOverdosing() const {
    return pImplPERSON->GetCurrentlyOverdosing();
}
int Person::GetNumberOfOverdoses() const {
    return pImplPERSON->GetNumberOfOverdoses();
}
FibrosisState Person::GetTrueFibrosisState() const {
    return pImplPERSON->GetTrueFibrosisState();
}
DeathReason Person::GetDeathReason() const {
    return pImplPERSON->GetDeathReason();
}
HCV Person::GetHCV() const { return pImplPERSON->GetHCV(); }
Behavior Person::GetBehavior() const { return pImplPERSON->GetBehavior(); }
int Person::GetTimeBehaviorChange() const {
    return pImplPERSON->GetTimeBehaviorChange();
}
int Person::GetTimeHCVChanged() const {
    return pImplPERSON->GetTimeHCVChanged();
}
int Person::GetTimeSinceHCVChanged() const {
    return pImplPERSON->GetTimeSinceHCVChanged();
}
int Person::GetTimeTrueFibrosisStateChanged() const {
    return pImplPERSON->GetTimeTrueFibrosisStateChanged();
}
bool Person::GetSeropositivity() const {
    return pImplPERSON->GetSeropositivity();
}
int Person::GetTimeHCVIdentified() const {
    return pImplPERSON->GetTimeHCVIdentified();
}
LinkageState Person::GetLinkState(InfectionType it) const {
    return pImplPERSON->GetLinkState(it);
}
int Person::GetTimeOfLinkChange(InfectionType it) const {
    return pImplPERSON->GetTimeOfLinkChange(it);
}
int Person::GetTimeSinceLinkChange(InfectionType it) const {
    return pImplPERSON->GetTimeSinceLinkChange(it);
}
int Person::GetLinkCount(InfectionType it) const {
    return pImplPERSON->GetLinkCount(it);
}
void Person::SetLinkageType(LinkageType link_type, InfectionType it) {
    return pImplPERSON->SetLinkageType(link_type, it);
}
LinkageType Person::GetLinkageType(InfectionType it) const {
    return pImplPERSON->GetLinkageType(it);
}
int Person::GetTimeOfTreatmentInitiation(InfectionType it) const {
    return pImplPERSON->GetTimeOfTreatmentInitiation(it);
}
int Person::GetTimeSinceTreatmentInitiation(InfectionType it) const {
    return pImplPERSON->GetTimeSinceTreatmentInitiation(it);
}
PregnancyState Person::GetPregnancyState() const {
    return pImplPERSON->GetPregnancyState();
}
int Person::GetTimeOfPregnancyChange() const {
    return pImplPERSON->GetTimeOfPregnancyChange();
}
int Person::GetTimeSincePregnancyChange() const {
    return pImplPERSON->GetTimeSincePregnancyChange();
}
std::vector<Child> Person::GetChildren() const {
    return pImplPERSON->GetChildren();
}
int Person::GetNumMiscarriages() const {
    return pImplPERSON->GetNumMiscarriages();
}
int Person::GetTimeOfFibrosisStaging() const {
    return pImplPERSON->GetTimeOfFibrosisStaging();
}
int Person::GetTimeSinceFibrosisStaging() const {
    return pImplPERSON->GetTimeSinceFibrosisStaging();
}
MOUD Person::GetMoudState() const { return pImplPERSON->GetMoudState(); }
MeasuredFibrosisState Person::GetMeasuredFibrosisState() const {
    return pImplPERSON->GetMeasuredFibrosisState();
}
int Person::GetTimeStartedMoud() const {
    return pImplPERSON->GetTimeStartedMoud();
}
Sex Person::GetSex() const { return pImplPERSON->GetSex(); }
LifetimeUtility Person::GetTotalUtility() const {
    return pImplPERSON->GetTotalUtility();
}
std::unordered_map<cost::CostCategory, std::pair<double, double>>
Person::GetCosts() const {
    return pImplPERSON->GetCosts();
}
std::pair<double, double> Person::GetCostTotals() const {
    return pImplPERSON->GetCostTotals();
}
HCVDetails Person::GetHCVDetails() const {
    return pImplPERSON->GetHCVDetails();
}
HIVDetails Person::GetHIVDetails() const {
    return pImplPERSON->GetHIVDetails();
}
HCCDetails Person::GetHCCDetails() const {
    return pImplPERSON->GetHCCDetails();
}
BehaviorDetails Person::GetBehaviorDetails() const {
    return pImplPERSON->GetBehaviorDetails();
}
int Person::GetLifeSpan() const { return pImplPERSON->GetLifeSpan(); }
double Person::GetDiscountedLifeSpan() const {
    return pImplPERSON->GetDiscountedLifeSpan();
}
void Person::AddDiscountedLifeSpan(double discounted_life) {
    pImplPERSON->AddDiscountedLifeSpan(discounted_life);
}
LinkageDetails Person::GetLinkStatus(InfectionType it) const {
    return pImplPERSON->GetLinkStatus(it);
}
MOUDDetails Person::GetMOUDDetails() const {
    return pImplPERSON->GetMOUDDetails();
}
PregnancyDetails Person::GetPregnancyDetails() const {
    return pImplPERSON->GetPregnancyDetails();
}
StagingDetails Person::GetFibrosisStagingDetails() const {
    return pImplPERSON->GetFibrosisStagingDetails();
}
ScreeningDetails Person::GetScreeningDetails(InfectionType it) const {
    return pImplPERSON->GetScreeningDetails(it);
}
TreatmentDetails Person::GetTreatmentDetails(InfectionType it) const {
    return pImplPERSON->GetTreatmentDetails(it);
}
int Person::GetNumberOfTreatmentStarts(InfectionType it) const {
    return pImplPERSON->GetNumberOfTreatmentStarts(it);
}
std::string Person::GetPersonDataString() const {
    return pImplPERSON->GetPersonDataString();
}
// Setters
void Person::SetDeathReason(DeathReason death_reason) {
    pImplPERSON->SetDeathReason(death_reason);
}
void Person::SetAge(int age) { pImplPERSON->SetAge(age); }
void Person::GiveSecondScreeningTest(bool state) {
    pImplPERSON->GiveSecondScreeningTest(state);
}
void Person::Diagnose(InfectionType it) { pImplPERSON->Diagnose(it); }
bool Person::CheckAntibodyPositive(InfectionType it) const {
    return pImplPERSON->CheckAntibodyPositive(it);
}
void Person::SetAntibodyPositive(bool result, InfectionType it) {
    pImplPERSON->SetAntibodyPositive(result, it);
}
void Person::SetSeropositivity(bool seropositive) {
    pImplPERSON->SetSeropositivity(seropositive);
}
void Person::SetHCV(HCV hcv) { pImplPERSON->SetHCV(hcv); }
void Person::InitiateTreatment(InfectionType it) {
    pImplPERSON->InitiateTreatment(it);
}
void Person::SetPregnancyState(PregnancyState state) {
    pImplPERSON->SetPregnancyState(state);
}
void Person::AddInfantExposure() { pImplPERSON->AddInfantExposure(); }
int Person::GetInfantHCVExposures() const {
    return pImplPERSON->GetInfantHCVExposures();
}
int Person::GetInfantHCVInfections() const {
    return pImplPERSON->GetInfantHCVInfections();
}
int Person::GetInfantHCVTests() const {
    return pImplPERSON->GetInfantHCVTests();
}
int Person::GetPregnancyCount() const {
    return pImplPERSON->GetPregnancyCount();
}
void Person::SetNumMiscarriages(int miscarriages) {
    pImplPERSON->SetNumMiscarriages(miscarriages);
}
void Person::UpdateTrueFibrosis(FibrosisState state) {
    pImplPERSON->UpdateTrueFibrosis(state);
}
void Person::SetGenotypeThree(bool genotype) {
    pImplPERSON->SetGenotypeThree(genotype);
}
void Person::TransitionMOUD() { pImplPERSON->TransitionMOUD(); }
void Person::AccumulateTotalUtility(std::pair<double, double> util,
                                    std::pair<double, double> discount_util) {
    pImplPERSON->AccumulateTotalUtility(util, discount_util);
}
std::pair<double, double> Person::GetUtility() const {
    return pImplPERSON->GetUtility();
}
void Person::SetUtility(double util, utility::UtilityCategory category) {
    pImplPERSON->SetUtility(util, category);
}
std::unordered_map<utility::UtilityCategory, double>
Person::GetCurrentUtilities() const {
    return pImplPERSON->GetCurrentUtilities();
}
void Person::SetBoomer(bool status) { pImplPERSON->SetBoomer(status); }
void Person::DevelopHCC(HCCState state) { pImplPERSON->DevelopHCC(state); }
HCCState Person::GetHCCState() const { return pImplPERSON->GetHCCState(); }
void Person::DiagnoseHCC() { pImplPERSON->DiagnoseHCC(); }
bool Person::IsDiagnosedWithHCC() const {
    return pImplPERSON->IsDiagnosedWithHCC();
}

// HIV
HIV Person::GetHIV() const { return pImplPERSON->GetHIV(); }
void Person::SetHIV(HIV hiv) { return pImplPERSON->SetHIV(hiv); }
int Person::GetLowCD4MonthCount() const {
    return pImplPERSON->GetLowCD4MonthCount();
}
void Person::InfectHIV() { return pImplPERSON->InfectHIV(); }
int Person::GetTimeHIVChanged() const {
    return pImplPERSON->GetTimeHIVChanged();
}
int Person::GetTimeSinceHIVChanged() const {
    return pImplPERSON->GetTimeSinceHIVChanged();
}

// Person Output
std::string Person::MakePopulationRow() const {
    return pImplPERSON->MakePopulationRow();
}
} // namespace person
