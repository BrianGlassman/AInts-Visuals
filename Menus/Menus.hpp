#pragma once

#include <functional>
#include <vector>

namespace Menus{
void nullopCallback(int val);

typedef void (*Callback)(int val);

void CreateMenus();

class Menu
{
public:
    Menu();
    virtual ~Menu() {};

    int id;

    /// @brief Shared creation tasks for all menus
    void Create();

    void Attach(int button);
    void AddSubMenu(Menu* menu);
protected:
    const char* name;
    std::vector<const char*> buttons;
    Callback callback;
    // std::vector<std::function<void(int)>> callbacks; // https://en.cppreference.com/w/cpp/utility/functional/function
};

class MainMenu : public Menu { public: MainMenu(); };

class idkMenu : public Menu { public: idkMenu(); };

class LightMenu : public Menu { public: LightMenu(); };

class ViewMenu: public Menu { public: ViewMenu(); };

} // namespace Menus