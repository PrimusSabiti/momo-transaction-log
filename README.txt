Mobile Money Agent Transaction Log  -  BCS 2204 Group Coursework 1, Part A
Scenario 2: singly linked list with head and tail pointers.  Sorts: insertion sort + merge sort.

FILES
  SinglyList.h    A1  class template SinglyList<T> (the linked list)
  Transaction.h   A1  Transaction struct, validation, loadLog / saveLog
  Sorting.h       A2  insertionSort, mergeSort, binarySearchFirst (all templates / comparators)
  main.cpp        menu program
  tests.cpp       A1 + A2 test table (prints PASS/FAIL per test)
  timing.cpp      A4 timing experiment (prints CSV)
  transactions.txt  sample data: 12 records, one per line: id,type,phone,amount,time

COMPILE AND RUN  (C++17, g++ or clang++; run everything from THIS folder)
  g++ -std=c++17 -Wall -Wextra -g -o app main.cpp      && ./app
  g++ -std=c++17 -Wall -Wextra -g -o tests tests.cpp   && ./tests
  g++ -std=c++17 -O2 -o timing timing.cpp              && ./timing
  (Windows/MinGW: use app.exe, tests.exe, timing.exe instead of ./app etc.)

MEMORY CHECK (Linux, macOS, or Windows WSL)
  g++ -std=c++17 -g -fsanitize=address,undefined -fno-omit-frame-pointer -o tests_asan tests.cpp
  ./tests_asan          (no report from the sanitizer = no leaks, no invalid accesses)

With make:  make all | make asan | make clean

NOTES
  - The program loads transactions.txt at start and saves back to it with menu option 9 or 0.
  - Do not use STL containers inside SinglyList.h; std::vector is used only in test/sort/helper code.
