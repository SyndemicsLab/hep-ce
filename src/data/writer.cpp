////////////////////////////////////////////////////////////////////////////////
// File: writer.cpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/writer_internals.hpp"

#include <filesystem>
#include <fstream>

#include <hepce/model/person.hpp>

namespace hepce {
namespace data {

std::string
GrabPersonDetailsAsStringInHeaderOrder(const model::Person &person) {
    std::stringstream compiled_attributes;
    compiled_attributes
        << person.GetID() << "," << std::boolalpha << person.GetSex() << ","
        << person.GetAge() << "," << person.IsAlive() << ","
        << person.GetDeathReason() << "," << person.IsIdentifiedAsInfected()
        << "," << person.GetTimeHCVIdentified() << "," << person.GetHCV() << ","
        << person.GetTrueFibrosisState() << "," << person.IsGenotypeThree()
        << "," << person.GetSeropositivity() << ","
        << person.GetTimeHCVChanged() << ","
        << person.GetTimeTrueFibrosisStateChanged() << ","
        << person.GetBehavior() << "," << person.GetTimeBehaviorChange() << ","
        << person.GetLinkState() << "," << person.GetTimeOfLinkChange() << ","
        << person.GetLinkageType() << "," << person.GetLinkCount() << ","
        << person.GetMeasuredFibrosisState() << ","
        << person.GetTimeOfFibrosisStaging() << ","
        << person.GetTimeOfLastScreening() << "," << person.GetNumberOfABTests()
        << "," << person.GetNumberOfRNATests() << ","
        << person.GetTimesHCVInfected() << "," << person.GetAcuteHCVClearances()
        << "," << person.HasInitiatedTreatment() << ","
        << person.GetTimeOfTreatmentInitiation() << ",";
    const auto &tu = person.GetTotalUtility();
    compiled_attributes << tu.min_util << "," << tu.mult_util << ","
                        << tu.discount_min_util << "," << tu.discount_mult_util
                        << "," << person.GetWithdrawals() << ","
                        << person.GetToxicReactions() << ","
                        << person.GetCompletedTreatments() << ","
                        << person.GetSVRs() << ",";
    const auto &cu = person.GetCurrentUtilities();
    compiled_attributes << cu.at(model::UtilityCategory::BEHAVIOR) << ","
                        << cu.at(model::UtilityCategory::LIVER) << ","
                        << cu.at(model::UtilityCategory::TREATMENT) << ","
                        << cu.at(model::UtilityCategory::kBackground) << ","
                        << cu.at(model::UtilityCategory::HIV) << ","
                        << person.GetLifeSpan() << ","
                        << person.GetDiscountedLifeSpan() << ","
                        << person.GetNumberOfTreatmentStarts() << ","
                        << person.GetRetreatments();
    return compiled_attributes.str();
}

std::string
WriterImpl::WritePopulation(const std::vector<model::Person> &population,
                            const std::string &filename,
                            const OutputType output_type) {
    std::filesystem::path path = filename;
    std::ofstream csvStream;
    csvStream.open(path, std::ofstream::out);
    if (!csvStream) {
        return "";
    }
    csvStream << "id," << POPULATION_HEADERS << ",cost,discount_cost"
              << std::endl;
    for (const auto &person : population) {
        std::pair<double, double> costTotals = person.GetCostTotals();
        csvStream << GrabPersonDetailsAsStringInHeaderOrder(person) << ","
                  << costTotals.first << "," << costTotals.second << std::endl;
    }
    csvStream.close();
    return 0;
}
} // namespace data
} // namespace hepce