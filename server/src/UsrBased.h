#pragma once
#include "user.h"

class UsrBase: public Base<User>
{
private:
    static const int usrAttr{ 5 };
    enum class m_Attr { indUUID, indName, indLogin, indPwd, indDel };
    string sepBeg[usrAttr]{ "<uuid>", "<name>", "<login>", "<pwd>", "<deleted>" };
    string sepEnd[usrAttr] = { "</uuid>", "</name>", "</login>", "</pwd>", "</deleted>" };
    SHA256 sha256;
public:
	UsrBase() = default;
	~UsrBase() = default;
    User splitUsrPkg(string& usrPkg);
    User getUser(int id);
    User getUser(const string& uuid);
    string packUsrPkg(User& account);
    void splitChgPwd(string& str, string arr[]);
    string tagStructParam(m_Attr id, string value);
    string unTag(m_Attr id, string& str);
};