#include "run.hpp"
#include <iostream>

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

int loadEvents(std::vector<Event::sharedEvent> &personEvents,
               std::unordered_map<std::string, Data::IDataTablePtr> &tables,
               Simulation::Simulation &sim, Data::Configuration &config,
               std::shared_ptr<spdlog::logger> logger) {
    personEvents.clear();

    std::vector<std::string> eventList =
        config.getStringVector("simulation.events");
    Event::EventFactory factory;
    for (std::string eventObj : eventList) {
        Event::sharedEvent ev;
        ev = factory.create(eventObj, tables[eventObj], config, logger,
                            sim.getGenerator());
        if (ev == nullptr) {
            logger->error("{} Event Failed to be Created!", eventObj);
            return -1;
        }
        personEvents.push_back(ev);
        logger->info("{} Event Created", eventObj);
    }
    return 0;
}

void writeEvents(std::vector<Event::sharedEvent> &personEvents,
                 std::string dirpath) {}

int loadTables(std::unordered_map<std::string, Data::IDataTablePtr> &tables,
               std::string dirpath) {

    // Added blank table for events that do not need tabular data
    Data::IDataTablePtr blank;
    tables["blank"] = blank;
    tables["Aging"] = blank;
    tables["Clearance"] = blank;
    tables["VoluntaryRelinking"] = blank;
    tables["Treatment"] = blank;

    // Costs and Utilities

    std::filesystem::path f =
        ((std::filesystem::path)dirpath) / "background_costs.csv";
    Data::IDataTablePtr backgroundCost = std::make_shared<Data::DataTable>(f);
    tables["backgroundCost"] = backgroundCost;

    f = ((std::filesystem::path)dirpath) / "behavior_costs.csv";
    Data::IDataTablePtr behaviorCosts = std::make_shared<Data::DataTable>(f);
    tables["behaviorCost"] = behaviorCosts;

    f = ((std::filesystem::path)dirpath) / "hcv_costs.csv";
    Data::IDataTablePtr hcv_cost = std::make_shared<Data::DataTable>(f);
    tables["hcvCost"] = hcv_cost;

    f = ((std::filesystem::path)dirpath) / "background_utilities.csv";
    Data::IDataTablePtr backgroundUtilities =
        std::make_shared<Data::DataTable>(f);
    tables["backgroundUtilities"] = backgroundUtilities;

    f = ((std::filesystem::path)dirpath) / "behavior_utilities.csv";
    Data::IDataTablePtr behaviorUtilities =
        std::make_shared<Data::DataTable>(f);
    tables["behaviorUtilities"] = behaviorUtilities;

    f = ((std::filesystem::path)dirpath) / "hcv_utilities.csv";
    Data::IDataTablePtr hcv_utilities = std::make_shared<Data::DataTable>(f);
    tables["hcvUtilities"] = hcv_utilities;

    // Events

    f = ((std::filesystem::path)dirpath) / "behavior_transitions.csv";
    Data::IDataTablePtr behaviorTransitions =
        std::make_shared<Data::DataTable>(f);
    tables["BehaviorChanges"] = behaviorTransitions;

    f = ((std::filesystem::path)dirpath) / "disease_progression.csv";
    Data::IDataTablePtr diseaseProgression =
        std::make_shared<Data::DataTable>(f);
    tables["DiseaseProgression"] = diseaseProgression;

    f = ((std::filesystem::path)dirpath) / "fibrosis.csv";
    Data::IDataTablePtr fibrosis = std::make_shared<Data::DataTable>(f);
    tables["FibrosisStaging"] = fibrosis;

    f = ((std::filesystem::path)dirpath) / "incidence.csv";
    Data::IDataTablePtr incidence = std::make_shared<Data::DataTable>(f);
    tables["Infections"] = incidence;

    f = ((std::filesystem::path)dirpath) / "screening_and_linkage.csv";
    Data::IDataTablePtr screen = std::make_shared<Data::DataTable>(f);
    tables["Screening"] = screen;

    tables["Linking"] = screen;

    f = ((std::filesystem::path)dirpath) / "all_types_overdose.csv";
    Data::IDataTablePtr overdoses = std::make_shared<Data::DataTable>(f);
    tables["Overdose"] = overdoses;

    f = ((std::filesystem::path)dirpath) / "background_mortality.csv";
    Data::IDataTablePtr backgroundMortality =
        std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "SMR.csv";
    Data::IDataTablePtr smr = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "fatal_overdose.csv";
    Data::IDataTablePtr fatalOverdoses = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "fibrosis_deaths.csv";
    Data::IDataTablePtr fibrosisDeaths = std::make_shared<Data::DataTable>(f);

    Data::IDataTablePtr death =
        backgroundMortality->innerJoin(smr, "gender", "gender");

    death = death->innerJoin(fatalOverdoses, "gender", "gender");
    death = death->innerJoin(fibrosisDeaths, "gender", "gender");

    tables["Death"] = death;

    // People

    f = ((std::filesystem::path)dirpath) / "population.csv";
    Data::IDataTablePtr population = std::make_shared<Data::DataTable>(f);
    tables["population"] = population;
    return 0;
}

int loadPopulation(std::vector<sharedPerson> &population,
                   std::unordered_map<std::string, Data::IDataTablePtr> &tables,
                   Simulation::Simulation &sim) {
    if (tables.find("population") != tables.end()) {
        for (int rowIdx = 0;
             rowIdx < tables["population"]->getShape().getNRows(); ++rowIdx) {
            population.push_back(
                makePerson<Person::Person>(tables["population"]->getRow(rowIdx),
                                           (int)sim.getCurrentTimestep()));
        }
    }
    return 0;
}

void writePopulation(std::vector<sharedPerson> &population,
                     std::string dirpath) {
    Data::IDataTablePtr newDT = std::make_shared<Data::DataTable>();
    for (int i = 0; i < population.size(); ++i) {
        if (newDT->empty()) {
            newDT = personToDataTable(population[i]);
        } else {
            newDT = (*newDT) + (*personToDataTable(population[i]));
        }
    }
    std::filesystem::path f =
        ((std::filesystem::path)dirpath) / "population.csv";

    newDT->toCSV(f.string());
}

Data::IDataTablePtr personToDataTable(sharedPerson &person) {
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
    Data::IDataTablePtr newDT =
        std::make_shared<Data::DataTable>(data, newShape, headerOrder);
    return newDT;
}
