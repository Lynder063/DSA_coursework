#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>

struct Vehicle {
    std::string plateNumber;
    std::string driverName;
    int penaltyPoints;
};

std::vector<Vehicle> vehicles;

const std::string FILENAME = "vehicles.txt";

// Funkce pro odstranění escape sekvencí
std::string unescape(const std::string& str) {
    std::string result;
    result.reserve(str.size());

    bool insideQuotes = false;

    for (char ch : str) {
        if (ch == '\"') {
            insideQuotes = !insideQuotes;
        }
        else if (insideQuotes && ch == '\\' && result.back() == '\"') {
            // Přeskočení escape sekvence pouze před uzavírací uvozovkou
        }
        else {
            result.push_back(ch);
        }
    }

    return result;
}

void saveToFile(const Vehicle& vehicle) {
    std::ofstream outFile(FILENAME, std::ios::app);

    if (!outFile.is_open()) {
        std::cerr << "Nelze otevrit soubor pro zapis.\n";
        return;
    }

    outFile << std::quoted(vehicle.plateNumber) << ',' << std::quoted(vehicle.driverName) << ',' << vehicle.penaltyPoints << ";\n";
    outFile.close();
}

void loadFromFile() {
    std::ifstream inFile(FILENAME);

    if (!inFile.is_open()) {
        std::cerr << "Soubor nebyl nalezen nebo nemohl byt otevren.\n";
        return;
    }

    vehicles.clear();

    std::string line;
    while (std::getline(inFile, line, ';')) {
        std::cout << "Nacteny radek: " << line << std::endl;

        std::istringstream iss(line);
        Vehicle vehicle;
        char comma; // Oddělovač

        // Pokusíme se načíst plateNumber
        if (!(iss >> std::quoted(vehicle.plateNumber) >> comma)) {
            std::cerr << "Chyba cteni ze souboru.\n";
            std::cerr << "Nepodarilo se nacist hodnotu plateNumber z radku: " << line << std::endl;
            break;
        }

        // Načítáme zbytek řádku jako jméno řidiče
        std::getline(iss >> comma, vehicle.driverName, ',');

        // Odstraníme escape sekvence v jménu řidiče
        vehicle.driverName = unescape(vehicle.driverName);

        // Odstraníme případné mezery na začátku jména a na konci jména
        vehicle.driverName = vehicle.driverName.substr(vehicle.driverName.find_first_not_of(" \t"), vehicle.driverName.find_last_not_of(" \t") + 1);

        // Pokusíme se načíst penaltyPoints
        if (!(iss >> vehicle.penaltyPoints)) {
            std::cerr << "Chyba cteni ze souboru.\n";
            std::cerr << "Nepodařilo se načíst hodnotu penaltyPoints z řádku: " << line << std::endl;
            break;
        }

        std::cout << "Nacteno: " << std::quoted(vehicle.plateNumber) << ", " << std::quoted(vehicle.driverName) << ", " << vehicle.penaltyPoints << std::endl;

        vehicles.push_back(vehicle);
    }

    inFile.close();
}

void displayAllVehicles() {
    std::cout << std::setw(25) << "SPZ" << std::setw(25) << "Ridic" << std::setw(25) << "Body za prestupky\n";
    std::cout << std::setfill('-') << std::setw(75) << "\n" << std::setfill(' ');

    for (const auto& vehicle : vehicles) {
        std::cout << std::setw(25) << vehicle.plateNumber << std::setw(25) << std::quoted(vehicle.driverName) << std::setw(25) << vehicle.penaltyPoints << "\n";
    }
}



void displayMenu() {
    std::cout << "Menu:\n";
    std::cout << "1. Pridat vozidlo\n";
    std::cout << "2. Vyhledat vozidlo podle ridice\n";
    std::cout << "3. Zobrazit vozidla s vysokym poctem bodu za prestupky\n";
    std::cout << "4. Ukoncit program\n";
}

void addVehicle() {
    Vehicle newVehicle;
    std::cout << "Zadejte registracni cislo vozidla: ";
    std::cin >> newVehicle.plateNumber;

    std::cin.ignore();

    std::cout << "Zadejte jmeno ridice: ";
    std::getline(std::cin, newVehicle.driverName);

    std::cout << "Zadejte pocet bodu za prestupky: ";
    std::cin >> newVehicle.penaltyPoints;

    vehicles.push_back(newVehicle);
    saveToFile(newVehicle);
}

void searchByDriver() {
    std::string driverName;
    std::cin.ignore();
    std::cout << "Zadejte jmeno ridice pro vyhledani: ";
    std::getline(std::cin, driverName);

    std::cout << std::setw(25) << "SPZ" << std::setw(25) << "Ridic" << std::setw(25) << "Body za přestupky\n";
    std::cout << std::setfill('-') << std::setw(75) << "\n" << std::setfill(' ');

    auto found = false;

    for (const auto& vehicle : vehicles) {
        if (vehicle.driverName.find(driverName) != std::string::npos) {
            std::cout << std::setw(25) << vehicle.plateNumber << std::setw(25) << vehicle.driverName << std::setw(25) << vehicle.penaltyPoints << "\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "Vozidlo nenalezeno.\n";
    }
}

void displayHighPenaltyVehicles() {
    int threshold;
    std::cout << "Zadejte limit bodu za prestupky: ";
    std::cin >> threshold;

    std::sort(vehicles.begin(), vehicles.end(), [](const Vehicle& a, const Vehicle& b) {
        return a.penaltyPoints > b.penaltyPoints;
    });

    std::cout << std::setw(25) << "SPZ" << std::setw(25) << "Ridic" << std::setw(25) << "Body za přestupky\n";
    std::cout << std::setfill('-') << std::setw(75) << "\n" << std::setfill(' ');

    for (size_t i = 0; i < std::min<size_t>(5, vehicles.size()); ++i) {
        std::cout << std::setw(25) << vehicles[i].plateNumber << std::setw(25) << vehicles[i].driverName << std::setw(25) << vehicles[i].penaltyPoints << "\n";
    }
}

int main() {
    loadFromFile();
    displayAllVehicles();

    int choice;
    do {
        displayMenu();
        std::cout << "Vyberte moznost (1-4): ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                addVehicle();
                break;
            case 2:
                searchByDriver();
                break;
            case 3:
                displayHighPenaltyVehicles();
                break;
            case 4:
                std::cout << "Program ukoncen.\n";
                break;
            default:
                std::cout << "Neplatna volba. Zkuste to znovu.\n";
        }
    } while (choice != 4);

    return 0;
}
