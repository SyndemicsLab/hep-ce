//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the DataWriter Class.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "DataWriter.hpp"

namespace DataWriter {
    void
    writePopulation(std::vector<std::shared_ptr<Person::Person>> &population,
                    std::string dirpath) {
        Data::IDataTablePtr newDT = std::make_shared<Data::DataTable>();
        for (int i = 0; i < population.size(); ++i) {
            if (newDT->empty()) {
                newDT = personToDataTable(population[i]);
            } else {
                newDT = (*newDT) + (*personToDataTable(population[i]));
            }
        }
        std::filesystem::path f =
            ((std::filesystem::path)dirpath) / "population.csv";

        newDT->toCSV(f.string());
    }

    Data::IDataTablePtr
    personToDataTable(std::shared_ptr<Person::Person> person) {
        std::map<std::string, std::vector<std::string>> data;
        std::vector<std::string> headerOrder = {"id",
                                                "sex",
                                                "age",
                                                "isAlive",
                                                "identifiedAsPositiveInfection",
                                                "timeInfectionIdentified",
                                                "HEPCState",
                                                "fibrosisState",
                                                "isGenotypeThree",
                                                "seropositivity",
                                                "timeHEPCStateChanged",
                                                "timeFibrosisStateChanged",
                                                "numInfections",
                                                "numClearances",
                                                "drugBehaviorClassification",
                                                "timeLastActiveDrugUse",
                                                "linkageState",
                                                "timeOfLinkChange",
                                                "linkageType",
                                                "linkCount",
                                                "measuredFibrosisState",
                                                "timeOfLastStaging",
                                                "timeOfLastScreening",
                                                "abCount",
                                                "rnaCount",
                                                "hasIncompleteTreatment",
                                                "initiatedTreatment",
                                                "timeOfTreatmentInitiation",
                                                "treatmentCount",
                                                "numEOT",
                                                "numSVR",
                                                "numTox",
                                                "numWithdrawals",
                                                "backgroundUtility",
                                                "behaviorUtility",
                                                "treatmentUtility",
                                                "liverUtility"};
        data["id"] = {std::to_string(person->getID())};
        data["sex"] = {person->sexEnumToStringMap[person->getSex()]};
        data["age"] = {std::to_string(person->age)};
        data["isAlive"] = {Utils::boolToString(person->getIsAlive())};
        data["identifiedAsPositiveInfection"] = {
            Utils::boolToString(person->isIdentifiedAsInfected())};
        data["timeInfectionIdentified"] = {
            std::to_string(person->getTimeIdentified())};
        data["HEPCState"] = {
            person->hepcStateEnumToStringMap[person->getHEPCState()]};
        data["fibrosisState"] = {
            person->fibrosisStateEnumToStringMap[person->getFibrosisState()]};
        data["isGenotypeThree"] = {Utils::boolToString(person->getGenotype())};
        data["seropositivity"] = {
            Utils::boolToString(person->getSeropositivity())};
        data["timeHEPCStateChanged"] = {
            std::to_string(person->getTimeHEPCStateChanged())};
        data["timeFibrosisStateChanged"] = {
            std::to_string(person->getTimeFibrosisStateChanged())};
        data["numInfections"] = {std::to_string(person->getNumInfections())};
        data["numClearances"] = {std::to_string(person->getClearances())};
        data["drugBehaviorClassification"] = {
            person->behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()]};
        data["timeLastActiveDrugUse"] = {
            std::to_string(person->getTimeBehaviorChange())};
        data["linkageState"] = {
            person->linkageStateEnumToStringMap[person->getLinkState()]};
        data["timeOfLinkChange"] = {
            std::to_string(person->getTimeOfLinkChange())};
        data["linkageType"] = {
            person->linkageTypeEnumToStringMap[person->getLinkageType()]};
        data["linkCount"] = {std::to_string(person->getLinkCount())};
        data["measuredFibrosisState"] = {
            person->measuredFibrosisStateEnumToStringMap
                [person->getMeasuredFibrosisState()]};
        data["hadFibTestTwo"] = {Utils::boolToString(person->hadFibTestTwo())};
        data["timeOfLastStaging"] = {
            std::to_string(person->getTimeOfLastStaging())};
        data["timeOfLastScreening"] = {
            std::to_string(person->getTimeOfLastScreening())};
        data["abCount"] = {std::to_string(person->getAbCount())};
        data["rnaCount"] = {std::to_string(person->getRnaCount())};
        data["initiatedTreatment"] = {
            Utils::boolToString(person->hasInitiatedTreatment())};
        data["timeOfTreatmentInitiation"] = {
            std::to_string(person->getTimeOfTreatmentInitiation())};
        // auto utility = person->getUtility();
        // data["backgroundUtility"] = {std::to_string(utility.background)};
        // data["behaviorUtility"] = {std::to_string(utility.behavior)};
        // data["treatmentUtility"] = {std::to_string(utility.treatment)};
        // data["liverUtility"] = {std::to_string(utility.liver)};

        Data::DataTableShape newShape(1, 37);
        Data::IDataTablePtr newDT =
            std::make_shared<Data::DataTable>(data, newShape, headerOrder);
        return newDT;
    }

    void
    writeGeneralStats(std::vector<std::shared_ptr<Person::Person>> &population,
                      std::string dirpath, Data::Config &config) {
        // discount fraction
        double discountRate;
        std::shared_ptr<Data::ReturnType> dr =
            config.get_optional("cost.discounting_rate", 0.03);
        if (dr) {
            discountRate = std::get<double>(*dr);
        } else {
            discountRate = 0.03;
        }

        // non-tabular values
        // cohort size
        int cohortSize = (int)population.size();
        // avg cost per person
        double totalCost = 0;
        double discountedTotal = 0;
        for (auto person : population) {
            Cost::CostTracker ct = person->getCosts();
            const auto &totals = ct.getTotals();
            const auto &discTotals = ct.getTotals(discountRate);
            std::for_each(
                totals.begin(), totals.end(),
                [&](std::pair<int, double> tot) { totalCost += tot.second; });
            std::for_each(discTotals.begin(), discTotals.end(),
                          [&](std::pair<int, double> tot) {
                              discountedTotal += tot.second;
                          });
        }
        std::cout << cohortSize << std::endl;
        std::cout << totalCost << std::endl;
        std::cout << discountedTotal << std::endl;
        // avg discounted cost per person
        // avg life span per person
        // avg discounted life span per person
        // avg qaly per person (min)
        // avg discounted qaly per person (min)
        // avg qaly per person (mult)
        // avg discounted qaly per person (mult)
        // num treatment inititations
        // num svr cases
        // num eot (includes svr)
        // num withdrawals (non-toxicity)
        // num toxicity cases
        // num cirrhotic (including dead)
        // num liver-related deaths
        // num hcv identifications
        // num hcv infections
        // num clearances
        // num ab screenings
        // num rna screenings
        // num linkages

        // tabular outcomes
        // num linkages | num people
        // num ab tests | num people
        // num rna tests | num people
        // num treatment inititations | num people
    }
} // namespace DataWriter
