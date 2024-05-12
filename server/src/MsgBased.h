#pragma once
#include "message.h"

class MsgBase: public Base<Message>
{
private:
    static const int msgAttr{ 7 };
    enum class m_Attr { indId, indFrom, indTo, indDate, indMsg, indStatus, indFor };
    string sepBeg[msgAttr] = { "<id>", "<from>", "<to>", "<date>", "<text>", "<status>", "<for_user>" };
    string sepEnd[msgAttr] = { "</id>", "</from>", "</to>", "</date>", "</text>", "</status>", "</for_user>" };
public:
	MsgBase() = default;
	~MsgBase() = default;
    Message splitMsgPkg(string& msgPkg);
    string packMsgPkg(Message& msgPkg);
    Message getMsg(int id);
    void splitPMStatus(string& str, int arr[]);
    void splitAMStatus(string& str, string arr[]);
    string tagStructParam(m_Attr id, const string& value);
    string unTag(m_Attr id, string& value);
};