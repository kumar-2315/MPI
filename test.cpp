#define IS_PARALLEL
#include <iostream>
#include <array>
#include <random>
#include <chrono>
#include "souffle/datastructure/BTree.h"
#include <omp.h>

using namespace souffle;


template <size_t N>
std::array<int, N> generateRandomTuple(int minVal = 0, int maxVal = 1000) {
    thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    std::array<int, N> tuple{};
    for (size_t i = 0; i < N; ++i) {
        tuple[i] = dist(rng);
    }
    return tuple;
}


template <size_t N>
void testBTree(size_t numInsertions, size_t numSearches) {
    using Tuple = std::array<int, N>;
    using BTree = souffle::btree_set<Tuple>;

    BTree tree;


    auto start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel num_threads(12)
    {
        #pragma omp for
        for (size_t i = 0; i < numInsertions; ++i) {
            auto tuple = generateRandomTuple<N>();
            tree.insert(tuple); 
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto insertionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Time taken for insertion: " << insertionTime << " microseconds\n";


    start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for num_threads(12)
    for (size_t i = 0; i < numSearches; ++i) {
        auto target = generateRandomTuple<N>();
        if (tree.contains(target)) {
            // Tuple found
        } else {
            // Tuple not found
        }
    }
    end = std::chrono::high_resolution_clock::now();
    auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Time taken for searching: " << searchTime << " microseconds\n";
}

int main() {
    constexpr size_t arity = 4; 
  
    size_t numInsertions = 10000000; 
    size_t numSearches = 1000;    
    std::cout << "Testing B-tree with tuple arity = " << arity << "\n";
    testBTree<arity>(numInsertions, numSearches);

    return 0;
}
