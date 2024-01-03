#include "Inputs.hpp"

namespace Data {
    Config::Config(std::string configFile) : Configuration(configFile) {}

    std::optional<uint64_t> Config::getSeed() {
        std::shared_ptr<uint64_t> seed =
            this->optional<uint64_t>("simulation.seed");
        if (seed) {
            return *seed;
        }
        return std::nullopt;
    }

    int Config::getPopulationSize() {
        return this->get<int>("simulation.population_size");
    }

    std::vector<std::string> Config::getEvents() {
        std::string events = this->get<std::string>("simulation.events");
        return this->parseString2VectorOfStrings(events);
    }

    int Config::getDuration() {
        return this->get<int>("simulation.duration");
    }

    std::optional<int> Config::getStartTime() {
        std::shared_ptr<int> startTime =
            this->optional<int>("simulation.start_time");
        if (startTime) {
            return *startTime;
        }
        return std::nullopt;
    }

    double Config::getF4Mortality() {
        return this->get<double>("mortality.f4");
    }

    double Config::getF4PostSVRMortalityMultiplier() {
        return this->get<double>("mortality.f4_post_svr_multiplier");
    }

    double Config::getDecompMortality() {
        return this->get<double>("mortality.decomp");
    }

    double Config::getDecompPostSVRMortalityMultiplier() {
        return this->get<double>("mortality.decomp_post_svr_multiplier");
    }

    std::optional<double> Config::getClearanceProb() {
        std::shared_ptr<double> clearanceProb =
            this->optional<double>("infection.clearance_prob");
        if (clearanceProb) {
            return *clearanceProb;
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string>> Config::getIneligibleDrugUse() {
        std::shared_ptr<std::string> ineligible =
            this->optional("eligibility.ineligible_drug_use");
        if (ineligible) {
            return this->parseString2VectorOfStrings(*ineligible);
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string>>
    Config::getIneligibleFibrosisStages() {
        std::shared_ptr<std::string> ineligible =
            this->optional("eligibility.ineligible_fibrosis_stages");
        if (ineligible) {
            return this->parseString2VectorOfStrings(*ineligible);
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string>>
    Config::getIneligibleTimeFormerThreshold() {
        std::shared_ptr<std::string> ineligible =
            this->optional("eligibility.ineligible_time_former_threshold");
        if (ineligible) {
            return this->parseString2VectorOfStrings(*ineligible);
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string>>
    Config::getIneligibleTimeSinceLinked() {
        std::shared_ptr<std::string> ineligible =
            this->optional("eligibility.ineligible_time_since_linked");
        if (ineligible) {
            return this->parseString2VectorOfStrings(*ineligible);
        }
        return std::nullopt;
    }

    int Config::getFibrosisStagingPeriod() {
        return this->get<int>("fibrosis_staging.period");
    }

    std::optional<std::string> Config::getFibrosisMultitestResultMethod() {
        std::shared_ptr<std::string> multitest_method =
            this->optional("fibrosis_staging.multitest_result_method");
        if (multitest_method) {
            return *multitest_method;
        }
        return std::nullopt;
    }

    std::string Config::getFibrosisTestOne() {
        return this->get<std::string>("fibrosis_staging.test_one");
    }

    double Config::getFibrosisTestOneCost() {
        return this->get<double>("fibrosis_staging.test_one_cost");
    }

    std::optional<std::vector<std::string>> Config::getFibrosisTestTwoStages() {
        std::shared_ptr<std::string> testTwoStages =
            this->optional("fibrosis_staging.test_two_eligible_stages");
        if (testTwoStages) {
            return this->parseString2VectorOfStrings(*testTwoStages);
        }
        return std::nullopt;
    }

    std::optional<std::string> Config::getFibrosisTestTwo() {
        std::shared_ptr<std::string> testTwo =
            this->optional("fibrosis_staging.test_two");
        if (testTwo) {
            return *testTwo;
        }
        return std::nullopt;
    }

    std::optional<double> Config::getFibrosisTestTwoCost() {
        std::shared_ptr<double> testTwoCost =
            this->optional<double>("fibrosis_staging.test_two_cost");
        if (testTwoCost) {
            return *testTwoCost;
        }
        return std::nullopt;
    }

    int Config::getScreeningPeriod() {
        return this->get<int>("screening.period");
    }

    double Config::getScreeningFalsePositiveCost() {
        return this->get<double>("screening.false_positive_test_cost");
    }

    double Config::getScreeningSeroposMultiplier() {
        return this->get<double>("screening.seropositivity_multiplier");
    }

    std::string Config::getScreeningInterventionType() {
        return this->get<std::string>("screening.intervention_type");
    }

    std::vector<double> Config::getScreenTest(std::string str) {
        // error if str is not one of the four screening types
        // maybe check versus keys in a map?
        std::vector<double> toReturn;
        for (std::string &attribute : SCREENING_ATTRIBUTES) {
            std::string key = "screening_" + str + "." + attribute;
            toReturn.push_back(this->get<double>(key));
        }
        return toReturn;
    }

    double Config::getLinkingInterventionCost() {
        return this->get<double>("linking.intervention_cost");
    }

    double Config::getRelinkMultiplier() {
        return this->get<double>("linking.relink_multiplier");
    }

    double Config::getVoluntaryRelinkProb() {
        return this->get<double>("linking.voluntary_relinkage_probability");
    }

    int Config::getRelinkDuration() {
        return this->get<int>("linking.relinkage_duration");
    }

    // void Config::getTreatments() {
    //     std::string courseString =
    //     this->configuration.get("treatment.courses");
    //     std::vector<std::string> courses =
    //         this->configuration.parseString2VectorOfStrings(courseString);
    //     for (std::string &course : courses) {
    //         std::string componentString =
    //             this->configuration.get("treatment_" + course +
    //             ".components");
    //         std::vector<std::string> components =
    //             this->configuration.parseString2VectorOfStrings(
    //                 componentString);
    //     }
    // }

    double Config::getDiscountRate() {
        return this->get<double>("cost.discounting_rate");
    }
} // namespace Data
