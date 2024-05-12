#include "functions.h"

void consoleClear()
{
#if defined (_WIN32) || defined (_WIN64)
	system("cls");
#elif defined (__linux__)
	system("clear");
#endif
}

void localeSetup()
{
#if defined(_WIN32) || defined (_WIN64)
	SetConsoleOutputCP(65001);
	SetConsoleCP(1251);
#elif defined(__linux__)
	setlocale(LC_ALL, "ru_RU.utf8");
#endif
}

void cinClearIgnore()
{
	cin.clear();
	cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

void incorrect()
{
	cinClearIgnore();
	cout << "Некорректный ввод, повторите:" << endl;
}

bool nonLatinChk(string& text)
{
	string letters = "абвгдеёжщийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
	if (text.find_first_of(letters) != string::npos)
	{
		cout << "ERROR: Некорректный ввод. Используйте латинницу." << endl;
		return false;
	}
	return true;
}

bool noSpacesChk(string& text)
{
	if (text.find_first_of(" ") != string::npos)
	{
		cout << "ERROR: Некорректный ввод. Не используйте пробел." << endl;
		return false;
	}
	return true;
}

bool lengthChk(string& text)
{
	if (text.size() < 4)
	{
		cout << "ERROR: Длина должна быть не менее 4 символов." << endl;
		return false;
	}
	return true;
}

bool regLoginChk(string& _login)
{
	int errCount = 0;
	if (!nonLatinChk(_login))
		errCount++;
	if (!noSpacesChk(_login))
		errCount++;
	if (!lengthChk(_login))
		errCount++;

	if (UserBase->logInChk(_login))
	{
		cout << "ERROR: Имя пользователя уже занято." << endl;
		errCount++;
	}

	if (errCount == 0)
	{
		return true;
	}
	else
	{
		cout << "Повторите ввод:" << endl;
		return false;
	}
}

bool regPwdChk(string& _pwd)
{
	int errCount = 0;
	if (!nonLatinChk(_pwd))
		errCount++;
	if (!noSpacesChk(_pwd))
		errCount++;
	if (!lengthChk(_pwd))
		errCount++;

	if (errCount == 0)
	{
		return true;
	}
	else
	{
		cout << "Повторите ввод:" << endl;
		return false;
	}
}

bool authPwdChk(string& _login, string& _pwd)
{
	int id = UserBase->getUsrId(_login);
	return UserBase->pwdChk(id, _pwd);
}

string getOsName()
{
#if defined(_WIN32) || defined (_WIN64)
	return "Windows";
#elif defined(__linux__)
	return "Linux";
#endif
}

int getProcId()
{
#if defined(__linux__)
	return getpid();
#elif defined(_WIN32) || defined(_WIN64)
	return GetCurrentProcessId();
#endif
}

string middle(const string& str)
{
	const int max_len = 80;

	size_t spaces_needed, str_len;
	str_len = str.length();
	spaces_needed = (max_len - str_len) / 2;

	string buffer(spaces_needed, ' ');
	buffer += str;

	return buffer;
}

void printHeader(const string& header)
{
	string border(80, '-');
	cout << middle(header) << endl;
	cout << border << endl;
}


void cp1251toUtf8(string& str)
{
#if defined(_WIN32) || defined(_WIN64)
	int result_u, result_c;
	result_u = MultiByteToWideChar(1251, 0, str.data(), -1, 0, 0);
	
	if (!result_u)
		return;

	wchar_t* ures = new wchar_t[result_u];	
	if (!MultiByteToWideChar(1251, 0, str.data(), -1, ures, result_u))
	{
		delete[] ures;
		return;
	}

	result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c)
	{
		delete[] ures;
		return;
	}

	char* cres = new char[result_c];
	if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0))
	{
		delete[] cres;
		return;
	}
	str = cres;
	delete[] ures, cres;
#endif
}


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

void menu_pause()
{
#if defined(_WIN32) || defined(_WIN64)
    system("pause");
#elif defined(__linux__)
    system("read -p \"Press any key to continue ...\" n");
#endif
}