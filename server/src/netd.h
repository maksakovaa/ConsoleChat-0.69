#pragma once
#include "sql_db.h"

extern std::shared_ptr<Logger> Log;
extern std::unique_ptr<Settings> Config;
extern std::unique_ptr<UsrBase> UserBase;
extern std::unique_ptr<MsgBase> AllMsgBase;
extern std::unique_ptr<MsgBase> PerMsgBase;
extern std::unique_ptr<SQL_DB> SQL_DataBase;

class Net
{
public:
	Net();
	~Net();
	void getConfig();
#if defined (_WIN32) || defined (_WIN64)
	void convertIp();
	void initWinsock();
	void disconnect();
#endif
	void createSocket();
	void bindSocket();
	void servlisten();
	void acceptConnection();
	void transmission();
	void sendRequest(const string& reqName);
	void getRequest();
	bool isExit();
	void requestAPI();
	void cutRequestHeader(string& request);
	void sendUsrBase();
	void sendMsgBase(const string& baseType);
	void regUser();
	void regMSG();
	void delUsr();
	void chgPwd();
	void setPMStatus();
	void setAMStatus();
private:
	void logging(const string& entry);
	string server_ip, chat_port, delim{"<|>"};
	static const int pkg_length = 1024;
	char package[pkg_length];
	string pkg_out;
	std::string_view pkg_in{package, std::size(package)};
	int erStat;
#if defined (_WIN32) || defined (_WIN64)
	in_addr ip_to_num;
	WSADATA wsaData;
	SOCKET ServSock;
	SOCKET ClientConn;
#elif defined (__linux__)
	struct sockaddr_in srvaddress, client;
	socklen_t length;
	int socket_fd, connection;
#endif
};