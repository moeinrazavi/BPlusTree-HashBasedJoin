#include <iostream>
#include <random>
#include <algorithm>
#include "b_plus_tree.h"

// RUN THIS IN TERMINAL TO COMPILE:
// g++ -std=c++11 -o main main.cpp b_plus_tree.cpp


//This code is a comprehensive project that explores the performance of B+ trees with different orders and densities under various operations. A B+ tree is a balanced tree data structure commonly used in databases and file systems for efficient search, insertion, and deletion operations.
//
//The project consists of several functions that generate records, build B+ trees, perform operations on the trees, and conduct experiments to analyze their performance. The main steps in the project are as follows:
//
//1. Generate a set of random records.
//2. Build B+ trees with different orders (dense_order and sparse_order) and tree densities (dense and sparse).
//3. Perform a series of operations (insertions, deletions, and searches) on the created B+ trees and print the tree structure after each operation.
//
//Here's a detailed explanation of the primary functions:
//
//1. `generate_records(int num_records, int min_key, int max_key)`: Generates a vector of random integers with `num_records` size, and values ranging from `min_key` to `max_key`. It uses the Mersenne Twister pseudo-random number generator (`std::mt19937`) to generate the random keys.
//
//2. `build_dense_tree(const std::vector<int>& records, int order)` and `build_sparse_tree(const std::vector<int>& records, int order)`: These functions create B+ trees using the given `records` and `order`. The `build_dense_tree` function inserts the records as they are, leading to a potentially unbalanced B+ tree. In contrast, the `build_sparse_tree` function inserts the records in a sorted manner, creating a more balanced B+ tree.
//
//3. `perform_operations(std::vector<BPlusTree*>& trees, int min_key, int max_key, std::mt19937& gen)`: This function performs a series of operations on the given `trees`:
//   - Insertions: Inserts 2 random keys (values between `min_key` and `max_key`) and prints the tree structure after each insertion using the `print_tree_disp` function.
//   - Deletions: Removes 2 random keys and prints the tree structure after each deletion using the `print_tree_disp` function.
//   - Search and remove otherwise insert: Searches for 5 random keys, removes them if found, otherwise inserts them, and prints the tree structure after each operation using the `print_tree_disp` function.
//   - Search: Searches for 5 random keys and prints the tree structure after each search using the `print_tree_disp` function.
//
//4. `perform_experiments(int num_records, int min_key, int max_key, int dense_order, int sparse_order)`: This function orchestrates the entire experiment by generating records, building B+ trees with different orders and densities, and performing operations on these trees. It first generates the random records and builds the dense and sparse trees with the given dense_order and sparse_order. Then, it performs operations on both sets of trees (dense and sparse) and prints the tree structures.
//
//5. `main()`: The main function initializes the parameters for the experiment and calls `perform_experiments()` to execute the experiment. It also demonstrates a simple example of a B+ tree with order 3 and a predefined set of records, printing the tree structure after inserting the records using the `print_tree_disp` function.
//
//In summary, this code tests the functionality and efficiency of B+ trees with different orders and densities by performing a series of insertions, deletions, and searches while printing the tree structure after each operation. The experiment aims to help understand how B+ trees behave under different configurations and tree densities, which can be beneficial in optimizing the performance of databases and file systems that use B+ trees.

void print_tree_disp(const BPlusTree& tree) {
    tree.print_tree();
    std::cout << "------------------------------------" << std::endl;
}



std::vector<int> generate_records(int num_records, int min_key, int max_key) {
    std::vector<int> records;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min_key, max_key);

    for (int i = 0; i < num_records; ++i) {
        records.push_back(dis(gen));
    }

    return records;
}

BPlusTree build_dense_tree(const std::vector<int>& records, int order) {
    BPlusTree dense_tree(order);
    for (const int& key : records) {
        dense_tree.insert(key, key);
    }
    return dense_tree;
}

BPlusTree build_sparse_tree(const std::vector<int>& records, int order) {
    BPlusTree sparse_tree(order);
    // Insert records in a sorted manner
    std::vector<int> sorted_records(records);
    std::sort(sorted_records.begin(), sorted_records.end());

    for (const int& key : sorted_records) {
        sparse_tree.insert(key, key);
    }
    return sparse_tree;
}

void perform_operations(std::vector<BPlusTree*>& trees, int min_key, int max_key, std::mt19937& gen) {
    std::uniform_int_distribution<> dis(min_key, max_key);

    for (int i = 0; i < 2; ++i) {
        int key = dis(gen);
        for (BPlusTree* tree : trees) {
            tree->insert(key, key);
            // Print the tree if needed
            std::cout << "INSERTING: " << key << std::endl;
            print_tree_disp(*tree);
        }
    }

    for (int i = 0; i < 2; ++i) {
        int key = dis(gen);
        for (BPlusTree* tree : trees) {
            tree->remove(key);
            
            std::cout << "REMOVING: " << key << std::endl;
            print_tree_disp(*tree);
            // Print the tree if needed
        }
    }

    for (int i = 0; i < 5; ++i) {
        int key = dis(gen);
        for (BPlusTree* tree : trees) {
            if (tree->search(key)) {
                tree->remove(key);
            } else {
                tree->insert(key, key);
            }
            std::cout << "SEARCH AND REMOVE OTHERWISE INSERT: " << key << std::endl;
            print_tree_disp(*tree);
            // Print the tree if needed
        }
    }

    for (int i = 0; i < 5; ++i) {
        int key = dis(gen);
        for (BPlusTree* tree : trees) {
            bool found = tree->search(key);
            // Print the search result if needed
        }
    }
}

void perform_experiments(int num_records, int min_key, int max_key, int dense_order, int sparse_order) {
    // Step (a): Generate records
    std::vector<int> records = generate_records(num_records, min_key, max_key);

    // Step (b): Build B+ trees
    BPlusTree dense_order_13 = build_dense_tree(records, dense_order);
    BPlusTree sparse_order_13 = build_sparse_tree(records, dense_order);
    BPlusTree dense_order_24 = build_dense_tree(records, sparse_order);
    BPlusTree sparse_order_24 = build_sparse_tree(records, sparse_order);

    std::vector<BPlusTree*> dense_trees = {&dense_order_13, &dense_order_24};
    std::vector<BPlusTree*> sparse_trees = {&sparse_order_13, &sparse_order_24};

    // Step (c): Test B+ tree operations
    std::random_device rd;
    std::mt19937 gen(rd());

    // Step (c1) and (c3): Apply insertions on dense trees
    // Step (c2) and (c3): Apply deletions on sparse trees
    perform_operations(dense_trees, min_key, max_key, gen);
    perform_operations(sparse_trees, min_key, max_key, gen);

    // Step (c4): Apply search operations on dense and sparse trees
    for (int i = 0; i < 5; ++i) {
        int key = gen() % (max_key - min_key + 1) + min_key;
        for (BPlusTree* tree : dense_trees) {
            bool found = tree->search(key);
            // Print the search result if needed
            print_tree_disp(*tree);
        }
        for (BPlusTree* tree : sparse_trees) {
            bool found = tree->search(key);
            print_tree_disp(*tree);
            // Print the search result if needed
        }
    }
}

int main() {
    std::cout << "SIMPLE EXP: " << std::endl;
    BPlusTree tree1(3);
    // Insert records in a sorted manner
    std::vector<int> records1 = {3, 5, 7, 9, 11, 13, 15, 18, 20, 25, 28, 29, 31, 32, 33, 34, 45, 60};
//    std::sort(sorted_records.begin(), sorted_records.end());

    for (const int& key : records1) {
        tree1.insert(key, key);
    }
    
    print_tree_disp(tree1);
    
    std::cout << std::endl;
    std::cout << "SIMPLE EXP DONE!" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;
    
    int num_records = 10000;
    int min_key = 100000;
    int max_key = 200000;
    int dense_order = 13;
    int sparse_order = 24;

    perform_experiments(num_records, min_key, max_key, dense_order, sparse_order);

    return 0;
}
