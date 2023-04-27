// b_plus_tree.h
#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <queue>

class Node {
public:
    bool is_leaf;
    int min_keys;
    int max_keys;
    int num_keys;
    std::vector<int> keys;
    Node* parent;

    Node(int order, bool is_leaf);
};

class InternalNode : public Node {
public:
    std::vector<Node*> pointers;

    InternalNode(int order);
};

class LeafNode : public Node {
public:
    std::vector<int> values;
    LeafNode* next;

    LeafNode(int order);
};

class BPlusTree {
private:
    int order;
    Node* root;

    Node* find_leaf_node(int key);
    void insert_into_leaf_node(LeafNode* leaf, int key, int value);
    void insert_into_internal_node(InternalNode* parent, int key, Node* child);
    void split_leaf_node(LeafNode* leaf);
    void split_internal_node(InternalNode* node);
    void delete_entry(Node* node, int key);
    void remove_from_leaf_node(LeafNode* leaf, int key);
    void remove_from_internal_node(InternalNode* node, int key);
//    void borrow_key(Node* left, Node* right, Node* parent, int left_index);
//    void merge_nodes(Node* left, Node* right, Node* parent, int left_index);
    
    void borrow_key(Node* left, Node* right, InternalNode* parent, int index);
    void merge_nodes(Node* left, Node* right, InternalNode* parent, int index);
//    void print_tree_recursively(Node* node, int level) const;
    void print_tree_recursively(Node* node, int level, std::vector<std::string>& tree_lines) const;


public:
    BPlusTree(int order);

    int search(int key);
    std::vector<int> range_search(int start_key, int end_key);
    void insert(int key, int value);
    void remove(int key);
    void print_tree() const;
};

#endif
