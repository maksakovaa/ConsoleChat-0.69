#include "user.h"

User::User(const string& newName, const string& newLogin, const string& newPwd): m_name(newName), m_login(newLogin), m_pwd(newPwd)
{
    m_uuid = "-";
    m_status = 0;
}

User::User(const string& newUUID, const string& newName, const string& newLogin, const string& newPwd, int status)
{
    m_uuid = newUUID;
    m_name = newName;
    m_login = newLogin;
    m_pwd = newPwd;
    m_status = status;
}

User::User(const User& other)
{
    m_uuid = other.m_uuid;
    m_name = other.m_name;
    m_login = other.m_login;
    m_pwd = other.m_pwd;
    m_status = other.m_status;
}

User& User::operator= (const User& other)
{
    if(&other == this)
        return *this;
    m_uuid = other.m_uuid;
    m_name = other.m_name;
    m_login = other.m_login;
    m_pwd = other.m_pwd;
    m_status = other.m_status;
    return *this;
}

User::User(User&& other)
{
    std::swap(m_uuid, other.m_uuid);
    std::swap(m_name, other.m_name);
    std::swap(m_login, other.m_login);
    std::swap(m_pwd, other.m_pwd);
    std::swap(m_status, other.m_status);
}

User& User::operator= (User&& other)
{
    if (this == &other)
        return *this;
    std::swap(m_uuid, other.m_uuid);
    std::swap(m_name, other.m_name);
    std::swap(m_login, other.m_login);
    std::swap(m_pwd, other.m_pwd);
    std::swap(m_status, other.m_status);
    return *this;
}

const string& User::getUUID() const {
    return m_uuid;
}
const string& User::getName() const {
    return m_name;
}
const string& User::getLogin() const {
    return m_login;
}
const string& User::getPwd() const {
    return m_pwd;
}
int User::getStatus() const {
    return m_status;
}

void User::setDeleted() {
    m_status = 1;
}