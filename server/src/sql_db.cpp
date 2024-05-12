#include "sql_db.h"

SQL_DB::SQL_DB()
{
	getConfig();
	mysql_init(&mysql);
	if (&mysql == NULL)
	{
		logging("[MySQL] ERROR: Ошибка создания MySQL дескриптора");
		exit(1);
	}

	if (!mysql_real_connect(&mysql, mysql_srv_ip.data(), mysql_login.data(), mysql_pass.data(), NULL, 0, NULL, 0))
	{
		logging("[MySQL] ERROR: Ошибка подключения к базе данных " + string(mysql_error(&mysql)));
		exit(1);
	}
	else
	{
		logging("[MySQL] Соединение установлено.");
	}
	mysql_set_character_set(&mysql, "utf8");
	firstRun();
}

SQL_DB::~SQL_DB()
{
	mysql_close(&mysql);
	logging("[MySQL] Соединение закрыто.");
}

void SQL_DB::getConfig()
{
	mysql_srv_ip = Config->get("mysql_srv_ip");
	mysql_login = Config->get("mysql_login");
	mysql_pass = Config->get("mysql_pass");
	mysql_database = Config->get("mysql_database");
	mysql_table_users = Config->get("mysql_table_users");
	mysql_table_AM = Config->get("mysql_table_AM");
	mysql_table_AMS = Config->get("mysql_table_AMS");
	mysql_table_PM = Config->get("mysql_table_PM");
}

void SQL_DB::firstRun()
{
	m_query = "CREATE DATABASE IF NOT EXISTS " + mysql_database;
	sendRequest("CREATE DATABASE");

	m_query = "USE " + mysql_database;
	sendRequest("USE DATABASE");

	m_query = "CREATE TABLE IF NOT EXISTS " + mysql_table_users + " "
	"(id SERIAL NOT NULL, uuid BINARY(16) PRIMARY KEY DEFAULT (UUID_TO_BIN(UUID())), "
	"name VARCHAR(100) NOT NULL, "
	"login VARCHAR(100) UNIQUE NOT NULL, "
	"pwd VARCHAR(200), "
	"deleted INTEGER NOT NULL DEFAULT 0)";
	sendRequest("CREATE TABLE USERS");

	m_query = "CREATE TABLE IF NOT EXISTS " + mysql_table_AM +
	" (id SERIAL PRIMARY KEY, "
	"msg_from BINARY(16) NOT NULL REFERENCES " + mysql_table_users + "(uuid), "
	"date DATETIME NOT NULL, "
	"message VARCHAR(255) NOT NULL)";
	sendRequest("CREATE TABLE ALL_MSGBASE");

	m_query = "CREATE TABLE IF NOT EXISTS " + mysql_table_PM +
	" (id SERIAL PRIMARY KEY, "
	"msg_from BINARY(16) NOT NULL REFERENCES " + mysql_table_users + "(uuid), "
	"msg_to BINARY(16) NOT NULL REFERENCES " + mysql_table_users + "(uuid), "
	"date DATETIME NOT NULL, "
	"message VARCHAR(255) NOT NULL, "
	"status INTEGER DEFAULT 0)";
	sendRequest("CREATE TABLE PRIVATE_MSGBASE");

	m_query = "CREATE TABLE IF NOT EXISTS " + mysql_table_AMS + " " +
	"(id SERIAL PRIMARY KEY, " +
	"msg_id integer NOT NULL REFERENCES " + mysql_table_AM + "(id), " +
	"for_user BINARY(16) NOT NULL REFERENCES " + mysql_table_users + "(uuid), " +
	"status INTEGER DEFAULT 0)";
	sendRequest("CREATE TABLE ALL MSG STATUS");

	m_query = "CREATE PROCEDURE IF NOT EXISTS status_on_create_msg(in a INT) " 
	"BEGIN "
	"DECLARE i INT DEFAULT 1; "
	"DECLARE user_count INT DEFAULT 0; "
	"DECLARE user_uuid BINARY(16); "
	"SET user_count = (SELECT count(u.id) FROM " + mysql_table_users + " u); "
	"WHILE i <= user_count DO "
		"SET user_uuid = (SELECT u.uuid FROM " + mysql_table_users + " u WHERE u.id = i); "
		"INSERT INTO " + mysql_table_AMS + " (msg_id, for_user) VALUES (a, user_uuid); "
		"SET i = i+1; "
	"END WHILE; "
	"END;";
	sendRequest("CREATE PROCEDURE status_on_create_msg");

	m_query = "CREATE TRIGGER IF NOT EXISTS on_create_msg AFTER INSERT ON " + mysql_table_AM + " "
	"FOR EACH ROW "
	"BEGIN "
	"CALL status_on_create_msg( NEW.id ); "
	"END;";
	sendRequest("CREATE TRIGGER on_create_msg");

	m_query = "CREATE PROCEDURE IF NOT EXISTS status_on_create_usr(in new_uuid BINARY(16)) "
	"BEGIN "
	"DECLARE i INT DEFAULT 1; "
	"DECLARE msg_count INT DEFAULT 0; "
	"SET msg_count = (SELECT count(id) FROM " + mysql_table_AM + "); "
	"IF msg_count >= 1 THEN "
	"WHILE i <= msg_count DO "
		"INSERT INTO " + mysql_table_AMS + " (msg_id, for_user) VALUES (i, new_uuid); "
		"SET i = i+1; "
	"END WHILE; "
	"END IF; "
	"END;";
	sendRequest("CREATE PROCEDURE status_on_create_usr");

	m_query = "CREATE TRIGGER IF NOT EXISTS on_create_usr AFTER INSERT ON " + mysql_table_users + " "
	"FOR EACH ROW "
	"BEGIN "
		"CALL status_on_create_usr( NEW.uuid ); "
	"END;";
	sendRequest("CREATE TRIGGER on_create_usr");

	getUserBase();
}

void SQL_DB::getRequest(const string& request)
{
	mysql_query(&mysql, m_query.data());
	if (result = mysql_store_result(&mysql))
	{
		logging("[MySQL] Запрос " + request + " выполнен успешно.");
	}
	else
	{
		logging("[MySQL] Ошибка MySQL " + string(mysql_error(&mysql)));
	}
	m_query.clear();
}

void SQL_DB::sendRequest(const string& request)
{
	int query = mysql_query(&mysql, m_query.data());
	if (query == 0)
	{
		logging("[MySQL] Запрос " + request + " выполнен успешно.");
	}
	else
	{
		logging("[MySQL] Запрос " + request + " выполнен c ошибкой: " + std::to_string(query));
	}
	m_query.clear();
}

void SQL_DB::getUserBase()
{
	UserBase->resetBase();
	m_query = "SELECT BIN_TO_UUID(uuid) uuid, name, login, pwd, deleted  FROM " + mysql_table_users + " ORDER BY id";
	getRequest("SELECT FOR USERBASE");
	while (row = mysql_fetch_row(result))
	{
		User newUser(row[0], row[1], row[2], row[3], atoi(row[4]));
		UserBase->addInBase(newUser);
		logging("[MySQL] Пользователь \"" + string(row[1]) + "\" добавлен в базу");
	}
	if (UserBase->getCount() == 0)
	{
		User newUser("Администратор", "admin", "da23890e111536e631be13a069ebc5432c9cf28cdbc5deb2a70770ec9597db6d");
		regUser(newUser);
	}
}

void SQL_DB::getPerMsgBase(const string& uuid)
{
	PerMsgBase->resetBase();

	m_query = "SELECT pm.id, BIN_TO_UUID(pm.msg_from), "
	"BIN_TO_UUID(pm.msg_to), pm.date, pm.message, pm.status "
	"FROM " + mysql_table_PM + " pm "
	"WHERE pm.msg_from = UUID_TO_BIN('" + uuid + "') OR pm.msg_to = UUID_TO_BIN('" + uuid + "') "
	"ORDER BY pm.id";
	getRequest("SELECT FOR PRIVATE_MSGBASE");
	while (row = mysql_fetch_row(result))
	{
		Message newMsg(atoi(row[0]), row[1], row[2], row[3], row[4], atoi(row[5]));
		PerMsgBase->addInBase(newMsg);
	}
}

void SQL_DB::getAllMsgBase(const string& uuid)
{
	AllMsgBase->resetBase();
	m_query = "SELECT m.id, BIN_TO_UUID(m.msg_from), m.date, m.message, ams.status "
	"FROM " + mysql_table_AM + " m "
	"JOIN " + mysql_table_AMS + " ams ON m.id = ams.msg_id "
	"WHERE ams.for_user = UUID_TO_BIN('" + uuid + "') "
	"ORDER BY m.id";
	getRequest("SELECT FOR ALL_MSGBASE");
	while (row = mysql_fetch_row(result))
	{
		Message newMsg(atoi(row[0]), row[1], "ALL", row[2], row[3], atoi(row[4]));
		AllMsgBase->addInBase(newMsg);
	}
}

void SQL_DB::delUser(const string& uuid)
{
	m_query = "UPDATE " + mysql_table_users + " "
	"SET deleted = 1 "
	"WHERE BIN_TO_UUID(uuid) = '" + uuid + "'";
	sendRequest("DELETE FOR USER");
}

void SQL_DB::regUser(User& newUser)
{
	m_query = "INSERT INTO " + mysql_table_users + " "
	"(name, login, pwd) VALUES('" + 
	newUser.getName() + "', '" + newUser.getLogin() + "', '" + newUser.getPwd() + "')";
	sendRequest("INSERT USER IN USERBASE");
}

void SQL_DB::chgPwd(const string& uuid, const string& pwd)
{
	m_query = "UPDATE " + mysql_table_users + " "
	"SET pwd = '" + pwd + "'"
	"WHERE uuid = UUID_TO_BIN('" + uuid + "')";
	sendRequest("UPDATE PWD FOR USER");
}

void SQL_DB::regPrivateMsg(Message& newMsg)
{
	m_query = "INSERT INTO " + mysql_table_PM + " "
	"(msg_from, msg_to, date, message) "
	"VALUES(UUID_TO_BIN('" + newMsg.get_mfrom() + "'),"
	" UUID_TO_BIN('" + newMsg.get_mto() + "'), "
	"'" + newMsg.get_mdate() + "', "
	"'" + newMsg.get_mtext() + "')";
	sendRequest("INSERT MSG IN PRIVATE_MSGBASE");
}

void SQL_DB::regAllMsg(Message& newMsg)
{
	m_query = "INSERT INTO " + mysql_table_AM + " "
	"(msg_from, date, message) "
	"VALUES(UUID_TO_BIN('" + newMsg.get_mfrom() + "'), "
	"'" + newMsg.get_mdate() + "', "
	"'" + newMsg.get_mtext() + "')";
	sendRequest("INSERT MSG IN ALL_MSGBASE");
}

void SQL_DB::updPMStatus(int msgId, int status)
{
	m_query = "UPDATE " + mysql_table_PM + " SET status = " + std::to_string(status) + " WHERE id = " + std::to_string(msgId);
	sendRequest("UPDATE PRIVATE MSG status");
}

void SQL_DB::updAMStatus(const string& msgId, const string& forUser, const string& status)
{
	m_query = "UPDATE " + mysql_table_AMS + " SET status = " + status + " WHERE msg_id = " + msgId + " AND for_user = UUID_TO_BIN('" + forUser + "')";
	sendRequest("UPDATE ALL MSG status");
}

void SQL_DB::logging(const string& entry)
{
	std::thread write(&Logger::recLogEntry, Log, std::cref(entry));
	std::thread read(&Logger::readLogEntry, Log);
	write.join();
	read.join();
}
