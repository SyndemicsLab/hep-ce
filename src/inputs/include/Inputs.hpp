#include "Configuration.hpp"
#include "DataTable.hpp"
#include <optional>

namespace Data {
    static std::vector<std::string> SCREENING_ATTRIBUTES = {
        "cost", "acute_sensitivity", "chronic_sensitivity", "specificity"};

    class Config {
    public:
        Config(std::string configFile);
        // simulation section
        /// @brief
        /// @return
        std::optional<uint64_t> getSeed();
        /// @brief
        /// @return
        uint32_t getPopulationSize();
        /// @brief
        /// @return
        std::vector<std::string> getEvents();
        /// @brief
        /// @return
        uint32_t getDuration();
        /// @brief
        /// @return
        std::optional<uint32_t> getStartTime();
        // mortality section
        /// @brief
        /// @return
        double getF4Mortality();
        /// @brief
        /// @return
        double getF4PostSVRMortalityMultiplier();
        /// @brief
        /// @return
        double getDecompMortality();
        /// @brief
        /// @return
        double getDecompPostSVRMortalityMultiplier();
        // infection section
        /// @brief
        /// @return
        std::optional<double> getClearanceProb();
        // eligibility section
        /// @brief
        /// @return
        std::optional<std::vector<std::string>> getIneligibleDrugUse();
        /// @brief
        /// @return
        std::optional<std::vector<std::string>> getIneligibleFibrosisStages();
        /// @brief
        /// @return
        std::optional<std::vector<std::string>>
        getIneligibleTimeFormerThreshold();
        /// @brief
        /// @return
        std::optional<std::vector<std::string>> getIneligibleTimeSinceLinked();
        // staging section
        /// @brief
        /// @return
        uint32_t getFibrosisStagingPeriod();
        /// @brief
        /// @return
        std::optional<std::string> getFibrosisMultitestResultMethod();
        /// @brief
        /// @return
        std::string getFibrosisTestOne();
        /// @brief
        /// @return
        double getFibrosisTestOneCost();
        /// @brief
        /// @return
        std::optional<std::vector<std::string>> getFibrosisTestTwoStages();
        /// @brief
        /// @return
        std::optional<std::string> getFibrosisTestTwo();
        /// @brief
        /// @return
        std::optional<double> getFibrosisTestTwoCost();
        // screening section
        /// @brief
        /// @return
        uint32_t getScreeningPeriod();
        /// @brief
        /// @return
        double getScreeningFalsePositiveCost();
        /// @brief
        /// @return
        double getScreeningSeroposMultiplier();
        /// @brief
        /// @return
        std::string getScreeningInterventionType();
        /// @brief
        /// @return
        std::vector<double> getScreenTest(std::string str);
        // linking section
        /// @brief
        /// @return
        double getLinkingInterventionCost();
        /// @brief
        /// @return
        double getRelinkMultiplier();
        /// @brief
        /// @return
        double getVoluntaryRelinkProb();
        /// @brief
        /// @return
        uint32_t getRelinkDuration();
        // treatment section
        // getTreatments();
        // cost section
        double getDiscountRate();

    private:
        Configuration configuration;
    };
} // namespace Data
