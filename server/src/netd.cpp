#include "netd.h"

Net::Net()
{
	getConfig();
#if defined (_WIN32) || defined (_WIN64)
	initWinsock();
#endif
	createSocket();
	bindSocket();
}

Net::~Net()
{
#if defined (_WIN32) || defined (_WIN64)
	disconnect();
#elif defined (__linux__)
	close(socket_fd);
#endif
}

void Net::getConfig()
{
	chat_port = Config->get("chat_port");
}

#if defined (_WIN32) || defined (_WIN64)

void Net::initWinsock()
{
	if ((erStat = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		logging("[NET] ERROR: Ошибка инициализации WSAStartup");
		exit(1);
	}

	else
	{
		logging("[NET] WSAStartup: " + string(wsaData.szSystemStatus));
	}
}

void Net::disconnect()
{
	shutdown(ClientConn, SD_BOTH);
	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();
}

#endif

void Net::createSocket()
{
#if defined (_WIN32) || defined (_WIN64)
	ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET)
	{
		logging("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка создания сокета.");
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] Инициализация сокета успешна.");
	}
#elif defined (__linux__)
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		logging("[NET] ERROR: Ошибка создания сокета.");
		exit(1);
	}
#endif
}

void Net::bindSocket()
{
#if defined (_WIN32) || defined (_WIN64)
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	servInfo.sin_port = htons(stoi(chat_port));

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
	if (erStat != 0)
	{
		logging("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка привязки сокета");
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] Привязка сокета выполнена.");
	}
#elif defined (__linux__)
	srvaddress.sin_addr.s_addr = htonl(INADDR_ANY);
	srvaddress.sin_port = htons(std::stoi(chat_port));
	srvaddress.sin_family = AF_INET;

	erStat = bind(socket_fd, (struct sockaddr*)&srvaddress, sizeof(srvaddress));
	if (erStat == -1)
	{
		logging("[NET] ERROR: Ошибка привязки сокета.");
		exit(1);
	}
	else
	{
		logging("[NET] Привязка сокета выполнена.");
	}
		
#endif
}

void Net::servlisten()
{
#if defined (_WIN32) || defined (_WIN64)
	erStat = listen(ServSock, SOMAXCONN);

	if (erStat != 0)
	{
		logging("[NET] ERROR: Ошибка при постановке на приём данных # " + std::to_string(WSAGetLastError()));
		closesocket(ServSock);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] Ожидание подключений...");
	}
#elif defined (__linux__)
	erStat = listen(socket_fd, 20);
	if (erStat == -1)
	{
		logging("[NET] ERROR: Ошибка при постановке на приём данных.");
		exit(1);
	}
	else
	{
		logging("[NET] Ожидание подключений...");
	}
#endif
}

void Net::acceptConnection()
{
#if defined (_WIN32) || defined (_WIN64)
	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));
	int clientInfo_size = sizeof(clientInfo);
	ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET)
	{
		logging("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Сервер несмог принять данные от клиента.");
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] Соединение успешно установлено");
	}
#elif defined (__linux__)
	length = sizeof(client);
	connection = accept(socket_fd, (struct sockaddr*)&client, &length);
	if (connection == -1)
	{
		logging("[NET] ERROR: Сервер несмог принять данные от клиента.");
		exit(1);
	}
	else
	{
		logging("[NET] Соединение успешно установлено");
	}
#endif
}

void Net::transmission()
{
	while (true)
	{
		servlisten();
		acceptConnection();
		while (true)
		{
			getRequest();
			if (isExit()) { break; }
			requestAPI();
		}
	}
}

void Net::sendRequest(const string& reqName)
{
	pkg_out.resize(pkg_length);
#if defined (_WIN32) || defined (_WIN64)
	int packet_size = send(ClientConn, pkg_out.data(), pkg_out.size(), 0);

	if (packet_size == SOCKET_ERROR)
	{
		logging("[NET] ERROR: " + std::to_string(WSAGetLastError()) + " Ошибка отправки сообщения клиенту.");
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		exit(1);
	}
	else
	{
		logging("[NET] " + reqName + " send");
	}
#elif defined (__linux__)
	ssize_t bytes = write(connection, pkg_out.data(), pkg_out.size());
	if (bytes >= 0)
	{
		logging("[NET] " + reqName + " send");
	}
#endif
}

void Net::getRequest()
{
#if defined (_WIN32) || defined (_WIN64)
	ZeroMemory(package, pkg_length);
	int packet_size = recv(ClientConn, package, sizeof(package), 0);
#elif defined (__linux__)
	bzero(package, pkg_length);
	read(connection, package, sizeof(package));
#endif
}

bool Net::isExit()
{
	if (pkg_in.starts_with("disconnect"))
	{
		logging("[NET] Соединение закрыто по запросу клиента.");
		return true;
	}
	else
		return false;
}

void Net::requestAPI()
{
	if (pkg_in.starts_with("GET_USRBASE"))
	{
		sendUsrBase();
	}
	else if (pkg_in.starts_with("GET_PER_MSGBASE"))
	{
		sendMsgBase("PER");
	}
	else if (pkg_in.starts_with("GET_ALL_MSGBASE"))
	{
		sendMsgBase("ALL");
	}
	else if (pkg_in.starts_with("REG_USER"))
	{
		regUser();
	}
	else if (pkg_in.starts_with("SND_MSG"))
	{
		regMSG();
	}
	else if (pkg_in.starts_with("DEL_USER"))
	{
		delUsr();
	}
	else if (pkg_in.starts_with("CHG_PWD"))
	{
		chgPwd();
	}
	else if (pkg_in.starts_with("SET_PMSG_STATUS"))
	{
		setPMStatus();
	}
	else if (pkg_in.starts_with("SET_AMSG_STATUS"))
	{
		setAMStatus();
	}
}

void Net::cutRequestHeader(string& request)
{
	request.erase(0, request.find(delim) + delim.length());
}

void Net::sendUsrBase()
{
	logging("[NET] GET_USRBASE request accepted");
	if (UserBase->getCount() == 0)
	{
		SQL_DataBase->getUserBase();
	}
	for (int i = 0; i < UserBase->getCount(); i++)
	{
		User account = UserBase->getUser(i);
		pkg_out = UserBase->packUsrPkg(account);
		string reqName = "USRBASE package ";
		sendRequest(reqName.append(std::to_string(i)));
		if (i == UserBase->getCount() - 1)
		{
			pkg_out = "USRBASE_END";
			sendRequest(pkg_out);
		}
	}
}

void Net::sendMsgBase(const string& baseType)
{
	logging("[NET] GET_" + baseType + "_MSGBASE request accepted");
	string uuid = package;
	cutRequestHeader(uuid);
	MsgBase* p;
	if (baseType == "ALL") { p = AllMsgBase.get(); SQL_DataBase->getAllMsgBase(uuid); }
	if (baseType == "PER") { p = PerMsgBase.get(); SQL_DataBase->getPerMsgBase(uuid); }

	if (p->getCount() == 0)
	{
		pkg_out = "MSGBASE_EMPTY";
		sendRequest(pkg_out);
		return;
	}

	for (int i = 0; i < p->getCount(); i++)
	{
		Message msg = p->getMsg(i);
		pkg_out = p->packMsgPkg(msg);
		sendRequest("MSGBASE package " + std::to_string(i));
		if (i == p->getCount() - 1)
		{
			pkg_out.clear();
			pkg_out = "MSGBASE_END";
			sendRequest(pkg_out);
		}
	}
}

void Net::regUser()
{
	logging("[NET] REG_USER request accepted");

	string temp = package;
	cutRequestHeader(temp);
	User newUser = UserBase->splitUsrPkg(temp);
	SQL_DataBase->regUser(newUser);
	SQL_DataBase->getUserBase();
	logging("[NET] User \"" + newUser.getName() + "\" registered");

}

void Net::regMSG()
{
	logging("[NET] SND_MSG request accepted");

	string temp = package;
	cutRequestHeader(temp);

	Message newMsg = AllMsgBase->splitMsgPkg(temp);
	if (newMsg.get_mto() == "ALL")
	{
		SQL_DataBase->regAllMsg(newMsg);
		SQL_DataBase->getAllMsgBase(newMsg.get_mfrom());
	}
	else
	{
		SQL_DataBase->regPrivateMsg(newMsg);
		SQL_DataBase->getPerMsgBase(newMsg.get_mfrom());
	}
	logging("[NET] Message \"" + newMsg.get_mtext() + "\" send");
}

void Net::delUsr()
{
	logging("[NET] DEL_USER request accepted");

	string uuid = package;
	cutRequestHeader(uuid);
	SQL_DataBase->delUser(uuid);
	SQL_DataBase->getUserBase();
	logging("[NET] User with UUID \"" + uuid + "\" deleted.");
}

void Net::chgPwd()
{
	logging("[NET] CHG_PWD request accepted");

	string temp = package;
	cutRequestHeader(temp);
	string arr[2];
	UserBase->splitChgPwd(temp, arr);
	SQL_DataBase->chgPwd(arr[0], arr[1]);
	SQL_DataBase->getUserBase();
	logging("[NET] Password for user \"" + UserBase->getUser(arr[0]).getName() + "\" changed");

}

void Net::setPMStatus()
{
	logging("[NET] SET_PMSG_STATUS request accepted");

	string temp = package;
	cutRequestHeader(temp);
	int arr[2];
	PerMsgBase->splitPMStatus(temp, arr);
	SQL_DataBase->updPMStatus(arr[0], arr[1]);
}

void Net::setAMStatus()
{
	logging("[NET] SET_PMSG_STATUS request accepted");

	string temp = package;
	cutRequestHeader(temp);
	string arr[3];
	AllMsgBase->splitAMStatus(temp, arr);
	SQL_DataBase->updAMStatus(arr[0], arr[1], arr[2]);
}

void Net::logging(const string& entry)
{
	std::thread write(&Logger::recLogEntry, Log, std::cref(entry));
	std::thread read(&Logger::readLogEntry, Log);
	write.join();
	read.join();
}
