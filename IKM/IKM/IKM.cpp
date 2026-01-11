#include <iostream>
#include <limits>
#include <windows.h>
#include <chrono>
#include <thread>
#include "header.h"

#ifdef max
#undef max
#endif

void SetColor(int color) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console_handle, color);
}

void ResetColor() {
    SetColor(7);
}

void PrintColored(const std::string& text, int color) {
    SetColor(color);
    std::cout << text;
    ResetColor();
}

void PrintHeader(const std::string& text) {
    PrintColored("\n========================================\n", 11);
    PrintColored("  " + text + "\n", 14);
    PrintColored("========================================\n", 11);
}

void PrintSuccess(const std::string& text) {
    PrintColored("[+] " + text + "\n", 10);
}

void PrintError(const std::string& text) {
    PrintColored("[-] " + text + "\n", 12);
}

void PrintWarning(const std::string& text) {
    PrintColored("[!] " + text + "\n", 14);
}

void PrintMenu() {
    system("cls");

    PrintHeader("Мини-СУБД: Управление таблицей пользователей");

    PrintColored("\nОсновные операции:\n", 15);
    PrintColored(" 1. ", 9); PrintColored("Добавить запись\n", 7);
    PrintColored(" 2. ", 9); PrintColored("Вывести все записи\n", 7);
    PrintColored(" 3. ", 9); PrintColored("Удалить запись по индексу\n", 7);
    PrintColored(" 4. ", 9); PrintColored("Поиск по полю\n", 7);
    PrintColored(" 5. ", 9); PrintColored("Сортировка по полю\n", 7);

    PrintColored("\nРабота с файлами:\n", 15);
    PrintColored(" 6. ", 2); PrintColored("Сохранить в файл\n", 7);
    PrintColored(" 7. ", 2); PrintColored("Загрузить из файла\n", 7);

    PrintColored("\n 0. ", 12); PrintColored("Выход\n", 12);
    PrintColored("\n========================================\n", 11);
    PrintColored("Выбор: ", 14);
}

const std::string kDbFile = "mini_table.txt";

bool IsValidName(const std::string& name) {
    if (name.empty()) {
        PrintError("Имя не может быть пустым");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return false;
    }

    for (char c : name) {
        if (!((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == ' ')) {
            PrintError("Имя должно содержать только латинские буквы и пробелы");
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return false;
        }
    }

    return true;
}

int main() {
    InitDb(kDbFile);
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    LoadFromFile();

    int choice = -1;
    while (choice != 0) {
        PrintMenu();

        if (!(std::cin >> choice)) {
            PrintError("Ошибка ввода. Введите число от 0 до 7.");
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            std::this_thread::sleep_for(std::chrono::seconds(2));
            choice = -1;
            continue;
        }
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        if (choice == 0) {
            PrintHeader("Выход из программы");
            PrintColored("Сохранить данные перед выходом? (y/n): ", 14);
            char save_choice;
            std::cin >> save_choice;
            if (save_choice == 'y' || save_choice == 'Y') {
                if (SaveToFile()) {
                    PrintSuccess("Данные сохранены успешно!");
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
            PrintColored("\nДо свидания!\n", 11);
            break;
        }

        switch (choice) {
        case 1: {
            PrintHeader("Добавление новой записи");

            UserRecord rec;
            bool valid_name = false;

            while (!valid_name) {
                PrintColored("Введите имя (только латинские буквы): ", 7);
                std::getline(std::cin, rec.name);

                valid_name = IsValidName(rec.name);
            }

            PrintColored("Введите возраст: ", 7);
            while (!(std::cin >> rec.age)) {
                PrintError("Некорректный ввод возраста. Введите целое число.");
                std::cin.clear();
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                PrintColored("Введите возраст: ", 7);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            if (rec.age <= 0 || rec.age > 150) {
                PrintError("Возраст должен быть в диапазоне от 1 до 150");
                std::this_thread::sleep_for(std::chrono::seconds(2));
                PrintColored("Введите корректный возраст (1-150): ", 7);
                std::cin >> rec.age;
            }

            PrintColored("Введите зарплату: ", 7);
            while (!(std::cin >> rec.salary)) {
                PrintError("Некорректный ввод зарплаты. Введите число.");
                std::cin.clear();
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                PrintColored("Введите зарплату: ", 7);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            if (rec.salary < 0) {
                PrintError("Зарплата не может быть отрицательной");
                std::this_thread::sleep_for(std::chrono::seconds(2));
                PrintColored("Введите положительную зарплату: ", 7);
                std::cin >> rec.salary;
            }

            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

            if (AddRecord(rec)) {
                PrintSuccess("Запись успешно добавлена!");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                PrintError("Не удалось добавить запись.");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            break;
        }
        case 2: {
            PrintHeader("Список всех записей");

            std::vector<UserRecord> all = GetAllRecords();
            if (all.empty()) {
                PrintColored("Записей нет.\n", 8);
            }
            else {
                PrintColored("\n", 15);
                PrintColored("№   Имя                 Возраст  Зарплата\n", 11);
                PrintColored("---------------------------------------------\n", 8);

                for (std::size_t i = 0; i < all.size(); ++i) {
                    const UserRecord& r = all[i];
                    std::cout << " ";
                    PrintColored(std::to_string(i), 9);
                    std::cout << "   ";
                    PrintColored(r.name, 14);
                    if (r.name.length() < 18) {
                        std::cout << std::string(18 - r.name.length(), ' ');
                    }
                    std::cout << "  ";
                    PrintColored(std::to_string(r.age), 10);
                    std::cout << "      ";
                    std::cout << r.salary << "\n";
                }
                PrintColored("---------------------------------------------\n", 8);
                PrintColored("Всего записей: ", 15);
                PrintColored(std::to_string(all.size()) + "\n", 11);
            }

            PrintColored("\nНажмите Enter для продолжения...", 8);
            std::cin.get();
            break;
        }
        case 3: {
            PrintHeader("Удаление записи");

            std::vector<UserRecord> all = GetAllRecords();
            if (all.empty()) {
                PrintError("Записей нет для удаления.");
                std::this_thread::sleep_for(std::chrono::seconds(2));
                break;
            }

            PrintColored("Доступные индексы: 0 - " + std::to_string(all.size() - 1) + "\n", 7);
            PrintColored("Введите индекс для удаления: ", 7);

            std::size_t idx;
            if (!(std::cin >> idx)) {
                PrintError("Некорректный ввод индекса. Введите целое число.");
                std::cin.clear();
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                std::this_thread::sleep_for(std::chrono::seconds(2));
                break;
            }

            // Проверка, что индекс не превышает доступный
            if (idx >= all.size()) {
                PrintError("ОШИБКА: Индекс " + std::to_string(idx) +
                    " превышает доступный диапазон (0-" +
                    std::to_string(all.size() - 1) + ")");
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                break;
            }

            if (DeleteRecordByIndex(idx)) {
                PrintSuccess("Запись успешно удалена!");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                PrintError("Не удалось удалить запись.");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            break;
        }
        case 4: {
            PrintHeader("Поиск по полю");

            std::string field, value;
            PrintColored("По какому полю искать? (name|age|salary): ", 7);
            std::getline(std::cin, field);

            if (field != "name" && field != "age" && field != "salary") {
                PrintError("Допустимые поля: name, age, salary");
                std::this_thread::sleep_for(std::chrono::seconds(2));
                break;
            }

            PrintColored("Введите значение для поиска: ", 7);
            std::getline(std::cin, value);

            std::vector<UserRecord> res = SearchByField(field, value);
            if (res.empty()) {
                PrintColored("Совпадений не найдено.\n", 12);
            }
            else {
                PrintSuccess("Найдено записей: " + std::to_string(res.size()));
                PrintColored("\nРезультаты поиска:\n", 15);
                PrintColored("---------------------------------------------\n", 8);

                for (const UserRecord& r : res) {
                    std::cout << " * " << r.name << ", " << r.age << ", " << r.salary << "\n";
                }
            }

            PrintColored("\nНажмите Enter для продолжения...", 8);
            std::cin.get();
            break;
        }
        case 5: {
            PrintHeader("Сортировка записей");

            std::string field;
            PrintColored("Сортировать по полю? (name|age|salary): ", 7);
            std::getline(std::cin, field);

            if (field == "name" || field == "age" || field == "salary") {
                SortByField(field);
                PrintSuccess("Записи отсортированы по полю: " + field);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                PrintError("Допустимые поля: name, age, salary");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            break;
        }
        case 6: {
            PrintHeader("Сохранение данных");

            if (SaveToFile()) {
                PrintSuccess("Все данные успешно сохранены в файл!");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                PrintError("Не удалось сохранить данные.");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            break;
        }
        case 7: {
            PrintHeader("Загрузка данных");

            if (LoadFromFile()) {
                PrintSuccess("Данные успешно загружены из файла!");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                PrintError("Не удалось загрузить данные.");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            break;
        }
        default:
            PrintError("Неверный пункт меню. Выберите 0-7.");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    return 0;
}