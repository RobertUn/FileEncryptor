#include <iostream>
#include <string>
#include <map>
#include <windows.h>
#include <functional>
#include "../DataModule/DataManager.h"
#include <stack>

#pragma once

using namespace std;

const string MAIN_FILE = "data.json";

enum class ScreenType {
	MAIN,
	CONTENT,
    ADD
};

// Базовый абстрактный класс для всех экранов
class BaseScreen {
protected:
	string status = "Готов к работе";
	ScreenType nextScreen = ScreenType::MAIN; // По умолчанию возвращаемся в главное меню

    void drawBox(const string& title, const map<int, string>& options) const {
        // Рассчитываем максимальную длину
        size_t max_length = title.length() + 4;
        for (const auto& [key, value] : options) {
            max_length = max(max_length, to_string(key).length() + value.length() + 3);
        }
        max_length = max(max_length, status.length());
        max_length += 4;

        // Верхняя рамка
        cout << "+" << string(max_length - 1, '-') << "+\n";

        // Заголовок
        cout << "| " << title << string(max_length - title.length() - 2, ' ') << "|\n";        // Разделитель
        cout << "+" << string(max_length - 1, '-') << "+\n";

        // Пункты меню
        for (const auto& [key, value] : options) {
            string line = to_string(key) + ". " + value;
            cout << "| " << line << string(max_length - line.length() - 2, ' ') << "|\n";
        }

        // Разделитель статуса
        cout << "+" << string(max_length - 1, '-') << "+\n";

        // Статус
        cout << "| " << status << string(max_length - status.length() - 2, ' ') << "|\n";

        // Нижняя рамка
        cout << "+" << string(max_length - 1, '-') << "+\n";
    }

    void clearScreen() const {
        system("cls"); // Для Windows
    }

public:
    virtual void show() = 0;
    virtual void handleInput(int choice) = 0;
    virtual ScreenType getNextScreen() const { return nextScreen; }
    virtual ~BaseScreen() = default;
};

class MainScreen : public BaseScreen {
private:
    map<int, pair<string, function<void()>>> menu_options;

    void exitProgramm() {
        ExitProcess(0);
    }

public:
    MainScreen() {
        menu_options = {
            {0, {"Выход", [this] { exitProgramm(); }}},
            {1, {"Расшифровать файл", [this] {}} },
            {2, {"Просмотреть содержимое", [this] { nextScreen = ScreenType::CONTENT; }}}
        };
        nextScreen = ScreenType::MAIN;
    } 
    /*
    * "Расшифровать файл" должен показать окно, где будет окно, в котором будет показанно
    * состояние файла в данный момент(в статус баре). Будет две функции: 1 - разшифровать
    * 2 - зашифровать. Нельзя будет расшифровать расшифрованный файл, с шифрование аналогично
    * После выбора функции программа запросит пароль и примит его в любом случае
    * Далее при попытке прочитать файл если парсинг json'a выбросит оишбку, то программа
    * сообщит, что либо файл поврежден, либо пароль неверный и вернет файлу обраный вид с
    * помощью бэкапа(состояние файла должно измениться)
    */

    void show() override {
        clearScreen();

        map<int, string> list_options;
        for (const auto& [key, value] : menu_options) {
            list_options[key] = value.first;
        }
        drawBox("Главное меню", list_options);
    }

    void handleInput(int input) override {
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
};

class ContentScreen : public BaseScreen {
private:
    map<int, string> list_content;
    DataManager dm;

    std::string getURL(int item_index) {
        auto* item = dm.getItem(item_index);
        return item ? item->url : "";
    }

    bool copyToClipboard(std::string_view text) {
        if (text.empty()) return false;

        // Определяем, в какой кодировке строка
        bool is_utf8 = false;
        bool is_cp1251 = false;

        for (unsigned char c : text) {
            if (c >= 0x80) {
                // Проверяем признаки UTF-8 (2-байтовые символы)
                if (c == 0xD0 || c == 0xD1) {
                    is_utf8 = true;
                }
                // В CP1251 русские буквы в диапазоне 0x80-0xFF
                else if ((c >= 0x80 && c <= 0xFF) && !(c == 0xD0 || c == 0xD1)) {
                    is_cp1251 = true;
                }
            }
        }

        UINT code_page = CP_UTF8; // по умолчанию UTF-8

        if (is_cp1251 && !is_utf8) {
            code_page = 1251;
            std::cout << "Detected CP1251 encoding" << std::endl;
        }
        else if (is_utf8) {
            std::cout << "Detected UTF-8 encoding" << std::endl;
        }
        else {
            std::cout << "ASCII only, using UTF-8" << std::endl;
        }

        // Конвертируем в UTF-16 с правильной кодовой страницей
        int size_needed = MultiByteToWideChar(
            code_page, 0, text.data(),
            static_cast<int>(text.size()), nullptr, 0
        );

        if (size_needed == 0) return false;

        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (size_needed + 1) * sizeof(wchar_t));
        if (!hMem) return false;

        wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hMem));
        if (!pMem) {
            GlobalFree(hMem);
            return false;
        }

        MultiByteToWideChar(code_page, 0, text.data(),
            static_cast<int>(text.size()), pMem, size_needed);
        pMem[size_needed] = L'\0';
        GlobalUnlock(hMem);

        if (!OpenClipboard(nullptr)) {
            GlobalFree(hMem);
            return false;
        }

        EmptyClipboard();
        bool result = (SetClipboardData(CF_UNICODETEXT, hMem) != nullptr);
        CloseClipboard();

        if (!result) GlobalFree(hMem);
        return result;
    }

public:
    ContentScreen() : dm(MAIN_FILE) { 
        list_content = { {0, "Назад"} };
        nextScreen = ScreenType::CONTENT;
    }


    void show() override {
        clearScreen();
        // Убираем создание нового DataManager, используем существующий
        if (dm.load()) {  // используем dm, а не новый объект
            list_content.clear();  // очищаем перед заполнением
            list_content[0] = "Назад";
            for (const auto& item : dm.getItems()) {
                list_content[item.id] = item.name;
            }
        }
        drawBox("Содержимое файла", list_content);
    }

    void handleInput(int input) override {
        if (input == 0) {
            nextScreen = ScreenType::MAIN;
            status = "Возврат в предыдущее меню";
        }
        else if (list_content.count(input)) {
            try {
                if (copyToClipboard(dm.getItem(input)->url)) {
                    status = "Успешно скопировано в буфер обмена!";
                }
                else {
                    status = "Ошибка копирования в буфер обмена!";
                }
                nextScreen = ScreenType::CONTENT;
            }
            catch (...) {
                status = "Ошибка при выполнении операции!";
                nextScreen = ScreenType::CONTENT;
            }
        }
        else {
            status = "Неверный пункт меню!";
            nextScreen = ScreenType::CONTENT;
        }
    }
};

class ScreenManager {
private:
    map<ScreenType, unique_ptr<BaseScreen>> screens;
    ScreenType currentScreen = ScreenType::MAIN;
    stack<ScreenType> screenHistory;

public:
    ScreenManager() {
        screens[ScreenType::MAIN] = make_unique<MainScreen>();
        screens[ScreenType::CONTENT] = make_unique<ContentScreen>();
    }

    void run() {
        while (true) {
            screens[currentScreen]->show();

            int choice;
            cout << "\nВыберите опцию: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                continue;
            }

            screens[currentScreen]->handleInput(choice);
            ScreenType nextScreen = screens[currentScreen]->getNextScreen();

            if (nextScreen != currentScreen) {
                screenHistory.push(currentScreen);
                currentScreen = nextScreen;
            }
            else if (choice == 0 && !screenHistory.empty()) {
                currentScreen = screenHistory.top();
                screenHistory.pop();
            }
        }
    }
};

