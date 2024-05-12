#pragma once
#include "sha256.h"

using std::getline;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;

extern string curDateTime();
extern string logTimeStamp();
extern void localeSetup();

class Net
{
public:
	Net();
	~Net();
	void getMsgBase(vector<string>& msgs, const string& uuid, const string& base);
	void getUsrBase(std::vector<string>& users);
	void regUser(const string& usrPkg);
	void delUser(const string& uuid);
	void setUsrPwd(const string& usrPkg);
	void setAllMsgStatus(const string& packet);
	void setPerMsgStatus(const string& packet);
	void sendMsg(const string& msgPkg, const string& baseType);
	void disconnect();
private:
	#if defined (_WIN32) || defined (_WIN64)
	void convertIp();
	void initWinsock();
#endif
	void createSocket();
	void connectToServer();
	void sendRequest(const string& request);
	void getRequest();
	void readConfig();
	void saveConfig();
	string server_ip, chat_port, delim{" = "};
	static const int pkg_length = 1024;
	char package[pkg_length];
	string pkg_out;
	std::string_view pkg_in{ package, std::size(package) };
	int erStat;
#if defined (_WIN32) || defined (_WIN64)
	in_addr ip_to_num;
	WSADATA wsaData;
	SOCKET ClientSock;
	string netCfgPath = "settings.ini";
#elif defined (__linux__)
	int socket_fd, connection;
	struct sockaddr_in srvaddress, client;
	string netCfgPath = "./settings.ini";
#endif
};