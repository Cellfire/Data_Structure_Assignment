#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include "json.hpp"
#include <set>

#define MAX_TRANSACTIONS 4000
#define MAX_TOTAL_RECORDS 4000
#define MAX_LIST_LIMIT 4000

using json = nlohmann::json;

// Struct to hold benchmark results
struct BenchmarkResults {
    long long arraySortTime = -1;
    long long listSortTime = -1;
    long long arraySearchTime = -1;
    long long listSearchTime = -1;
};

// Declare global variable to store benchmark results from Option 2
BenchmarkResults benchmark;


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

    static Node *merge(Node *left, Node *right)
    {
        if (!left)
            return right;
        if (!right)
            return left;

        Node *result;
        if (left->data->location < right->data->location)
        {
            result = left;
            result->next = merge(left->next, right);
        }
        else
        {
            result = right;
            result->next = merge(left, right->next);
        }
        return result;
    }

    static void split(Node *source, Node **front, Node **back)
    {
        Node *slow = source;
        Node *fast = source->next;

        while (fast)
        {
            fast = fast->next;
            if (fast)
            {
                slow = slow->next;
                fast = fast->next;
            }
        }

        *front = source;
        *back = slow->next;
        slow->next = nullptr;
    }

    static void mergeSort(Node **headRef)
    {
        Node *head = *headRef;
        if (!head || !head->next)
            return;

        Node *a;
        Node *b;

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

    void print(int limit = 20) const
    {
        Node *temp = head;
        int count = 0;

        while (temp && count < limit)
        {
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

    void sortByTransactionType()
    {
        mergeSortByType(&head);
    }

    static Node *mergeByType(Node *left, Node *right)
    {
        if (!left)
            return right;
        if (!right)
            return left;

        Node *result;
        if (toLower(left->data->transaction_type) < toLower(right->data->transaction_type))
        {
            result = left;
            result->next = mergeByType(left->next, right);
        }
        else
        {
            result = right;
            result->next = mergeByType(left, right->next);
        }
        return result;
    }

    static void mergeSortByType(Node **headRef)
    {
        Node *head = *headRef;
        if (!head || !head->next)
            return;

        Node *a;
        Node *b;
        split(head, &a, &b);
        mergeSortByType(&a);
        mergeSortByType(&b);
        *headRef = mergeByType(a, b);
    }

    void sortByLocation()
    {
        auto start = std::chrono::high_resolution_clock::now();
        mergeSort(&head);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "[INFO] Linked List sorted by location (ascending) using MergeSort.\n";
        std::cout << "[DEBUG] Sort time: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << " ms\n\n";
    }


    void jumpSearchTransactionType(const std::string &type) const
    {
        std::string searchType = toLower(type);
        int step = sqrt(countNodes());
        Node *prev = nullptr;
        Node *curr = head;
        int i = 0;

        while (curr && toLower(curr->data->transaction_type) < searchType)
        {
            prev = curr;
            for (int j = 0; j < step && curr; ++j)
            {
                curr = curr->next;
                ++i;
            }
        }

        bool found = false;
        while (prev && toLower(prev->data->transaction_type) <= searchType)
        {
            if (toLower(prev->data->transaction_type) == searchType)
            {
                std::cout << prev->data->transaction_id << " | "
                          << prev->data->transaction_type << " | "
                          << prev->data->location << "\n";
                found = true;
            }
            prev = prev->next;
        }

        if (!found)
            std::cout << "[INFO] No transactions found for type: " << type << "\n";
    }

    int countNodes() const
    {
        int count = 0;
        Node *temp = head;
        while (temp)
        {
            ++count;
            temp = temp->next;
        }
        return count;
    }

    long long benchmarkSearch(const std::string &type) const
    {
        std::string searchType = toLower(type);
        auto start = std::chrono::high_resolution_clock::now();

        Node *temp = head;
        while (temp)
        {
            if (toLower(temp->data->transaction_type) == searchType)
            {
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
Node *merge(Node *left, Node *right)
{
    if (!left)
        return right;
    if (!right)
        return left;

    Node *result;
    if (left->data->location < right->data->location)
    {
        result = left;
        result->next = merge(left->next, right);
    }
    else
    {
        result = right;
        result->next = merge(left, right->next);
    }
    return result;
}

// Splits list into two halves
void split(Node *source, Node **front, Node **back)
{
    Node *slow = source;
    Node *fast = source->next;

    while (fast)
    {
        fast = fast->next;
        if (fast)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = nullptr;
}

// Recursive Merge Sort for Linked List
void mergeSort(Node **headRef)
{
    Node *head = *headRef;
    if (!head || !head->next)
        return;

    Node *a;
    Node *b;

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

    void sortByTransactionType()
    {
        std::sort(data, data + size, [](Transaction *a, Transaction *b)
                  { return toLower(a->transaction_type) < toLower(b->transaction_type); });
    }

    void binarySearchTransactionType(const std::string &type) const
    {
        std::string searchType = toLower(type);
        int left = 0, right = size - 1;
        bool found = false;

        while (left <= right)
        {
            int mid = (left + right) / 2;
            std::string midType = toLower(data[mid]->transaction_type);

            if (midType == searchType)
            {
                // Search adjacent entries with the same type
                int i = mid;
                while (i >= 0 && toLower(data[i]->transaction_type) == searchType)
                    --i;
                ++i;
                while (i < size && toLower(data[i]->transaction_type) == searchType)
                {
                    std::cout << data[i]->transaction_id << " | "
                              << data[i]->transaction_type << " | "
                              << data[i]->location << "\n";
                    ++i;
                    found = true;
                }
                break;
            }
            if (midType < searchType)
                left = mid + 1;
            else
                right = mid - 1;
        }

        if (!found)
            std::cout << "[INFO] No transactions found for type: " << type << "\n";
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

    long long benchmarkSearch(const std::string &type) const
    {
        std::string searchType = toLower(type);
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < size; ++i)
        {
            if (toLower(data[i]->transaction_type) == searchType)
            {
                volatile auto tmp = data[i]->amount;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
};

int loadCSV(TransactionArray &array, TransactionList &fullList,
             TransactionList &cardList, TransactionList &achList,
             TransactionList &upiList, TransactionList &wireList,
             const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "[ERROR] Failed to open file: " << filename << "\n";
        return false;
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
    return true;
}

void showMenu()
{
    std::cout << "\n=== Transaction Manager ===\n";
    std::cout << "1. Payment channel sort\n";
    std::cout << "2. Run Benchmark\n";
    std::cout << "3. Sort Array by Location\n";
    std::cout << "4. Sort Linked List by Location\n";
    std::cout << "5. Search Transaction Type (Array)\n";
    std::cout << "6. Search Transaction Type (List)\n";
    std::cout << "7. Compare Performance (Array vs Linked List)\n";
    std::cout << "8. Export to JSON\n";
    std::cout << "9. Exit\n";
    std::cout << "Enter choice: ";
}

void exportCustomJSON(TransactionArray &array, TransactionList &list)
{
    int choice;
    std::cout << "\nChoose export type:\n";
    std::cout << "1. Full dataset (Array)\n";
    std::cout << "2. Sorted by Location (Array)\n";
    std::cout << "3. Sorted by Location (Linked List)\n";
    std::cout << "4. Search by Transaction Type (Array)\n";
    std::cout << "5. Search by Transaction Type (Linked List)\n";
    std::cout << "Enter choice: ";
    std::cin >> choice;
    std::cin.ignore();

    std::string filename;
    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);
    if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".json")
        filename += ".json";

    json jArray = json::array();

    if (choice == 1)
    {
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
    }
    else if (choice == 2)
    {
        array.sortByLocation();
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
    }
    else if (choice == 3)
    {
        list.sortByLocation();
        Node *curr = *((Node **)(&list));
        while (curr)
        {
            Transaction *t = curr->data;
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
            curr = curr->next;
        }
    }
    else if (choice == 4)
    {
        std::string type;
        std::cout << "Enter transaction type to search (array): ";
        std::getline(std::cin, type);
        array.sortByTransactionType();

        for (int i = 0; i < array.getSize(); ++i)
        {
            if (toLower(array.getData()[i]->transaction_type) == toLower(type))
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
        }
    }
    else if (choice == 5)
    {
        std::string type;
        std::cout << "Enter transaction type to search (linked list): ";
        std::getline(std::cin, type);
        list.sortByTransactionType();

        Node *curr = *((Node **)(&list));
        while (curr)
        {
            if (toLower(curr->data->transaction_type) == toLower(type))
            {
                Transaction *t = curr->data;
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
            curr = curr->next;
        }
    }
    else
    {
        std::cout << "[ERROR] Invalid export type.\n";
        return;
    }

    std::ofstream out(filename);
    if (!out)
    {
        std::cerr << "[ERROR] Failed to write to JSON file.\n";
        return;
    }
    out << std::setw(4) << jArray << std::endl;
    std::cout << "[SUCCESS] Exported to " << filename << "\n";
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

void runSortBenchmark(TransactionArray &array, TransactionList &fullList)
{
    std::cout << "\nRUNTIME CALCULATION\n\n";

    //____________LOCATION_____________________________


    //SORT BY LOCATION (ARRAY)
    auto startArraySort = std::chrono::high_resolution_clock::now();
    array.sortByLocation();
    auto endArraySort = std::chrono::high_resolution_clock::now();
    auto arraySortTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endArraySort - startArraySort).count();

    //SORT BY LOCATION (LINKED LIST)
    auto startListSort = std::chrono::high_resolution_clock::now();
    fullList.sortByLocation();
    auto endListSort = std::chrono::high_resolution_clock::now();
    auto listSortTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endListSort - startListSort).count();

    //_________________TRANSACTION TYPE_______________________/


    //COLLECT UNIQUE TRANSACTION TYPES
    std::set<std::string> uniqueTypes;
    for (int i = 0; i < array.getSize(); ++i)
    {
        uniqueTypes.insert(toLower(array.getData()[i]->transaction_type));
    }

    //MEASURE TOTAL SEARCH TIME FOR ALL TYPES (ARRAY) 
    long long totalArraySearchTime = 0;
    for (const std::string &type : uniqueTypes)
    {
        auto start = std::chrono::high_resolution_clock::now();
        array.benchmarkSearch(type);
        auto end = std::chrono::high_resolution_clock::now();
        totalArraySearchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    //MEASURE TOTAL SEARCH TIME FOR ALL TYPES (LINKED LIST)
    long long totalListSearchTime = 0;
    for (const std::string &type : uniqueTypes)
    {
        auto start = std::chrono::high_resolution_clock::now();
        fullList.benchmarkSearch(type);
        auto end = std::chrono::high_resolution_clock::now();
        totalListSearchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    //PRINT OUTPUT
    std::cout << "[ ARRAY ]\n";
    std::cout << "Searching (transaction type) : " << totalArraySearchTime << " ns\n";
    std::cout << "Sorting ( location ) : " << arraySortTime << " ns\n\n";

    std::cout << "[ LINKEDLIST ]\n";
    std::cout << "Searching (transaction type ) : " << totalListSearchTime << " ns\n";
    std::cout << "Sorting  ( location ) : " << listSortTime << " ns\n";

    //store results for option 7
    benchmark.arraySortTime = arraySortTime;
    benchmark.listSortTime = listSortTime;
    benchmark.arraySearchTime = totalArraySearchTime;
    benchmark.listSearchTime = totalListSearchTime;




}

int main()
{
    TransactionArray array;
    TransactionList fullList, cardList, achList, upiList, wireList;
    int choice;
    std::string filename;

    std::cout << "Enter CSV filename: ";
    std::getline(std::cin, filename);
    if (!loadCSV(array, fullList, cardList, achList, upiList, wireList, filename)) {
    std::cerr << "[FATAL] Failed to load data. Exiting...\n";
    return 1;  // or return from main()
}
    std::cout << "[DEBUG] Array size after load: " << array.getSize() << "\n";

    do
    {
        showMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice)
        {
        case 1:
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
        case 2:
            {
                runSortBenchmark(array, fullList);
                break;
            }

            break;
        case 3:
            std::cout << "[DEBUG] Running Option 4: Sorting now...\n";
            array.sortByLocation();
            std::cout << "[DEBUG] Sort complete, printing first 20...\n";
            array.print(100);
            break;
        case 4:
        {
            std::cout << "[DEBUG] Sorting Linked List by location...\n";
            fullList.sortByLocation();
            std::cout << "[DEBUG] Sort complete, printing first 20...\n";
            fullList.print(100);
            break;
        }
        case 5:
        {
            std::string type;
            std::cout << "Enter transaction type to search (array): ";
            std::getline(std::cin, type);
            array.sortByTransactionType();
            array.binarySearchTransactionType(type);

            break;
        }
        case 6:
        {
            std::string type;
            std::cout << "Enter transaction type to search (list): ";
            std::getline(std::cin, type);
            fullList.sortByTransactionType();
            fullList.jumpSearchTransactionType(type);

            break;
        }
        case 7:
        {
            if (benchmark.arraySortTime == -1 || benchmark.listSortTime == -1)
            {
                std::cout << "[INFO] Please run Option 2 (Benchmark) first to compare performance.\n";
                break;
            }

            
            // MEMORY USAGE COMPARISON
            size_t arrayMemory = estimateArrayMemory(array);
            size_t listMemory = estimateListMemory(fullList);

            std::cout << "\n=== PERFORMANCE COMPARISON ===\n";

            std::cout << "[ ARRAY ]\n";
            std::cout << "Sorting Time (location)               : " << benchmark.arraySortTime << " ns\n";
            std::cout << "Searching Time (transaction type)     : " << benchmark.arraySearchTime << " ns\n\n";

            std::cout << "[ LINKED LIST ]\n";
            std::cout << "Sorting Time (location)               : " << benchmark.listSortTime << " ns\n";
            std::cout << "Searching Time (transaction type)     : " << benchmark.listSearchTime << " ns\n\n";

            std::cout << ">>> SORTING: ";
                if (benchmark.arraySortTime < benchmark.listSortTime)
                    std::cout << "Array is faster for sorting by location.\n";
                        else if (benchmark.listSortTime < benchmark.arraySortTime)
                    std::cout << "Linked List is faster for sorting by location.\n";
                    else
                    std::cout << "Both are equally fast for sorting.\n";

            std::cout << ">>> SEARCHING: ";
                if (benchmark.arraySearchTime < benchmark.listSearchTime)
            std::cout << "Array is faster for searching transaction types.\n";
                else if (benchmark.listSearchTime < benchmark.arraySearchTime)
            std::cout << "Linked List is faster for searching transaction types.\n";
                else
                std::cout << "Both are equally fast for searching.\n";


            std::cout << "\nARRAY Memory Usage      : " << arrayMemory << " bytes\n";
            std::cout << "LINKED LIST Memory Usage: " << listMemory << " bytes\n\n";

            std::cout << ">>> MEMORY: ";  
                if (arrayMemory < listMemory)
            std::cout << "Array uses less memory.\n";
                else if (listMemory < arrayMemory)
            std::cout << "Linked List uses less memory.\n";
                else
            std::cout << "Both use equal memory.\n";



            break;
        }
        case 8:
        {
            exportCustomJSON(array, fullList);
            break;
        }



        case 9:
        {
            std::cout << "Exiting program .\n";
            break;
        }


        default:
            std::cout << "Invalid choice.\n";
        }
    } while (choice != 9);

    return 0;
}
