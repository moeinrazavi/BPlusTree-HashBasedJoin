#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <cstdlib>

// RUN THIS IN TERMINAL TO COMPILE:
// g++ -std=c++17 hashing_based_join.cpp -o hashing_based_join
// ./hashing_based_join

// DESCRIPTION:
//1. Data Generation:
//`generateRelationS(int size)` generates a relation S with a specified number of tuples where B is the key attribute, and C can be of any type. The values of attribute B are random integers between 10,000 and 50,000. The function takes the size of the relation as a parameter and returns the relation S in the form of a vector of tuples.
//
//2. Virtual Disk I/O:
//`readBlock(std::vector<Tuple>& memory, std::vector<Tuple>& disk, int blockNum)` reads a block from the virtual disk to the virtual main memory. It takes the main memory, virtual disk, and block number as arguments, and transfers the contents of the specified block from the disk to the memory.
//
//`writeBlock(std::vector<Tuple>& memory, std::vector<Tuple>& disk)` writes a block from the virtual main memory to the virtual disk. It takes the main memory and virtual disk as arguments, and transfers the contents of the main memory to the disk, then clears the main memory.
//
//3. Hash Function:
//`hashFunction(int value)` maps the B-values of the relations to a proper range for the algorithm. It takes an integer value as an argument and returns its hash using the modulo operator.
//
//4. Join Algorithm:
//`twoPassJoin(std::vector<Tuple>& R, std::vector<Tuple>& S, int& diskIOs)` performs a natural join operation based on hashing. It takes relations R and S as input and counts the number of disk I/Os during the join operation. The function first checks if a one-pass join is possible, otherwise, it proceeds with a two-pass join.
//
//- One-pass join: If the total number of tuples in R and S is less than or equal to the available tuples in the virtual main memory, a one-pass join is performed. The function builds hash tables for both relations R and S, and joins the tuples based on their B-values. The disk I/Os count for one-pass join is set to 0 since it does not require accessing the virtual disk.
//- Two-pass join: If a one-pass join is not possible, the function proceeds with the two-pass join algorithm. The join operation is divided into two phases: Partitioning and Join.
//  - Partitioning: This phase divides both relations R and S into blocks using the hash function. It partitions the tuples based on their B-values and writes them to the virtual disk when the virtual main memory is full.
//  - Join: This phase reads the tuples from the virtual main memory and the virtual disk into hash tables. It then joins the corresponding tuples from R and S based on their B-values and stores the results in an output vector.
//
//5. Experiment:
//`generateRelationR(int size, const std::vector<Tuple>& S)` generates a relation R with a specified number of tuples, where the values of the attribute B are randomly picked from the relation S, and the attribute A can be of any type. It returns the generated relation R.
//
//The main function performs three experiments:
//
//- One-pass join example: Generates relations S_small and R_small with a total number of tuples that fit within the virtual main memory (120 tuples). This example will execute the one-pass join algorithm in the twoPassJoin function. It then prints the disk I/Os used and the resulting tuples in the join.
//- 5.1: Generates a relation R and calculates its natural join with the relation S using the twoPassJoin function. It then prints the disk I/Os used and the tuples in the join with random B-values.
//- 5.2: Generates another relation R with 1,200 tuples and calculates its natural join with the relation S using the twoPassJoin function. In this experiment, the values of the attribute B are randomly picked from integers between 20,000 and 30,000, but not necessarily from the B-values in the relation S. It then prints the disk I/Os used and all the tuples in the join R(A, B) ⋈ S(B, C).
//
//In summary, the code generates relations R and S, simulates virtual disk I/Os, and performs one-pass and two-pass natural join operations using a hash-based approach. It also counts the disk I/Os used during the join operations and provides output for different experiments.

const int BLOCK_SIZE = 8;
const int MEMORY_BLOCKS = 15;
const int TUPLE_R = 1000;
const int TUPLE_S = 5000;

template<typename T>
struct Tuple {
    int A, B;
    T C;
};

// Part 1: Data Generation

template<typename T>
std::vector<Tuple<T>> generateRelationS(int size) {
    std::vector<Tuple<T>> relation;
    for (int i = 0; i < size; ++i) {
        int B = rand() % 40001 + 10000;
        T C;
        if constexpr (std::is_same_v<T, std::string>) {
            C = "_STR_" + std::to_string(rand() % 100000);
        } else {
            C = static_cast<T>(rand() % 100000);
        }
        relation.push_back({0, B, C});
    }
    return relation;
}


// Part 2: Virtual Disk I/O
template<typename T>
void readBlock(std::vector<Tuple<T>>& memory, std::vector<Tuple<T>>& disk, int blockNum) {
    int startIndex = blockNum * BLOCK_SIZE;
    for (int i = startIndex; i < startIndex + BLOCK_SIZE && i < disk.size(); ++i) {
        memory.push_back(disk[i]);
    }
}

template<typename T>
void writeBlock(std::vector<Tuple<T>>& memory, std::vector<Tuple<T>>& disk) {
    for (const auto& tuple : memory) {
        disk.push_back(tuple);
    }
    memory.clear();
}

// Part 3: Hash Function
int hashFunction(int value) {
    return value % MEMORY_BLOCKS;
}

// Part 4: Join Algorithm
template<typename T>
std::vector<Tuple<T>> twoPassJoin(std::vector<Tuple<T>>& R, std::vector<Tuple<T>>& S, int& diskIOs) {
    std::vector<Tuple<T>> output;
    int totalTuples = static_cast<int>(R.size() + S.size());

    // One-pass join, if possible
    if (totalTuples <= MEMORY_BLOCKS * BLOCK_SIZE) {
        std::unordered_map<int, std::vector<Tuple<T>>> R_map, S_map;

        for (const auto& tuple : R) {
            R_map[tuple.B].push_back(tuple);
        }
        for (const auto& tuple : S) {
            S_map[tuple.B].push_back(tuple);
        }

        for (const auto& pair : R_map) {
            if (S_map.find(pair.first) != S_map.end()) {
                for (const auto& r_tuple : pair.second) {
                    for (const auto& s_tuple : S_map[pair.first]) {
                        output.push_back({r_tuple.A, r_tuple.B, s_tuple.C});
                    }
                }
            }
        }

        return output;
    }
    std::vector<std::vector<Tuple<T>>> memoryHashTable(MEMORY_BLOCKS);
    std::vector<std::vector<Tuple<T>>> diskHashTable(MEMORY_BLOCKS);

    // Phase 1: Partitioning
    for (const auto& tuple : R) {
        int bucket = hashFunction(tuple.B);
        if (memoryHashTable[bucket].size() < BLOCK_SIZE) {
            memoryHashTable[bucket].push_back(tuple);
        } else {
            writeBlock(memoryHashTable[bucket], diskHashTable[bucket]);
            diskIOs++;
            memoryHashTable[bucket].push_back(tuple);
        }
    }

    for (const auto& tuple : S) {
        int bucket = hashFunction(tuple.B);
        if (memoryHashTable[bucket].size() < BLOCK_SIZE) {
            memoryHashTable[bucket].push_back(tuple);
        } else {
            writeBlock(memoryHashTable[bucket], diskHashTable[bucket]);
            diskIOs++;
            memoryHashTable[bucket].push_back(tuple);
        }
    }

    // Phase 2: Join
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
            std::unordered_map<int, std::vector<Tuple<T>>> R_map, S_map;

            for (const auto& tuple : memoryHashTable[i]) {
                if constexpr (std::is_same_v<T, std::string>) {
                    if (tuple.C == "0") {
                        R_map[tuple.B].push_back(tuple);
                    } else {
                        S_map[tuple.B].push_back(tuple);
                    }
                } else {
                    if (tuple.C == 0) {
                        R_map[tuple.B].push_back(tuple);
                    } else {
                        S_map[tuple.B].push_back(tuple);
                    }
                }
            }

            for (const auto& tuple : diskHashTable[i]) {
                if constexpr (std::is_same_v<T, std::string>) {
                    if (tuple.C == "0") {
                        R_map[tuple.B].push_back(tuple);
                    } else {
                        S_map[tuple.B].push_back(tuple);
                    }
                } else {
                    if (tuple.C == 0) {
                        R_map[tuple.B].push_back(tuple);
                    } else {
                        S_map[tuple.B].push_back(tuple);
                    }
                }
            }

            for (const auto& pair : R_map) {
                if (S_map.find(pair.first) != S_map.end()) {
                    for (const auto& r_tuple : pair.second) {
                        for (const auto& s_tuple : S_map[pair.first]) {
                            if constexpr (std::is_same_v<T, std::string>) {
                                output.push_back({r_tuple.A, r_tuple.B, s_tuple.C.substr(1)}); // Remove the "C" prefix
                            } else {
                                output.push_back({r_tuple.A, r_tuple.B, s_tuple.C});
                            }
                        }
                    }
                }
            }
        }
    
    return output;
}

// Part 5: Experiment
template<typename T>
std::vector<Tuple<T>> generateRelationR(int size, const std::vector<Tuple<T>>& S) {
    std::vector<Tuple<T>> relation;
    for (int i = 0; i < size; ++i) {
        int A = rand() % 100000;
        int B = S[rand() % S.size()].B;
        T C;
        if constexpr (std::is_same_v<T, std::string>) {
            C = "0";
        } else {
            C = static_cast<T>(0);
        }
        relation.push_back({A, B, C});
    }
    return relation;
}


int main() {
    srand(time(0));
    int diskIOs = 0;

    // One-pass join example
    std::vector<Tuple<int>> S_small = generateRelationS<int>(100);
    std::vector<Tuple<int>> R_small = generateRelationR<int>(20, S_small);
    std::vector<Tuple<int>> joinResult_small = twoPassJoin<int>(R_small, S_small, diskIOs);
    std::cout << "One-pass join example\n";
    std::cout << "Disk I/Os for join: " << diskIOs << std::endl;
    if (diskIOs == 0) {
        std::cout << "One-pass join succeeded! --> diskIOs = 0" << std::endl;
    } else {
        
        std::cout << "One-pass join failed because totalTuples > MEMORY_BLOCKS * BLOCK_SIZE" << std::endl;
        std::cout << "Applying Two-pass join" << std::endl;
    }
    std::cout << "All tuples in the join R(A, B) ⋈ S(B, C):\n";
    for (const auto& tuple : joinResult_small) {
        std::cout << "(" << tuple.A << ", " << tuple.B << ", " << tuple.C << ")\n";
    }
    std::cout << "\n";

    // 5.1
    std::vector<Tuple<int>> S = generateRelationS<int>(TUPLE_S);
    std::vector<Tuple<int>> R = generateRelationR<int>(TUPLE_R, S);
    std::vector<Tuple<int>> joinResult = twoPassJoin<int>(R, S, diskIOs);
    std::cout << "Disk I/Os for join: " << diskIOs << std::endl;

    std::vector<int> randomBvalues;
    for (int i = 0; i < 20; ++i) {
        randomBvalues.push_back(S[rand() % S.size()].B);
    }

    std::cout << "Tuples with random B-values:\n";
    for (const auto& tuple : joinResult) {
        if (std::find(randomBvalues.begin(),randomBvalues.end(), tuple.B) != randomBvalues.end()) {
            std::cout << "(" << tuple.A << ", " << tuple.B << ", " << tuple.C << ")\n";
        }
    }

    // 5.2
    
    std::cout << "Two-pass join example\n";
    std::vector<Tuple<int>> R2(TUPLE_R + 200);
    for (int i = 0; i < TUPLE_R + 200; ++i) {
        int A = rand() % 100000;
        int B = rand() % 10001 + 20000;
        R2[i] = {A, B, 0};
    }

    diskIOs = 0;
    joinResult = twoPassJoin<int>(R2, S, diskIOs);
    std::cout << "Disk I/Os for join: " << diskIOs << std::endl;
    if (diskIOs == 0) {
        std::cout << "One-pass join succeeded! --> diskIOs = 0" << std::endl;
    } else {
        
        std::cout << "One-pass join failed because totalTuples > MEMORY_BLOCKS * BLOCK_SIZE" << std::endl;
        std::cout << "Applying Two-pass join" << std::endl;
    }

    std::cout << "All tuples in the join R(A, B) ⋈ S(B, C):\n";
    for (const auto& tuple : joinResult) {
        std::cout << "(" << tuple.A << ", " << tuple.B << ", " << tuple.C << ")\n";
    }
    // Example with string C
    std::vector<Tuple<std::string>> S_string = generateRelationS<std::string>(10000);
    std::vector<Tuple<std::string>> R_string = generateRelationR<std::string>(20, S_string);
    int stringDiskIOs = 0;
    std::vector<Tuple<std::string>> joinResult_string = twoPassJoin<std::string>(R_string, S_string, stringDiskIOs);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Join example with string C\n";
    std::cout << "Disk I/Os for join: " << stringDiskIOs << std::endl;
    if (stringDiskIOs == 0) {
        std::cout << "One-pass join succeeded! --> diskIOs = 0" << std::endl;
    } else {
        
        std::cout << "One-pass join failed because totalTuples > MEMORY_BLOCKS * BLOCK_SIZE" << std::endl;
        std::cout << "Applying Two-pass join" << std::endl;
    }
    std::cout << "All tuples in the join R(A, B) ⋈ S(B, C):\n";
    for (const auto& tuple : joinResult_string) {
        std::cout << "(" << tuple.A << ", " << tuple.B << ", " << tuple.C << ")\n";
    }
    std::cout << "\n";

    return 0;
}
