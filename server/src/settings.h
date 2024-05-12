#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <chrono>
#include <new>
#include <functional>
#include <memory>
#include <thread>
#include <shared_mutex>

#if defined (_WIN32) || defined (_WIN64)
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mysql.h>
#pragma comment (lib, "Ws2_32.lib")
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#elif defined (__linux__)
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <mysql/mysql.h>
#endif

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;

string curDateTime();
string logTimeStamp();

class Settings
{
public:
	Settings();
	~Settings();
	const string& get(const string& name);
private:
	void set_locale();
	void saveConfig();
	string& getValue(string& str);
	std::unordered_map<string, string> conf;
	string delim_settings = " = ";
#if defined (_WIN32) || defined (_WIN64)
	string cfgPath = "settings.ini";
#elif defined (__linux__)
	string cfgPath = "./settings.ini";
#endif
};