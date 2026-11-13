// Sorting.h - Task A2: insertion sort, merge sort (registration number ends in an EVEN digit),
// and iterative binary search. less(a, b) must return true when a goes STRICTLY before b.
#ifndef SORTING_H
#define SORTING_H

#include <cstddef>
#include <vector>

// Stable, in place. Best O(n) (sorted input), average/worst O(n^2).
// Loop invariant: before the pass with index i, a[0..i-1] holds the original first i records in sorted order.
template <typename T, typename Less>
void insertionSort(T* a, std::size_t n, Less less) {
    for (std::size_t i = 1; i < n; ++i) {
        T current = a[i];
        std::size_t pos = i;
        while (pos > 0 && less(current, a[pos - 1])) {   // STRICT: equal records are never jumped over
            a[pos] = a[pos - 1];
            --pos;
        }
        a[pos] = current;
    }
}

namespace sorting_detail {

// Merge the two sorted runs a[lo..mid) and a[mid..hi) using buf as scratch space.
template <typename T, typename Less>
void mergeRuns(T* a, T* buf, std::size_t lo, std::size_t mid, std::size_t hi, Less less) {
    std::size_t left = lo, right = mid, out = lo;
    while (left < mid && right < hi) {
        // Take from the right run only when it is STRICTLY smaller; on a tie the left record goes first.
        if (less(a[right], a[left])) buf[out++] = a[right++];
        else buf[out++] = a[left++];
    }
    while (left < mid) buf[out++] = a[left++];
    while (right < hi) buf[out++] = a[right++];
    for (std::size_t k = lo; k < hi; ++k) a[k] = buf[k];
}

template <typename T, typename Less>
void splitAndMerge(T* a, T* buf, std::size_t lo, std::size_t hi, Less less) {
    if (hi - lo < 2) return;                              // 0 or 1 record: already sorted
    std::size_t mid = lo + (hi - lo) / 2;
    splitAndMerge(a, buf, lo, mid, less);
    splitAndMerge(a, buf, mid, hi, less);
    mergeRuns(a, buf, lo, mid, hi, less);
}

} // namespace sorting_detail

// Stable, Theta(n log n) in every case, O(n) extra memory.
template <typename T, typename Less>
void mergeSort(T* a, std::size_t n, Less less) {
    if (n < 2) return;
    std::vector<T> buf(n);
    sorting_detail::splitAndMerge(a, buf.data(), 0, n, less);
}

// Iterative binary search on an array sorted ascending by keyOf(record).
// Returns the index of the FIRST record whose key equals 'key', or -1 if there is none. O(log n).
template <typename T, typename K, typename KeyOf>
long binarySearchFirst(const T* a, std::size_t n, const K& key, KeyOf keyOf) {
    std::size_t lo = 0, hi = n;                           // window [lo, hi)
    while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        if (keyOf(a[mid]) < key) lo = mid + 1;            // answer is to the right of mid
        else hi = mid;                                    // mid could be the answer: keep it
    }
    if (lo < n && !(key < keyOf(a[lo]))) return static_cast<long>(lo);   // key == keyOf(a[lo])
    return -1;
}

#endif
