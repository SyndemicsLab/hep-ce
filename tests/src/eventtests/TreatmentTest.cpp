#include "Treatment.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class TreatmentTest : public EventTest {};

std::string const DURATION_QUERY =
    "SELECT retreatment, genotype_three, cirrhotic, duration FROM treatments;";
std::string const COST_QUERY =
    "SELECT retreatment, genotype_three, cirrhotic, cost FROM treatments;";
std::string const SVR_QUERY = "SELECT retreatment, genotype_three, cirrhotic, "
                              "svr_prob_if_completed FROM treatments;";
std::string const TOXICITY_QUERY =
    "SELECT retreatment, genotype_three, cirrhotic, "
    "toxicity_prob_if_withdrawal FROM "
    "treatments;";
std::string const WITHDRAWAL_QUERY = "SELECT retreatment, genotype_three, "
                                     "cirrhotic, withdrawal FROM treatments;";

TEST_F(TreatmentTest, NewTreatmentInitiation) {
    // Person Setup
    EXPECT_CALL(*testPerson, HasInitiatedTreatment())
        .WillOnce(Return(false))       // False Positive
        .WillOnce(Return(false))       // Initiate
        .WillRepeatedly(Return(true)); // Remainder
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(0));
    ON_CALL(*testPerson, IsGenotypeThree()).WillByDefault(Return(false));
    ON_CALL(*testPerson, IsCirrhotic()).WillByDefault(Return(false));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::NONE));
    ON_CALL(*testPerson, GetTimeSinceLinkChange()).WillByDefault(Return(2));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::FORMER_NONINJECTION));
    ON_CALL(*testPerson, GetTimeBehaviorChange()).WillByDefault(Return(120));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::NONE));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::LINKED));
    ON_CALL(*testPerson, GetCompletedTreatments()).WillByDefault(Return(0));
    ON_CALL(*testPerson, GetWithdrawals()).WillByDefault(Return(0));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.allow_retreatment", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.retreatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("eligibility.ineligible_drug_use", _))
        .WillByDefault(DoAll(SetArgReferee<1>(""), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("eligibility.ineligible_fibrosis_stages", _))
        .WillByDefault(DoAll(SetArgReferee<1>(""), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("eligibility.ineligible_time_former_threshold", _))
        .WillByDefault(DoAll(SetArgReferee<1>(""), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("eligibility.ineligible_time_since_linked", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));

    // Duration Setup
    double duration = 2;
    Utils::tuple_3i tup_3i = std::make_tuple(0, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        dstorage;
    dstorage[tup_3i] = duration;
    ON_CALL(*event_dm, SelectCustomCallback(DURATION_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&dstorage), Return(0)));

    // Cost Setup
    double cost = 25.25;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        cstorage;
    cstorage[tup_3i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&cstorage), Return(0)));

    // SVR Setup
    double svr = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        sstorage;
    sstorage[tup_3i] = svr;
    ON_CALL(*event_dm, SelectCustomCallback(SVR_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&sstorage), Return(0)));

    // Toxicity Setup
    double toxicity = 1.0;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = toxicity;
    ON_CALL(*event_dm, SelectCustomCallback(TOXICITY_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&tstorage), Return(0)));

    // Withdrawal Setup
    double withdrawal = 0.9;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        wstorage;
    wstorage[tup_3i] = withdrawal;
    ON_CALL(*event_dm, SelectCustomCallback(WITHDRAWAL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&wstorage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))        // Is Not Lost to Follow Up
        .WillOnce(Return(0))        // Initiate Treatment
        .WillOnce(Return(1))        // Do Not Withdraw
        .WillRepeatedly(Return(0)); // The Rest

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(2);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(1);
    EXPECT_CALL(*testPerson, InitiateTreatment()).Times(1);
    EXPECT_CALL(*testPerson, AddSVR()).Times(0);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(0);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Treatment", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(TreatmentTest, FinishTreatment) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(true));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(2));
    ON_CALL(*testPerson, IsGenotypeThree()).WillByDefault(Return(false));
    ON_CALL(*testPerson, IsCirrhotic()).WillByDefault(Return(false));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::LINKED));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.allow_retreatment", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.retreatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Duration Setup
    double duration = 2;
    Utils::tuple_3i tup_3i = std::make_tuple(0, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        dstorage;
    dstorage[tup_3i] = duration;
    ON_CALL(*event_dm, SelectCustomCallback(DURATION_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&dstorage), Return(0)));

    // Cost Setup
    double cost = 25.25;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        cstorage;
    cstorage[tup_3i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&cstorage), Return(0)));

    // SVR Setup
    double svr = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        sstorage;
    sstorage[tup_3i] = svr;
    ON_CALL(*event_dm, SelectCustomCallback(SVR_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&sstorage), Return(0)));

    // Toxicity Setup
    double toxicity = 1.0;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = toxicity;
    ON_CALL(*event_dm, SelectCustomCallback(TOXICITY_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&tstorage), Return(0)));

    // Withdrawal Setup
    double withdrawal = 0.9;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        wstorage;
    wstorage[tup_3i] = withdrawal;
    ON_CALL(*event_dm, SelectCustomCallback(WITHDRAWAL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&wstorage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))        // Do Not Withdraw
        .WillOnce(Return(0))        // Achieve SVR
        .WillRepeatedly(Return(0)); // The Rest

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(2);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(2);
    EXPECT_CALL(*testPerson, AddSVR()).Times(1);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(1);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(1);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Treatment", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(TreatmentTest, LostToFollowUp) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(false));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(0));
    ON_CALL(*testPerson, IsGenotypeThree()).WillByDefault(Return(false));
    ON_CALL(*testPerson, IsCirrhotic()).WillByDefault(Return(false));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::LINKED));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.allow_retreatment", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.retreatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Duration Setup
    double duration = 2;
    Utils::tuple_3i tup_3i = std::make_tuple(0, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        dstorage;
    dstorage[tup_3i] = duration;
    ON_CALL(*event_dm, SelectCustomCallback(DURATION_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&dstorage), Return(0)));

    // Cost Setup
    double cost = 25.25;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        cstorage;
    cstorage[tup_3i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&cstorage), Return(0)));

    // SVR Setup
    double svr = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        sstorage;
    sstorage[tup_3i] = svr;
    ON_CALL(*event_dm, SelectCustomCallback(SVR_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&sstorage), Return(0)));

    // Toxicity Setup
    double toxicity = 1.0;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = toxicity;
    ON_CALL(*event_dm, SelectCustomCallback(TOXICITY_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&tstorage), Return(0)));

    // Withdrawal Setup
    double withdrawal = 0.9;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        wstorage;
    wstorage[tup_3i] = withdrawal;
    ON_CALL(*event_dm, SelectCustomCallback(WITHDRAWAL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&wstorage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0)); // Lost To Follow-Up

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(1);
    EXPECT_CALL(*testPerson, AddSVR()).Times(0);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(0);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Treatment", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(TreatmentTest, Withdraw) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(true));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(5));
    ON_CALL(*testPerson, IsGenotypeThree()).WillByDefault(Return(false));
    ON_CALL(*testPerson, IsCirrhotic()).WillByDefault(Return(false));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::LINKED));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.allow_retreatment", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.retreatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Duration Setup
    double duration = 2;
    Utils::tuple_3i tup_3i = std::make_tuple(0, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        dstorage;
    dstorage[tup_3i] = duration;
    ON_CALL(*event_dm, SelectCustomCallback(DURATION_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&dstorage), Return(0)));

    // Cost Setup
    double cost = 25.25;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        cstorage;
    cstorage[tup_3i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&cstorage), Return(0)));

    // SVR Setup
    double svr = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        sstorage;
    sstorage[tup_3i] = svr;
    ON_CALL(*event_dm, SelectCustomCallback(SVR_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&sstorage), Return(0)));

    // Toxicity Setup
    double toxicity = 1.0;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = toxicity;
    ON_CALL(*event_dm, SelectCustomCallback(TOXICITY_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&tstorage), Return(0)));

    // Withdrawal Setup
    double withdrawal = 0.9;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        wstorage;
    wstorage[tup_3i] = withdrawal;
    ON_CALL(*event_dm, SelectCustomCallback(WITHDRAWAL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&wstorage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0))  // Withdraw
        .WillOnce(Return(1)); // Toxicity

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_, _, _))
        .Times(2); // Cost of Visit and Course
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(2); // Cost of Visit and Quit
    EXPECT_CALL(*testPerson, AddSVR()).Times(0);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(0);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);
    EXPECT_CALL(*testPerson, AddWithdrawal()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Treatment", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(TreatmentTest, DevelopToxicity) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(true));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(5));
    ON_CALL(*testPerson, IsGenotypeThree()).WillByDefault(Return(false));
    ON_CALL(*testPerson, IsCirrhotic()).WillByDefault(Return(false));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::LINKED));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.allow_retreatment", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.retreatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Duration Setup
    double duration = 2;
    Utils::tuple_3i tup_3i = std::make_tuple(0, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        dstorage;
    dstorage[tup_3i] = duration;
    ON_CALL(*event_dm, SelectCustomCallback(DURATION_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&dstorage), Return(0)));

    // Cost Setup
    double cost = 25.25;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        cstorage;
    cstorage[tup_3i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&cstorage), Return(0)));

    // SVR Setup
    double svr = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        sstorage;
    sstorage[tup_3i] = svr;
    ON_CALL(*event_dm, SelectCustomCallback(SVR_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&sstorage), Return(0)));

    // Toxicity Setup
    double toxicity = 1.0;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = toxicity;
    ON_CALL(*event_dm, SelectCustomCallback(TOXICITY_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&tstorage), Return(0)));

    // Withdrawal Setup
    double withdrawal = 0.9;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        wstorage;
    wstorage[tup_3i] = withdrawal;
    ON_CALL(*event_dm, SelectCustomCallback(WITHDRAWAL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&wstorage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0))  // Withdraw
        .WillOnce(Return(0)); // Experience Toxicity

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(3);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(3); // Visit, Quit, Toxicity
    EXPECT_CALL(*testPerson, AddSVR()).Times(0);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(0);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);
    EXPECT_CALL(*testPerson, AddToxicReaction()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Treatment", event_dm);
    event->Execute(testPerson, event_dm, decider);
}