#include "FibrosisStaging.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class FibrosisStagingTest : public EventTest {};

TEST_F(FibrosisStagingTest, FibrosisStaging) {

    std::shared_ptr<event::Event> event = efactory.create("FibrosisStaging");

    std::vector<double> storage = {0.0, 0.0, 1.0, 0.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(DoAll(SetArg2ToCallbackValue(&storage), Return(0)));

    std::string t1 = std::string("0.0");
    EXPECT_CALL(*event_dm, GetFromConfig(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(t1), Return(0)));

    std::string period = std::string("12");
    EXPECT_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(period), Return(0)));

    std::string col = std::string("colname");
    EXPECT_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(col), Return(0)));

    event->Execute(testPerson, event_dm, decider);
    EXPECT_EQ(person::MeasuredFibrosisState::F4,
              testPerson->GetMeasuredFibrosisState());
}