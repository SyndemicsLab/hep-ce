#include "run.hpp"

bool argChecks(int argc, char **argv, std::string &rootInputDir, int &taskStart,
               int &taskEnd) {
    if (argc > 1 && argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << "[INPUT FOLDER] [RUN START] [RUN END]\n\n"
                  << "HEP-CE, a microsimulation studying individuals with HCV";
        return false;
    }

    if (argc == 1) {
        std::cout << "Please provide the input folder path: ";
        std::cin >> rootInputDir;
        std::cout << std::endl
                  << "Please provide the first input folder number: ";
        std::cin >> taskStart;
        std::cout << std::endl
                  << "Please provide the last input folder number: ";
        std::cin >> taskEnd;
    } else {
        taskStart = std::stoi(argv[2]);
        taskEnd = std::stoi(argv[3]);
        rootInputDir = argv[1];
    }
    return true;
}

void loadEvents(std::vector<sharedEvent> &personEvents,
                std::unordered_map<std::string, Data::DataTable> &tables,
                Simulation::Simulation &sim, Data::Configuration &config) {

    sharedEvent aging = makeEvent<Event::Aging>(tables["blank"], config);
    sharedEvent behavior = makeEvent<Event::BehaviorChanges>(
        sim.getGenerator(), tables["behaviorTransitions"], config);
    sharedEvent clearance = makeEvent<Event::Clearance>(
        sim.getGenerator(), tables["blank"], config);
    sharedEvent disease = makeEvent<Event::DiseaseProgression>(
        sim.getGenerator(), tables["disease"], config);
    sharedEvent infection = makeEvent<Event::Infections>(
        sim.getGenerator(), tables["incidence"], config);
    sharedEvent backgroundScreen = makeEvent<Event::Screening>(
        sim.getGenerator(), tables["screenTestLink"], config);
    sharedEvent linking =
        makeEvent<Event::ScreenageLinking>(tables["screenTestLink"], config);
    sharedEvent voluntaryRelink = makeEvent<Event::VoluntaryRelinking>(
        sim.getGenerator(), tables["blank"], config);
    sharedEvent fibrosis =
        makeEvent<Event::FibrosisStaging>(tables["fibrosis"], config);
    sharedEvent treatment =
        makeEvent<Event::Treatment>(tables["blank"], config);
    sharedEvent overdose = makeEvent<Event::Overdose>(
        sim.getGenerator(), tables["overdose"], config);
    sharedEvent death =
        makeEvent<Event::Death>(sim.getGenerator(), tables["death"], config);

    personEvents.insert(personEvents.end(),
                        {aging, overdose, death, behavior, clearance, disease,
                         infection, voluntaryRelink, backgroundScreen, linking,
                         fibrosis, treatment});
}

void loadTables(std::unordered_map<std::string, Data::DataTable> &tables,
                std::string dirpath) {

    std::filesystem::path f =
        ((std::filesystem::path)dirpath) / "antibody_testing.csv";
    Data::DataTable antibody(f);
    tables["antibody"] = antibody;

    f = ((std::filesystem::path)dirpath) / "background_costs.csv";
    Data::DataTable backgroundCost(f);
    tables["backgroundCost"] = backgroundCost;

    f = ((std::filesystem::path)dirpath) / "background_mortality.csv";
    Data::DataTable backgroundMortality(f);
    tables["backgroundMortality"] = backgroundMortality;

    f = ((std::filesystem::path)dirpath) / "background_utilities.csv";
    Data::DataTable backgroundUtilities(f);
    tables["backgroundUtilities"] = backgroundUtilities;

    f = ((std::filesystem::path)dirpath) / "behavior_costs.csv";
    Data::DataTable behaviorCosts(f);
    tables["behaviorCosts"] = behaviorCosts;

    f = ((std::filesystem::path)dirpath) / "behavior_transitions.csv";
    Data::DataTable behaviorTransitions(f);
    tables["behaviorTransitions"] = behaviorTransitions;

    f = ((std::filesystem::path)dirpath) / "behavior_utilities.csv";
    Data::DataTable behaviorUtilities(f);
    tables["behaviorUtilities"] = behaviorUtilities;

    f = ((std::filesystem::path)dirpath) / "fibrosis.csv";
    Data::DataTable fibrosis(f);
    tables["fibrosis"] = fibrosis;

    f = ((std::filesystem::path)dirpath) / "hcv_costs.csv";
    Data::DataTable hcv_cost(f);
    tables["hcvCost"] = hcv_cost;

    f = ((std::filesystem::path)dirpath) / "hcv_utilities.csv";
    Data::DataTable hcv_utilities(f);
    tables["hcvUtilities"] = hcv_utilities;

    f = ((std::filesystem::path)dirpath) / "incidence.csv";
    Data::DataTable incidence(f);
    tables["incidence"] = incidence;

    f = ((std::filesystem::path)dirpath) / "population.csv";
    Data::DataTable population(f);
    tables["population"] = population;

    f = ((std::filesystem::path)dirpath) / "screening_and_linkage.csv";
    Data::DataTable screen(f);
    tables["screenTestLink"] = screen;

    f = ((std::filesystem::path)dirpath) / "SMR.csv";
    Data::DataTable smr(f);
    tables["SMR"] = smr;

    Data::DataTable blank;
    tables["blank"] = blank;
}

void loadPopulation(std::vector<sharedPerson> &population,
                    std::unordered_map<std::string, Data::DataTable> &tables,
                    Simulation::Simulation &sim) {
    if (tables.find("population") != tables.end()) {
        for (int rowIdx = 0;
             rowIdx < tables["population"].getShape().getNRows(); ++rowIdx) {
            population.push_back(makePerson<Person::Person>(
                tables["population"][rowIdx], (int)sim.getCurrentTimestep()));
        }
    }
}