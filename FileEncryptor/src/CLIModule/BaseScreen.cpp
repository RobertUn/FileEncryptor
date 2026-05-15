#include "BaseScreen.h"
#include <cstdlib>  // для system()

void BaseScreen::drawBox(const std::string& title, const std::map<int, std::string>& options) const {
    // Рассчитываем максимальную длину
    size_t max_length = title.length() + 4;
    for (const auto& [key, value] : options) {
        max_length = std::max(max_length, std::to_string(key).length() + value.length() + 3);
    }
    max_length = std::max(max_length, status.length());
    max_length += 4;

    // Верхняя рамка
    std::cout << "+" << std::string(max_length - 1, '-') << "+\n";

    // Заголовок
    std::cout << "| " << title << std::string(max_length - title.length() - 2, ' ') << "|\n";
    // Разделитель
    std::cout << "+" << std::string(max_length - 1, '-') << "+\n";

    // Пункты меню
    for (const auto& [key, value] : options) {
        std::string line = std::to_string(key) + ". " + value;
        std::cout << "| " << line << std::string(max_length - line.length() - 2, ' ') << "|\n";
    }

    // Разделитель статуса
    std::cout << "+" << std::string(max_length - 1, '-') << "+\n";

    // Статус
    std::cout << "| " << status << std::string(max_length - status.length() - 2, ' ') << "|\n";

    // Нижняя рамка
    std::cout << "+" << std::string(max_length - 1, '-') << "+\n";
}

void BaseScreen::clearScreen() const {
    system("cls"); // Для Windows
}