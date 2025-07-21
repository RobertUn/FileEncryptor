#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>
#include <sstream>

using namespace std;


const string KEY = "rqwertyuiopsdfghjklZXC";
const string TRUE_FILE = "Porno.txt";

class Base64 {
private:
    const string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
public:
    Base64() = default;

    static inline bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    bool isBase64(const string& s) {
        size_t pad = count(s.begin(), s.end(), '=');
        if (pad > 2 || (pad > 0 && s.size() % 4 != 0)) return false;

        for (char c : s) {
            if (!is_base64(c) && c != '=') return false;
        }
        return true;
    }

    string base64_encode(const string& in) {
        string out;
        int val = 0, valb = -6;
        for (unsigned char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back(base64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4) out.push_back('=');
        return out;
    }

    string base64_decode(const string& in) {
        string out;
        vector<int> T(256, -1);
        for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

        int val = 0, valb = -8;
        for (unsigned char c : in) {
            if (T[c] == -1) continue; // Пропускаем невалидные символы вместо break
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                out.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return out;
    }
};

// Класс для работы с шифрованием/дешифрованием
class Cryption {
private:
    Base64 bs = Base64();

    string xorEncrypt(const string& data, const string& key) {
        if (key.empty()) return data;

        string result;
        for (size_t i = 0; i < data.size(); ++i) {
            result.push_back(data[i] ^ key[i % key.size()]);
        }
        return result;
    }

    string readFile(const string& file_patch) {
        // Чтение файла
        ifstream inFile(file_patch, ios::binary);
        if (!inFile.is_open()) {
            cerr << "Ошибка открытия файла!" << endl;
        }

        string data((istreambuf_iterator<char>(inFile)), {});
        inFile.close();
        return data;
    }
public:
    Cryption() = default;

    void encrypt(const string& file_patch, const string& key) {
        const string file_content = readFile(file_patch);
        if (bs.isBase64(file_content)) {
            cerr << "Файл уже зашифрован!" << endl;
        }

        // Шифрование → Base64
        string encrypted = xorEncrypt(file_content, key);
        string base64Encrypted = bs.base64_encode(encrypted);

        // Запись
        ofstream outFile(file_patch, ios::binary | ios::trunc);
        outFile.write(base64Encrypted.c_str(), base64Encrypted.size());
        outFile.close();

        cout << "Файл успешно зашифрован." << endl;
    }

    void decrypt(const string& file_patch, const string& key) {
        const string file_content = readFile(file_patch);
        if (!bs.isBase64(file_content)) {
            cerr << "Файл не зашифрован или поврежден!" << endl;
        }

        // Дешифровка
        string readEncrypted = bs.base64_decode(file_content);
        string decrypted = xorEncrypt(readEncrypted, key);

        // Запись
        ofstream outFile(file_patch, ios::binary | ios::trunc);
        outFile.write(decrypted.c_str(), decrypted.size());
        outFile.close();

        cout << "Файл успешно расшифрован." << endl;
    }
};

// Базовый абстрактный класс для всех экранов
class BaseScreen {
protected:
    string status = "Готов к работе";

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
        cout << "| " << title << string(max_length - title.length() - 2, ' ') << "|\n";

        // Разделитель
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
    virtual ~BaseScreen() = default;
};

class MainMenuScreen : public BaseScreen {
private:
    map<int, pair<string, function<void()>>> menu_options;
    Cryption crpt = Cryption();

public:
    MainMenuScreen() {
        menu_options = {
            {1, {"Зашифровать файл", [this] { crpt.encrypt(TRUE_FILE, KEY); }}},
            {2, {"Расшифровать файл", [this] { crpt.decrypt(TRUE_FILE, KEY); }}},
            {3, {"Просмотреть содержимое", [] { /* Просмотр */ }}},
            {0, {"Выход", [] { exit(0); }}}
        };
    }

    void show() override {
        clearScreen();

        // Подготавливаем данные для отрисовки
        map<int, string> options;
        for (const auto& [key, value] : menu_options) {
            options[key] = value.first;
        }

        drawBox("Главное меню", options);
    }

    void handleInput(int choice) override {
        if (menu_options.count(choice)) {
            try {
                menu_options[choice].second();
                status = "Успешно: " + menu_options[choice].first;
            }
            catch (...) {
                status = "Ошибка при выполнении операции!";
            }
        }
        else {
            status = "Неверный пункт меню!";
        }
    }
};

class ContentMenuScreen : public BaseScreen {
private:
    map<int, string> list_strings;
    Cryption crpt = Cryption();

    map<int, string> readFile(const string& file_patch) {
        ifstream inFile(file_patch);
        if (!inFile.is_open()) {
            cerr << "Ошибка открытия файла!" << endl;
            return list_strings;
        }

        string line;
        int line_number = 1;
        while (getline(inFile, line)) {
            list_strings[line_number++] = line;
        }
        inFile.close();
        return list_strings;
    }

    bool copyToClipboard(std::string_view text) {
        if (!OpenClipboard(nullptr)) return false;

        // Автоматическое освобождение ресурсов
        struct ClipboardGuard {
            ~ClipboardGuard() { CloseClipboard(); }
        } guard;

        EmptyClipboard();

        // Прямое преобразование в wstring (C++20)
        std::wstring wtext(text.begin(), text.end());

        // Выделение памяти с автоматическим управлением
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (wtext.size() + 1) * sizeof(wchar_t));
        if (!hMem) return false;

        auto pMem = static_cast<wchar_t*>(GlobalLock(hMem));
        wcscpy_s(pMem, wtext.size() + 1, wtext.c_str());
        GlobalUnlock(hMem);

        return SetClipboardData(CF_UNICODETEXT, hMem) != nullptr;
    }

public:
    ContentMenuScreen() {
        list_strings = { {0, "Назад"}};
    }

    void show() override {
        clearScreen();
        list_strings = readFile(TRUE_FILE);
        list_strings[0] = "Назад"; // Гарантируем, что пункт "Назад" есть
        drawBox("Содержимое файла", list_strings);
    }

    void handleInput(int choice) override {
        if (choice == 0) {
            status = "Возврат в предыдущее меню";
        }
        else if (list_strings.count(choice)) {
            try {
                if (copyToClipboard(list_strings[choice])) {
                    status = "Успешно скопировано в буфер обмена: " + list_strings[choice];
                }
                else {
                    status = "Ошибка копирования в буфер обмена!";
                }
            }
            catch (...) {
                status = "Ошибка при выполнении операции!";
            }
        }
        else {
            status = "Неверный пункт меню!";
        }
    }
};

// Менеджер экранов
class ScreenManager {
    map<string, unique_ptr<BaseScreen>> screens;
    string current_screen;

public:
    ScreenManager() {
        screens["main"] = make_unique<MainMenuScreen>();
        screens["content"] = make_unique<ContentMenuScreen>();
        current_screen = "main";
    }



    void run() {
        while (true) {
            screens[current_screen]->show();

            int choice;
            cout << "\nВыберите опцию: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                continue;
            }

            screens[current_screen]->handleInput(choice);
        }
    }
};

int main() {
    // Настройка кодировки
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");

    ScreenManager manager;
    manager.run();
    return 0;
}