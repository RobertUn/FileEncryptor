#pragma once

#include "BaseScreen.h"
#include "../AuthenticationModule/CryptoManager.h"
#include <map>
#include <functional>
#include <string>

class CryptionScreen : public BaseScreen {
private:
    CryptoManager cm;
    std::map<int, std::pair<std::string, std::function<void()>>> menu_options;

    void checkFileStatus();
    void crypting();

public:
    CryptionScreen();
    void show() override;
    void handleInput(int input) override;
};