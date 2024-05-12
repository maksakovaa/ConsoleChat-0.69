#include "net.h"

Net::Net()
{
	localeSetup();
	readConfig();
#if defined (_WIN32) || defined (_WIN64)
	convertIp();
	initWinsock();
#endif
	createSocket();
	connectToServer();
}

Net::~Net()
{
#if defined (_WIN32) || defined (_WIN64)
	closesocket(ClientSock);
	WSACleanup();
#elif defined (__linux__)
	close(socket_fd);
#endif
}

void Net::readConfig()
{
	std::ifstream config(netCfgPath);
	if (config.is_open())
	{
		string str;
		while (std::getline(config, str))
		{
			if (str.starts_with("server_ip"))
				server_ip = str.erase(0, str.find(delim) + delim.length());
			if (str.starts_with("chat_port"))
				chat_port = str.erase(0, str.find(delim) + delim.length());
		}
		config.close();
	}
	else
	{
		server_ip = "127.0.0.1";
		chat_port = "9999";
		saveConfig();
	}
}

void Net::saveConfig()
{
	std::ofstream config(netCfgPath, std::ios::trunc);
	if (!config.is_open())
	{
		cout << logTimeStamp() << "ERROR: Ошибка открытия файла!" << endl;
	}
	else
	{
		config << "server_ip = ";
		config << server_ip;
		config << "\n";
		config << "chat_port = ";
		config << chat_port;
		config << "\n";
		config.close();
	}
}

#if defined (_WIN32) || defined (_WIN64)

void Net::convertIp()
{
	inet_pton(AF_INET, server_ip.data(), &ip_to_num);
}

void Net::initWinsock()
{
	if ((erStat = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		cout << logTimeStamp() << "ERROR: " << erStat << "WSAStartup failed" << endl;
		exit(1);
	}
	else
	{
		cout << logTimeStamp() << "WSAStartup: " << wsaData.szSystemStatus << endl;
	}
}
#endif

void Net::createSocket()
{
#if defined (_WIN32) || defined (_WIN64)
	ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		cout << logTimeStamp() << "ERROR: " << WSAGetLastError() << " Ошибка создания сокета." << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(1);
	}
#elif defined (__linux__)
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_fd == -1)
	{
		cout << logTimeStamp() << "ERROR: Ошибка создания сокета." << endl;
		exit(1);
	}
#endif
}

void Net::connectToServer()
{
#if defined (_WIN32) || defined (_WIN64)
	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(stoi(chat_port));

	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << logTimeStamp() << "ERROR: " << WSAGetLastError() << " Ошибка подключения к серверу." << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(1);
	}

#elif defined (__linux__)
	srvaddress.sin_addr.s_addr = inet_addr(server_ip.data());
	srvaddress.sin_port = htons(stoi(chat_port));
	srvaddress.sin_family = AF_INET;

	connection = connect(socket_fd, (struct sockaddr*)&srvaddress, sizeof(srvaddress));

	if (connection == -1)
	{
		cout << logTimeStamp() << "ERROR: Ошибка подключения к серверу." << endl;
		exit(1);
	}
#endif
}

void Net::sendRequest(const string& request)
{
	pkg_out.resize(pkg_length);
#if defined (_WIN32) || defined (_WIN64)
	int packet_size = send(ClientSock, pkg_out.data(), pkg_out.size(), 0);
	if (packet_size == SOCKET_ERROR)
	{
		cout << logTimeStamp() << "ERROR: " << WSAGetLastError() << " Ошибка отправки запроса на сервер." << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(1);
	}
#elif defined (__linux__)
	ssize_t bytes = write(socket_fd, pkg_out.data(), pkg_out.size());
#endif
}

void Net::getRequest()
{
#if defined (_WIN32) || defined (_WIN64)
	ZeroMemory(&package, sizeof(package));
	int packet_size = recv(ClientSock, package, sizeof(package), 0);
	if (packet_size == SOCKET_ERROR)
	{
		cout << logTimeStamp() << "ERROR: " << WSAGetLastError() << " Ошибка получения ответа от сервера." << endl;
		closesocket(ClientSock);
		WSACleanup();
		exit(1);
	}
#elif defined (__linux__)
	bzero(package, pkg_length);
	ssize_t bytes = recv(socket_fd, package, sizeof(package), 0);
	if (bytes == -1)
	{
		cout << logTimeStamp() << "ERROR: Ошибка получения ответа от сервера." << endl;
		exit(1);
	}
#endif
}

void Net::getUsrBase(std::vector<string>& users)
{
	users.clear();
	pkg_out = "GET_USRBASE";  
	sendRequest(pkg_out);
	while (true)
	{
		getRequest();
		if (!pkg_in.starts_with("USRBASE_END"))
		{
			string temp = package;
			users.push_back(std::move(temp));
		}
		else
		{
			break;
		}
	}
}

void Net::getMsgBase(vector<string>& msgs, const string& uuid, const string& base)
{
	msgs.clear();
	pkg_out = "GET_" + base + "_MSGBASE<|>" + uuid;
	sendRequest(pkg_out);
	while (true)
	{
		getRequest();
		if (!pkg_in.starts_with("MSGBASE_EMPTY") && !pkg_in.starts_with("MSGBASE_END"))
		{
			string temp = package;
			msgs.push_back(std::move(temp));
		}
		
		if (pkg_in.starts_with("MSGBASE_EMPTY") || pkg_in.starts_with("MSGBASE_END"))
		{
			break;
		}
	}
}

void Net::regUser(const string& usrPkg)
{
	pkg_out = "REG_USER<|>" + usrPkg;
	sendRequest(pkg_out);
}

void Net::delUser(const string& uuid)
{
	pkg_out = "DEL_USER<|>" + uuid;
	sendRequest(pkg_out);
}

void Net::setUsrPwd(const string& usrPkg)
{
	pkg_out = "CHG_PWD<|>" + usrPkg;
	sendRequest(pkg_out);
}

void Net::setAllMsgStatus(const string& packet)
{
	pkg_out = "SET_AMSG_STATUS<|>" + packet;
	sendRequest(pkg_out);
}

void Net::setPerMsgStatus(const string& packet)
{
	pkg_out = "SET_PMSG_STATUS<|>" + packet;
	sendRequest(pkg_out);
}

void Net::sendMsg(const string& msgPkg, const string& baseType)
{
	pkg_out = std::move(msgPkg);
	sendRequest(pkg_out);
}

void Net::disconnect()
{
	pkg_out = "disconnect";
	sendRequest(pkg_out);
}