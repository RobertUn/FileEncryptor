#include "CryptionScreen.h"
#include <iostream>
#include <limits>

CryptionScreen::CryptionScreen() { 
    menu_options = {
        {0, {"Назад", [this] { nextScreen = ScreenType::MAIN; }}},
        {1, {"Шифрование", [this] { crypting(); }}},
        {2, {"Загрузи последний бэкап", [this] { cm.restoreLastFile(); }}},
        {3, {"Просмотреть статус файла", [this] { checkFileStatus(); }}}
    };
    nextScreen = ScreenType::CRYPTION;
}

void CryptionScreen::checkFileStatus() {
    if (cm.isFileEncrypted()) {
        status = "Файл зашифрован";
    }
    else {
        status = "Файл расшифрован";
    }
}

void CryptionScreen::crypting() {
    std::string input_key;
    std::cout << "\nВведите ключ для шифрования/дешифрования: ";
    std::cin >> input_key;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    cm.processCrypto(input_key);
}

void CryptionScreen::show() {
    clearScreen();
    std::map<int, std::string> list_options;
    for (const auto& [key, value] : menu_options) {
        list_options[key] = value.first;
    }
    drawBox("Меню шифрования", list_options);
}

void CryptionScreen::handleInput(int input) {
    if (menu_options.count(input)) {
        try {
            nextScreen = ScreenType::CRYPTION;
            menu_options[input].second();
            // status = "Успешно: " + menu_options[input].first;  // Раскомментировать при необходимости
        }
        catch (...) {
            status = "Ошибка при выполнении операции!";
            nextScreen = ScreenType::CRYPTION;
        }
    }
    else {
        status = "Неверный пункт меню!";
        nextScreen = ScreenType::CRYPTION;
    }
}