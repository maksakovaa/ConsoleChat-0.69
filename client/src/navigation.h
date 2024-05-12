#pragma once
#include "functions.h"
class Chat
{
public:
    void menuMain();
    void menuRegUser();
    void menuAuth();
    void menuAbout();
    void menuAuthorized();
    void menuUserList();
    void menuAllMsg();
    void menuPerMsg();
    void menuAdmin();
    void menuChgPwd(int id);
    void menuSetPwd();
    void menuDelUser();
private:
    int exit();
    void printUserBase();
    bool authStatChk();
    template<typename... Args>
    void checkInput(char from, char to, Args... args);
    char m_choice;
};

template<typename... Args> void Chat::checkInput(char from, char to, Args... args)
{
    if (!(m_choice >= from && m_choice <= to) && ([&]{return m_choice != args;}()&& ...))
    {
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::cout << "Некорректный ввод, повторите:\n";
        std::cin >> m_choice;
    }
}