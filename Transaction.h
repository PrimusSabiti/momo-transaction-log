// Transaction.h - the record type, validation, and text-file load/save.
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <fstream>
#include <string>
#include <vector>
#include "SinglyList.h"

struct Transaction {
    int id = 0;            // unique transaction id (> 0)
    std::string type;      // "deposit", "withdraw" or "send"
    std::string phone;     // customer phone, 10 digits, kept as text so the leading 0 survives
    int amount = 0;        // UGX, above 0
    int time = 0;          // minutes after the agent opened, 0 or more
};

inline bool validType(const std::string& t) { return t == "deposit" || t == "withdraw" || t == "send"; }

inline bool validPhone(const std::string& p) {
    if (p.size() != 10 || p[0] != '0') return false;
    for (char c : p) if (c < '0' || c > '9') return false;
    return true;
}

// Whole-string integer parse (rejects "12abc", "", " 5").
inline bool toInt(const std::string& s, int& out) {
    if (s.empty()) return false;
    try {
        std::size_t used = 0;
        out = std::stoi(s, &used);
        return used == s.size();
    } catch (...) { return false; }
}

inline bool validRecord(const Transaction& t) {
    return t.id > 0 && validType(t.type) && validPhone(t.phone) && t.amount > 0 && t.time >= 0;
}

// Parse "id,type,phone,amount,time". Returns false for any malformed or invalid line.
inline bool parseLine(const std::string& line, Transaction& t) {
    std::vector<std::string> f;
    std::size_t start = 0;
    for (;;) {
        std::size_t comma = line.find(',', start);
        if (comma == std::string::npos) { f.push_back(line.substr(start)); break; }
        f.push_back(line.substr(start, comma - start));
        start = comma + 1;
    }
    if (f.size() != 5) return false;
    Transaction r;
    if (!toInt(f[0], r.id) || !toInt(f[3], r.amount) || !toInt(f[4], r.time)) return false;
    r.type = f[1];
    r.phone = f[2];
    if (!validRecord(r)) return false;
    t = r;
    return true;
}

// Loads valid lines into the list (in file order). Lines starting with '#' and blank lines are ignored.
// A line whose id is already in the list is skipped. Returns how many were loaded, or -1 if the file
// cannot be opened. 'skipped' (optional) receives the number of rejected lines.
inline int loadLog(const std::string& path, SinglyList<Transaction>& list, int* skipped = nullptr) {
    std::ifstream in(path);
    if (!in) return -1;
    int good = 0, bad = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();       // Windows line endings
        if (line.empty() || line[0] == '#') continue;
        Transaction t;
        bool ok = parseLine(line, t);
        if (ok) {
            int id = t.id;
            ok = (list.findWhere([id](const Transaction& x) { return x.id == id; }) == nullptr);
        }
        if (ok) { list.pushBack(t); ++good; } else ++bad;
    }
    if (skipped) *skipped = bad;
    return good;
}

// Writes the whole list (one pass through a temporary array). Returns false if the file cannot be written.
inline bool saveLog(const std::string& path, const SinglyList<Transaction>& list) {
    std::ofstream out(path);
    if (!out) return false;
    out << "# id,type,phone,amount,time\n";
    std::vector<Transaction> tmp(list.size());
    list.toArray(tmp.data());
    for (const Transaction& t : tmp)
        out << t.id << ',' << t.type << ',' << t.phone << ',' << t.amount << ',' << t.time << '\n';
    return static_cast<bool>(out);
}

#endif
