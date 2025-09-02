////////////////////////////////////////////////////////////////////////////////
// File: simulation_internals.hpp                                             //
// Project: hep-ce                                                            //
// Created Date: 2025-07-25                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-09-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_SIMULATIONINTERNALS_HPP_
#define HEPCE_MODEL_SIMULATIONINTERNALS_HPP_

#include <hepce/model/simulation.hpp>

#include <string>
#include <vector>

namespace hepce {
namespace model {
class HepceImpl : public virtual Hepce {
public:
    HepceImpl(datamanagement::ModelData &model_data,
              const std::string &log_name);
    ~HepceImpl() = default;
    void Run(std::vector<std::unique_ptr<model::Person>> &people,
             const std::vector<std::unique_ptr<event::Event>> &discrete_events)
        override;
    std::vector<std::unique_ptr<event::Event>>
    CreateEvents(datamanagement::ModelData &model_data) const override;
    std::vector<std::unique_ptr<model::Person>>
    CreatePopulation(datamanagement::ModelData &model_data) const override;

    int GetDuration() const override { return _duration; }
    int GetSeed() const override { return _sim_seed; }

private:
    const std::string _log_name;
    int _duration;
    int _sim_seed;

    const std::string GetLogName() const { return _log_name; }

    std::vector<std::unique_ptr<model::Person>>
    ReadICPopulation(const int population_size,
                     datamanagement::ModelData &model_data) const;

    std::vector<std::unique_ptr<model::Person>>
    ReadPopPopulation(const int population_size,
                      datamanagement::ModelData &model_data) const;

    std::unique_ptr<event::Event>
    CreateEvent(std::string event_name,
                datamanagement::ModelData &model_data) const;

    inline std::string InitialCohortSQL(int N) const {
        std::stringstream ss;
        ss << "SELECT age_months, gender, drug_behavior, "
              "time_last_active_drug_use, seropositivity, genotype_three, "
              "fibrosis_state, identified_as_hcv_positive, link_state, "
              "hcv_status, pregnancy_state ";
        ss << "FROM init_cohort ";
        ss << "ORDER BY id ";
        ss << "LIMIT " << std::to_string(N) << ";";
        return ss.str();
    }

    static inline void PersonVecCallback(std::any &storage,
                                         const SQLite::Statement &stmt) {
        std::vector<data::PersonSelect> *temp_vec =
            std::any_cast<std::vector<data::PersonSelect>>(&storage);
        data::PersonSelect temp;
        temp.sex = static_cast<data::Sex>(stmt.getColumn(0).getInt());
        temp.age = stmt.getColumn(1).getInt();
        temp.is_alive = stmt.getColumn(2).getInt();
        temp.boomer_classification = stmt.getColumn(3).getInt();
        temp.death_reason =
            static_cast<data::DeathReason>(stmt.getColumn(4).getInt());
        temp.drug_behavior =
            static_cast<data::Behavior>(stmt.getColumn(5).getInt());
        temp.time_last_active_drug_use = stmt.getColumn(6).getInt();
        temp.hcv = static_cast<data::HCV>(stmt.getColumn(7).getInt());
        temp.fibrosis_state =
            static_cast<data::FibrosisState>(stmt.getColumn(8).getInt());
        temp.is_genotype_three = stmt.getColumn(9).getInt();
        temp.seropositive = stmt.getColumn(10).getInt();
        temp.time_hcv_changed = stmt.getColumn(11).getInt();
        temp.time_fibrosis_state_changed = stmt.getColumn(12).getInt();
        temp.times_hcv_infected = stmt.getColumn(13).getInt();
        temp.times_acute_cleared = stmt.getColumn(14).getInt();
        temp.svrs = stmt.getColumn(15).getInt();
        temp.hiv = static_cast<data::HIV>(stmt.getColumn(16).getInt());
        temp.time_hiv_changed = stmt.getColumn(17).getInt();
        temp.low_cd4_months_count = stmt.getColumn(18).getInt();
        temp.hcc_state =
            static_cast<data::HCCState>(stmt.getColumn(19).getInt());
        temp.hcc_diagnosed = stmt.getColumn(20).getInt();
        temp.currently_overdosing = stmt.getColumn(21).getInt();
        temp.num_overdoses = stmt.getColumn(22).getInt();
        temp.moud_state = static_cast<data::MOUD>(stmt.getColumn(23).getInt());
        temp.time_started_moud = stmt.getColumn(24).getInt();
        temp.current_moud_concurrent_months = stmt.getColumn(25).getInt();
        temp.total_moud_months = stmt.getColumn(26).getInt();
        temp.pregnancy_state =
            static_cast<data::PregnancyState>(stmt.getColumn(27).getInt());
        temp.time_of_pregnancy_change = stmt.getColumn(28).getInt();
        temp.pregnancy_count = stmt.getColumn(29).getInt();
        temp.num_infants = stmt.getColumn(30).getInt();
        temp.num_stillbirths = stmt.getColumn(31).getInt();
        temp.num_infant_hcv_exposures = stmt.getColumn(32).getInt();
        temp.num_infant_hcv_infections = stmt.getColumn(33).getInt();
        temp.num_infant_hcv_tests = stmt.getColumn(34).getInt();
        temp.measured_fibrosis_state = static_cast<data::MeasuredFibrosisState>(
            stmt.getColumn(35).getInt());
        temp.had_second_test = stmt.getColumn(36).getInt();
        temp.time_of_last_staging = stmt.getColumn(37).getInt();
        temp.hcv_link_state =
            static_cast<data::LinkageState>(stmt.getColumn(38).getInt());
        temp.time_of_hcv_link_change = stmt.getColumn(39).getInt();
        temp.hcv_link_type =
            static_cast<data::ScreeningType>(stmt.getColumn(40).getInt());
        temp.hcv_link_count = stmt.getColumn(41).getInt();
        temp.hiv_link_state =
            static_cast<data::LinkageState>(stmt.getColumn(42).getInt());
        temp.time_of_hiv_link_change = stmt.getColumn(43).getInt();
        temp.hiv_link_type =
            static_cast<data::ScreeningType>(stmt.getColumn(44).getInt());
        temp.hiv_link_count = stmt.getColumn(45).getInt();
        temp.time_of_last_hcv_screening = stmt.getColumn(46).getInt();
        temp.num_hcv_ab_tests = stmt.getColumn(47).getInt();
        temp.num_hcv_rna_tests = stmt.getColumn(48).getInt();
        temp.hcv_antibody_positive = stmt.getColumn(49).getInt();
        temp.hcv_identified = stmt.getColumn(50).getInt();
        temp.time_hcv_identified = stmt.getColumn(51).getInt();
        temp.time_of_last_hiv_screening = stmt.getColumn(52).getInt();
        temp.num_hiv_ab_tests = stmt.getColumn(53).getInt();
        temp.num_hiv_rna_tests = stmt.getColumn(54).getInt();
        temp.hiv_antibody_positive = stmt.getColumn(55).getInt();
        temp.hiv_identified = stmt.getColumn(56).getInt();
        temp.time_hiv_identified = stmt.getColumn(57).getInt();
        temp.initiated_hcv_treatment = stmt.getColumn(58).getInt();
        temp.time_of_hcv_treatment_initiation = stmt.getColumn(59).getInt();
        temp.num_hcv_treatment_starts = stmt.getColumn(60).getInt();
        temp.num_hcv_treatment_withdrawals = stmt.getColumn(61).getInt();
        temp.num_hcv_treatment_toxic_reactions = stmt.getColumn(62).getInt();
        temp.num_completed_hcv_treatments = stmt.getColumn(63).getInt();
        temp.num_hcv_salvages = stmt.getColumn(64).getInt();
        temp.in_hcv_salvage = stmt.getColumn(65).getInt();
        temp.initiated_hiv_treatment = stmt.getColumn(66).getInt();
        temp.time_of_hiv_treatment_initiation = stmt.getColumn(67).getInt();
        temp.num_hiv_treatment_starts = stmt.getColumn(68).getInt();
        temp.num_hiv_treatment_withdrawals = stmt.getColumn(69).getInt();
        temp.num_hiv_treatment_toxic_reactions = stmt.getColumn(70).getInt();
        temp.behavior_utility = stmt.getColumn(71).getDouble();
        temp.liver_utility = stmt.getColumn(72).getDouble();
        temp.treatment_utility = stmt.getColumn(73).getDouble();
        temp.background_utility = stmt.getColumn(74).getDouble();
        temp.hiv_utility = stmt.getColumn(75).getDouble();
        temp_vec->emplace_back(temp);
    }

    static inline void InitCohortVecCallback(std::any &storage,
                                             const SQLite::Statement &stmt) {
        std::vector<data::PersonSelect> *temp_vec =
            std::any_cast<std::vector<data::PersonSelect>>(&storage);
        data::PersonSelect temp;
        temp.age = stmt.getColumn(0).getInt();
        temp.sex = static_cast<data::Sex>(stmt.getColumn(1).getInt());
        temp.drug_behavior =
            static_cast<data::Behavior>(stmt.getColumn(2).getInt());
        temp.time_last_active_drug_use = stmt.getColumn(3).getInt();
        temp.seropositive = stmt.getColumn(4).getInt();
        temp.is_genotype_three = stmt.getColumn(5).getInt();
        temp.fibrosis_state =
            static_cast<data::FibrosisState>(stmt.getColumn(6).getInt());
        temp.hcv_identified = stmt.getColumn(7).getInt();
        temp.hcv_link_state =
            static_cast<data::LinkageState>(stmt.getColumn(8).getInt());
        temp.hcv = static_cast<data::HCV>(stmt.getColumn(9).getInt());
        temp.pregnancy_state =
            static_cast<data::PregnancyState>(stmt.getColumn(10).getInt());
        temp_vec->emplace_back(temp);
    }
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SIMULATIONINTERNALS_HPP_