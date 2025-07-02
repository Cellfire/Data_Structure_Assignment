//Add this function
//Waiting for the search algorithm to be implemented to call it in this
void exportDataToJSON(TransactionArray& array) {
    std::string exportType;
    std::cout << "Export what data? (full / sorted / search): ";
    std::getline(std::cin, exportType);

    std::string jsonFile;
    std::cout << "Enter filename (without extension): ";
    std::getline(std::cin, jsonFile);
    if (jsonFile.size() < 5 || jsonFile.substr(jsonFile.size() - 5) != ".json") jsonFile += ".json";
    if (toLower(exportType) == "full") {
        exportToJSON(array, jsonFile);
    } else if (toLower(exportType) == "sorted") {
        array.sortByLocation();
        exportToJSON(array, jsonFile);
    } else if (toLower(exportType) == "search") {
        std::string type;
        std::cout << "Enter transaction type to search and export: ";
        std::getline(std::cin, type);

        TransactionArray filtered;
        for (int i = 0; i < array.getSize(); ++i) {
            if (toLower(array.getData()[i]->transaction_type) == toLower(type)) {
                filtered.insert(array.getData()[i]);
            }
        }
        exportToJSON(filtered, jsonFile);
    } else {
        std::cout << "[ERROR] Invalid export type.\n";
    }
}




// The case 8 change to this:
case 8:
        {
            exportDataToJSON(array);
            break;
        }
