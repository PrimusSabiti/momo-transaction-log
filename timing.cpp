// timing.cpp - Task A4: time insertion sort vs merge sort on random Transaction records
// (sorted by amount), n = 1000, 2000, 4000, ... 64000, three runs per size, averaged.
// Build with optimisation:  g++ -std=c++17 -O2 -o timing timing.cpp
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>
#include "Sorting.h"
#include "Transaction.h"

static bool lessAmount(const Transaction& a, const Transaction& b) { return a.amount < b.amount; }

template <typename SortFn>
double runOnce(SortFn sortFn, const std::vector<Transaction>& original) {
    std::vector<Transaction> work = original;            // fresh unsorted copy for every run
    auto start = std::chrono::steady_clock::now();
    sortFn(work.data(), work.size());
    auto stop = std::chrono::steady_clock::now();
    for (std::size_t i = 1; i < work.size(); ++i)        // verify, so the work cannot be optimised away
        if (work[i].amount < work[i - 1].amount) { std::fprintf(stderr, "NOT SORTED\n"); std::exit(1); }
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

int main() {
    std::mt19937 rng(2026);
    std::printf("n,insertion_ms,merge_ms\n");
    for (std::size_t n = 1000; n <= 64000; n *= 2) {
        double sumInsertion = 0, sumMerge = 0;
        for (int rep = 0; rep < 3; ++rep) {
            std::vector<Transaction> data(n);
            for (std::size_t i = 0; i < n; ++i) {
                data[i].id = static_cast<int>(i) + 1;
                data[i].type = "deposit";
                data[i].phone = "0770000000";
                data[i].amount = static_cast<int>(rng() % 1000000) + 1;
                data[i].time = static_cast<int>(i);
            }
            sumInsertion += runOnce([](Transaction* p, std::size_t m) { insertionSort(p, m, lessAmount); }, data);
            sumMerge += runOnce([](Transaction* p, std::size_t m) { mergeSort(p, m, lessAmount); }, data);
        }
        std::printf("%zu,%.4f,%.4f\n", n, sumInsertion / 3, sumMerge / 3);
    }
}
