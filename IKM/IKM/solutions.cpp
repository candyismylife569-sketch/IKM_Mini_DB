#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cctype>
#include "header.h"

using std::string;
using std::vector;
using std::getline;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::sort;

static string g_filename;
static vector<UserRecord> g_records;

// Преобразование строки в запись
static UserRecord parseLine(const string& line) {
    UserRecord r;
    std::stringstream ss(line);
    std::string item;
    if (getline(ss, item, ',')) r.name = item;
    if (getline(ss, item, ',')) r.age = std::stoi(item);
    if (getline(ss, item, ',')) r.salary = std::stod(item);
    return r;
}

// Вспомогательная: очистка пробелов
static string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace((unsigned char)s[start])) ++start;
    size_t end = s.size();
    while (end > start && std::isspace((unsigned char)s[end - 1])) --end;
    return s.substr(start, end - start);
}

// Глобальные функции, работа с g_records
void initDb(const string& filename) {
    g_filename = filename;
}

void clearDb() {
    g_records.clear();
}

bool addRecord(const UserRecord& rec) {
    g_records.push_back(rec);
    return true;
}

vector<UserRecord> getAllRecords() {
    return g_records;
}

bool deleteRecordByIndex(size_t index) {
    if (index >= g_records.size()) return false;
    g_records.erase(g_records.begin() + index);
    return true;
}

vector<UserRecord> searchByField(const string& field, const string& value) {
    vector<UserRecord> result;
    for (const auto& r : g_records) {
        if (field == "name") {
            if (r.name == value) result.push_back(r);
        }
        else if (field == "age") {
            if (std::to_string(r.age) == value) result.push_back(r);
        }
        else if (field == "salary") {
            if (std::to_string(r.salary) == value) result.push_back(r);
        }
    }
    return result;
}

void sortByField(const string& field) {
    if (field == "name") {
        sort(g_records.begin(), g_records.end(), [](const UserRecord& a, const UserRecord& b) {
            return a.name < b.name;
            });
    }
    else if (field == "age") {
        sort(g_records.begin(), g_records.end(), [](const UserRecord& a, const UserRecord& b) {
            return a.age < b.age;
            });
    }
    else if (field == "salary") {
        sort(g_records.begin(), g_records.end(), [](const UserRecord& a, const UserRecord& b) {
            return a.salary < b.salary;
            });
    }
}

bool saveToFile() {
    std::ofstream ofs(g_filename, std::ios::trunc);
    if (!ofs.is_open()) return false;
    for (const auto& r : g_records) {
        ofs << r.name << "," << r.age << "," << r.salary << "\n";
    }
    return true;
}

bool loadFromFile() {
    std::ifstream ifs(g_filename);
    if (!ifs.is_open()) {
        return false;
    }
    g_records.clear();
    string line;
    while (getline(ifs, line)) {
        if (line.empty()) continue;
        g_records.push_back(parseLine(line));
    }
    return true;
}