# PART1: B+ Trees

This project explores the performance of B+ trees with different orders and densities under various operations. B+ trees are balanced tree data structures commonly used in databases and file systems for efficient search, insertion, and deletion operations.

## Features

- Generate random records
- Build B+ trees with different orders and densities (dense and sparse)
- Perform a series of operations on the trees, including insertions, deletions, and searches
- Print the tree structure after each operation
- Conduct experiments to analyze B+ tree performance under different configurations

## Usage

Compile the project using a C++ compiler that supports C++11 or later, such as GCC or Clang:

```bash
g++ -std=c++11 -o main main.cpp b_plus_tree.cpp
```

Run the compiled binary:

```bash
./main
```

## Project Structure

- `main.cpp`: Contains the main function and all necessary functions for generating records, building B+ trees, performing operations, and conducting experiments.

### Functions

1. `generate_records(int num_records, int min_key, int max_key)`: Generates a vector of random integers with a specified size and value range.
2. `build_dense_tree(const std::vector<int>& records, int order)` and `build_sparse_tree(const std::vector<int>& records, int order)`: Creates B+ trees using the given records and order, resulting in dense or sparse trees.
3. `perform_operations(std::vector<BPlusTree*>& trees, int min_key, int max_key, std::mt19937& gen)`: Performs a series of operations on the given trees, such as insertions, deletions, and searches, and prints the tree structure after each operation.
4. `perform_experiments(int num_records, int min_key, int max_key, int dense_order, int sparse_order)`: Orchestrates the entire experiment by generating records, building B+ trees with different orders and densities, and performing operations on these trees.
5. `main()`: The main function initializes the parameters for the experiment and calls `perform_experiments()` to execute the experiment. It also demonstrates a simple example of a B+ tree with order 3 and a predefined set of records, printing the tree structure after inserting the records.


# PART2: Join Algorithm Based on Hashing

This C++ project implements a two-pass join algorithm based on hashing. It simulates the join of two relations R(A, B) and S(B, C) using a virtual main memory and a virtual disk. The project is divided into five parts: Data Generation, Virtual Disk I/O, Hash Function, Join Algorithm, and Experiment. The code is designed to handle different data types for the C value in the tuples using C++ templates.

## Features

- Data Generation for relations R and S with customizable data type for the C value using C++ templates
- Virtual Disk I/O simulation for read and write operations
- Custom hash function for partitioning the relations
- Two-pass join algorithm that incorporates one-pass join when possible
- Experiments to test the join algorithm and count the number of disk I/Os

## Usage

Compile the project using a C++ compiler that supports C++11 or later, such as GCC or Clang:

```bash
g++ -std=c++17 hashing_based_join.cpp -o hashing_based_join
```

Run the compiled binary:

```bash
./hashing_based_join
```
1. Clone the repository to your local machine.
2. Compile the C++ code using a C++ compiler.
3. Run the compiled code.
4. Observe the output for the experiments, including disk I/O counts and resulting tuples from the join operations. Customize the data type of the C value in the tuples by modifying the code to use a different template parameter.

## Experiments

Four experiments are included in the main function of the project:

1. One-pass join example: This experiment demonstrates the one-pass join when the total number of tuples in R and S can fit within the virtual main memory.
2. Experiment 5.1: Generates a relation R and calculates its natural join with the relation S. The output includes disk I/Os used and tuples in the join with random B-values.
3. Experiment 5.2: Generates a different relation R with 1,200 tuples and calculates its natural join with the relation S. The output includes disk I/Os used and all the tuples in the join R(A, B) ⋈ S(B, C).
4. Example with string C: This experiment generates relations R and S with string type C values and calculates their natural join using the two-pass join algorithm.

In the code, you can change the type of the C value in the tuples by modifying the template parameter for the `Tuple`, `generateRelationS`, `generateRelationR`, and `twoPassJoin` functions.

## License

This project is released under the MIT License. See the `LICENSE` file for more information.
