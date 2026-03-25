#include "DataManager.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <windows.h>

DataManager::DataManager(const std::string& file_name) : file_name(file_name) {}

std::string DataManager::convertEncoding(const std::string& input,
    const char* from_encoding,
    const char* to_encoding) {
    int from_cp = 0;
    int to_cp = 0;

    if (std::strcmp(from_encoding, "UTF-8") == 0) from_cp = CP_UTF8;
    else if (std::strcmp(from_encoding, "CP1251") == 0) from_cp = 1251;
    else return input;

    if (std::strcmp(to_encoding, "UTF-8") == 0) to_cp = CP_UTF8;
    else if (std::strcmp(to_encoding, "CP1251") == 0) to_cp = 1251;
    else return input;

    // Определяем размер буфера
    int size_needed = MultiByteToWideChar(from_cp, 0,
        input.c_str(), (int)input.size(), NULL, 0);

    if (size_needed <= 0) return input;

    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(from_cp, 0,
        input.c_str(), (int)input.size(), &wstr[0], size_needed);

    int utf8_size = WideCharToMultiByte(to_cp, 0,
        wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);

    if (utf8_size <= 0) return input;

    std::string output(utf8_size, 0);
    WideCharToMultiByte(to_cp, 0,
        wstr.c_str(), (int)wstr.size(), &output[0], utf8_size, NULL, NULL);

    return output;
}

// Конвертация всей структуры Item из CP1251 в UTF-8
Item DataManager::convertItemToUTF8(const Item& item) {
    return Item{
        item.id,
        convertEncoding(item.name, "CP1251", "UTF-8"),
        convertEncoding(item.description, "CP1251", "UTF-8"),
        convertEncoding(item.url, "CP1251", "UTF-8")
    };
}

// Конвертация всей структуры Item из UTF-8 в CP1251
Item DataManager::convertItemFromUTF8(const Item& item) {
    return Item{
        item.id,
        convertEncoding(item.name, "UTF-8", "CP1251"),
        convertEncoding(item.description, "UTF-8", "CP1251"),
        convertEncoding(item.url, "UTF-8", "CP1251")
    };
}

// Вспомогательные функции для сортировки по именам
const std::locale& DataManager::getLocale() {
    static std::locale loc("Russian");
    return loc;
}

bool DataManager::less(const std::string& a, const std::string& b) {
    const auto& collate = std::use_facet<std::collate<char>>(getLocale());
    return collate.compare(a.data(), a.data() + a.size(),
        b.data(), b.data() + b.size()) < 0;
}

// Загрузить из файла (файл в UTF-8, конвертируем в CP1251)
bool DataManager::load() {
    try {
        std::ifstream file(file_name, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << file_name << std::endl;
            return false;
        }

        // Читаем весь файл
        std::string content((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        // Проверяем BOM 
        if (content.size() >= 3 &&
            static_cast<unsigned char>(content[0]) == 0xEF &&
            static_cast<unsigned char>(content[1]) == 0xBB &&
            static_cast<unsigned char>(content[2]) == 0xBF) {
            content = content.substr(3); // Убираем BOM UTF-8
        }

        // Парсим JSON
        ordered_json j = json::parse(content);

        // Получаем данные как UTF-8
        auto items_utf8 = j.get<std::vector<Item>>();

        // Конвертируем каждый элемент в CP1251
        items.clear();
        for (const auto& item_utf8 : items_utf8) {
            items.push_back(convertItemFromUTF8(item_utf8));
        }

        std::cout << "Successfully loaded " << items.size() << " items" << std::endl;
        return true;
    }
    catch (const json::exception& e) {
        std::cerr << "JSON load error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Load error: " << e.what() << std::endl;
        return false;
    }
}

// Сохранить в файл (конвертируем в UTF-8, сохраняем как UTF-8)
bool DataManager::save() {
    try {
        // Отладочный код для поиска проблемного элемента
        for (size_t i = 0; i < items.size(); ++i) {
            try {
                ordered_json j_test = items[i]; // Попытка сериализации каждого элемента
            }
            catch (const json::exception& e) {
                std::cerr << "Problem with item at index " << i << ": " << e.what() << std::endl;
                // Здесь можно добавить дополнительную диагностику
            }
        }

        // Создаем временный вектор в UTF-8
        std::vector<Item> items_utf8;
        for (const auto& item : items) {
            items_utf8.push_back(convertItemToUTF8(item));
        }

        // Создаем JSON из UTF-8 данных
        ordered_json j = items_utf8;

        // Открываем файл в бинарном режиме
        std::ofstream file(file_name, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Cannot create file: " << file_name << std::endl;
            return false;
        }

        // Записываем JSON (уже в UTF-8)
        file << j.dump(4);

        std::cout << "Successfully saved " << items.size() << " items" << std::endl;
        return true;
    }
    catch (const json::exception& e) {
        std::cerr << "JSON save error: " << e.what() << std::endl;
        return false;
    }
}

// Получить все элементы
const std::vector<Item>& DataManager::getItems() const {
    return items;
}

void DataManager::sortItemsById() {
    std::sort(items.begin(), items.end(),
        [](const Item& a, const Item& b) { return a.id < b.id; });
}

void DataManager::sortItemsByName() {
    std::sort(items.begin(), items.end(),
        [this](const Item& a, const Item& b) { return less(a.name, b.name); });
}

// Получить элемент по индексу
Item* DataManager::getItem(int id) {
    auto it = std::find_if(items.begin(), items.end(),
        [id](const Item& item) { return item.id == id; });

    if (it != items.end()) {
        return &(*it);
    }
    return nullptr;
}

// Очистить все элементы
void DataManager::clear() {
    items.clear();
}

// Добавить элемент
void DataManager::addItem(const std::string& name, const std::string& description, const std::string& url) {
    if (items.empty()) {
        items.push_back(Item(1, name, description, url));
        return;
    }

    auto it = std::max_element(items.begin(), items.end(),
        [](const Item& a, const Item& b) { return a.id < b.id; });
    items.push_back(Item(it->id + 1, name, description, url));
}

// Удалить элемент по индексу
void DataManager::removeItem(int id) {
    auto it = std::remove_if(items.begin(), items.end(),
        [id](const Item& item) { return item.id == id; });

    if (it != items.end()) {
        items.erase(it, items.end());
    }
}