#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "Item.hpp"
#include <vector>
#include <string>
#include <locale>

class DataManager {
private:
    std::vector<Item> items;
    std::string file_name;

    // Конвертация кодировок
    std::string convertEncoding(const std::string& input,
        const char* from_encoding,
        const char* to_encoding);

    // Конвертация всей структуры Item
    Item convertItemToUTF8(const Item& item);
    Item convertItemFromUTF8(const Item& item);

    // Вспомогательные функции для сортировки по именам
    static const std::locale& getLocale();
    static bool less(const std::string& a, const std::string& b);

public:
    explicit DataManager(const std::string& file_name);

    // Загрузить из файла (файл в UTF-8, конвертируем в CP1251)
    bool load();

    // Сохранить в файл (конвертируем в UTF-8, сохраняем как UTF-8)
    bool save();

    // Получить все элементы
    const std::vector<Item>& getItems() const;

    void sortItemsById();
    void sortItemsByName();

    // Получить элемент по индексу
    Item* getItem(int id);

    // Очистить все элементы
    void clear();

    // Добавить элемент
    void addItem(const std::string& name, const std::string& description, const std::string& url);

    // Удалить элемент по индексу
    void removeItem(int id);
};

#endif // DATAMANAGER_H