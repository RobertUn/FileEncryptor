#include "MainScreen.h"
#include <stdexcept>
#include <windows.h>

void MainScreen::exitProgram() {
    ExitProcess(0);
}

MainScreen::MainScreen() {
    menu_options = {
        {0, {"Выход", [this] { exitProgram(); }}},
        {1, {"Шифрование", [this] { nextScreen = ScreenType::CRYPTION; }}},
        {2, {"Просмотреть содержимое", [this] { nextScreen = ScreenType::CONTENT; }}}
    };
    nextScreen = ScreenType::MAIN;
}

void MainScreen::show() {
    clearScreen();

    std::map<int, std::string> list_options;
    for (const auto& [key, value] : menu_options) {
        list_options[key] = value.first;
    }
    drawBox("Главное меню", list_options);
}

void MainScreen::handleInput(int input) {
    if (menu_options.count(input)) {
        try {
            nextScreen = ScreenType::MAIN;
            menu_options[input].second();
            status = "Успешно: " + menu_options[input].first;
        }
        catch (...) {
            status = "Ошибка при выполнении операции!";
            nextScreen = ScreenType::MAIN;
        }
    }
    else {
        status = "Неверный пункт меню!";
        nextScreen = ScreenType::MAIN;
    }
}