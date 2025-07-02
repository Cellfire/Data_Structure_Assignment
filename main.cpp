#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include "json.hpp"

#define MAX_TRANSACTIONS 50000
#define MAX_TOTAL_RECORDS 50000
#define MAX_LIST_LIMIT 50000

using json = nlohmann::json;

std::string toLower(const std::string &str)
{
    std::string lowerStr = str;
    for (char &ch : lowerStr)
        ch = std::tolower(static_cast<unsigned char>(ch));
    return lowerStr;
}

struct Transaction
{
    std::string transaction_id;
    std::string timestamp;
    std::string sender_account;
    std::string receiver_account;
    double amount;
    std::string transaction_type;
    std::string merchant_category;
    std::string location;
    std::string device_used;
    bool is_fraud;
    std::string fraud_type;
    double time_since_last_transaction;
    double spending_deviation_score;
    double velocity_score;
    double geo_anomaly_score;
    std::string payment_channel;
    std::string ip_address;
    std::string device_hash;
};

struct Node
{
    Transaction *data;
    Node *next;
};

class TransactionList
{
private:
    Node *head;
    Node *tail;

    static Node* merge(Node* left, Node* right) {
    if (!left) return right;
    if (!right) return left;

    Node* result;
    if (left->data->location < right->data->location) {
        result = left;
        result->next = merge(left->next, right);
    } else {
        result = right;
        result->next = merge(left, right->next);
    }
    return result;
}

static void split(Node* source, Node** front, Node** back) {
    Node* slow = source;
    Node* fast = source->next;

    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = nullptr;
}

static void mergeSort(Node** headRef) {
    Node* head = *headRef;
    if (!head || !head->next) return;

    Node* a;
    Node* b;

    split(head, &a, &b);
    mergeSort(&a);
    mergeSort(&b);

    *headRef = merge(a, b);
}

public:
    TransactionList() : head(nullptr), tail(nullptr) {}

    void append(Transaction *t)
    {
        Node *newNode = new Node{t, nullptr};
        if (!head)
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
    }


void print(int limit = 20) const {
    Node *temp = head;
    int count = 0;

    while (temp && count < limit) {
        std::cout << temp->data->transaction_id << "|"
                  << temp->data->timestamp << "|"
                  << temp->data->sender_account << "|"
                  << temp->data->receiver_account << "|"
                  << temp->data->amount << "|"
                  << temp->data->payment_channel << "|"
                  << temp->data->location << "\n";
        temp = temp->next;
        ++count;
    }
}

void sortByLocation() {
    auto start = std::chrono::high_resolution_clock::now();
    mergeSort(&head);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] Linked List sorted by location (ascending) using MergeSort.\n";
    std::cout << "[DEBUG] Sort time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";
}

    void benchmarkOperation() const
    {
        auto start = std::chrono::high_resolution_clock::now();
        Node *temp = head;
        while (temp)
        {
            volatile double x = temp->data->amount * 2.0;
            temp = temp->next;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Traversal benchmark (Linked List): "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                  << " ns\n";
    }





void searchTransactionType(const std::string &type) const
{
    std::string searchType = toLower(type);
    std::cout << "Searching for transaction type: " << type << "\n";

    std::cout << std::left
              << std::setw(12) << "ID"
              << std::setw(20) << "Transaction Type"
              << std::setw(20) << "Location" << "\n";
    std::cout << std::string(52, '-') << "\n";

    Node *temp = head;
    bool found = false;

    while (temp)
    {
        if (toLower(temp->data->transaction_type) == searchType)
        {
            std::cout << std::left
                      << std::setw(12) << temp->data->transaction_id
                      << std::setw(20) << temp->data->transaction_type
                      << std::setw(20) << temp->data->location << "\n";
            found = true;
        }
        temp = temp->next;
    }

    

    if (!found)
    {
        std::cout << "[INFO] No transactions found with transaction type: " << type << "\n";
    }

    
}

long long benchmarkSearch(const std::string& type) const {
    std::string searchType = toLower(type);
    auto start = std::chrono::high_resolution_clock::now();

    Node* temp = head;
    while (temp) {
        if (toLower(temp->data->transaction_type) == searchType) {
            volatile auto tmp = temp->data->amount;
        }
        temp = temp->next;
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}


    ~TransactionList()
    {
        Node *temp = head;
        while (temp)
        {
            Node *toDelete = temp;
            temp = temp->next;
            delete toDelete->data;
            delete toDelete;
        }
    }
};

    // Merge Sort for Linked List by Location****************
Node* merge(Node* left, Node* right) {
    if (!left) return right;
    if (!right) return left;

    Node* result;
    if (left->data->location < right->data->location) {
        result = left;
        result->next = merge(left->next, right);
    } else {
        result = right;
        result->next = merge(left, right->next);
    }
    return result;
}

// Splits list into two halves
void split(Node* source, Node** front, Node** back) {
    Node* slow = source;
    Node* fast = source->next;

    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = nullptr;
}

// Recursive Merge Sort for Linked List
void mergeSort(Node** headRef) {
    Node* head = *headRef;
    if (!head || !head->next) return;

    Node* a;
    Node* b;

    split(head, &a, &b);

    mergeSort(&a);
    mergeSort(&b);

    *headRef = merge(a, b);
}
//////////////*********************************************** */

class TransactionArray
{
private:
    Transaction **data;
    int size;
    int capacity;

public:
    TransactionArray(int initialSize = MAX_TRANSACTIONS) : size(0), capacity(initialSize)
    {
        data = new Transaction *[capacity];
    }

    Transaction **getData() const { return data; }
    int getSize() const { return size; }

    void insert(Transaction *t)
    {
        if (size == capacity)
        {
            std::cerr << "[ERROR] Max array capacity reached. Aborting load.\n";
            exit(EXIT_FAILURE);
        }
        data[size++] = t;
    }

    void quickSort(int left, int right)
    {
        if (left >= right)
            return;

        std::string pivot = data[(left + right) / 2]->location;
        int i = left;
        int j = right;

        while (i <= j)
        {
            while (data[i]->location < pivot)
                i++;
            while (data[j]->location > pivot)
                j--;

            if (i <= j)
            {
                std::swap(data[i], data[j]);
                i++;
                j--;
            }
        }

        if (left < j)
            quickSort(left, j);
        if (i < right)
            quickSort(i, right);
    }

    void print(int limit = 20) const
    {
        std::cout << "[INFO] Showing first " << limit << " sorted transactions:\n";
        for (int i = 0; i < size && i < limit; ++i)
        {
            std::cout << data[i]->transaction_id << " | "
                      << data[i]->transaction_type << " | "
                      << data[i]->timestamp << " | "
                      << data[i]->sender_account << " | "
                      << data[i]->receiver_account << " | "
                      << data[i]->amount << " | "
                      << data[i]->payment_channel << " | "
                      << data[i]->location << "\n";
        }
    }

    void benchmarkOperation() const
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < size; ++i)
        {
            volatile double x = data[i]->amount * 2.0;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Traversal benchmark (Array): "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                  << " ns\n";
    }


    void sortByLocation()
    {
        auto start = std::chrono::high_resolution_clock::now();
        quickSort(0, size - 1);
        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "[INFO] Sorted by location (ascending) using QuickSort.\n";
        std::cout << "[DEBUG] Sort time: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << " ms\n";
    }

    void searchTransactionType(const std::string &type) const
    {
        std::string searchType = toLower(type);
        std::cout << "Searching for transaction type: " << type << "\n";

        bool found = false;
        for (int i = 0; i < size; ++i)
        {
            if (toLower(data[i]->transaction_type) == searchType)
            {
                std::cout << data[i]->transaction_id << " | "
                          << data[i]->transaction_type << " | "
                          << data[i]->location << "\n";
                found = true;
            }
        }

        if (!found)
        {
            std::cout << "[INFO] No transactions found with payment channel: " << type << "\n";
        }
    }

    long long benchmarkSearch(const std::string& type) const {
    std::string searchType = toLower(type);
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < size; ++i) {
        if (toLower(data[i]->transaction_type) == searchType) {
            volatile auto tmp = data[i]->amount;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}
};



void loadCSV(TransactionArray &array, TransactionList &fullList,
             TransactionList &cardList, TransactionList &achList,
             TransactionList &upiList, TransactionList &wireList,
             const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "[ERROR] Failed to open file: " << filename << "\n";
        return;
    }

    std::string line;
    int lineNum = 0, totalLoaded = 0, totalSkipped = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (std::getline(file, line))
    {
        ++lineNum;
        if (lineNum == 1)
            continue;
        if (lineNum % 100000 == 0)
            std::cout << "[INFO] Processing line " << lineNum << "...\n";

        std::stringstream ss(line);
        std::string token;
        Transaction *t = new Transaction();

        try
        {
            std::getline(ss, t->transaction_id, ',');
            std::getline(ss, t->timestamp, ',');
            std::getline(ss, t->sender_account, ',');
            std::getline(ss, t->receiver_account, ',');
            std::getline(ss, token, ',');
            t->amount = token.empty() ? 0.0 : std::stod(token);
            std::getline(ss, t->transaction_type, ',');
            std::getline(ss, t->merchant_category, ',');
            std::getline(ss, t->location, ',');
            std::getline(ss, t->device_used, ',');
            std::getline(ss, token, ',');
            t->is_fraud = (token == "1" || token == "true" || token == "True");
            std::getline(ss, t->fraud_type, ',');
            std::getline(ss, token, ',');
            t->time_since_last_transaction = token.empty() ? 0.0 : std::stod(token);
            std::getline(ss, token, ',');
            t->spending_deviation_score = token.empty() ? 0.0 : std::stod(token);
            std::getline(ss, token, ',');
            t->velocity_score = token.empty() ? 0.0 : std::stod(token);
            std::getline(ss, token, ',');
            t->geo_anomaly_score = token.empty() ? 0.0 : std::stod(token);
            std::getline(ss, t->payment_channel, ',');
            std::getline(ss, t->ip_address, ',');
            std::getline(ss, t->device_hash, ',');

            if (totalLoaded < MAX_TRANSACTIONS)
            {
                array.insert(t);
                fullList.append(t);

                std::string ch = t->payment_channel;
                if (ch == "card")
                    cardList.append(t);
                else if (ch == "ACH")
                    achList.append(t);
                else if (ch == "UPI")
                    upiList.append(t);
                else if (ch == "wire_transfer")
                    wireList.append(t);
            }

            ++totalLoaded;
            if (totalLoaded >= MAX_TOTAL_RECORDS)
                break;
        }
        catch (...)
        {
            delete t;
            ++totalSkipped;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[DONE] CSV load complete. Loaded: " << totalLoaded
              << ", Skipped: " << totalSkipped
              << ", Time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
              << " seconds.\n";
}

void showMenu()
{
    std::cout << "\n=== Transaction Manager ===\n";
    std::cout << "1. Load CSV Data\n";
    std::cout << "2. Payment channel sort\n";
    std::cout << "3. Run Benchmark\n";
    std::cout << "4. Sort Array by Location\n";
    std::cout << "5. Search Transaction Type (Array)\n";
    std::cout << "6. Search Transaction Type (List)\n";
    std::cout << "7. Compare Performance (Array vs Linked List)\n";
    std::cout << "8. Export to JSON\n";
    std::cout << "9. Sort Linked List by Location\n";
    std::cout << "10. Exit\n";
    std::cout << "Enter choice: ";
}

void exportToJSON(const TransactionArray &array, const std::string &outputFilename)
{
    json jArray = json::array();

    for (int i = 0; i < array.getSize(); ++i)
    {
        Transaction *t = array.getData()[i];
        json jt;
        jt["transaction_id"] = t->transaction_id;
        jt["timestamp"] = t->timestamp;
        jt["sender_account"] = t->sender_account;
        jt["receiver_account"] = t->receiver_account;
        jt["amount"] = t->amount;
        jt["transaction_type"] = t->transaction_type;
        jt["merchant_category"] = t->merchant_category;
        jt["location"] = t->location;
        jt["device_used"] = t->device_used;
        jt["is_fraud"] = t->is_fraud;
        jt["fraud_type"] = t->fraud_type;
        jt["time_since_last_transaction"] = t->time_since_last_transaction;
        jt["spending_deviation_score"] = t->spending_deviation_score;
        jt["velocity_score"] = t->velocity_score;
        jt["geo_anomaly_score"] = t->geo_anomaly_score;
        jt["payment_channel"] = t->payment_channel;
        jt["ip_address"] = t->ip_address;
        jt["device_hash"] = t->device_hash;

        jArray.push_back(jt);
    }

    std::ofstream out(outputFilename);
    if (!out)
    {
        std::cerr << "[ERROR] Failed to open JSON output file.\n";
        return;
    }
    out << std::setw(4) << jArray << std::endl;
    std::cout << "[SUCCESS] Exported " << array.getSize() << " transactions to " << outputFilename << "\n";
}

size_t estimateArrayMemory(const TransactionArray &arr)
{
    return arr.getSize() * sizeof(Transaction *) + arr.getSize() * sizeof(Transaction);
}

size_t estimateListMemory(const TransactionList &list)
{
    size_t total = 0;
    Node *current = *((Node **)(&list));
    while (current)
    {
        total += sizeof(Node) + sizeof(Transaction);
        current = current->next;
    }
    return total;
}

int main()
{
    TransactionArray array;
    TransactionList fullList, cardList, achList, upiList, wireList;
    int choice;
    std::string filename;

    do
    {
        showMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice)
        {
        case 1:
            std::cout << "Enter CSV filename: ";
            std::getline(std::cin, filename);
            loadCSV(array, fullList, cardList, achList, upiList, wireList, filename);
            std::cout << "[DEBUG] Array size after load: " << array.getSize() << "\n";
            break;
        case 2:
             std::cout << "\n-- Array Data --\n";
             array.print(20);

             std::cout << "\n-- Full Linked List --\n";
             fullList.print(20);

             std::cout << "\n-- Card Transactions --\n";
             cardList.print(20);

             std::cout << "\n-- ACH Transactions --\n";
             achList.print(20);

             std::cout << "\n-- UPI Transactions --\n";
             upiList.print(20);

             std::cout << "\n-- Wire Transfer Transactions --\n";
             wireList.print(20);
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
            array.print(20);
            break;
        case 5:
        {
            std::string type;
            std::cout << "Enter transaction type to search (array): ";
            std::getline(std::cin, type);
            array.searchTransactionType(type);
            break;
        }
        case 6:
        {
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

            std::cout << "\n=== SORT BENCHMARK ===\n";
            auto startASort = std::chrono::high_resolution_clock::now();
            array.sortByLocation();
            auto endASort = std::chrono::high_resolution_clock::now();
            auto arraySortTime = std::chrono::duration_cast<std::chrono::microseconds>(endASort - startASort).count();

            auto startLSort = std::chrono::high_resolution_clock::now();
            fullList.sortByLocation();
            auto endLSort = std::chrono::high_resolution_clock::now();
            auto listSortTime = std::chrono::duration_cast<std::chrono::microseconds>(endLSort - startLSort).count();

            std::cout << "Array Sort Time: " << arraySortTime << " µs\n";
            std::cout << "List  Sort Time: " << listSortTime << " µs\n";

            std::cout << "\n=== SEARCH BENCHMARK ===\n";
            std::string sampleSearch = "card"; // or another known type in your data
            auto arraySearchTime = array.benchmarkSearch(sampleSearch);
            auto listSearchTime = fullList.benchmarkSearch(sampleSearch);

            std::cout << "Array Search Time ('" << sampleSearch << "'): " << arraySearchTime << " µs\n";
            std::cout << "List  Search Time ('" << sampleSearch << "'): " << listSearchTime << " µs\n";

            break;
        }


        case 8:
        {
            std::string jsonFile;
            std::cout << "Enter filename to export (e.g., data.json): ";
            std::getline(std::cin, jsonFile);
            exportToJSON(array, jsonFile);
            break;
        }

        case 9:
        {
            std::cout << "[DEBUG] Sorting Linked List by location...\n";
            fullList.sortByLocation();
            std::cout << "[DEBUG] Sort complete, printing first 20...\n";
            fullList.print(); 
            break; 
        }

        case 10:
        {
            std::cout << "Exiting program.\n";
            break;
        }

        default:
            std::cout << "Invalid choice.\n";
        }
    } while (choice != 10);

    return 0;
}
