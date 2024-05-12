#include "message.h"

Message::Message(const string& msgFrom, const string& msgTo, const string& msgText)
{
	m_id = 0;
	m_from = msgFrom;
	m_to = msgTo;
    m_date = curDateTime();
	m_text = msgText;
    m_status = 0;
}

Message::Message(int id, const string& msgFrom, const string& msgTo, const string& msgDate, const string& msgText, int msgStatus)
{
	m_id = id;
	m_from = msgFrom;
	m_to = msgTo;
    m_date = msgDate;
	m_text = msgText;
    m_status = msgStatus;
}

Message::Message(const Message& other)
{
	m_id = other.m_id;
	m_from = other.m_from;
	m_to = other.m_to;
	m_date = other.m_date;
	m_text = other.m_text;
	m_status = other.m_status;
}
Message& Message::operator= (const Message& other)
{
    if(&other == this)
        return *this;
	m_id = other.m_id;
	m_from = other.m_from;
	m_to = other.m_to;
	m_date = other.m_date;
	m_text = other.m_text;
	m_status = other.m_status;
	return *this;
}

Message::Message(Message&& other)
{
	std::swap(m_id, other.m_id);
	std::swap(m_from, other.m_from);
	std::swap(m_to, other.m_to);
	std::swap(m_date, other.m_date);
	std::swap(m_text, other.m_text);
	std::swap(m_status, other.m_status);
}

Message& Message::operator= (Message&& other)
{
	if(&other == this)
        return *this;
	std::swap(m_id, other.m_id);
	std::swap(m_from, other.m_from);
	std::swap(m_to, other.m_to);
	std::swap(m_date, other.m_date);
	std::swap(m_text, other.m_text);
	std::swap(m_status, other.m_status);
	return *this;
}

int Message::get_mid() const {
	return m_id;
}
const string& Message::get_mfrom() const {
	return m_from;
}
const string& Message::get_mto() const {
	return m_to;
}
const string& Message::get_mdate() const {
	return m_date;
}
const string& Message::get_mtext() const {
	return m_text;
}
int Message::get_mstatus() const {
	return m_status;
}

void Message::set_read()
{
	m_status = 1;
}