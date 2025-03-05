#include "Screening.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class ScreeningTest : public EventTest {};

std::string const BACKGROUND_SCREEN_QUERY =
    "SELECT at.age_years, sl.gender, sl.drug_behavior, "
    "background_screen_probability"
    " FROM antibody_testing AS at INNER JOIN "
    "screening_and_linkage AS sl ON ((at.age_years = "
    "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";

std::string const INTERVENTION_SCREEN_QUERY =
    "SELECT at.age_years, sl.gender, sl.drug_behavior, "
    "intervention_screen_probability"
    " FROM antibody_testing AS at INNER JOIN "
    "screening_and_linkage AS sl ON ((at.age_years = "
    "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";

TEST_F(ScreeningTest, FirstPeriodicScreening_TTtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(12));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(0));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    double sensitivity = 0.6;
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(sensitivity)), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));

    // Background Link Setup
    double link_prob = 0.5;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    // Decision to intervention screen
    std::vector<double> expected_link_prob = {link_prob};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob)).Times(1);
    // Screening test decisions
    std::vector<double> expected_sensitivity = {sensitivity};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(2);
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, SetLinkageType(person::LinkageType::INTERVENTION))
        .Times(1);
    EXPECT_CALL(*testPerson, DiagnoseHCV()).Times(1);
    EXPECT_CALL(*testPerson, Unlink()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Screening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, FirstPeriodicScreening_TFtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(7));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(0));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    double ab_sensitivity = 0.6;
    ON_CALL(*event_dm,
            GetFromConfig("screening_intervention_ab.acute_sensitivity", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(ab_sensitivity)), Return(0)));
    double rna_sensitivity = 0.4;
    ON_CALL(*event_dm,
            GetFromConfig("screening_intervention_rna.acute_sensitivity", _))
        .WillByDefault(DoAll(SetArgReferee<1>(std::to_string(rna_sensitivity)),
                             Return(0)));

    // Background Link Setup
    double link_prob = 0.5;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));
    ON_CALL(*decider, GetDecision(testing::ElementsAre(testing::Lt(0.5))))
        .WillByDefault(Return(1));

    // Expectations
    // decision to intervention screen
    std::vector<double> expected_link_prob = {link_prob};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob)).Times(1);
    // antibody screen result -- true
    std::vector<double> expected_sensitivity = {ab_sensitivity};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(1);
    // rna screen result -- false
    expected_sensitivity = {rna_sensitivity};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(1);
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    // because RNA false, no linking
    EXPECT_CALL(*testPerson, SetLinkageType(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Screening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, BackgroundScreening_TFtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(3));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    double ab_sensitivity = 0.6;
    ON_CALL(*event_dm,
            GetFromConfig("screening_background_ab.acute_sensitivity", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(ab_sensitivity)), Return(0)));
    double rna_sensitivity = 0.4;
    ON_CALL(*event_dm,
            GetFromConfig("screening_background_rna.acute_sensitivity", _))
        .WillByDefault(DoAll(SetArgReferee<1>(std::to_string(rna_sensitivity)),
                             Return(0)));

    // Background Link Setup
    double link_prob = 0.5;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));
    ON_CALL(*decider, GetDecision(testing::ElementsAre(testing::Lt(0.5))))
        .WillByDefault(Return(1));

    // Expectations
    // decision to background screen
    std::vector<double> expected_link_prob = {link_prob};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob)).Times(1);
    // antibody screen result -- true
    std::vector<double> expected_sensitivity = {ab_sensitivity};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(1);
    // rna screen result -- false
    expected_sensitivity = {rna_sensitivity};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(1);
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, SetLinkageType(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Screening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, BackgroundScreening_TTtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(3));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    double sensitivity = 0.6;
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(sensitivity)), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));

    // Background Link Setup
    double link_prob = 0.5;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    // Decision to intervention screen
    std::vector<double> expected_link_prob = {link_prob};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob)).Times(1);
    // Screening test decisions
    std::vector<double> expected_sensitivity = {sensitivity};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(2);
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, SetLinkageType(person::LinkageType::BACKGROUND))
        .Times(1);
    EXPECT_CALL(*testPerson, DiagnoseHCV()).Times(1);
    EXPECT_CALL(*testPerson, Unlink()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Screening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, NoScreen) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(3));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));

    // Background Link Setup
    double link_prob = 0.5;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_SCREEN_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_))
        .WillByDefault(Return(1)); // Do Not Background Screen

    // Expectations
    EXPECT_CALL(*testPerson, MarkScreened()).Times(0);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(0);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(0);
    EXPECT_CALL(*testPerson, SetLinkageType(_)).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Screening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
