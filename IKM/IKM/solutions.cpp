#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cctype>
#include <iostream>
#include <chrono>
#include <thread>
#include "header.h"

std::string g_filename;
std::vector<UserRecord> g_records;

void PrintErrorWithDelay(const std::string& error) {
    std::cerr << "ОШИБКА: " << error << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

std::string Trim(const std::string& str) {
    std::size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }

    std::size_t end = str.size();
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }

    return str.substr(start, end - start);
}

bool IsNumeric(const std::string& str) {
    if (str.empty()) {
        PrintErrorWithDelay("Пустая строка при проверке числа");
        return false;
    }

    std::size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }

    if (start >= str.size()) {
        PrintErrorWithDelay("Число содержит только знак: " + str);
        return false;
    }

    bool has_digit = false;
    bool has_dot = false;

    for (std::size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.' && !has_dot) {
            has_dot = true;
        }
        else if (std::isdigit(static_cast<unsigned char>(str[i]))) {
            has_digit = true;
        }
        else {
            PrintErrorWithDelay("Строка содержит нечисловые символы: " + str);
            return false;
        }
    }

    if (!has_digit) {
        PrintErrorWithDelay("Число не содержит цифр: " + str);
        return false;
    }

    return true;
}

bool StringToInt(const std::string& str, int* result) {
    if (!IsNumeric(str)) {
        return false;
    }

    std::stringstream ss(str);
    ss >> *result;

    if (ss.fail()) {
        PrintErrorWithDelay("Не удалось преобразовать '" + str + "' в целое число");
        return false;
    }

    return true;
}

bool StringToDouble(const std::string& str, double* result) {
    if (!IsNumeric(str)) {
        return false;
    }

    std::stringstream ss(str);
    ss >> *result;

    if (ss.fail()) {
        PrintErrorWithDelay("Не удалось преобразовать '" + str + "' в число с плавающей точкой");
        return false;
    }

    return true;
}

UserRecord ParseLine(const std::string& line) {
    UserRecord record = { "", 0, 0.0 };

    std::string trimmed_line = Trim(line);
    if (trimmed_line.empty()) {
        return record;
    }

    std::stringstream ss(trimmed_line);
    std::string token;
    int field_index = 0;

    while (std::getline(ss, token, ',')) {
        std::string trimmed_token = Trim(token);

        if (field_index == 0) {
            record.name = trimmed_token;
        }
        else if (field_index == 1) {
            if (!StringToInt(trimmed_token, &record.age)) {
                PrintErrorWithDelay("Некорректный возраст в строке: " + line);
                record.age = 0;
            }
        }
        else if (field_index == 2) {
            if (!StringToDouble(trimmed_token, &record.salary)) {
                PrintErrorWithDelay("Некорректная зарплата в строке: " + line);
                record.salary = 0.0;
            }
        }
        ++field_index;
    }

    if (field_index < 3) {
        PrintErrorWithDelay("В строке недостаточно полей: " + line);
    }

    return record;
}

void InitDb(const std::string& filename) {
    if (filename.empty()) {
        PrintErrorWithDelay("Имя файла не может быть пустым");
        return;
    }
    g_filename = filename;
}

void ClearDb() {
    g_records.clear();
}

bool AddRecord(const UserRecord& rec) {
    if (rec.name.empty()) {
        PrintErrorWithDelay("Нельзя добавить запись с пустым именем");
        return false;
    }

    if (rec.age <= 0 || rec.age > 150) {
        PrintErrorWithDelay("Некорректный возраст " + std::to_string(rec.age) +
            ". Допустимый диапазон: 1-150");
        return false;
    }

    if (rec.salary < 0) {
        PrintErrorWithDelay("Зарплата не может быть отрицательной: " + std::to_string(rec.salary));
        return false;
    }

    g_records.push_back(rec);
    return true;
}

std::vector<UserRecord> GetAllRecords() {
    return g_records;
}

bool DeleteRecordByIndex(std::size_t index) {
    if (g_records.empty()) {
        PrintErrorWithDelay("База данных пуста, нечего удалять");
        return false;
    }

    if (index >= g_records.size()) {
        PrintErrorWithDelay("Индекс " + std::to_string(index) +
            " выходит за пределы базы данных (размер: " +
            std::to_string(g_records.size()) + ")");
        return false;
    }

    g_records.erase(g_records.begin() + index);
    return true;
}

std::vector<UserRecord> SearchByField(const std::string& field, const std::string& value) {
    std::vector<UserRecord> result;
    std::string trimmed_value = Trim(value);

    if (field != "name" && field != "age" && field != "salary") {
        PrintErrorWithDelay("Некорректное поле для поиска: '" + field +
            "'. Допустимые: name, age, salary");
        return result;
    }

    if (trimmed_value.empty() && field != "name") {
        PrintErrorWithDelay("Значение для поиска не может быть пустым для поля: " + field);
        return result;
    }

    for (const auto& record : g_records) {
        if (field == "name") {
            if (record.name == trimmed_value) {
                result.push_back(record);
            }
        }
        else if (field == "age") {
            int search_age;
            if (StringToInt(trimmed_value, &search_age) && record.age == search_age) {
                result.push_back(record);
            }
        }
        else if (field == "salary") {
            double search_salary;
            if (StringToDouble(trimmed_value, &search_salary) && record.salary == search_salary) {
                result.push_back(record);
            }
        }
    }

    return result;
}

void SortByField(const std::string& field) {
    if (g_records.empty()) {
        PrintErrorWithDelay("База данных пуста, нечего сортировать");
        return;
    }

    if (field == "name") {
        std::sort(g_records.begin(), g_records.end(),
            [](const UserRecord& a, const UserRecord& b) {
                return a.name < b.name;
            });
    }
    else if (field == "age") {
        std::sort(g_records.begin(), g_records.end(),
            [](const UserRecord& a, const UserRecord& b) {
                return a.age < b.age;
            });
    }
    else if (field == "salary") {
        std::sort(g_records.begin(), g_records.end(),
            [](const UserRecord& a, const UserRecord& b) {
                return a.salary < b.salary;
            });
    }
    else {
        PrintErrorWithDelay("Некорректное поле для сортировки: '" + field +
            "'. Допустимые: name, age, salary");
    }
}

bool SaveToFile() {
    if (g_filename.empty()) {
        PrintErrorWithDelay("Имя файла не установлено");
        return false;
    }

    std::ofstream ofs(g_filename, std::ios::trunc);
    if (!ofs.is_open()) {
        PrintErrorWithDelay("Не удалось открыть файл для записи: " + g_filename);
        return false;
    }

    for (const auto& record : g_records) {
        ofs << record.name << "," << record.age << "," << record.salary << "\n";

        if (ofs.fail()) {
            PrintErrorWithDelay("Ошибка записи в файл: " + g_filename);
            ofs.close();
            return false;
        }
    }

    ofs.close();
    return true;
}

bool LoadFromFile() {
    if (g_filename.empty()) {
        PrintErrorWithDelay("Имя файла не установлено");
        return false;
    }

    std::ifstream ifs(g_filename);
    if (!ifs.is_open()) {
        PrintErrorWithDelay("Не удалось открыть файл для чтения: " + g_filename +
            ". Будет создана новая база.");
        g_records.clear();
        return false;
    }

    int errors_count = 0;
    g_records.clear();
    std::string line;

    while (std::getline(ifs, line)) {
        std::string trimmed_line = Trim(line);

        if (trimmed_line.empty()) {
            continue;
        }

        UserRecord record = ParseLine(trimmed_line);

        if (!record.name.empty()) {
            g_records.push_back(record);
        }
        else {
            errors_count++;
        }
    }

    ifs.close();

    if (errors_count > 0) {
        PrintErrorWithDelay("При загрузке пропущено " + std::to_string(errors_count) +
            " некорректных строк");
    }

    return true;
}