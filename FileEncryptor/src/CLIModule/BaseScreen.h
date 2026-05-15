#pragma once

#include <string>
#include <map>
#include <iostream>

const std::string MAIN_FILE = "data/data.json";

enum class ScreenType {
    MAIN,
    CRYPTION,
    CONTENT,
    ADD
};

// Базовый абстрактный класс для всех экранов
class BaseScreen {
protected:
    std::string status = "Готов к работе";
    ScreenType nextScreen = ScreenType::MAIN; // По умолчанию возвращаемся в главное меню

    void drawBox(const std::string& title, const std::map<int, std::string>& options) const;
    void clearScreen() const;

public:
    virtual void show() = 0;
    virtual void handleInput(int choice) = 0;
    virtual ScreenType getNextScreen() const { return nextScreen; }
    virtual ~BaseScreen() = default;
};