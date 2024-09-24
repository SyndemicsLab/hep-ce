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
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class Pregnancy::PregnancyIMPL {
    private:
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            double *d = (double *)storage;
            *d = std::stod(data[0]); // First Column Selected
            return 0;
        }
        std::string buildSQL(person::Person &person, std::string column) {
            std::stringstream sql;
            std::string age_years =
                std::to_string((int)(person.GetAge() / 12.0));
            sql << "SELECT '" + column + "' FROM pregnancy";
            sql << " WHERE age_years = '" << age_years << "'";
            sql << " AND gestation = '"
                << std::to_string(person.GetTimeSincePregnancyChange()) << "';";
            return sql.str();
        }

        bool
        CheckMiscarriage(person::Person &person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         std::unique_ptr<stats::Decider> &decider) {
            std::vector<double> prob =
                this->GetSingleProbability(person, dm, "miscarriage");
            return (!decider->GetDecision(prob)) ? true : false;
        }

        std::vector<double> GetSingleProbability(
            person::Person &person,
            std::shared_ptr<datamanagement::DataManagerBase> dm,
            std::string column) {
            std::string query = this->buildSQL(person, column);
            double storage = 0.0;
            std::string error;
            int rc = dm->SelectCustomCallback(query, callback, &storage, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "No cost avaliable for Fibrosis Progression!");
                return;
            }
            std::vector<double> result = {storage, 1 - storage};
            return result;
        }

        int
        GetNumberOfBirths(person::Person &person,
                          std::shared_ptr<datamanagement::DataManagerBase> dm,
                          std::unique_ptr<stats::Decider> &decider) {
            std::string storage = "";
            dm->GetFromConfig("pregnancy.multiple_delivery_probability",
                              storage);
            if (storage.empty()) {
                spdlog::get("main")->warn(
                    "No Multiple Delivery Probability Found! Assuming 0.");
                storage = "0";
            }
            double prob = std::stod(storage);
            std::vector<double> result = {prob, 1 - prob};
            // Currently only deciding between single birth or twins
            return (decider->GetDecision(result) == 0) ? 2 : 1;
        }

        bool
        DoChildrenGetTested(std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::unique_ptr<stats::Decider> &decider) {
            std::string storage = "";
            dm->GetFromConfig("pregnancy.infant_hcv_tested_probability",
                              storage);
            if (storage.empty()) {
                spdlog::get("main")->warn(
                    "No Infant HCV Testing Probability Found! Assuming 0.");
                storage = "0";
            }
            double prob = std::stod(storage);
            std::vector<double> result = {prob, 1 - prob};
            return (decider->GetDecision(result) == 0) ? true : false;
        }

        bool
        DrawChildInfection(std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::unique_ptr<stats::Decider> &decider) {
            std::string storage = "";
            dm->GetFromConfig("pregnancy.vertical_hcv_transition_probability",
                              storage);
            if (storage.empty()) {
                spdlog::get("main")->warn("No Infant HCV Vertical Transmission "
                                          "Probability Found! Assuming 0.");
                storage = "0";
            }
            double prob = std::stod(storage);
            std::vector<double> result = {prob, 1 - prob};
            return (decider->GetDecision(result) == 0) ? true : false;
        }

        void
        AttemptHaveChild(person::Person &person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         std::unique_ptr<stats::Decider> &decider) {
            if (CheckMiscarriage(person, dm, decider)) {
                person.Stillbirth();
                return;
            }

            int numberOfBirths = GetNumberOfBirths(person, dm, decider);

            if (person.GetHCV() != person::HCV::CHRONIC) {
                for (int child = 0; child < numberOfBirths; ++child) {
                    person.AddChild(person::HCV::NONE, false);
                }
                return;
            }

            bool tested = DoChildrenGetTested(dm, decider);
            for (int child = 0; child < numberOfBirths; ++child) {
                if (DrawChildInfection(dm, decider)) {
                    person.AddChild(person::HCV::CHRONIC, tested);
                } else {
                    person.AddChild(person::HCV::NONE, tested);
                }
            }
        }

        void
        AttemptHealthyMonth(person::Person &person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::unique_ptr<stats::Decider> &decider) {
            if (CheckMiscarriage(person, dm, decider)) {
                person.Miscarry();
            }
        }

    public:
        void doEvent(person::Person &person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::unique_ptr<stats::Decider> &decider) {

            if (person.GetSex() == person::Sex::MALE || person.GetAge() < 15 ||
                person.GetAge() > 45 ||
                (person.GetPregnancyState() ==
                     person::PregnancyState::POSTPARTUM &&
                 person.GetTimeSincePregnancyChange() < 3)) {
                return;
            }

            if (person.GetPregnancyState() ==
                person::PregnancyState::POSTPARTUM) {
                person.EndPostpartum();
            }

            if (person.GetPregnancyState() ==
                person::PregnancyState::PREGNANT) {
                if (person.GetTimeSincePregnancyChange() >= 9) {
                    AttemptHaveChild(person, dm, decider);
                } else {
                    AttemptHealthyMonth(person, dm, decider);
                }
            } else {
                std::vector<double> prob = this->GetSingleProbability(
                    person, dm, "pregnancy_probability");
                // if you are pregnant (i.e. getDecision returns 0)
                if (decider->GetDecision(prob) == 0) {
                    person.Impregnate();
                }
            }
            return;
        }
    };

    // Pregnancy::Pregnancy(std::mt19937_64 &generator, Data::IDataTablePtr
    // table,
    //                      Data::Config &config,
    //                      std::shared_ptr<spdlog::logger> logger,
    //                      std::string name)
    //     : ProbEvent(generator, table, config, logger, name) {
    //     this->multDeliveryProb = this->config.get(
    //         "pregnancy.multiple_delivery_probability", (double)-1.0);

    //     this->verticalHCVTransProb = this->config.get(
    //         "pregnancy.vertical_hcv_transition_probability", (double)-1.0);

    //     this->infantHCVTestedProb = this->config.get(
    //         "pregnancy.infant_hcv_tested_probability", (double)-1.0);
    // }

    Pregnancy::Pregnancy() { impl = std::make_unique<PregnancyIMPL>(); }

    void Pregnancy::doEvent(person::Person &person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::unique_ptr<stats::Decider> &decider) {
        impl->doEvent(person, dm, decider);
    }

    Pregnancy::~Pregnancy() = default;
    Pregnancy::Pregnancy(Pregnancy &&) noexcept = default;
    Pregnancy &Pregnancy::operator=(Pregnancy &&) noexcept = default;

} // namespace event
