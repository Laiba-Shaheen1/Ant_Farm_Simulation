#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <random>
#include <sstream>
#include <map>

using namespace std;




class Ant {
public:
    virtual void performTask() = 0;
    virtual string getSpecies() const = 0;
    virtual ~Ant() = default;
};


class WorkerAnt : public Ant {
public:
    void performTask() override {
        cout << "Worker Ant is foraging for food." << endl;
    }

    string getSpecies() const override {
        return "Worker Ant";
    }
};

class SoldierAnt : public Ant {
public:
    void performTask() override {
        cout << "Soldier Ant is guarding the colony." << endl;
    }

    string getSpecies() const override {
        return "Soldier Ant";
    }
};

class QueenAnt : public Ant {
public:
    void performTask() override {
        cout << "Queen Ant is laying eggs." << endl;
    }

    string getSpecies() const override {
        return "Queen Ant";
    }
};


class AntCreator {
public:
    static unique_ptr<Ant> createAnt(const string& type) {
        if (type == "worker") {
            return make_unique<WorkerAnt>();
        } else if (type == "soldier") {
            return make_unique<SoldierAnt>();
        } else if (type == "queen") {
            return make_unique<QueenAnt>();
        }
        return nullptr;
    }
};



class Room {
public:
    virtual void addAnt(unique_ptr<Ant> ant) = 0;
    virtual void process() = 0;
    virtual ~Room() = default;
};

class WorkerRoom : public Room {
    vector<unique_ptr<Ant>> ants;

public:
    void addAnt(unique_ptr<Ant> ant) override {
        ants.push_back(std::move(ant));
    }

    void process() override {
        for (auto& ant : ants) {
            ant->performTask();
        }
    }
};


class AntFarm {
    string farmName;
    vector<unique_ptr<Room>> rooms;
    vector<unique_ptr<Ant>> colonyAnts;
    map<string, int> resources;
public:
    AntFarm(const string& name) : farmName(name) {}

    void addRoom(unique_ptr<Room> room) {
        rooms.push_back(std::move(room));
    }

    void addAnt(unique_ptr<Ant> ant) {
        colonyAnts.push_back(std::move(ant));
    }

    void addResource(const string& resource, int quantity) {
        resources[resource] += quantity;
    }

    void processAnts() {
        for (auto& room : rooms) {
            room->process();
        }
        for (auto& ant : colonyAnts) {
            ant->performTask();
        }
    }

    void displayInfo() {
        cout << "Ant Farm: " << farmName << endl;
        cout << "Resources: " << endl;
        for (const auto& res : resources) {
            cout << res.first << ": " << res.second << endl;
        }
        cout << "Ants: " << endl;
        for (const auto& ant : colonyAnts) {
            cout << " - " << ant->getSpecies() << endl;
        }
    }
};



class Meadow {
    unordered_map<int, unique_ptr<AntFarm>> antFarms;
    int nextFarmId = 1;
    static Meadow* instance;

    Meadow() {}

public:
    static Meadow* getInstance() {
        if (!instance) {
            instance = new Meadow();
        }
        return instance;
    }

    int createAntFarm(const string& species) {
        auto farm = make_unique<AntFarm>("Farm" + to_string(nextFarmId));
        for (int i = 0; i < 5; ++i) {
            auto ant = AntCreator::createAnt(species);
            farm->addAnt(std::move(ant));
        }
        int farmId = nextFarmId++;
        antFarms[farmId] = std::move(farm);
        return farmId;
    }

    void addResourceToFarm(int farmId, const string& resource, int quantity) {
        if (antFarms.find(farmId) != antFarms.end()) {
            antFarms[farmId]->addResource(resource, quantity);
        }
    }

    void processAllFarms() {
        for (auto& [id, farm] : antFarms) {
            farm->processAnts();
        }
    }

    bool isSimulationOver() {
        return antFarms.size() <= 1;
    }

    void showFarmInfo(int farmId) {
        if (antFarms.find(farmId) != antFarms.end()) {
            antFarms[farmId]->displayInfo();
        }
    }

    unordered_map<int, unique_ptr<AntFarm>>& getAllFarms() {
        return antFarms;
    }
};


Meadow* Meadow::instance = nullptr;



void handleCommand(const string& command) {
    Meadow* meadow = Meadow::getInstance();
    stringstream ss(command);
    string action;
    ss >> action;

    if (action == "spawn") {
        int x, y;
        string species;
        ss >> x >> y >> species;
        int farmId = meadow->createAntFarm(species);
        cout << "Ant Farm " << farmId << " created with species: " << species << " at position (" << x << ", " << y << ")." << endl;
    }
    else if (action == "give") {
        int farmId, amount;
        string resource;
        ss >> farmId >> resource >> amount;
        meadow->addResourceToFarm(farmId, resource, amount);
        cout << "Given " << amount << " " << resource << " to farm " << farmId << "." << endl;
    }
    else if (action == "tick") {
        meadow->processAllFarms();
        cout << "Performed a simulation tick." << endl;
    }
    else if (action == "summary") {
        int farmId;
        ss >> farmId;
        meadow->showFarmInfo(farmId);
    }
    else {
        cout << "Invalid command." << endl;
    }
}



int main() {
    string command;
    cout << "Welcome to the Ant Farm Simulation!" << endl;
    cout << "Commands available: spawn X Y T, give I R A, tick, summary I" << endl;

    while (true) {
        cout << "> ";
        getline(cin, command);
        if (command == "exit") {
            break;
        }
        handleCommand(command);
    }

    cout << "Simulation ended!" << endl;
    return 0;
}
