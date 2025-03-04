#include "Aging.hpp"
#include "EventTest.cpp"
#include "Utils.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class AgingTest : public EventTest {};

std::string SQLQuery = "SELECT age_years, gender, drug_behavior, cost, utility "
                       "FROM background_impacts;";

TEST_F(AgingTest, Aging) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::INJECTION));

    // Data Setup
    struct cost_util cost = {25.00, 0.5};
    double discount_rate = 0.025;
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(discount_rate)), Return(0)));
    Utils::tuple_3i tup = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, struct cost_util, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        custorage;
    custorage[tup] = cu;
    ON_CALL(*event_dm, SelectCustomCallback(SQLQuery, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_CU(&custorage), Return(0)));

    double discounted_cost = Utils::discount(cu.cost, discount_rate, 1);
    std::pair<double, double> expected_utils = {cu.util, cu.util};
    std::pair<double, double> expected_discounted_utils = {
        Utils::discount(0.5, discount_rate, 1),
        Utils::discount(0.5, discount_rate, 1)};
    double expected_discounted_life = Utils::discount(1, discount_rate, 1);

    // Expectations
    EXPECT_CALL(*testPerson, Grow()).Times(1);
    EXPECT_CALL(*testPerson, GetCurrentTimestep()).WillRepeatedly(Return(1));
    EXPECT_CALL(*testPerson, AddCost(cu.cost, discounted_cost,
                                     cost::CostCategory::BACKGROUND))
        .Times(1);
    EXPECT_CALL(*testPerson,
                SetUtility(cu.util, utility::UtilityCategory::BACKGROUND))
        .Times(1);
    EXPECT_CALL(*testPerson, GetUtility()).WillOnce(Return(expected_utils));
    EXPECT_CALL(*testPerson, AccumulateTotalUtility(expected_utils,
                                                    expected_discounted_utils))
        .Times(1);
    EXPECT_CALL(*testPerson, AddDiscountedLifeSpan(expected_discounted_life))
        .Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Aging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
