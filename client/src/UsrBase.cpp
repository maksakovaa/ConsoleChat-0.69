#include "UsrBase.h"

UsrBase::UsrBase()
{
    getUsrBase();
}

void UsrBase::getUsrBase()
{
    base->clear();
    vector<string> temp;
    Online->getUsrBase(temp);
    for (string str: temp)
    {
        User newUser = splitUsrPkg(str);
        addInBase(newUser);
    }
}

void UsrBase::printUsrBase()
{
    cout << std::format("{:5}", "ID") << std::format("{:12}", "Login") << std::format("{:32}", "Имя пользователя") << endl; 
    for (int i = 0; i < base->size(); i++)
    {
        if (base->at(i).getUUID() == m_uuid)
        {
            continue;
        }
        if (base->at(i).getStatus() == 1)
        {
            continue;
        }
        cout << std::format("{:<5}", i) << std::format("{:12}", base->at(i).getLogin()) << std::format("{:32}", base->at(i).getName()) << endl;
    }
}

User UsrBase::splitUsrPkg(string& usrPkg)
{
    string uuid, name, login, pwdHash;
    int deleted;
    uuid = unTag(m_Attr::indUUID, usrPkg);
    name = unTag(m_Attr::indName, usrPkg);
    login = unTag(m_Attr::indLogin, usrPkg);
    pwdHash = unTag(m_Attr::indPwd, usrPkg);
    deleted = stoi(unTag(m_Attr::indDel, usrPkg));
    User newUser(uuid, name, login, pwdHash, deleted);
    return newUser;
}

string UsrBase::packUsrPkg(const User& account)
{
    string result = tagStructParam(m_Attr::indName, account.getName());
    result += tagStructParam(m_Attr::indLogin, account.getLogin());
    result += tagStructParam(m_Attr::indPwd, account.getPwd());
    return result;
}

string UsrBase::getUsrName(int id)
{
    return base->at(id).getName();
}

string UsrBase::getUsrName(const string& uuid)
{
    for (User account: *base)
    {
        if (account.getUUID() == uuid)
        {
            return account.getName();
        }
    }
    return "deleted";
}

string UsrBase::getUsrUUID(int id)
{
    return base->at(id).getUUID();
}

string UsrBase::getUsrUUID(const string& login)
{
    for (User account: *base)
    {
        if (account.getLogin() == login)
        {
            return account.getUUID();
        }
    }
    return {};
}

string UsrBase::getUsrLogin(int id)
{
    return base->at(id).getLogin();
}

string UsrBase::getUsrLogin(const string& uuid)
{
    for (User account: *base)
    {
        if (account.getUUID() == uuid)
        {
            return account.getLogin();
        }
    }
    return "";
}

int UsrBase::getUsrId()
{
    for (int i = 0; i < (int)base->size(); i++)
    {
        if (base->at(i).getUUID() == m_uuid)
        {
            return i;
        }
    }
    return -1;
}

int UsrBase::getUsrId(string& login)
{
    for (int i = 0; i < (int)base->size(); i++)
    {
        if (base->at(i).getLogin() == login)
        {
            return i;
        }
    }
    return -1;
}

void UsrBase::setUsrPwd(int id, string& newPwd)
{
    string usrPkg = tagStructParam(m_Attr::indUUID, base->at(id).getUUID());
    usrPkg += tagStructParam(m_Attr::indPwd, sha256(newPwd));
    Online->setUsrPwd(usrPkg);
    getUsrBase();
}

void UsrBase::delUsr(int id)
{
    Online->delUser(base->at(id).getUUID());
    base->at(id).setDeleted();
}

void UsrBase::regUsr(string& name, string& login, string& pwd)
{
    User newUser(name, login, sha256(pwd));
    string usrPkg = packUsrPkg(newUser);
    Online->regUser(usrPkg);
    getUsrBase();
}

bool UsrBase::logInChk(const string& login)
{
    for (User person: *base)
    {
        if (person.getLogin() == login && person.getStatus() == 0)
        {
            if (person.getStatus() == 0)
            {
                return true;
            }
            else
            {
                return false;
            }

        }
     }
    return false;
}

bool UsrBase::pwdChk(int id, const string& pwd)
{
    string temp = sha256(pwd);
    if (base->at(id).getPwd() == temp)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool UsrBase::isDeleted(int id)
{
    if (base->at(id).getStatus() == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

string UsrBase::tagStructParam(m_Attr id, const string& value)
{
    string result = sepBeg[static_cast<int>(id)];
    result += value;
    result += sepEnd[static_cast<int>(id)];
    return result;
}

string UsrBase::unTag(m_Attr id, string& str)
{
    string result;
    result = str.substr(str.find(sepBeg[static_cast<int>(id)]) + sepBeg[static_cast<int>(id)].length(), str.find(sepEnd[static_cast<int>(id)]) - sepBeg[static_cast<int>(id)].length());
    str.erase(0, str.find(sepEnd[static_cast<int>(id)]) + sepEnd[static_cast<int>(id)].length());
    return result;
}

string UsrBase::getCurUUID()
{
    return m_uuid;
}

void UsrBase::setAuthData(const string& uuid)
{
    m_uuid = uuid;
    m_auth = true;
}

bool UsrBase::getCurAuth()
{
    return m_auth;
}

void UsrBase::logOut()
{
    m_auth = false;
    m_uuid.clear();
}