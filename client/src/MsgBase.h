#pragma once
#include "message.h"

class MsgBase: public Base<Message>
{
public:
	MsgBase() = default;
	~MsgBase() = default;
    void getMsgBase(const string& uuid);
    Message splitMsgPkg(string& msgPkg);
    string packMsgPkg(Message& msgPkg);
    void printPers(const string& uuidFrom, const string& uuidTo);
    void printMain();
    void printEmpty();
    void getMsgComp(vector<int>& msgComp, const string& msgTo = "ALL");
    void getMsgComp(vector<int>& msgComp, const string& uuidFrom, const string& uuidTo);
    void printMsgComp(vector<int>& msgComp);
	void printMsgStruct(Message& mesg);
    void sendMsg(const string& msgFrom, const string& msgTo, const string& msg);
    int countUnreadPM();
    int countUnreadPM(const string& from);
    int countUnreadAM();
private:
    static const int msgAttr{ 7 };
    enum class m_Attr {indId, indFrom, indTo, indDate, indMsg, indStatus, indFor};
    string sepBeg[msgAttr] = { "<id>", "<from>", "<to>", "<date>", "<text>", "<status>", "<for_user>" };
    string sepEnd[msgAttr] = { "</id>", "</from>", "</to>", "</date>", "</text>", "</status>", "</for_user>" };
public:
    string tagStructParam(m_Attr id, const string& value);
    string unTag(m_Attr id, string& str);
};