#include "ScreenManager.h"
#include <limits>

ScreenManager::ScreenManager() {
    screens[ScreenType::MAIN] = std::make_unique<MainScreen>();
    screens[ScreenType::CONTENT] = std::make_unique<ContentScreen>();
    screens[ScreenType::CRYPTION] = std::make_unique<CryptionScreen>();
}

void ScreenManager::run() {
    while (true) {
        if (!screens.count(currentScreen) || !screens[currentScreen]) {
            std::cerr << "Ошибка: экран " << static_cast<int>(currentScreen) << " не инициализирован!" << std::endl;
            break;
        }
        screens[currentScreen]->show();

        int choice;
        std::cout << "\nВыберите опцию: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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