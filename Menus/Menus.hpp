#pragma once

#include <functional>
#include <vector>

void nullopCallback(int val);
void callback(int val);

class Menu
{
public:
    Menu();

    int id;

    /// @brief Shared creation tasks for all menus
    virtual void Create();
    void Attach(int button);
    void AddSubMenu(Menu* menu);
protected:
    const char* name;
    std::vector<const char*> buttons;
    std::vector<std::function<void(int)>> callbacks; // https://en.cppreference.com/w/cpp/utility/functional/function
};

class MainMenu : public Menu { public: MainMenu(); };

class idkMenu : public Menu { public: idkMenu(); };

class LightMenu : public Menu { public: LightMenu(); };