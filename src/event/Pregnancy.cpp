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
/// This file contains the implementation of the Pregnancy Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Pregnancy.hpp"

namespace event {
    Pregnancy::Pregnancy(std::mt19937_64 &generator, Data::IDataTablePtr table,
                         Data::Config &config,
                         std::shared_ptr<spdlog::logger> logger,
                         std::string name)
        : ProbEvent(generator, table, config, logger, name) {
        this->multDeliveryProb = this->config.get(
            "pregnancy.multiple_delivery_probability", (double)-1.0);

        this->verticalHCVTransProb = this->config.get(
            "pregnancy.vertical_hcv_transition_probability", (double)-1.0);

        this->infantHCVTestedProb = this->config.get(
            "pregnancy.infant_hcv_tested_probability", (double)-1.0);
    }

    void Pregnancy::doEvent(person::PersonBase &person) {
        // If a person is dead, male, younger than 15, older than 45, or been in
        // postpartum for less than 3 months then skip

        int timeSincePregnancyChange =
            this->currentTime - person->getTimeOfPregnancyChange();
        if (person->getSex() == person::Sex::MALE || person->age < 15 ||
            person->age > 45 ||
            (person->getPregnancyState() ==
                 person::PregnancyState::POSTPARTUM &&
             timeSincePregnancyChange < 3)) {
            return;
        }

        if (person->getPregnancyState() == person::PregnancyState::PREGNANCY) {
            if (person->timeSincePregnancyChange() >= 9) {
                // have child
                checkMiscarriage(person);

            } else {
                // Another month of pregnancy
                checkMiscarriage(person);
                return;
            }
        }

        // 1. Get the probability of pregnancy
        std::vector<double> prob = this->getPregnancyProb(person);
        // if you are not pregnant (i.e. getDecision returns > 0)
        if (this->getDecision(prob)) {
            return;
        }
        person->setPregnancyState(person::PregnancyState::PREGNANT);

        // 2. Decide whether the person clears
        int doesNotClear = this->getDecision(prob);
        // if you do not clear, return immediately
        if (doesNotClear) {
            return;
        }
        person->clearHCV(this->getCurrentTimestep());
    }

    void Pregnancy::checkMiscarriage(person::PersonBase &person) {
        std::vector<double> prob = this->getMiscarriageProb(person);
        // if a miscarriage (getDecision == 0)
        if (!this->getDecision(prob)) {
            person->setPregnancyState(person::PregnancyState::POSTPARTUM);
            person->setNumMiscarriages(1);
            person->setTimeOfPregnancyChange(this->currentTime);
        }
    }

    std::vector<double>
    Pregnancy::getPregnancyProb(person::PersonBase &person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age"] = std::to_string((int)(person->age / 12.0));
        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty() == 0) {
            // error
            return {};
        }
        double probPregnancy =
            std::stod((*resultTable)["pregnancy_probability"][0]);
        std::vector<double> result = {probPregnancy, 1 - probPregnancy};
        return result;
    }

    std::vector<double>
    Pregnancy::getLiveBirthProb(person::PersonBase &person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age"] = std::to_string((int)(person->age / 12.0));
        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty() == 0) {
            // error
            return {};
        }
        double probLiveBirth =
            std::stod((*resultTable)["live_birth_probability"][0]);
        std::vector<double> result = {probLiveBirth, 1 - probLiveBirth};
        return result;
    }

    std::vector<double>
    Pregnancy::getMiscarriageProb(person::PersonBase &person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gestation"] =
            person::person::sexEnumToStringMap[person->getSex()];
        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty() == 0) {
            // error
            return {};
        }
        double probMiscarriage = std::stod((*resultTable)["probability"][0]);
        std::vector<double> result = {probMiscarriage, 1 - probMiscarriage};
        return result;
    }
} // namespace event
