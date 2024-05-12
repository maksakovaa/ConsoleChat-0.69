#pragma once
#include "UsrBase.h"

class Message
{
private:
    int m_id;
    string m_from;
    string m_to;
    string m_date;
    string m_text;
    int m_status;
public:
    Message(const string& msgFrom, const string& msgTo, const string& msgText);
    Message(int id, const string& msgFrom, const string& msgTo, const string& msgDate, const string& msgText, int msgStatus);
    Message(const Message& other);
    Message& operator= (const Message& other);
    Message(Message&& other);
    Message& operator= (Message&& other);
    int get_mid() const;
    const string& get_mfrom() const;
    const string& get_mto() const;
    const string& get_mdate() const;
    const string& get_mtext() const;
    int get_mstatus() const;
    void set_read();
};