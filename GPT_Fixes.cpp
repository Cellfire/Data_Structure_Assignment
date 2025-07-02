// === New Feature: Sort Linked List by Location + Enhanced Benchmarking ===

// Merge Sort for Linked List by Location
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

// Wrapper inside TransactionList
void sortByLocation() {
    auto start = std::chrono::high_resolution_clock::now();
    mergeSort(&head);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] Linked List sorted by location (ascending) using MergeSort.\n";
    std::cout << "[DEBUG] Sort time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";
}

// Add this into TransactionList class declaration:
// void sortByLocation();

// ===================== Benchmark Enhancements =====================

// Add search benchmarking to both Array and List classes:
long long benchmarkSearch(const std::string& type) const {
    std::string searchType = toLower(type);
    auto start = std::chrono::high_resolution_clock::now();

    // For array
    for (int i = 0; i < size; ++i) {
        if (toLower(data[i]->transaction_type) == searchType) {
            volatile auto tmp = data[i]->amount;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

long long TransactionList::benchmarkSearch(const std::string& type) const {
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

// ===================== Update case 7 Menu =====================
// Add timing for sort and search:

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
std::string sampleSearch = "payment";
auto arraySearchTime = array.benchmarkSearch(sampleSearch);
auto listSearchTime = fullList.benchmarkSearch(sampleSearch);

std::cout << "Array Search Time ('" << sampleSearch << "'): " << arraySearchTime << " µs\n";
std::cout << "List  Search Time ('" << sampleSearch << "'): " << listSearchTime << " µs\n";
