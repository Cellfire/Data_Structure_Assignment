// Combined part
#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 

using namespace std;


struct Transaction {
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

    Transaction* next = nullptr; // For linked list
};

bool safe_stod(const string& str, double& out) {
    try {
        if (str.empty()) {
            out = 0.0; // or use std::nan("") if needed
            return false;
        }
        out = stod(str);
        return true;
    } catch (...) {
        out = 0.0;
        return false;
    }
}

//Linked List Appending Function
void append_to_list(Transaction*& head, Transaction*& tail, const Transaction& t) {
    Transaction* new_node = new Transaction(t); // make a copy
    new_node->next = nullptr;

    if (head == nullptr) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
}

//Linked list printing function (for testing)
void print_linked_list(Transaction* head, int max_items = 5) {
    Transaction* curr = head;
    int count = 0;
    while (curr != nullptr && count < max_items) {
        cout << "Transaction ID: " << curr->transaction_id << ", Amount: " << curr->amount << " Fraud Type (Should be empty): " << curr->fraud_type << "\n";
        curr = curr->next;
        count++;
    }
}

int main(){

    Transaction* head = nullptr; // linked list head
    Transaction* tail = nullptr; // linked list tail for O(1) computing

    int capacity = 1000;
    int row_count = 0;
    Transaction** data = new Transaction*[capacity];

    ifstream iFile("financial_fraud_detection_dataset.csv");
    string line = "";


    if (!iFile.is_open()) {
        cerr << "Failed to open the file.\n";
        return 1;
    }

    getline(iFile, line);
    int line_number = 1;

     while (getline(iFile, line)) {
        line_number++;
        stringstream ss(line);
        string field;
        Transaction* t = new Transaction();

        try {
            getline(ss, t->transaction_id, ',');
            getline(ss, t->timestamp, ',');
            getline(ss, t->sender_account, ',');
            getline(ss, t->receiver_account, ',');

            getline(ss, field, ',');
            if (!safe_stod(field, t->amount)) throw runtime_error("Invalid amount");

            getline(ss, t->transaction_type, ',');
            getline(ss, t->merchant_category, ',');
            getline(ss, t->location, ',');
            getline(ss, t->device_used, ',');

            getline(ss, field, ',');
            t->is_fraud = (field == "1" || field == "true" || field == "True");

            getline(ss, t->fraud_type, ',');

            getline(ss, field, ',');
            safe_stod(field, t->time_since_last_transaction);

            getline(ss, field, ',');
            safe_stod(field, t->spending_deviation_score); 

            getline(ss, field, ',');
            safe_stod(field, t->velocity_score);

            getline(ss, field, ',');
            safe_stod(field, t->geo_anomaly_score); 

            getline(ss, t->payment_channel, ',');
            getline(ss, t->ip_address, ',');
            getline(ss, t->device_hash, ',');

            if (row_count >= capacity) {
                int new_capacity = capacity * 2;
                Transaction** new_data = new Transaction*[new_capacity];
                for (int i = 0; i < row_count; ++i)
                    new_data[i] = data[i];
                delete[] data;
                data = new_data;
                capacity = new_capacity;
            }

            data[row_count++] = t;

            append_to_list  (head, tail, *t);

        } catch (const exception& e) {
            cerr << "Error on line " << line_number << ": " << e.what() << "\n";
            continue; // Skip malformed line
        }
    }

    cout << "Loaded " << row_count << " transactions using array.\n";

    // Example output
    for (int i = 0; i < row_count && i < 5; ++i) {
        Transaction* t = data[i];
        cout << "[" << i << "] ID: " << t->transaction_id
             << ", Amount: " << t->amount
             << ", Fraud: " << (t->is_fraud ? "Yes" : "No") << "\n";
    }

    // Linked List printing testing
    cout << "\nFirst few transactions from linked list:\n";
    print_linked_list(head);

    //Free memory to prevent memory leaks!
    Transaction* curr = head;
    while (curr != nullptr) {
        Transaction* next = curr->next;
        delete curr;
        curr = next;
    }

    for (int i = 0; i < row_count; ++i)
        delete data[i];
        delete[] data;
    
    return 0;
}
