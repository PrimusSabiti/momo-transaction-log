// tests.cpp - test table for Tasks A1 and A2.
// Prints one tab-separated row per test:  ID <TAB> operation <TAB> expected <TAB> actual <TAB> PASS/FAIL
// Run from the folder that contains transactions.txt.
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "Sorting.h"
#include "Transaction.h"

static int passCount = 0, failCount = 0;

static void check(const std::string& id, const std::string& op, const std::string& expected, const std::string& actual) {
    bool ok = (expected == actual);
    (ok ? passCount : failCount)++;
    std::cout << id << '\t' << op << '\t' << expected << '\t' << actual << '\t' << (ok ? "PASS" : "FAIL") << '\n';
}
static std::string yn(bool v) { return v ? "true" : "false"; }
static std::string num(long v) { return std::to_string(v); }

static Transaction rec(int id, int amount, int time = 0, const std::string& phone = "0770000000") {
    Transaction t; t.id = id; t.type = "deposit"; t.phone = phone; t.amount = amount; t.time = time;
    return t;
}
static std::string idsOf(const std::vector<Transaction>& v) {
    std::string s;
    for (std::size_t i = 0; i < v.size(); ++i) s += (i ? "," : "") + num(v[i].id);
    return s.empty() ? "(empty)" : s;
}
static std::string idsOf(const SinglyList<Transaction>& l) {
    std::vector<Transaction> v(l.size());
    l.toArray(v.data());
    return idsOf(v);
}
static SinglyList<Transaction> listOf(std::initializer_list<int> ids) {
    SinglyList<Transaction> l;
    for (int id : ids) l.pushBack(rec(id, 10));
    return l;
}
static auto byAmount = [](const Transaction& a, const Transaction& b) { return a.amount < b.amount; };
static auto byAmountDesc = [](const Transaction& a, const Transaction& b) { return a.amount > b.amount; };
static auto byTime = [](const Transaction& a, const Transaction& b) { return a.time < b.time; };
static auto byId = [](const Transaction& a, const Transaction& b) { return a.id < b.id; };
static auto byPhone = [](const Transaction& a, const Transaction& b) { return a.phone < b.phone; };
static auto isId(int id) { return [id](const Transaction& t) { return t.id == id; }; }
static auto amountOf = [](const Transaction& t) { return t.amount; };

static void writeFile(const char* path, const std::string& text) { std::ofstream(path) << text; }

int main() {
    Transaction out;

    // ======================= A1: the linked list =======================
    SinglyList<Transaction> empty;
    check("A1-01", "new list: empty(), size()", "true, 0", yn(empty.empty()) + ", " + num(empty.size()));
    check("A1-02", "removeAt(0) on an empty list", "false", yn(empty.removeAt(0)));
    check("A1-03", "front(), back(), at(0) on an empty list", "all nullptr",
          (empty.front() == nullptr && empty.back() == nullptr && empty.at(0) == nullptr) ? "all nullptr" : "not null");
    bool r4a = empty.removeWhere(isId(1)), r4b = empty.popFront(out);
    check("A1-04", "removeWhere and popFront on an empty list", "false, false", yn(r4a) + ", " + yn(r4b));

    SinglyList<Transaction> one;
    one.pushBack(rec(1, 50));
    check("A1-05", "one record: size, front id, back id", "1, 1, 1", num(one.size()) + ", " + num(one.front()->id) + ", " + num(one.back()->id));
    bool r6 = one.removeAt(0, &out);
    std::string s6 = yn(r6) + ", removed " + num(out.id) + ", " + (one.empty() ? "empty" : "not empty");
    one.pushBack(rec(7, 50));
    check("A1-06", "one record: removeAt(0), then pushBack(7)", "true, removed 1, empty / front 7 back 7",
          s6 + " / front " + num(one.front()->id) + " back " + num(one.back()->id));

    SinglyList<Transaction> l = listOf({1, 2, 3});
    check("A1-07", "pushBack 1,2,3: order and back id", "1,2,3, back 3", idsOf(l) + ", back " + num(l.back()->id));
    l.pushFront(rec(0, 10));
    check("A1-08", "pushFront(0): order and front id", "0,1,2,3, front 0", idsOf(l) + ", front " + num(l.front()->id));

    SinglyList<Transaction> dup;
    dup.pushBack(rec(1, 70)); dup.pushBack(rec(2, 70));
    check("A1-09", "duplicate key: two transactions of amount 70 are both stored", "2, 1,2", num(dup.size()) + ", " + idsOf(dup));

    SinglyList<Transaction> r = listOf({1, 2, 3, 4, 5});
    bool x10 = r.removeAt(0, &out);
    check("A1-10", "removeAt(0): remove the FIRST record", "true, removed 1, left 2,3,4,5, front 2",
          yn(x10) + ", removed " + num(out.id) + ", left " + idsOf(r) + ", front " + num(r.front()->id));
    bool x11 = r.removeAt(r.size() - 1, &out);
    check("A1-11", "removeAt(size-1): remove the LAST record (tail moves back)", "true, removed 5, left 2,3,4, back 4",
          yn(x11) + ", removed " + num(out.id) + ", left " + idsOf(r) + ", back " + num(r.back()->id));
    r.pushBack(rec(9, 10));
    check("A1-12", "pushBack after removing the last record", "2,3,4,9, back 9", idsOf(r) + ", back " + num(r.back()->id));
    bool x13 = r.removeAt(1, &out);
    check("A1-13", "removeAt(1): remove a middle record", "true, removed 3, left 2,4,9", yn(x13) + ", removed " + num(out.id) + ", left " + idsOf(r));
    bool x14 = r.removeAt(99);
    check("A1-14", "removeAt(99): invalid position", "false, unchanged 2,4,9", yn(x14) + ", unchanged " + idsOf(r));
    bool x15a = r.removeWhere(isId(555));
    bool x15b = r.removeWhere(isId(4), &out);
    check("A1-15", "removeWhere: absent id 555, then present id 4", "false / true, removed 4, left 2,9",
          yn(x15a) + " / " + yn(x15b) + ", removed " + num(out.id) + ", left " + idsOf(r));
    bool x16 = r.popFront(out);
    check("A1-16", "popFront", "true, popped 2, left 9", yn(x16) + ", popped " + num(out.id) + ", left " + idsOf(r));

    SinglyList<Transaction> f = listOf({1, 2, 3});
    Transaction* p = f.findWhere(isId(2));
    if (p) p->amount = 999;
    check("A1-17", "findWhere id 2 and edit the amount through the pointer; findWhere an absent id",
          "999, nullptr", num(f.at(1)->amount) + ", " + (f.findWhere(isId(77)) == nullptr ? "nullptr" : "found"));

    SinglyList<Transaction> orig = listOf({1, 2, 3});
    SinglyList<Transaction> cp(orig);
    cp.removeAt(0);
    cp.pushBack(rec(9, 10));
    check("A1-18", "copy constructor is deep: change the copy, original unchanged", "orig 1,2,3 / copy 2,3,9", "orig " + idsOf(orig) + " / copy " + idsOf(cp));
    SinglyList<Transaction> asg;
    asg = orig;
    asg.removeAt(0);
    SinglyList<Transaction>& self = asg;
    asg = self;                                    // self-assignment must be harmless
    check("A1-19", "copy assignment is deep; self-assignment is safe", "orig 1,2,3 / copy 2,3", "orig " + idsOf(orig) + " / copy " + idsOf(asg));

    SinglyList<Transaction> ins = listOf({10, 20});
    bool i1 = ins.insertAt(0, rec(5, 10));        // front
    bool i2 = ins.insertAt(2, rec(15, 10));       // middle
    bool i3 = ins.insertAt(ins.size(), rec(25, 10));  // end (tail must follow)
    bool i4 = ins.insertAt(ins.size() + 1, rec(99, 10));  // invalid
    check("A1-20", "insertAt: front, middle, end, and an invalid position",
          "true,true,true,false / 5,10,15,20,25 / back 25",
          yn(i1) + "," + yn(i2) + "," + yn(i3) + "," + yn(i4) + " / " + idsOf(ins) + " / back " + num(ins.back()->id));

    SinglyList<Transaction> rv = listOf({1, 2, 3, 4, 5});
    rv.reverse();
    rv.pushBack(rec(0, 10));
    SinglyList<Transaction> rv0, rv1 = listOf({8});
    rv0.reverse(); rv1.reverse();
    check("A1-21", "reverse: 1..5, then pushBack(0) (tail correct); reverse of empty and of one record",
          "5,4,3,2,1,0 / (empty) / 8", idsOf(rv) + " / " + idsOf(rv0) + " / " + idsOf(rv1));

    SinglyList<Transaction> big;
    for (int i = 1; i <= 10000; ++i) big.pushBack(rec(i, 10));
    check("A1-22", "10,000 pushBack calls: size, front id, back id", "10000, 1, 10000", num(big.size()) + ", " + num(big.front()->id) + ", " + num(big.back()->id));

    SinglyList<Transaction> logList;
    int skipped = -1;
    int loaded = loadLog("transactions.txt", logList, &skipped);
    check("A1-23", "load transactions.txt (12 valid records)", "12 loaded, 0 skipped", num(loaded) + " loaded, " + num(skipped) + " skipped");
    SinglyList<Transaction> none;
    check("A1-24", "load a file that does not exist", "-1", num(loadLog("no_such_file.txt", none)));

    logList.removeWhere(isId(6008));
    logList.pushBack([] { Transaction t = rec(6040, 30000, 560, "0791010013"); return t; }());
    bool saved = saveLog("test_tmp.txt", logList);
    SinglyList<Transaction> again;
    int reloaded = loadLog("test_tmp.txt", again);
    check("A1-25", "remove 6008, add 6040, save, reload", "true, 12, 6021,6015,6002,6030,6011,6026,6004,6019,6007,6033,6012,6040",
          yn(saved) + ", " + num(reloaded) + ", " + idsOf(again));
    std::remove("test_tmp.txt");

    writeFile("test_bad.txt",
              "# comment line\n"
              "1,deposit,0770000001,100,5\n"
              "abc\n"
              "2,loan,0770000002,100,5\n"
              "3,send,0770000003,-5,5\n"
              "4,send,0770000004,100\n"
              "5,send,07700,100,5\n"
              "\n"
              "6,withdraw,0770000006,50,10\n"
              "1,deposit,0770000009,200,6\n");
    SinglyList<Transaction> bad;
    int badSkipped = 0;
    int badLoaded = loadLog("test_bad.txt", bad, &badSkipped);
    check("A1-26", "file with 6 bad lines (junk, type loan, amount -5, missing field, short phone, repeated id 1) and 2 good",
          "2 loaded, 6 skipped, ids 1,6", num(badLoaded) + " loaded, " + num(badSkipped) + " skipped, ids " + idsOf(bad));
    std::remove("test_bad.txt");

    // ======================= A2: sorting and searching =======================
    std::vector<Transaction> e0;
    insertionSort(e0.data(), e0.size(), byAmount);
    mergeSort(e0.data(), e0.size(), byAmount);
    check("A2-01", "both sorts on an empty array", "no crash, (empty)", "no crash, " + idsOf(e0));

    std::vector<Transaction> o1 = {rec(1, 50)}, o2 = o1;
    insertionSort(o1.data(), o1.size(), byAmount);
    mergeSort(o2.data(), o2.size(), byAmount);
    check("A2-02", "both sorts on one record", "1 / 1", idsOf(o1) + " / " + idsOf(o2));

    std::vector<Transaction> s1 = {rec(1, 10), rec(2, 20), rec(3, 30), rec(4, 40), rec(5, 50), rec(6, 60)}, s2 = s1;
    insertionSort(s1.data(), s1.size(), byAmount);
    mergeSort(s2.data(), s2.size(), byAmount);
    check("A2-03", "already sorted input (insertion / merge)", "1,2,3,4,5,6 / 1,2,3,4,5,6", idsOf(s1) + " / " + idsOf(s2));

    // Reversed input; amounts 60,60,40,40,20,20 so each pair is a tie. Ties must keep their input order.
    std::vector<Transaction> v1 = {rec(1, 60), rec(2, 60), rec(3, 40), rec(4, 40), rec(5, 20), rec(6, 20)}, v2 = v1;
    insertionSort(v1.data(), v1.size(), byAmount);
    mergeSort(v2.data(), v2.size(), byAmount);
    check("A2-04", "reversed input (60,60,40,40,20,20); equal amounts keep input order", "5,6,3,4,1,2 / 5,6,3,4,1,2", idsOf(v1) + " / " + idsOf(v2));

    std::mt19937 rng(7);
    std::vector<Transaction> rnd(500);
    for (int i = 0; i < 500; ++i) rnd[i] = rec(i + 1, static_cast<int>(rng() % 50), i);   // many ties on purpose
    std::vector<Transaction> ref = rnd, ra = rnd, rb = rnd;
    std::stable_sort(ref.begin(), ref.end(), byAmount);
    insertionSort(ra.data(), ra.size(), byAmount);
    mergeSort(rb.data(), rb.size(), byAmount);
    check("A2-05", "500 random records (many equal amounts) compared with std::stable_sort", "identical, identical",
          std::string(idsOf(ra) == idsOf(ref) ? "identical" : "DIFFERENT") + ", " + (idsOf(rb) == idsOf(ref) ? "identical" : "DIFFERENT"));

    SinglyList<Transaction> sample;
    loadLog("transactions.txt", sample);
    std::vector<Transaction> day(sample.size());
    sample.toArray(day.data());
    std::vector<Transaction> rank1 = day, rank2 = day;
    insertionSort(rank1.data(), rank1.size(), byAmountDesc);
    mergeSort(rank2.data(), rank2.size(), byAmountDesc);
    std::string wantRank = "6011,6019,6007,6021,6002,6033,6015,6026,6008,6030,6004,6012";
    check("A2-06", "stability: sample log (in time order) ranked by amount, biggest first; ties stay in time order",
          wantRank + " / " + wantRank, idsOf(rank1) + " / " + idsOf(rank2));

    std::vector<Transaction> ph = {rec(3, 10, 0, "0773333333"), rec(1, 10, 0, "0771111111"), rec(4, 10, 0, "0774444444"), rec(2, 10, 0, "0772222222")}, ph2 = ph;
    insertionSort(ph.data(), ph.size(), byPhone);
    mergeSort(ph2.data(), ph2.size(), byPhone);
    check("A2-07", "sort by a different field (phone); input ids 3,1,4,2", "1,2,3,4 / 1,2,3,4", idsOf(ph) + " / " + idsOf(ph2));

    check("A2-08", "binary search in an empty array", "-1", num(binarySearchFirst(e0.data(), e0.size(), 5, amountOf)));
    std::vector<Transaction> single = {rec(1, 50)};
    check("A2-09", "binary search, one record: present key 50 / absent key 40", "0 / -1",
          num(binarySearchFirst(single.data(), single.size(), 50, amountOf)) + " / " + num(binarySearchFirst(single.data(), single.size(), 40, amountOf)));

    std::vector<Transaction> sd = {rec(1, 1000), rec(2, 2000), rec(3, 2000), rec(4, 2000), rec(5, 3000)};
    check("A2-10", "duplicate key: search for amount 2000 returns the FIRST occurrence", "1", num(binarySearchFirst(sd.data(), sd.size(), 2000, amountOf)));
    check("A2-11", "search the first key (1000) and the last key (3000)", "0 / 4",
          num(binarySearchFirst(sd.data(), sd.size(), 1000, amountOf)) + " / " + num(binarySearchFirst(sd.data(), sd.size(), 3000, amountOf)));
    check("A2-12", "search absent keys: below (500), between (2500), above (9999)", "-1 / -1 / -1",
          num(binarySearchFirst(sd.data(), sd.size(), 500, amountOf)) + " / " + num(binarySearchFirst(sd.data(), sd.size(), 2500, amountOf)) + " / " + num(binarySearchFirst(sd.data(), sd.size(), 9999, amountOf)));

    std::vector<Transaction> byIdArr = day;
    mergeSort(byIdArr.data(), byIdArr.size(), byId);
    long hit = binarySearchFirst(byIdArr.data(), byIdArr.size(), 6026, [](const Transaction& t) { return t.id; });
    long miss = binarySearchFirst(byIdArr.data(), byIdArr.size(), 6099, [](const Transaction& t) { return t.id; });
    check("A2-13", "sample log copied to an array, sorted by id, search id 6026 / absent id 6099",
          "phone 0772770007 / -1", "phone " + (hit >= 0 ? byIdArr[hit].phone : std::string("?")) + " / " + num(miss));

    std::vector<Transaction> back = byIdArr;
    insertionSort(back.data(), back.size(), byTime);
    check("A2-14", "id-sorted copy re-sorted by time restores the chronological order of the file",
          "6021,6008,6015,6002,6030,6011,6026,6004,6019,6007,6033,6012", idsOf(back));

    std::cout << "\nRESULT: " << passCount << " passed, " << failCount << " failed\n";
    return failCount == 0 ? 0 : 1;
}
