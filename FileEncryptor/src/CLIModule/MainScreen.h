#pragma once

#include "BaseScreen.h"
#include <map>
#include <functional>
#include <string>

class MainScreen : public BaseScreen {
private:
    std::map<int, std::pair<std::string, std::function<void()>>> menu_options;

    void exitProgram();

public:
    MainScreen();

    void show() override;
    void handleInput(int input) override;
};