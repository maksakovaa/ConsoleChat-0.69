#pragma once
#include "user.h"

class UsrBase;
class MsgBase;
extern std::unique_ptr<Net> Online;
extern std::unique_ptr<UsrBase> UserBase;
extern std::unique_ptr<MsgBase> AllMsgBase;
extern std::unique_ptr<MsgBase> PerMsgBase;

class UsrBase: public Base<User>
{
private:
    static const int usrAttr = { 5 };
    enum class m_Attr { indUUID, indName, indLogin, indPwd, indDel };
    string sepBeg[usrAttr]{ "<uuid>", "<name>", "<login>", "<pwd>", "<deleted>"};
    string sepEnd[usrAttr] = { "</uuid>", "</name>", "</login>", "</pwd>", "</deleted>"};
    SHA256 sha256;
    string m_uuid;
    bool m_auth;
public:
    UsrBase();
    ~UsrBase() = default;
    void getUsrBase();
    void printUsrBase();
    User splitUsrPkg(string& usrPkg);
    string packUsrPkg(const User& account);
    string getUsrName(int id);
    string getUsrName(const string& uuid);
    int getUsrId();
    int getUsrId(string& login);
    string getUsrUUID(int id);
    string getUsrUUID(const string& login);
    string getUsrLogin(int id);
    string getUsrLogin(const string& uuid);
    void setUsrPwd(int id, string& newPwd);
    void delUsr(int id);
    void regUsr(string& name, string& login, string& pwd);
    bool logInChk(const string& login);
    bool pwdChk(int id, const string& pwd);
    bool isDeleted(int id);
    string tagStructParam(m_Attr id, const string& value);
    string unTag(m_Attr id, string& str);
    string getCurUUID();
    void setAuthData(const string& uuid);
    bool getCurAuth();
    void logOut();
};