#include "settings.h"

string curDateTime()
{
	auto utcTime = std::chrono::system_clock::now();
	auto utcTime2 = std::chrono::floor<std::chrono::seconds>(utcTime);
	auto curTime = std::chrono::zoned_time{ std::chrono::current_zone(), utcTime2 };
	return std::format("{0:%Y.%m.%d} {0:%T}", curTime);
}

string logTimeStamp()
{
	return "[" + curDateTime() + "] ";
}

Settings::Settings()
{
	set_locale();
	std::ifstream config(cfgPath);
	if (config.is_open())
	{
		string str;
		while (std::getline(config, str))
		{
			if (str.starts_with("mysql_server_ip"))
			{
				conf["mysql_srv_ip"] = getValue(str);
			}
			else if (str.starts_with("mysql_login"))
			{
				conf["mysql_login"] = getValue(str);
			}
			else if (str.starts_with("mysql_pass"))
			{
				conf["mysql_pass"] = getValue(str);
			}
			else if (str.starts_with("mysql_database"))
			{
				conf["mysql_database"] = getValue(str);
			}
			else if (str.starts_with("mysql_table_users"))
			{
				conf["mysql_table_users"] = getValue(str);
			}
			else if (str.starts_with("mysql_table_messages"))
			{
				conf["mysql_table_AM"] = getValue(str);
			}
			else if (str.starts_with("mysql_table_all_msg_status"))
			{
				conf["mysql_table_AMS"] = getValue(str);
			}
			else if (str.starts_with("mysql_table_private_msg"))
			{
				conf["mysql_table_PM"] = getValue(str);
			}
			else if (str.starts_with("server_ip"))
			{
				conf["server_ip"] = getValue(str);
			}
			else if (str.starts_with("chat_port"))
			{
				conf["chat_port"] = getValue(str);
			}
		}
		config.close();
	}
	else
	{
		conf["mysql_srv_ip"] = "127.0.0.1";
		conf["mysql_login"] = "root";
		conf["mysql_pass"] = "YrZBQJIijJ9W0mlpqrj4";
		conf["mysql_database"] = "console_chat";
		conf["mysql_table_users"] = "users";
		conf["mysql_table_AM"] = "messages";
		conf["mysql_table_AMS"] = "all_msg_status";
		conf["mysql_table_PM"] = "p_messages";
		conf["server_ip"] = "127.0.0.1";
		conf["chat_port"] = "9999";
		saveConfig();
	}
}

Settings::~Settings()
{
	saveConfig();
}

const string& Settings::get(const string& name)
{
	return conf[name];
}

void Settings::saveConfig()
{
	std::ofstream config(cfgPath, std::ios::trunc);
	if (!config.is_open())
	{
		cout << logTimeStamp() << "ERROR: Ошибка открытия файла!" << endl;
	}
	else
	{
		config << ("mysql_server_ip = " + get("mysql_srv_ip") + "\n");
		config << ("mysql_login = " + get("mysql_login") + "\n");
		config << ("mysql_pass = " + get("mysql_pass") + "\n");
		config << ("mysql_database = " + get("mysql_database") + "\n");
		config << ("mysql_table_users = " + get("mysql_table_users") + "\n");
		config << ("mysql_table_messages = " + get("mysql_table_AM") + "\n");
		config << ("mysql_table_all_msg_status = " + get("mysql_table_AMS") + "\n");
		config << ("mysql_table_private_msg = " + get("mysql_table_PM") + "\n");
		config << ("server_ip = " + get("server_ip") + "\n");
		config << ("chat_port = " + get("chat_port") + "\n");
		config.close();
	}
}

string& Settings::getValue(string& str)
{
	return str.erase(0, str.find(delim_settings) + delim_settings.length());
}

void Settings::set_locale()
{
#if defined (_WIN32) || defined (_WIN64)
	SetConsoleOutputCP(65001);
	SetConsoleCP(1251);
#elif defined (__linux__)
	setlocale(LC_ALL, "ru_RU.utf8");
#endif
}