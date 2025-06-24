// Hann's part

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

// Define the Transaction struct
struct Transaction {
    std::string transaction_id;
    std::string timestamp;
    std::string location;
    std::string payment_channel;
    std::string transaction_type;
    double amount;
    bool is_fraud;

    Transaction* next;
};

// Convert linked list to JSON array
json convertTransactionsToJson(Transaction* head) {
    json jArray = json::array();
    Transaction* current = head;

    while (current != nullptr) {
        jArray.push_back({
            {"transaction_id", current->transaction_id},
            {"timestamp", current->timestamp},
            {"location", current->location},
            {"payment_channel", current->payment_channel},
            {"transaction_type", current->transaction_type},
            {"amount", current->amount},
            {"is_fraud", current->is_fraud}
        });
        current = current->next;
    }

    return jArray;
}

// Write JSON to file
void writeJsonToFile(const std::string& filename, const json& jData) {
    std::ofstream file(filename);
    if (file) {
        file << std::setw(4) << jData << std::endl;
        std::cout << "JSON written to " << filename << "\n";
    } else {
        std::cerr << "Failed to open file for writing.\n";
    }
}

// Load first 500 lines from CSV into linked list
Transaction* loadCSV(const std::string& filename, int maxLines = 500) {
    std::ifstream file(filename);
    std::string line;
    Transaction* head = nullptr;
    Transaction* tail = nullptr;
    int count = 0;

    if (!file.is_open()) {
        std::cerr << "Error opening file.\n";
        return nullptr;
    }

    std::getline(file, line); // Skip header

    while (std::getline(file, line) && count < maxLines) {
        std::stringstream ss(line);
        std::string token;

        std::string id, timestamp, sender, receiver, amountStr, type, category, location, device;
        std::string isFraudStr, fraudType, paymentChannel;

        std::getline(ss, id, ',');
        std::getline(ss, timestamp, ',');
        std::getline(ss, sender, ',');
        std::getline(ss, receiver, ',');
        std::getline(ss, amountStr, ',');
        std::getline(ss, type, ',');
        std::getline(ss, category, ',');
        std::getline(ss, location, ',');
        std::getline(ss, device, ',');
        std::getline(ss, isFraudStr, ',');
        std::getline(ss, fraudType, ',');
        std::getline(ss, token, ','); // time_since_last_transaction
        std::getline(ss, token, ','); // spending_deviation_score
        std::getline(ss, token, ','); // velocity_score
        std::getline(ss, token, ','); // geo_anomaly_score
        std::getline(ss, paymentChannel, ',');

        // Convert and validate
        double amount = std::stod(amountStr);
        bool is_fraud = (isFraudStr == "True" || isFraudStr == "true" || isFraudStr == "1");

        Transaction* t = new Transaction{id, timestamp, location, paymentChannel, type, amount, is_fraud, nullptr};

        if (!head) head = tail = t;
        else {
            tail->next = t;
            tail = t;
        }

        count++;
    }

    file.close();
    return head;
}

int main() {
    std::string filename = "financial_fraud_detection_dataset.csv";
    Transaction* head = loadCSV(filename, 500); // load only 500 rows

    if (!head) {
        std::cerr << "No transactions loaded.\n";
        return 1;
    }

    json jData = convertTransactionsToJson(head);
    writeJsonToFile("exported_transactions.json", jData);

    // Clean up memory
    Transaction* temp;
    while (head) {
        temp = head;
        head = head->next;
        delete temp;
    }

    return 0;
}
