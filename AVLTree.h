#pragma once
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct Student {
    string name;
    int roll_no;
    int marks;
    string grade;
};

class AVLTree {
public:
    struct Node {
        Student data;
        Node* left;
        Node* right;
        int height;
        Node(const Student& s) : data(s), left(nullptr), right(nullptr), height(1) {}
    };

private:
    Node* root = nullptr;

    int height(Node* n) { return n ? n->height : 0; }
    int get_balance(Node* n) { return n ? height(n->left) - height(n->right) : 0; }

    Node* right_rotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }

    Node* left_rotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    Node* insert(Node* node, const Student& student) {
        if (!node) return new Node(student);
        if (student.roll_no < node->data.roll_no)
            node->left = insert(node->left, student);
        else if (student.roll_no > node->data.roll_no)
            node->right = insert(node->right, student);
        else
            return node; // Duplicate

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = get_balance(node);

        if (balance > 1 && student.roll_no < node->left->data.roll_no)
            return right_rotate(node);
        if (balance < -1 && student.roll_no > node->right->data.roll_no)
            return left_rotate(node);
        if (balance > 1 && student.roll_no > node->left->data.roll_no) {
            node->left = left_rotate(node->left);
            return right_rotate(node);
        }
        if (balance < -1 && student.roll_no < node->right->data.roll_no) {
            node->right = right_rotate(node->right);
            return left_rotate(node);
        }
        return node;
    }

    Node* min_value_node(Node* node) {
        Node* current = node;
        while (current->left)
            current = current->left;
        return current;
    }

    Node* delete_node(Node* root, int roll_no) {
        if (!root) return root;
        if (roll_no < root->data.roll_no)
            root->left = delete_node(root->left, roll_no);
        else if (roll_no > root->data.roll_no)
            root->right = delete_node(root->right, roll_no);
        else {
            if (!root->left || !root->right) {
                Node* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else
                    *root = *temp;
                delete temp;
            } else {
                Node* temp = min_value_node(root->right);
                root->data = temp->data;
                root->right = delete_node(root->right, temp->data.roll_no);
            }
        }

        if (!root) return root;
        root->height = 1 + max(height(root->left), height(root->right));
        int balance = get_balance(root);

        if (balance > 1 && get_balance(root->left) >= 0)
            return right_rotate(root);
        if (balance > 1 && get_balance(root->left) < 0) {
            root->left = left_rotate(root->left);
            return right_rotate(root);
        }
        if (balance < -1 && get_balance(root->right) <= 0)
            return left_rotate(root);
        if (balance < -1 && get_balance(root->right) > 0) {
            root->right = right_rotate(root->right);
            return left_rotate(root);
        }

        return root;
    }

    Node* search(Node* root, int roll_no) {
        if (!root || root->data.roll_no == roll_no)
            return root;
        if (roll_no < root->data.roll_no)
            return search(root->left, roll_no);
        return search(root->right, roll_no);
    }

    void list_students_json_helper(Node* node, json& j) {
        if (!node) return;
        list_students_json_helper(node->left, j);
        j[to_string(node->data.roll_no)] = {
            {"name", node->data.name},
            {"roll_no", node->data.roll_no},
            {"marks", node->data.marks},
            {"grade", node->data.grade}
        };
        list_students_json_helper(node->right, j);
    }

public:
    void insert(const Student& student) { root = insert(root, student); }
    void delete_student(int roll_no) { root = delete_node(root, roll_no); }
    Student* search(int roll_no) {
        Node* res = search(root, roll_no);
        return res ? &res->data : nullptr;
    }
    Node* getRoot() { return root; }
    void list_students_json(json& j) { list_students_json_helper(root, j); }
};
