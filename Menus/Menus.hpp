#pragma once

#include <functional>
#include <vector>

void callback(int val);

class Menu
{
public:
    Menu();

    int id;

    /// @brief Shared creation tasks for all menus
    virtual void Create();
protected:
    std::vector<const char*> options;
    std::vector<std::function<void(int)>> callbacks; // https://en.cppreference.com/w/cpp/utility/functional/function
};

class idkMenu : public Menu { public: idkMenu(); };

class LightMenu : public Menu { public: LightMenu(); };