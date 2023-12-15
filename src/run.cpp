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
        sim.getGenerator(), tables["diseaseProgression"], config);
    sharedEvent infection = makeEvent<Event::Infections>(
        sim.getGenerator(), tables["incidence"], config);
    sharedEvent screen = makeEvent<Event::Screening>(
        sim.getGenerator(), tables["screenTestLink"], config);
    sharedEvent linking =
        makeEvent<Event::Linking>(tables["screenTestLink"], config);
    sharedEvent voluntaryRelink = makeEvent<Event::VoluntaryRelinking>(
        sim.getGenerator(), tables["blank"], config);
    sharedEvent fibrosisStaging =
        makeEvent<Event::FibrosisStaging>(tables["fibrosis"], config);
    sharedEvent treatment =
        makeEvent<Event::Treatment>(tables["blank"], config);
    sharedEvent overdose = makeEvent<Event::Overdose>(
        sim.getGenerator(), tables["overdose"], config);
    sharedEvent death =
        makeEvent<Event::Death>(sim.getGenerator(), tables["death"], config);

    personEvents.insert(personEvents.end(),
                        {aging, behavior, clearance, disease, infection, screen,
                         linking, voluntaryRelink, fibrosisStaging, treatment,
                         overdose, death});
}

void writeEvents(std::vector<sharedEvent> &personEvents, std::string dirpath) {}

void loadTables(std::unordered_map<std::string, Data::DataTable> &tables,
                std::string dirpath) {

    // Added blank table for events that do not need tabular data
    Data::DataTable blank;
    tables["blank"] = blank;

    // Costs and Utilities

    std::filesystem::path f =
        ((std::filesystem::path)dirpath) / "background_costs.csv";
    Data::DataTable backgroundCost(f);
    tables["backgroundCost"] = backgroundCost;

    f = ((std::filesystem::path)dirpath) / "behavior_costs.csv";
    Data::DataTable behaviorCosts(f);
    tables["behaviorCost"] = behaviorCosts;

    f = ((std::filesystem::path)dirpath) / "hcv_costs.csv";
    Data::DataTable hcv_cost(f);
    tables["hcvCost"] = hcv_cost;

    f = ((std::filesystem::path)dirpath) / "background_utilities.csv";
    Data::DataTable backgroundUtilities(f);
    tables["backgroundUtilities"] = backgroundUtilities;

    f = ((std::filesystem::path)dirpath) / "behavior_utilities.csv";
    Data::DataTable behaviorUtilities(f);
    tables["behaviorUtilities"] = behaviorUtilities;

    f = ((std::filesystem::path)dirpath) / "hcv_utilities.csv";
    Data::DataTable hcv_utilities(f);
    tables["hcvUtilities"] = hcv_utilities;

    // Events

    f = ((std::filesystem::path)dirpath) / "behavior_transitions.csv";
    Data::DataTable behaviorTransitions(f);
    tables["behaviorTransitions"] = behaviorTransitions;

    f = ((std::filesystem::path)dirpath) / "disease_progression.csv";
    Data::DataTable diseaseProgression;
    tables["diseaseProgression"] = diseaseProgression;

    f = ((std::filesystem::path)dirpath) / "fibrosis.csv";
    Data::DataTable fibrosis(f);
    tables["fibrosis"] = fibrosis;

    f = ((std::filesystem::path)dirpath) / "incidence.csv";
    Data::DataTable incidence(f);
    tables["incidence"] = incidence;

    f = ((std::filesystem::path)dirpath) / "screening_and_linkage.csv";
    Data::DataTable screen(f);
    tables["screenTestLink"] = screen;

    f = ((std::filesystem::path)dirpath) / "all_types_overdose.csv";
    Data::DataTable overdoses(f);
    tables["overdoses"] = overdoses;

    f = ((std::filesystem::path)dirpath) / "background_mortality.csv";
    Data::DataTable backgroundMortality(f);

    f = ((std::filesystem::path)dirpath) / "SMR.csv";
    Data::DataTable smr(f);

    f = ((std::filesystem::path)dirpath) / "fatal_overdose.csv";
    Data::DataTable fatalOverdoses(f);

    f = ((std::filesystem::path)dirpath) / "fibrosis_deaths.csv";
    Data::DataTable fibrosisDeaths(f);

    Data::DataTable death =
        backgroundMortality.innerJoin(smr, "gender", "gender");

    death = death.innerJoin(fatalOverdoses, "gender", "gender");
    death = death.innerJoin(fibrosisDeaths, "gender", "gender");

    tables["death"] = death;

    // People

    f = ((std::filesystem::path)dirpath) / "population.csv";
    Data::DataTable population(f);
    tables["population"] = population;
}

void loadPopulation(std::vector<sharedPerson> &population,
                    std::unordered_map<std::string, Data::DataTable> &tables,
                    Simulation::Simulation &sim) {
    if (tables.find("population") != tables.end()) {
        for (int rowIdx = 0;
             rowIdx < tables["population"].getShape().getNRows(); ++rowIdx) {
            population.push_back(
                makePerson<Person::Person>(tables["population"].getRow(rowIdx),
                                           (int)sim.getCurrentTimestep()));
        }
    }
}

void writePopulation(std::vector<sharedPerson> &population,
                     std::string dirpath) {
    Data::DataTable newDT;
    for (int i = 0; i < population.size(); ++i) {
        if (newDT.empty()) {
            newDT = personToDataTable(population[i]);
        } else {
            newDT = newDT + personToDataTable(population[i]);
        }
    }
    std::filesystem::path f =
        ((std::filesystem::path)dirpath) / "population.csv";

    newDT.toCSV(f.string());
}

Data::DataTable personToDataTable(sharedPerson &person) {
    std::map<std::string, std::vector<std::string>> data;
    std::vector<std::string> headerOrder = {"id",
                                            "sex",
                                            "age",
                                            "isAlive",
                                            "timeOfLastScreening",
                                            "screeningFrequency",
                                            "hasInterventionScreening",
                                            "timeIdentified",
                                            "identifiedAsPositive",
                                            "hepcState",
                                            "timeHEPCStateChanged",
                                            "seropositivity",
                                            "liverState",
                                            "timeLiverStateChanged",
                                            "measuredLiverState",
                                            "timeOfLastStaging",
                                            "drugBehavior",
                                            "timeLastActiveDrugUse",
                                            "linkageState",
                                            "timeOfLinkChange",
                                            "linkageType",
                                            "isOverdosed",
                                            "hasIncompleteTreatment",
                                            "MOUDState",
                                            "timeStartedMOUD",
                                            "pregnancyState",
                                            "timeOfPregnancyChange",
                                            "infantCount",
                                            "miscarriageCount"};
    data["id"] = {std::to_string(person->getID())};
    data["sex"] = {person->sexEnumToStringMap[person->getSex()]};
    data["age"] = {std::to_string(person->age)};
    data["isAlive"] = {boolToString(person->getIsAlive())};
    data["timeOfLastScreening"] = {
        std::to_string(person->getTimeOfLastScreening())};
    data["screeningFrequency"] = {
        std::to_string(person->getScreeningFrequency())};
    data["hasInterventionScreening"] = {
        boolToString(person->isInterventionScreened())};
    data["timeIdentified"] = {std::to_string(person->getTimeIdentified())};
    data["identifiedAsPositive"] = {
        boolToString(person->isIdentifiedAsInfected())};
    data["hepcState"] = {
        person->hepcStateEnumToStringMap[person->getHEPCState()]};
    data["timeHEPCStateChanged"] = {
        std::to_string(person->getTimeHEPCStateChanged())};
    data["seropositivity"] = {boolToString(person->getSeropositivity())};
    data["liverState"] = {
        person->liverStateEnumToStringMap[person->getLiverState()]};
    data["timeLiverStateChanged"] = {
        std::to_string(person->getTimeLiverStateChanged())};
    data["measuredLiverState"] = {person->measuredLiverStateEnumToStringMap
                                      [person->getMeasuredLiverState()]};
    data["timeOfLastStaging"] = {
        std::to_string(person->getTimeOfLastStaging())};
    data["drugBehavior"] = {person->behaviorClassificationEnumToStringMap
                                [person->getBehaviorClassification()]};
    data["timeLastActiveDrugUse"] = {
        std::to_string(person->getTimeBehaviorChange())};
    data["linkageState"] = {
        person->linkageStateEnumToStringMap[person->getLinkState()]};
    data["timeOfLinkChange"] = {std::to_string(person->getTimeOfLinkChange())};
    data["linkageType"] = {
        person->linkageTypeEnumToStringMap[person->getLinkageType()]};
    data["isOverdosed"] = {boolToString(person->getOverdose())};
    data["hasIncompleteTreatment"] = {
        boolToString(person->hadIncompleteTreatment())};
    data["MOUDState"] = {person->moudEnumToStringMap[person->getMoudState()]};
    data["timeStartedMOUD"] = {std::to_string(person->getTimeStartedMoud())};
    data["pregnancyState"] = {
        person->pregnancyStateEnumToStringMap[person->getPregnancyState()]};
    data["timeOfPregnancyChange"] = {
        std::to_string(person->getTimeOfPregnancyChange())};
    data["infantCount"] = {std::to_string(person->getInfantCount())};
    data["miscarriageCount"] = {std::to_string(person->getMiscarriageCount())};
    Data::DataTableShape newShape(1, 29);
    Data::DataTable newDT(data, newShape, headerOrder);
    return newDT;
}