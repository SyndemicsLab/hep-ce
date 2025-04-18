////////////////////////////////////////////////////////////////////////////////
// File: DataWriter.cpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2024-06-12                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-18                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2024-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "DataWriter.hpp"

#include "Cost.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <filesystem>
#include <fstream>

namespace writer {
class DataWriter::DataWriterIMPL {
private:
    std::string GrabPersonDetailsAsStringInHeaderOrder(
        std::shared_ptr<person::PersonBase> person) {
        std::stringstream compiled_attributes;
        compiled_attributes
            << person->GetID() << "," << std::boolalpha << person->GetSex()
            << "," << person->GetAge() << "," << person->IsAlive() << ","
            << person->GetDeathReason() << ","
            << person->IsIdentifiedAsInfected() << ","
            << person->GetTimeHCVIdentified() << "," << person->GetHCV() << ","
            << person->GetTrueFibrosisState() << ","
            << person->IsGenotypeThree() << "," << person->GetSeropositivity()
            << "," << person->GetTimeHCVChanged() << ","
            << person->GetTimeTrueFibrosisStateChanged() << ","
            << person->GetBehavior() << "," << person->GetTimeBehaviorChange()
            << "," << person->GetLinkState() << ","
            << person->GetTimeOfLinkChange() << "," << person->GetLinkageType()
            << "," << person->GetLinkCount() << ","
            << person->GetMeasuredFibrosisState() << ","
            << person->GetTimeOfFibrosisStaging() << ","
            << person->GetTimeOfLastScreening() << ","
            << person->GetNumberOfABTests() << ","
            << person->GetNumberOfRNATests() << ","
            << person->GetTimesHCVInfected() << ","
            << person->GetAcuteHCVClearances() << ","
            << person->HasInitiatedTreatment() << ","
            << person->GetTimeOfTreatmentInitiation() << ",";
        const auto &tu = person->GetTotalUtility();
        compiled_attributes
            << tu.min_util << "," << tu.mult_util << "," << tu.discount_min_util
            << "," << tu.discount_mult_util << "," << person->GetWithdrawals()
            << "," << person->GetToxicReactions() << ","
            << person->GetCompletedTreatments() << "," << person->GetSVRs()
            << ",";
        const auto &cu = person->GetCurrentUtilities();
        compiled_attributes << cu.at(utility::UtilityCategory::BEHAVIOR) << ","
                            << cu.at(utility::UtilityCategory::LIVER) << ","
                            << cu.at(utility::UtilityCategory::TREATMENT) << ","
                            << cu.at(utility::UtilityCategory::BACKGROUND)
                            << "," << cu.at(utility::UtilityCategory::HIV)
                            << "," << person->GetLifeSpan() << ","
                            << person->GetDiscountedLifeSpan() << ","
                            << person->GetNumberOfTreatmentStarts() << ","
                            << person->GetRetreatments() << ",";
        // pregnancy quantities
        const auto &preg = person->GetPregnancyDetails();
        compiled_attributes << preg.numInfants << "," << preg.numMiscarriages
                            << "," << preg.numHCVExposures << ","
                            << preg.numHCVInfections << "," << preg.numHCVTests;
        return compiled_attributes.str();
    }

public:
    int UpdatePopulation(
        std::vector<std::shared_ptr<person::PersonBase>> new_population,
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        return -1;
    }
    int WriteOutputPopulationToTable(
        std::vector<std::shared_ptr<person::PersonBase>> new_population,
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        return -1;
    }
    std::string PopulationToString(
        std::vector<std::shared_ptr<person::PersonBase>> population) {
        std::stringstream population_table;
        population_table << "id," << person::POPULATION_HEADERS
                         << ",cost,discount_cost" << std::endl;
        for (std::shared_ptr<person::PersonBase> &person : population) {
            std::pair<double, double> costTotals = person->GetCostTotals();
            population_table << GrabPersonDetailsAsStringInHeaderOrder(person)
                             << "," << costTotals.first << ","
                             << costTotals.second << std::endl;
        }
        return population_table.str();
    }
    int WriteOutputPopulationToFile(
        std::vector<std::shared_ptr<person::PersonBase>> new_population,
        std::string &filepath) {
        std::filesystem::path path = filepath;
        std::ofstream csvStream;
        csvStream.open(path, std::ofstream::out);
        if (!csvStream) {
            return -1;
        }
        csvStream << "id," << person::POPULATION_HEADERS
                  << ",cost,discount_cost" << std::endl;
        for (std::shared_ptr<person::PersonBase> &person : new_population) {
            std::pair<double, double> costTotals = person->GetCostTotals();
            csvStream << GrabPersonDetailsAsStringInHeaderOrder(person) << ","
                      << costTotals.first << "," << costTotals.second
                      << std::endl;
        }
        csvStream.close();
        return 0;
    }
};

DataWriter::DataWriter() { impl = std::make_unique<DataWriterIMPL>(); }

DataWriter::~DataWriter() = default;

int DataWriter::UpdatePopulation(
    std::vector<std::shared_ptr<person::PersonBase>> new_population,
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    return impl->UpdatePopulation(new_population, dm);
}

std::string DataWriter::PopulationToString(
    std::vector<std::shared_ptr<person::PersonBase>> population) {
    return impl->PopulationToString(population);
}
int DataWriter::WritePopulationToFile(
    std::vector<std::shared_ptr<person::PersonBase>> new_population,
    std::string &filepath) {
    return impl->WriteOutputPopulationToFile(new_population, filepath);
}
} // namespace writer
