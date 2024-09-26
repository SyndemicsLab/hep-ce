#include "BehaviorChanges.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class BehaviorChangesTest : public EventTest {};

TEST_F(BehaviorChangesTest, BehaviorChanges) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::INJECTION));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetMoudState())
        .WillByDefault(Return(person::MOUD::NONE));

    // Data Setup
    struct cost_util cost = {25.00, 0.5};
    std::vector<struct cost_util> cstorage = {cost};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, &cstorage, _))
        .WillByDefault(
            DoAll(SetArg2ToCostUtilCallbackValue(&cstorage), Return(0)));
    struct behavior_trans_select trans = {0.0, 0.0, 1.0, 0.0, 0.0};
    std::vector<struct behavior_trans_select> tstorage = {trans};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(
            SetArg2ToBehaviorTransitionsCallbackValue(&tstorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(2));

    // Expectations
    cost::Cost backgroundCost = {cost::CostCategory::BEHAVIOR, "Drug Behavior",
                                 cost.cost};
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(1);
    EXPECT_CALL(*testPerson, SetBehavior(person::Behavior::FORMER_INJECTION))
        .Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("BehaviorChanges");
    event->Execute(testPerson, event_dm, decider);
}