#pragma once

#include "BaseScreen.h"
#include "../DataModule/DataManager.h"
#include <map>
#include <string>
#include <string_view>

class ContentScreen : public BaseScreen {
private:
    std::map<int, std::string> list_content;
    DataManager dm;

    std::string getURL(int item_index);
    bool copyToClipboard(std::string_view text);

public:
    ContentScreen();
    void show() override;
    void handleInput(int input) override;
};