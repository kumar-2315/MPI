#include <iostream>
#include <array>
#include <random>
#include <chrono>
#include <mpi.h>
#include "/Users/shivamkumar/project/slog-lang1/backend/src/btree/btree_set.h"

// Alias for B-tree
template <typename T>
using SlogBTree = btree::btree_set<T>;

// Generate a random tuple
template <size_t N>
std::array<int, N> generateRandomTuple(int minVal = 0, int maxVal = 1000) {
    std::array<int, N> tuple{};
    for (size_t i = 0; i < N; ++i) {
        tuple[i] = rand() % (maxVal - minVal + 1) + minVal;
    }
    return tuple;
}

// Print a tuple
template <size_t N>
void printTuple(const std::array<int, N>& tuple) {
    std::cout << "{";
    for (size_t i = 0; i < N; ++i) {
        std::cout << tuple[i];
        if (i < N - 1) std::cout << ", ";
    }
    std::cout << "}";
}

// Measure time and execute a function
template <typename Func>
auto measureTime(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

// Test B-tree performance with MPI
template <size_t N>
void testBTreePerformanceMPI(size_t numInsertions = 10000, size_t numSearches = 5) {
    using Tuple = std::array<int, N>;
    SlogBTree<Tuple> tree;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    size_t localInsertions = numInsertions / size; // Divide workload
    size_t localSearches = numSearches / size;

    // Insert elements and measure time
    auto insertionTime = measureTime([&]() {
        for (size_t i = 0; i < localInsertions; ++i) {
            auto tuple = generateRandomTuple<N>();
            tree.insert(tuple);
        }
    });

    // Gather insertion times from all processes
    long globalInsertionTime;
    MPI_Reduce(&insertionTime, &globalInsertionTime, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Total Insertion Time: " << globalInsertionTime / size << " microseconds\n";
    }

    // Perform searches and measure time
    auto searchTime = measureTime([&]() {
        for (size_t i = 0; i < localSearches; ++i) {
            auto target = generateRandomTuple<N>();
            if (tree.find(target) != tree.end()) {
                // Found
            } else {
                // Not Found
            }
        }
    });

    // Gather search times from all processes
    long globalSearchTime;
    MPI_Reduce(&searchTime, &globalSearchTime, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Total Search Time: " << globalSearchTime / size << " microseconds\n";
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    constexpr size_t arity = 4;          // Tuple arity
    constexpr size_t numInsertions = 10000000; // Adjust as needed
    constexpr size_t numSearches = 10;       // Adjust as needed

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        std::cout << "Testing B-tree with tuple arity = " << arity << " using MPI\n";
    }

    testBTreePerformanceMPI<arity>(numInsertions, numSearches);

    MPI_Finalize();
    return 0;
}