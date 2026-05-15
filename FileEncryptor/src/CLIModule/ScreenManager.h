#pragma once

#include "BaseScreen.h"
#include "MainScreen.h"
#include "ContentScreen.h"
#include "CryptionScreen.h"
#include <map>
#include <memory>
#include <stack>
#include <iostream>

class ScreenManager {
private:
    std::map<ScreenType, std::unique_ptr<BaseScreen>> screens;
    ScreenType currentScreen = ScreenType::MAIN;
    std::stack<ScreenType> screenHistory;

public:
    ScreenManager();
    void run();
};