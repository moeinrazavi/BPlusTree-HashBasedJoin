// b_plus_tree.cpp
#include "b_plus_tree.h"
#include <sstream>

Node::Node(int order, bool is_leaf) : is_leaf(is_leaf), parent(nullptr) {
    min_keys = is_leaf ? std::floor((order + 1) / 2) : std::ceil((order + 1) / 2) - 1;
    max_keys = order;
    num_keys = keys.size();
}

InternalNode::InternalNode(int order) : Node(order, false) {}

LeafNode::LeafNode(int order) : Node(order, true), next(nullptr) {}

BPlusTree::BPlusTree(int order) : order(order), root(nullptr) {}

Node* BPlusTree::find_leaf_node(int key) {
    if (!root) return nullptr;

    Node* node = root;
    while (!node->is_leaf) {
        InternalNode* internal_node = static_cast<InternalNode*>(node);
        auto it = std::upper_bound(internal_node->keys.begin(), internal_node->keys.end(), key);
        int index = it - internal_node->keys.begin();
        node = internal_node->pointers[index];
    }

    return node;
}

void BPlusTree::insert_into_leaf_node(LeafNode* leaf, int key, int value) {
    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    int index = it - leaf->keys.begin();
    leaf->keys.insert(it, key);
    leaf->values.insert(leaf->values.begin() + index, value);
}

void BPlusTree::insert_into_internal_node(InternalNode* parent, int key, Node* child) {
    auto it = std::lower_bound(parent->keys.begin(), parent->keys.end(), key);
    int index = it - parent->keys.begin();
    parent->keys.insert(it, key);
    parent->pointers.insert(parent->pointers.begin() + index + 1, child);
    child->parent = parent;
}

void BPlusTree::split_leaf_node(LeafNode* leaf) {
    int mid = order / 2;
    LeafNode* new_leaf = new LeafNode(order);

    new_leaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
    new_leaf->values.assign(leaf->values.begin() + mid, leaf->values.end());
    leaf->keys.erase(leaf->keys.begin() + mid, leaf->keys.end());
    leaf->values.erase(leaf->values.begin() + mid, leaf->values.end());

    new_leaf->next = leaf->next;
    leaf->next = new_leaf;

    if (!leaf->parent) {
        InternalNode* new_root = new InternalNode(order);
        root = new_root;
        new_root->keys.push_back(new_leaf->keys[0]);
        new_root->pointers.push_back(leaf);
        new_root->pointers.push_back(new_leaf);
        leaf->parent = new_root;
        new_leaf->parent = new_root;
    } else {
        InternalNode* parent = static_cast<InternalNode*>(leaf->parent);
        int new_key = new_leaf->keys[0];
        insert_into_internal_node(parent, new_key, new_leaf);

        while (parent->keys.size() > parent->max_keys) {
            split_internal_node(parent);

            if (!parent->parent) break;
            parent = static_cast<InternalNode*>(parent->parent);
        }
    }
}

void BPlusTree::split_internal_node(InternalNode* node) {
    int mid = order / 2;
    InternalNode* new_node = new InternalNode(order);
    new_node->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
    new_node->pointers.assign(node->pointers.begin() + mid + 1, node->pointers.end());
    node->keys.erase(node->keys.begin() + mid, node->keys.end());
    node->pointers.erase(node->pointers.begin() + mid + 1, node->pointers.end());

    for (Node* child : new_node->pointers) {
        child->parent = new_node;
    }

    if (!node->parent) {
        InternalNode* new_root = new InternalNode(order);
        root = new_root;
        new_root->keys.push_back(node->keys[mid]);
        new_root->pointers.push_back(node);
        new_root->pointers.push_back(new_node);
        node->parent = new_root;
        new_node->parent = new_root;
    } else {
        InternalNode* parent = static_cast<InternalNode*>(node->parent);
        int new_key = node->keys[mid];
        insert_into_internal_node(parent, new_key, new_node);

        while (parent->keys.size() > parent->max_keys) {
            split_internal_node(parent);

            if (!parent->parent) break;
            parent = static_cast<InternalNode*>(parent->parent);
        }
    }
}

void BPlusTree::delete_entry(Node* node, int key) {
    if (node->is_leaf) {
        LeafNode* leaf = static_cast<LeafNode*>(node);
        remove_from_leaf_node(leaf, key);
    } else {
        InternalNode* internal_node = static_cast<InternalNode*>(node);
        remove_from_internal_node(internal_node, key);
    }

    while (node->parent && node->keys.size() < node->min_keys) {
        InternalNode* parent = static_cast<InternalNode*>(node->parent);
        int index = std::lower_bound(parent->keys.begin(), parent->keys.end(), node->keys[0]) - parent->keys.begin();
        Node* left_sibling = index > 0 ? parent->pointers[index - 1] : nullptr;
        Node* right_sibling = index < parent->keys.size() ? parent->pointers[index + 1] : nullptr;

        if (left_sibling && left_sibling->keys.size() > left_sibling->min_keys) {
            borrow_key(left_sibling, node, parent, index - 1);
        } else if (right_sibling && right_sibling->keys.size() > right_sibling->min_keys) {
            borrow_key(node, right_sibling, parent, index);
        } else if (left_sibling) {
            merge_nodes(left_sibling, node, parent, index - 1);
            node = left_sibling;
        } else {
            merge_nodes(node, right_sibling, parent, index);
        }
    }

    if (root->keys.empty() && !root->is_leaf) {
        InternalNode* old_root = static_cast<InternalNode*>(root);
        root = old_root->pointers[0];
        root->parent = nullptr;
        delete old_root;
    }
}

//void BPlusTree::remove_from_leaf_node(LeafNode* leaf, int key) {
//    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
//    int index = it - leaf->keys.begin();
//    leaf->keys.erase(it);
//    leaf->values.erase(leaf->values.begin() + index);
//}

void BPlusTree::remove_from_leaf_node(LeafNode* leaf, int key) {
    int index = 0;
    while (index < leaf->num_keys && leaf->keys[index] < key) {
        ++index;
    }

    if (index < leaf->num_keys && leaf->keys[index] == key) {
        // Shift keys and values to the left to fill the gap
        for (int i = index; i < leaf->num_keys - 1; ++i) {
            leaf->keys[i] = leaf->keys[i + 1];
            leaf->values[i] = leaf->values[i + 1];
        }
        leaf->num_keys -= 1;
    }
}



void BPlusTree::remove_from_internal_node(InternalNode* node, int key) {
    auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key);
    int index = it - node->keys.begin();
    node->keys.erase(it);
    node->pointers.erase(node->pointers.begin() + index + 1);
}

void BPlusTree::borrow_key(Node* left, Node* right, InternalNode* parent, int index) {
    if (left->is_leaf) {
        LeafNode* left_leaf = static_cast<LeafNode*>(left);
        LeafNode* right_leaf = static_cast<LeafNode*>(right);
        left_leaf->keys.push_back(right_leaf->keys[0]);
        left_leaf->values.push_back(right_leaf->values[0]);
        right_leaf->keys.erase(right_leaf->keys.begin());
        right_leaf->values.erase(right_leaf->values.begin());
        parent->keys[index] = right_leaf->keys[0];
    } else {
        InternalNode* left_internal = static_cast<InternalNode*>(left);
        InternalNode* right_internal = static_cast<InternalNode*>(right);
        left_internal->keys.push_back(parent->keys[index]);
        left_internal->pointers.push_back(right_internal->pointers[0]);
        right_internal->pointers[0]->parent = left_internal;
        parent->keys[index] = right_internal->keys[0];
        right_internal->keys.erase(right_internal->keys.begin());
        right_internal->pointers.erase(right_internal->pointers.begin());
    }
}

void BPlusTree::merge_nodes(Node* left, Node* right, InternalNode* parent, int index) {
    if (left->is_leaf) {
        LeafNode* left_leaf = static_cast<LeafNode*>(left);
        LeafNode* right_leaf = static_cast<LeafNode*>(right);
        left_leaf->keys.insert(left_leaf->keys.end(), right_leaf->keys.begin(), right_leaf->keys.end());
        left_leaf->values.insert(left_leaf->values.end(), right_leaf->values.begin(), right_leaf->values.end());
        left_leaf->next = right_leaf->next;
    } else {
        InternalNode* left_internal = static_cast<InternalNode*>(left);
        InternalNode* right_internal = static_cast<InternalNode*>(right);
        left_internal->keys.push_back(parent->keys[index]);
        left_internal->keys.insert(left_internal->keys.end(), right_internal->keys.begin(), right_internal->keys.end());
        left_internal->pointers.insert(left_internal->pointers.end(), right_internal->pointers.begin(), right_internal->pointers.end());

        for (Node* child : right_internal->pointers) {
            child->parent = left_internal;
        }
    }

    delete_entry(parent, parent->keys[index]);
    delete right;
}

void BPlusTree::insert(int key, int value) {
    Node* leaf = find_leaf_node(key);
    if (!leaf) {
        root = new LeafNode(order);
        leaf = root;
    }

    insert_into_leaf_node(static_cast<LeafNode*>(leaf), key, value);

    if (leaf->keys.size() > leaf->max_keys) {
        split_leaf_node(static_cast<LeafNode*>(leaf));
    }
}

void BPlusTree::remove(int key) {
    Node* node = find_leaf_node(key);
    if (!node) return;

    delete_entry(node, key);
}

int BPlusTree::search(int key) {
    Node* leaf = find_leaf_node(key);
    if (!leaf) return -1;

    LeafNode* leaf_node = static_cast<LeafNode*>(leaf);
    auto it = std::lower_bound(leaf_node->keys.begin(), leaf_node->keys.end(), key);
    if (it == leaf_node->keys.end() || *it != key) {
        return -1;
    }

    int index = it - leaf_node->keys.begin();
    return leaf_node->values[index];
}

std::vector<int> BPlusTree::range_search(int start, int end) {
    std::vector<int> result;
    Node* leaf = find_leaf_node(start);
    if (!leaf) return result;

    LeafNode* leaf_node = static_cast<LeafNode*>(leaf);
    while (leaf_node) {
        for (size_t i = 0; i < leaf_node->keys.size(); ++i) {
            if (leaf_node->keys[i] >= start && leaf_node->keys[i] <= end) {
                result.push_back(leaf_node->values[i]);
            } else if (leaf_node->keys[i] > end) {
                return result;
            }
        }
        leaf_node = leaf_node->next;
    }

    return result;
}

void BPlusTree::print_tree() const {
    if (root == nullptr) {
        std::cout << "The tree is empty." << std::endl;
        return;
    }

    std::queue<Node*> q;
    q.push(root);

    while (!q.empty()) {
        int sz = q.size();
        for (int i = 0; i < sz; i++) {
            Node* node = q.front();
            q.pop();

            // Print keys in the node
            for (int key : node->keys) {
                std::cout << key << " ";
            }

            std::cout << "|| ";  // Separator for adjacent nodes

            if (!node->is_leaf) {
                for (Node* child : static_cast<InternalNode*>(node)->pointers) {
                    q.push(child);
                }
            }
        }
        std::cout << std::endl;
    }
}

//void BPlusTree::print_tree() const {
//    std::vector<std::string> tree_lines;
//    print_tree_recursively(root, 0, tree_lines);
//    for (const auto& line : tree_lines) {
//        std::cout << line << std::endl;
//    }
//}
//
//void BPlusTree::print_tree_recursively(Node* node, int level, std::vector<std::string>& tree_lines) const {
//    if (node == nullptr) return;
//
//    if (level >= static_cast<int>(tree_lines.size())) {
//        tree_lines.push_back("");
//    }
//
//    std::stringstream node_repr;
//    if (node->is_leaf) {
//        LeafNode* leaf = static_cast<LeafNode*>(node);
//        node_repr << "[";
//        for (size_t i = 0; i < leaf->keys.size(); i++) {
//            node_repr << "(" << leaf->keys[i] << ", " << leaf->values[i] << ")";
//            if (i < leaf->keys.size() - 1) {
//                node_repr << ", ";
//            }
//        }
//        node_repr << "]";
//    } else {
//        InternalNode* internal = static_cast<InternalNode*>(node);
//        node_repr << "<";
//        for (size_t i = 0; i < internal->keys.size(); i++) {
//            node_repr << internal->keys[i];
//            if (i < internal->keys.size() - 1) {
//                node_repr << ", ";
//            }
//        }
//        node_repr << ">";
//    }
//
//    tree_lines[level] += node_repr.str() + "  ";
//
//    if (!node->is_leaf) {
//        InternalNode* internal = static_cast<InternalNode*>(node);
//        for (Node* child : internal->pointers) {
//            print_tree_recursively(child, level + 1, tree_lines);
//        }
//    }
//}


