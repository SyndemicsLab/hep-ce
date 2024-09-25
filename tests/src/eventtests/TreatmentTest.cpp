#include "Treatment.hpp"
#include "DataManagerMock.hpp"
#include "EventFactory.hpp"
#include "PersonFactory.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class TreatmentTest : public ::testing::Test {
private:
    void RegisterLogger() {
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks{console_sink};
        auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(),
                                                       sinks.end());
        spdlog::register_logger(logger);
        spdlog::flush_every(std::chrono::seconds(3));
    }

protected:
    std::shared_ptr<person::PersonBase> testPerson;
    std::shared_ptr<datamanagement::MOCKDataManager> person_dm;
    event::EventFactory efactory;
    std::unique_ptr<stats::Decider> decider;
    void SetUp() override {
        RegisterLogger();
        person_dm = std::make_unique<datamanagement::MOCKDataManager>();
        decider = std::make_unique<stats::Decider>();
        person::PersonFactory pfactory;
        testPerson = pfactory.create();
        decider = std::make_unique<stats::Decider>();
    }
    void TearDown() override { spdlog::drop("main"); }
};