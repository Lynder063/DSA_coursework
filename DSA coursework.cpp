#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

struct Vehicle {
    std::string plateNumber;
    std::string driverName;
    int penaltyPoints;
};

struct VehicleNode {
    Vehicle data;
    VehicleNode* next;
    VehicleNode(const Vehicle& v, VehicleNode* n = nullptr) : data(v), next(n) {}
};

VehicleNode* head = nullptr;

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \n\r\t");
    size_t last = str.find_last_not_of(" \n\r\t");
    if (first == std::string::npos || last == std::string::npos)
        return str;
    return str.substr(first, (last - first + 1));
}
void addVehicleNode(const Vehicle& vehicle) {
    VehicleNode* newNode = new VehicleNode(vehicle);
    if (!head) {
        head = newNode;
    }
    else {
        VehicleNode* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void loadFromFile() {
    std::ifstream inFile("vehicles.txt");
    if (!inFile.is_open()) {
        std::cerr << "Nelze otevrit soubor pro cteni.\n";
        return;
    }

    std::string line;
    while (std::getline(inFile, line, ';')) {
        std::istringstream iss(line);
        Vehicle vehicle;
        char comma;

        if (std::getline(iss, vehicle.plateNumber, ',') &&
            std::getline(iss, vehicle.driverName, ',') &&
            (iss >> vehicle.penaltyPoints)) {

            vehicle.plateNumber = trim(vehicle.plateNumber);
            vehicle.driverName = trim(vehicle.driverName);

                addVehicleNode(vehicle);
        }
    }
    inFile.close();
}

void saveAllVehiclesToFile() {
    std::ofstream outFile("vehicles.txt", std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Nelze otevrit soubor pro zapis.\n";
        return;
    }
    for (VehicleNode* current = head; current != nullptr; current = current->next) {
        outFile << current->data.plateNumber << ","
            << current->data.driverName << ","
            << current->data.penaltyPoints;
        if (current->next != nullptr) {
            outFile << ";";
        }
    }
    outFile.close();
}

void displayAllVehicles() {
    const int plateWidth = 15;
    const int nameWidth = 25;
    const int pointsWidth = 15;

    // Print headers
    std::cout << "SPZ" + std::string(plateWidth - std::string("SPZ").length(), ' ')
        << "Ridic" + std::string(nameWidth - std::string("Ridic").length(), ' ')
        << "Prestupky\n";

    // Print a line under headers
    std::cout << std::string(plateWidth + nameWidth + pointsWidth, '-') << "\n";

    for (VehicleNode* current = head; current != nullptr; current = current->next) {
        std::cout << current->data.plateNumber + std::string(plateWidth - current->data.plateNumber.length(), ' ')
            << current->data.driverName + std::string(nameWidth - current->data.driverName.length(), ' ')
            << std::to_string(current->data.penaltyPoints) + std::string(pointsWidth - std::to_string(current->data.penaltyPoints).length(), ' ') << "\n";
    }
    std::cout << "\n";
}

void deleteVehicleByPlateNumber(const std::string& plateNumber) {
    VehicleNode* current = head, * prev = nullptr;
    bool found = false;
    while (current != nullptr) {
        if (current->data.plateNumber == plateNumber) {
            if (prev == nullptr) {
                head = current->next;
            }
            else {
                prev->next = current->next;
            }
            delete current;
            found = true;
            break;
        }
        prev = current;
        current = current->next;
    }
    if (found) {
        saveAllVehiclesToFile();
        std::cout << "Vozidlo s registracnim cislem " << plateNumber << " odstraneno.\n";
    }
    else {
        std::cout << "Vozidlo s registracnim cislem " << plateNumber << " nenalezeno.\n";
    }
}

void addVehicle() {
    Vehicle newVehicle;
    std::cout << "Zadejte registracni cislo vozidla / SPZ: ";
    std::cin >> newVehicle.plateNumber;
    std::cin.ignore();
    std::cout << "Zadejte jmeno ridice: ";
    std::getline(std::cin, newVehicle.driverName);
    std::cout << "Zadejte pocet bodu za prestupky: ";
    std::cin >> newVehicle.penaltyPoints;

    VehicleNode* newNode = new VehicleNode(newVehicle);
    if (!head) {
        head = newNode;
    }
    else {
        VehicleNode* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }

    saveAllVehiclesToFile();
}

void deleteLinkedList() {
    VehicleNode* current = head;
    while (current) {
        VehicleNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
}
void displayVehiclesWithPenaltyPointsAbove(int threshold) {
    const int plateWidth = 15;
    const int nameWidth = 25;
    const int pointsWidth = 15;

    std::cout << "Vozidla s " << threshold << " nebo vetsim poctem prestupku:\n";
    std::cout << "SPZ" + std::string(plateWidth - std::string("SPZ").length(), ' ')
        << "Ridic" + std::string(nameWidth - std::string("Ridic").length(), ' ')
        << "Prestupky\n";

    // Print a line under headers
    std::cout << std::string(plateWidth + nameWidth + pointsWidth, '-') << "\n";

    bool found = false;
    for (VehicleNode* current = head; current != nullptr; current = current->next) {
        if (current->data.penaltyPoints >= threshold) {
            std::cout << current->data.plateNumber + std::string(plateWidth - current->data.plateNumber.length(), ' ')
                << current->data.driverName + std::string(nameWidth - current->data.driverName.length(), ' ')
                << std::to_string(current->data.penaltyPoints) + std::string(pointsWidth - std::to_string(current->data.penaltyPoints).length(), ' ') << "\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "Neni zadne vozidlo s " << threshold << " nebo vice prestupky.\n";
    }
    std::cout << "\n";
}
void displaySimilarDriverNames(const std::string& searchName) {
    const int nameWidth = 25;
    const int plateWidth = 15;
    const int pointsWidth = 15;

    std::cout << "Ridic" + std::string(nameWidth - std::string("Ridic").length(), ' ')
        << "SPZ" + std::string(plateWidth - std::string("SPZ").length(), ' ')
        << "Prestupky\n";

    // Print a line under headers
    std::cout << std::string(nameWidth + plateWidth + pointsWidth, '-') << "\n";

    bool found = false;
    for (VehicleNode* current = head; current != nullptr; current = current->next) {
        if (current->data.driverName.find(searchName) != std::string::npos) {
            std::cout << current->data.driverName + std::string(nameWidth - current->data.driverName.length(), ' ')
                << current->data.plateNumber + std::string(plateWidth - current->data.plateNumber.length(), ' ')
                << std::to_string(current->data.penaltyPoints) + std::string(pointsWidth - std::to_string(current->data.penaltyPoints).length(), ' ') << "\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "Zadny podobny ridic nebyl nalezen.\n";
    }
    std::cout << "\n";
}
int main() {
    loadFromFile();
    int choice;
    do {
        std::cout << "1. Pridat vozidlo\n";
        std::cout << "2. Odstranit vozidlo\n";
        std::cout << "3. Vypis vozidel\n";
        std::cout << "4. Vypis vozidel, ridicu a bodu za prestupky\n";
        std::cout << "5. Vyhledat vozidlo podle ridice\n";
        std::cout << "6. Konec programu\n";
        std::cout << "Vyberte moznost (1-6): ";
        std::cout << "\n";

        while (!(std::cin >> choice)) {
            std::cout << "Neplatny vstup. Vyberte moznost (1-6): ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch (choice) {
        case 1:
            addVehicle();
            break;
        case 2: {
            std::string plateNumber;
            std::cout << "Zadejte registracni cislo vozidla k odstraneni: ";
            std::cin >> plateNumber;
            deleteVehicleByPlateNumber(plateNumber);
            break;
        }
        case 3:
            displayAllVehicles();
            break;
        case 4:
            int threshold;
            std::cout << "Zadejte limit bodu za prestupky: ";
            std::cin >> threshold;
            displayVehiclesWithPenaltyPointsAbove(threshold);
            break;
        case 5: {
            std::string name;
            std::cout << "Zadejte jmeno ridice pro vyhledani: ";
            std::cin.ignore(); // Clear the input buffer
            std::getline(std::cin, name);
            displaySimilarDriverNames(name);
            break;
        }
        case 6:
            std::cout << "Konec programu.\n";
            break;
        default:
            std::cout << "Neplatna volba. Zkuste to znovu.\n";
        }
    } while (choice != 6);

    deleteLinkedList();
    return 0;
}