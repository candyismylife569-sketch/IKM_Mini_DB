#include <iostream>
#include <limits>
#include "header.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

static const string DB_FILE = "mini_table.txt";

void printMenu() {
    cout << "Мини-СУБД: управление таблицей пользователей\n";
    cout << "1. Добавить запись\n";
    cout << "2. Вывести все записи\n";
    cout << "3. Удалить запись по индексу\n";
    cout << "4. Поиск по полю\n";
    cout << "5. Сортировка по полю\n";
    cout << "6. Сохранить в файл\n";
    cout << "7. Загрузить из файла\n";
    cout << "0. Выход\n";
    cout << "Выбор: ";
}

int main() {
    initDb(DB_FILE);
    setlocale(LC_ALL, "Russian");
    // Загрузим данные по умолчанию
    loadFromFile();

    int choice = -1;
    while (choice != 0) {
        printMenu();
        if (!(cin >> choice)) {
            cout << "Ошибка ввода. Повторите.\n";
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) {
            cout << "Выход.\n";
            break;
        }

        switch (choice) {
        case 1: {
            UserRecord rec;
            cout << "Введите имя: ";
            std::getline(cin, rec.name);
            cout << "Введите возраст: ";
            cin >> rec.age;
            cout << "Введите зарплату: ";
            cin >> rec.salary;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (addRecord(rec)) {
                cout << "Запись добавлена.\n";
            }
            else {
                cout << "Ошибка при добавлении.\n";
            }
            break;
        }
        case 2: {
            auto all = getAllRecords();
            if (all.empty()) {
                cout << "Записей нет.\n";
            }
            else {
                cout << "Все записи:\n";
                for (size_t i = 0; i < all.size(); ++i) {
                    const auto& r = all[i];
                    cout << i << ": " << r.name << ", " << r.age << ", " << r.salary << "\n";
                }
            }
            break;
        }
        case 3: {
            size_t idx;
            cout << "Введите индекс удаления: ";
            if (!(cin >> idx)) {
                cout << "Неверный индекс.\n";
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            if (deleteRecordByIndex(idx)) {
                cout << "Запись удалена.\n";
            }
            else {
                cout << "Некорректный индекс.\n";
            }
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        case 4: {
            string field, value;
            cout << "По какому полю искать? (name|age|salary): ";
            std::getline(cin, field);
            cout << "Значение: ";
            std::getline(cin, value);

            auto res = searchByField(field, value);
            if (res.empty()) {
                cout << "Совпадений не найдено.\n";
            }
            else {
                cout << "Найденные записи:\n";
                for (const auto& r : res) {
                    cout << r.name << ", " << r.age << ", " << r.salary << "\n";
                }
            }
            break;
        }
        case 5: {
            string field;
            cout << "Сортировать по полю? (name|age|salary): ";
            std::getline(cin, field);
            sortByField(field);
            cout << "Отсортировано по " << field << ".\n";
            break;
        }
        case 6: {
            if (saveToFile()) {
                cout << "Данные сохранены в файл.\n";
            }
            else {
                cout << "Ошибка сохранения.\n";
            }
            break;
        }
        case 7: {
            if (loadFromFile()) {
                cout << "Данные загружены из файла.\n";
            }
            else {
                cout << "Ошибка загрузки.\n";
            }
            break;
        }
        default:
            cout << "Неверный пункт меню.\n";
        }
    }

    return 0;
}