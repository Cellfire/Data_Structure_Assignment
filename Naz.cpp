void showMenu() {
    std::cout << "\n=== Transaction Manager ===\n";
    std::cout << "1. Load CSV Data\n";
    std::cout << "2. Print All Transactions\n";
    std::cout << "3. Run Benchmark\n";
    std::cout << "4. Sort Array by Location\n";
    std::cout << "5. Search Transaction Type (Array)\n";
    std::cout << "6. Search Transaction Type (List)\n";
    std::cout << "7. Compare Performance (Array vs Linked List)\n";
    std::cout << "8. Export to JSON\n";
    std::cout << "9. Exit\n";
    std::cout << "Enter choice: ";
}


size_t estimateArrayMemory(const TransactionArray& arr) {
    return arr.getSize() * sizeof(Transaction*) + arr.getSize() * sizeof(Transaction);
}

size_t estimateListMemory(const TransactionList& list) {
    size_t total = 0;
    Node* current = *((Node**)(&list)); 
    while (current) {
        total += sizeof(Node) + sizeof(Transaction);
        current = current->next;
    }
    return total;
}





int main() {
    TransactionArray array;
    TransactionList fullList, cardList, achList, upiList, wireList;
    int choice;
    std::string filename;

    do {
        showMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1:
                std::cout << "Enter CSV filename: ";
                std::getline(std::cin, filename);
                loadCSV(array, fullList, cardList, achList, upiList, wireList, filename);
                std::cout << "[DEBUG] Array size after load: " << array.getSize() << "\n";
                break;
            case 2:
                std::cout << "\n-- Array Data --\n";
                array.print();
                std::cout << "\n-- Full Linked List --\n";
                fullList.print();
                break;
            case 3:
                array.benchmarkOperation();

                 std::cout << "Traversal benchmark (cardList): ";
                 cardList.benchmarkOperation();

                std::cout << "Traversal benchmark (achList): ";
                achList.benchmarkOperation();

                std::cout << "Traversal benchmark (upiList): ";
                upiList.benchmarkOperation();

                std::cout << "Traversal benchmark (wireList): ";
                wireList.benchmarkOperation();
                break;
            case 4:
                std::cout << "[DEBUG] Running Option 4: Sorting now...\n";
                array.sortByLocation();
                std::cout << "[DEBUG] Sort complete, printing first 20...\n";
                array.print(100000); 
                break;
            case 5: {
                std::string type;
                std::cout << "Enter transaction type to search (array): ";
                std::getline(std::cin, type);
                array.searchTransactionType(type);
                break;
            }
            case 6: {
                std::string type;
                std::cout << "Enter transaction type to search (list): ";
                std::getline(std::cin, type);
                fullList.searchTransactionType(type);
                break;
            }
            case 7:{
                std::cout << "\n=== PERFORMANCE COMPARISON ===\n";

                 auto startArray = std::chrono::high_resolution_clock::now();
                 array.benchmarkOperation();
                 auto endArray = std::chrono::high_resolution_clock::now();

                 auto startList = std::chrono::high_resolution_clock::now();
                 fullList.benchmarkOperation();
                 auto endList = std::chrono::high_resolution_clock::now();

                 size_t arrayMem = estimateArrayMemory(array);
                 size_t listMem = estimateListMemory(fullList);

                 std::cout << "\n=== MEMORY USAGE ===\n";
                 std::cout << "Array Memory Usage: " << arrayMem / 1024.0 << " KB\n";
                 std::cout << "List  Memory Usage: " << listMem / 1024.0 << " KB\n";
                 break;
            }

            case 8: {
                 std::string jsonFile;
                 std::cout << "Enter filename to export (e.g., data.json): ";
                 std::getline(std::cin, jsonFile);
                 exportToJSON(array, jsonFile);
                break;
            }
            case 9: {
                std::cout << "Exiting program.\n";
                break;
                }
       
            default:
                std::cout << "Invalid choice.\n";
        }
    } while (choice != 9);

    return 0;
}