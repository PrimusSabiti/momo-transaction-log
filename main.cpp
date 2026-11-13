// main.cpp - Mobile Money Agent Transaction Log: menu-driven program.
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include "Sorting.h"
#include "Transaction.h"

static const char* DATA_FILE = "transactions.txt";

static bool earlier(const Transaction& a, const Transaction& b) { return a.time < b.time; }
static bool biggerFirst(const Transaction& a, const Transaction& b) { return a.amount > b.amount; }
static bool lowerId(const Transaction& a, const Transaction& b) { return a.id < b.id; }

static void show(const Transaction& t) {
    std::cout << "  #" << t.id << "  " << t.type << "  " << t.phone << "  UGX " << t.amount
              << "  at minute " << t.time << "\n";
}
static void showAll(const std::vector<Transaction>& v) {
    if (v.empty()) { std::cout << "  (no transactions)\n"; return; }
    for (const Transaction& t : v) show(t);
}
static std::vector<Transaction> snapshot(const SinglyList<Transaction>& list) {
    std::vector<Transaction> v(list.size());
    list.toArray(v.data());
    return v;
}
static int askInt(const char* prompt) {
    int v;
    std::cout << prompt;
    while (!(std::cin >> v)) {
        if (std::cin.eof()) throw std::runtime_error("input closed");
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a whole number: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return v;
}
static std::string askText(const char* prompt) {
    std::string s;
    std::cout << prompt;
    if (!std::getline(std::cin, s)) throw std::runtime_error("input closed");
    return s;
}

static void summary(const SinglyList<Transaction>& log) {
    const char* names[3] = {"deposit", "withdraw", "send"};
    long count[3] = {0, 0, 0}, total[3] = {0, 0, 0};
    for (const Transaction& t : snapshot(log))
        for (int k = 0; k < 3; ++k)
            if (t.type == names[k]) { ++count[k]; total[k] += t.amount; }
    std::cout << "Daily summary (" << log.size() << " transactions)\n";
    for (int k = 0; k < 3; ++k)
        std::cout << "  " << names[k] << ": " << count[k] << " transactions, UGX " << total[k] << "\n";
}

int main() {
    SinglyList<Transaction> log;
    int skipped = 0;
    int loaded = loadLog(DATA_FILE, log, &skipped);
    if (loaded < 0) std::cout << "Could not open " << DATA_FILE << " - starting with an empty log.\n";
    else std::cout << "Loaded " << loaded << " transactions (" << skipped << " bad lines skipped).\n";

    try {
        for (;;) {
            std::cout << "\n 1 Record transaction    2 Show all             3 Correct an amount\n"
                         " 4 Remove transaction    5 Sort log by time     6 Rank by amount (view only)\n"
                         " 7 Find by id (binary search)    8 Daily summary\n"
                         " 9 Save                  0 Save and exit\n";
            int choice = askInt("Choice: ");
            if (choice == 1) {
                Transaction t;
                t.id = askInt("Id: ");
                t.type = askText("Type (deposit/withdraw/send): ");
                t.phone = askText("Customer phone (10 digits): ");
                t.amount = askInt("Amount (UGX): ");
                t.time = askInt("Minutes after opening: ");
                int id = t.id;
                if (!validRecord(t)) std::cout << "Rejected: check id > 0, type, 10-digit phone, amount > 0, time >= 0.\n";
                else if (log.findWhere([id](const Transaction& x) { return x.id == id; })) std::cout << "Rejected: that id already exists.\n";
                else { log.pushBack(t); std::cout << "Recorded.\n"; }
            } else if (choice == 2) {
                showAll(snapshot(log));
            } else if (choice == 3) {
                int id = askInt("Id: ");
                int amount = askInt("Corrected amount (UGX): ");
                Transaction* p = log.findWhere([id](const Transaction& x) { return x.id == id; });
                if (!p) std::cout << "No such transaction.\n";
                else if (amount <= 0) std::cout << "Rejected: amount must be above 0.\n";
                else { p->amount = amount; std::cout << "Corrected.\n"; }
            } else if (choice == 4) {
                int id = askInt("Id to remove: ");
                Transaction gone;
                if (log.removeWhere([id](const Transaction& x) { return x.id == id; }, &gone)) { std::cout << "Removed:\n"; show(gone); }
                else std::cout << "No such transaction.\n";
            } else if (choice == 5) {
                std::vector<Transaction> v = snapshot(log);
                mergeSort(v.data(), v.size(), earlier);
                log.fromArray(v.data(), v.size());
                std::cout << "Log is now in time order.\n";
            } else if (choice == 6) {
                std::vector<Transaction> v = snapshot(log);
                mergeSort(v.data(), v.size(), earlier);        // first by time ...
                mergeSort(v.data(), v.size(), biggerFirst);    // ... then by amount; stability keeps ties in time order
                std::cout << "Ranking by amount (ties stay in time order):\n";
                showAll(v);
            } else if (choice == 7) {
                std::vector<Transaction> v = snapshot(log);    // a list cannot jump to its middle, so search a sorted copy
                mergeSort(v.data(), v.size(), lowerId);
                int id = askInt("Id to find: ");
                long pos = binarySearchFirst(v.data(), v.size(), id, [](const Transaction& t) { return t.id; });
                if (pos < 0) std::cout << "Not found.\n";
                else show(v[static_cast<std::size_t>(pos)]);
            } else if (choice == 8) {
                summary(log);
            } else if (choice == 9 || choice == 0) {
                if (saveLog(DATA_FILE, log)) std::cout << "Saved " << log.size() << " transactions.\n";
                else std::cout << "Save failed.\n";
                if (choice == 0) break;
            } else {
                std::cout << "Unknown choice.\n";
            }
        }
    } catch (const std::runtime_error& e) {
        std::cout << "\nInput ended (" << e.what() << "); exiting without saving.\n";
    }
    return 0;
}
