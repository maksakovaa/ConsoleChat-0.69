#include "MsgBase.h"

void MsgBase::getMsgBase(const string& uuid)
{
	base->clear();
	vector<string> temp;
	if(this == AllMsgBase.get()) { Online->getMsgBase(temp, uuid, "ALL"); }
	else if (this == PerMsgBase.get()) { Online->getMsgBase(temp, uuid, "PER"); }
	for (string str: temp)
	{
		Message newMsg = splitMsgPkg(str);
		addInBase(newMsg);
	}
}

Message MsgBase::splitMsgPkg(string& msgPkg)
{
	int id, status;
	string from, to, date, msg;
	id = stoi(unTag(m_Attr::indId, msgPkg));
	from = unTag(m_Attr::indFrom, msgPkg);
	to = unTag(m_Attr::indTo, msgPkg);
	date = unTag(m_Attr::indDate, msgPkg);
	msg = unTag(m_Attr::indMsg, msgPkg);
	status = stoi(unTag(m_Attr::indStatus, msgPkg));

	Message newMsg(id, from, to, date, msg, status);
	return newMsg;
}

string MsgBase::packMsgPkg(Message& msgPkg)
{
	string result = tagStructParam(m_Attr::indId, std::to_string(msgPkg.get_mid()));
	result += tagStructParam(m_Attr::indFrom, msgPkg.get_mfrom());
	result += tagStructParam(m_Attr::indTo, msgPkg.get_mto());
	result += tagStructParam(m_Attr::indDate, msgPkg.get_mdate());
	result += tagStructParam(m_Attr::indMsg, msgPkg.get_mtext());
	result += tagStructParam(m_Attr::indStatus, std::to_string(msgPkg.get_mstatus()));
	return result;
}

void MsgBase::printPers(const string& uuidFrom, const string& uuidTo)
{
	if (getCount() == 0)
	{
		printEmpty();
		return;
	}
	vector<int> msgComp;
	getMsgComp(msgComp, uuidFrom, uuidTo);
	printMsgComp(msgComp);
}

void MsgBase::printMain()
{
	if (getCount() == 0)
	{
		printEmpty();
		return;
	}
	vector<int> msgComp;
	getMsgComp(msgComp, "ALL");
	printMsgComp(msgComp);
}

void MsgBase::printEmpty()
{
	cout << "Пока здесь еще никто ничего не писал" << endl;
}

void MsgBase::getMsgComp(vector<int>& msgComp, const string& uuidTo)
{
	for (int i = 0; i < base->size(); i++)
	{
		if (base->at(i).get_mto() == uuidTo)
		{
			msgComp.push_back(i);
		}
	}
}

void MsgBase::getMsgComp(vector<int>& msgComp, const string& uuidFrom, const string& uuidTo)
{
	for (int i = 0; i < base->size(); i++)
	{
		if (base->at(i).get_mfrom() == uuidFrom && base->at(i).get_mto() == uuidTo || base->at(i).get_mfrom() == uuidTo && base->at(i).get_mto() == uuidFrom)
		{
			msgComp.push_back(i);
		}
	}
}

void MsgBase::printMsgComp(vector<int>& msgComp)
{
	if (msgComp.empty())
	{
		printEmpty();
		return;
	}
	else
	{
		for (int i = 0; i < msgComp.size(); i++)
		{
			printMsgStruct(base->at(msgComp[i]));
			if (base->at(msgComp[i]).get_mstatus() == 0 && base->at(msgComp[i]).get_mto() == "ALL")
			{
				string packet = tagStructParam(m_Attr::indId, std::to_string(base->at(msgComp[i]).get_mid()));
				packet += tagStructParam(m_Attr::indFor, UserBase->getCurUUID());
				packet += tagStructParam(m_Attr::indStatus, "1");
				Online->setAllMsgStatus(packet);
				base->at(msgComp[i]).set_read();
			}
			if (base->at(msgComp[i]).get_mstatus() == 0 && base->at(msgComp[i]).get_mto() == UserBase->getCurUUID())
			{
				string packet = tagStructParam(m_Attr::indId, std::to_string(base->at(msgComp[i]).get_mid()));
				packet += tagStructParam(m_Attr::indStatus, "1");
				Online->setPerMsgStatus(packet);
				base->at(msgComp[i]).set_read();
			}
		}
	}
}

void MsgBase::printMsgStruct(Message& mesg)
{
	string border(80, '-');
	cout << "[" << mesg.get_mdate() << " | " << UserBase->getUsrName(mesg.get_mfrom()) << "]\n\n" << mesg.get_mtext() << "\n" << border << endl;
}

void MsgBase::sendMsg(const string& msgFrom, const string& msgTo, const string& msg)
{
	Message newMsg(msgFrom, msgTo, msg);
	string req = "SND_MSG<|>";
	req.append(packMsgPkg(newMsg));
	if (this == AllMsgBase.get()) {	Online->sendMsg(req, "ALL"); }
	if (this == PerMsgBase.get()) {	Online->sendMsg(req, "PER"); }
	getMsgBase(msgFrom);
}

int MsgBase::countUnreadPM()
{
	int result = 0;
	for (Message msg: *base)
	{
		if (msg.get_mto() == UserBase->getCurUUID() && msg.get_mstatus() == 0)
		{
			result++;
		}
	}
	return result;
}

int MsgBase::countUnreadPM(const string& from)
{
	int result = 0;
	for (Message msg : *base)
	{
		if (msg.get_mto() == UserBase->getCurUUID() && msg.get_mfrom() == from && msg.get_mstatus() == 0)
		{
			result++;
		}
	}
	return result;
}

int MsgBase::countUnreadAM()
{
	int result = 0;
	for (Message msg : *base)
	{
		if (msg.get_mto() == "ALL" && msg.get_mstatus() == 0)
		{
			++result;
		}
	}
	return result;
}

string MsgBase::tagStructParam(m_Attr id, const string& value)
{
	string result = sepBeg[static_cast<int>(id)];
	result += value;
	result += sepEnd[static_cast<int>(id)];
	return result;
}

string MsgBase::unTag(m_Attr id, string& str)
{
	string result;
	result = str.substr(str.find(sepBeg[static_cast<int>(id)]) + sepBeg[static_cast<int>(id)].length(), str.find(sepEnd[static_cast<int>(id)]) - sepBeg[static_cast<int>(id)].length());
	str.erase(0, str.find(sepEnd[static_cast<int>(id)]) + sepEnd[static_cast<int>(id)].length());
	return result;
}