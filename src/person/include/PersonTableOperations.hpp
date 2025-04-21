////////////////////////////////////////////////////////////////////////////////
// File: PersonTableOperations.hpp                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-21                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-21                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef PERSON_PERSONTABLEOPERATIONS_HPP_
#define PERSON_PERSONTABLEOPERATIONS_HPP_

#include "Containers.hpp"
#include "Utils.hpp"
#include <string>

namespace person {
struct person_select {
    Sex sex = Sex::MALE;
    int age = 0;
    bool is_alive = true;
    DeathReason deathReason = DeathReason::NA;
    int identifiedHCV = false;
    int timeInfectionIdentified = -1;
    HCV hcv = HCV::NONE;
    FibrosisState fibrosisState = FibrosisState::NONE;
    bool isGenotypeThree = false;
    bool seropositive = false;
    int timeHCVChanged = -1;
    int timeFibrosisStateChanged = -1;
    Behavior drugBehavior = Behavior::NEVER;
    int timeLastActiveDrugUse = -1;
    LinkageState linkageState = LinkageState::NEVER;
    int timeOfLinkChange = -1;
    LinkageType linkageType = LinkageType::INTERVENTION;
    int linkCount = 0;
    MeasuredFibrosisState measuredFibrosisState = MeasuredFibrosisState::NONE;
    int timeOfLastStaging = -1;
    int timeOfLastScreening = -1;
    int numABTests = 0;
    int numRNATests = 0;
    int timesInfected = 0;
    int timesAcuteCleared = 0;
    bool initiatedTreatment = false;
    int timeOfTreatmentInitiation = -1;
    double min_utility = 0.0;
    double mult_utility = 0.0;
    double discount_min_utility = 0.0;
    double discount_mult_utility = 0.0;
    double behaviorUtility = 1.0;
    double liverUtility = 1.0;
    double treatmentUtility = 1.0;
    double backgroundUtility = 1.0;
    double hivUtility = 1.0;
    int treatmentWithdrawals = 0;
    int treatmentToxicReactions = 0;
    int completedTreatments = 0;
    int numberOfTreatmentStarts = 0;
    bool retreatment = false;
    int svrs = 0;
};

static inline int person_callback(void *storage, int count, char **data,
                                  char **columns) {
    struct person_select *temp = (struct person_select *)storage;
    temp->sex << data[0];
    temp->age = std::stoi(data[1]);
    temp->is_alive = std::stoi(data[2]);
    temp->deathReason << data[3];
    temp->identifiedHCV = std::stoi(data[4]);
    temp->timeInfectionIdentified = std::stoi(data[5]);
    temp->hcv << data[6];
    temp->fibrosisState << data[7];
    temp->isGenotypeThree = std::stoi(data[8]);
    temp->seropositive = std::stoi(data[9]);
    temp->timeHCVChanged = std::stoi(data[10]);
    temp->timeFibrosisStateChanged = std::stoi(data[11]);
    temp->drugBehavior << data[12];
    temp->timeLastActiveDrugUse = std::stoi(data[13]);
    temp->linkageState << data[14];
    temp->timeOfLinkChange = std::stoi(data[15]);
    temp->linkageType << data[16];
    temp->linkCount = std::stoi(data[17]);
    temp->measuredFibrosisState << data[18];
    temp->timeOfLastStaging = std::stoi(data[19]);
    temp->timeOfLastScreening = std::stoi(data[20]);
    temp->numABTests = std::stoi(data[21]);
    temp->numRNATests = std::stoi(data[22]);
    temp->timesInfected = std::stoi(data[23]);
    temp->timesAcuteCleared = std::stoi(data[24]);
    temp->initiatedTreatment = std::stoi(data[25]);
    temp->timeOfTreatmentInitiation = std::stoi(data[26]);
    temp->min_utility = Utils::stod_positive(data[27]);
    temp->mult_utility = Utils::stod_positive(data[28]);
    temp->discount_min_utility = Utils::stod_positive(data[29]);
    temp->discount_mult_utility = Utils::stod_positive(data[30]);
    temp->treatmentWithdrawals = std::stoi(data[31]);
    temp->treatmentToxicReactions = std::stoi(data[32]);
    temp->completedTreatments = std::stoi(data[33]);
    temp->svrs = std::stoi(data[34]);
    temp->behaviorUtility = Utils::stod_positive(data[35]);
    temp->liverUtility = Utils::stod_positive(data[36]);
    temp->treatmentUtility = Utils::stod_positive(data[37]);
    temp->backgroundUtility = Utils::stod_positive(data[38]);
    temp->hivUtility = Utils::stod_positive(data[39]);
    return 0;
}
} // namespace person
#endif
