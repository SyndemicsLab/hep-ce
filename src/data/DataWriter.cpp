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

#include "Cost.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManager.hpp>
#include <filesystem>
#include <fstream>

namespace writer {
    class DataWriter::DataWriterIMPL {
    private:
        std::string
        GrabPersonDetailsAsStringInHeaderOrder(person::PersonBase &person) {
            std::stringstream compiled_attributes;
            compiled_attributes
                << person.GetID() << "," << std::boolalpha << person.GetSex()
                << "," << person.GetAge() << "," << person.GetIsAlive() << ","
                << person.GetDeathReason() << ","
                << person.IsIdentifiedAsInfected() << ","
                << person.GetTimeIdentified() << "," << person.GetHCV() << ","
                << person.GetFibrosisState() << "," << person.IsGenotypeThree()
                << "," << person.GetSeropositive() << ","
                << person.GetTimeHCVChanged() << ","
                << person.GetTimeFibrosisStateChanged() << ","
                << person.GetBehavior() << "," << person.GetTimeBehaviorChange()
                << "," << person.GetLinkState() << ","
                << person.GetTimeOfLinkChange() << ","
                << person.GetLinkageType() << "," << person.GetLinkCount()
                << "," << person.GetMeasuredFibrosisState() << ","
                << person.GetTimeOfLastStaging() << ","
                << person.GetTimeOfLastScreening() << ","
                << person.GetNumABTests() << "," << person.GetNumRNATests()
                << "," << person.GetTimesInfected() << ","
                << person.GetClearances() << ","
                << person.HasInitiatedTreatment() << ","
                << person.GetTimeOfTreatmentInitiation() << ","
                << person.GetUtility().minUtil << ","
                << person.GetUtility().multUtil << ","
                << person.GetWithdrawals() << "," << person.GetToxicReactions()
                << "," << person.GetCompletedTreatments() << ","
                << person.GetSVRs();
            return compiled_attributes.str();
        }

    public:
        int UpdatePopulation(
            std::vector<std::shared_ptr<person::PersonBase>> new_population,
            std::shared_ptr<datamanagement::DataManager> dm) {
            return -1;
        }
        int WriteOutputPopulationToTable(
            std::vector<std::shared_ptr<person::PersonBase>> new_population,
            std::shared_ptr<datamanagement::DataManager> dm) {
            return -1;
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
            csvStream << "id," << person::POPULATION_HEADERS << std::endl;
            for (std::shared_ptr<person::PersonBase> &person : new_population) {
                csvStream << GrabPersonDetailsAsStringInHeaderOrder(*person)
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
        std::shared_ptr<datamanagement::DataManager> dm) {
        return impl->UpdatePopulation(new_population, dm);
    }
    int DataWriter::WritePopulationToFile(
        std::vector<std::shared_ptr<person::PersonBase>> new_population,
        std::string &filepath) {
        return impl->WriteOutputPopulationToFile(new_population, filepath);
    }
} // namespace writer
