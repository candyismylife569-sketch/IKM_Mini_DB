#ifndef MINI_SGBD_H
#define MINI_SGBD_H

#include <string>
#include <vector>

struct UserRecord {
    std::string name;
    int age;
    double salary;
};


void initDb(const std::string& filename);
void clearDb();
bool addRecord(const UserRecord& rec);
std::vector<UserRecord> getAllRecords();
bool deleteRecordByIndex(size_t index);
std::vector<UserRecord> searchByField(const std::string& field, const std::string& value);
void sortByField(const std::string& field);
bool saveToFile();
bool loadFromFile();

#endif 