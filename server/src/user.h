#pragma once
#include "TmplBased.h"

class User
{
private:
    string m_uuid;
    string m_name;
    string m_login;
    string m_pwd;
    int m_status;
public:
    User(const string& newName, const string& newLogin, const string& newPwd);
    User(const string& newUUID, const string& newName, const string& newLogin, const string& newPwd, int status);
    User(const User& other);
    User& operator= (const User& other);
    User(User&& other);
    User& operator= (User&& other);
    const string& getUUID() const;
    const string& getName() const;
    const string& getLogin() const;
    const string& getPwd() const;
    int getStatus() const;
    void setDeleted();
};