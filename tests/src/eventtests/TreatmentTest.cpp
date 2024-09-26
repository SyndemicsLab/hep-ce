#include "Treatment.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class TreatmentTest : public EventTest {};

TEST_F(TreatmentTest, Treatment) {

    std::shared_ptr<event::Event> event = efactory.create("Treatment");

    std::vector<double> storage = {1.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
}