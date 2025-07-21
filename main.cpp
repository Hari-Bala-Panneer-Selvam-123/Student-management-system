#include "httplib.h"
#include <fstream>
#include <sstream>
#include <iostream>
 #include <nlohmann/json.hpp>
#include <string>
#include "AVLTree.h"

using json = nlohmann::json;
using namespace std;

AVLTree tree;

// Helper: Convert Student to JSON
json student_to_json(const Student& s) {
    return {
        {"name", s.name},
        {"roll_no", s.roll_no},
        {"marks", s.marks},
        {"grade", s.grade}
    };
}

// Helper: Convert JSON to Student
Student json_to_student(const json& j) {
    return { j["name"], j["roll_no"], j["marks"], j["grade"] };
}

// Load students from file into AVL tree
void load_students() {
    ifstream in("students.json");
    if (in) {
        json j;
        in >> j;
        for (auto& el : j.items()) {
            Student s = json_to_student(el.value());
            tree.insert(s);
        }
    }
}

// Save students from AVL tree to file
void save_students_recursive(AVLTree::Node* node, json& j) {
    if (!node) return;
    save_students_recursive(node->left, j);
    j[to_string(node->data.roll_no)] = student_to_json(node->data);
    save_students_recursive(node->right, j);
}

void save_students() {
    json j;
    save_students_recursive(tree.getRoot(), j);
    ofstream out("students.json");
    out << j.dump(4);
}

int main() {
    httplib::Server svr;

    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        return httplib::Server::HandlerResponse::Unhandled;
    });

    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    load_students();

    svr.Get("/students", [](const httplib::Request&, httplib::Response& res) {
        json j;
        tree.list_students_json(j);
        res.set_content(j.dump(), "application/json");
    });

    svr.Get(R"(/students/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int roll_no = stoi(req.matches[1]);
        Student* s = tree.search(roll_no);
        if (s) {
            res.set_content(student_to_json(*s).dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content("Student not found", "text/plain");
        }
    });

    svr.Post("/students", [](const httplib::Request& req, httplib::Response& res) {
        auto j = json::parse(req.body);
        Student s = json_to_student(j);
        tree.insert(s);
        save_students();
        res.set_content("Student added", "text/plain");
    });

    svr.Put(R"(/students/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int roll_no = stoi(req.matches[1]);
        Student* s = tree.search(roll_no);
        if (s) {
            auto j = json::parse(req.body);
            tree.delete_student(roll_no);
            tree.insert(json_to_student(j));
            save_students();
            res.set_content("Student updated", "text/plain");
        } else {
            res.status = 404;
            res.set_content("Student not found", "text/plain");
        }
    });

    svr.Delete(R"(/students/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int roll_no = stoi(req.matches[1]);
        Student* s = tree.search(roll_no);
        if (s) {
            tree.delete_student(roll_no);
            save_students();
            res.set_content("Student deleted", "text/plain");
        } else {
            res.status = 404;
            res.set_content("Student not found", "text/plain");
        }
    });

    cout << "Server started at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);

    return 0;
}
