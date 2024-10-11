#include "Aging.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class AgingTest : public EventTest {};

TEST_F(AgingTest, Aging) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::INJECTION));

    // Data Setup
    Utils::tuple_3i tup = std::make_tuple(25, 0, 4);
    struct cost_util cost = {25.00, 0.5};
    std::unordered_map<Utils::tuple_3i, struct cost_util, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        cstorage;
    cstorage[tup] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_CU(&cstorage), Return(0)));

    cost::Cost backgroundCost = {cost::CostCategory::MISC, "Background Cost",
                                 cost.cost};

    // Expectations
    EXPECT_CALL(*testPerson, Grow()).Times(1);
    EXPECT_CALL(*testPerson, AddCost(backgroundCost)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(cost.util)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Aging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}