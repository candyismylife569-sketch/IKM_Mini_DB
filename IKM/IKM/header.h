#ifndef MINI_SGBD_H
#define MINI_SGBD_H

#include <string>
#include <vector>

struct UserRecord {
    std::string name;
    int age;
    double salary;
};

// Основные функции
void InitDb(const std::string& filename);
void ClearDb();
bool AddRecord(const UserRecord& rec);
std::vector<UserRecord> GetAllRecords();
bool DeleteRecordByIndex(std::size_t index);
std::vector<UserRecord> SearchByField(const std::string& field, const std::string& value);
void SortByField(const std::string& field);
bool SaveToFile();
bool LoadFromFile();

#endif