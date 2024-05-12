#include "navigation.h"

void Chat::menuMain()
{
	if (authStatChk())
	{
		menuAuthorized();
	}
	
	consoleClear();
	printHeader("[Главное меню]");
	cout << "Добро пожаловать в консольный чат!\nДля просмотра и отправки сообщений необходимо войти или зарегистрироваться. " << endl;
	cout << "Выберите действие:\n [1] - Регистрация пользователя \n [2] - Список пользователей\n [3] - Авторизация \n [4] - О программе \n [Q] - Выход из программы" << endl;
	cin >> m_choice;
	checkInput('1', '4', 'Q', 'q');
	if (m_choice == '1') { menuRegUser(); }
	else if (m_choice == '2') { menuUserList(); }
	else if (m_choice == '3') { menuAuth(); }
	else if (m_choice == '4') { menuAbout(); }
	else if (m_choice == 'q' || m_choice == 'Q') { exit(); }
}

void Chat::menuRegUser()
{
	string name, login, pwd;
	cinClearIgnore();
	consoleClear();
	printHeader("[Регистрация]");
	cout << "Введите имя пользователя:" << endl;
	getline(cin, name);
	cp1251toUtf8(name);
	cout << "Введите логин для пользователя " << name << ":" << endl;
	getline(cin, login);
	cp1251toUtf8(login);
	while (!regLoginChk(login))
	{
		getline(cin, login);
		cp1251toUtf8(login);
	}
	cout << "Введите желаемый пароль для " << name << ":" << endl;
	getline(cin, pwd);
	cp1251toUtf8(pwd);
	while (!regPwdChk(pwd))
	{
		getline(cin, pwd);
		cp1251toUtf8(pwd);
	}
	UserBase->regUsr(name, login, pwd);
	cout << "Пользователь " << name << " успешно зарегистрирован. \n [1] - Главное меню\n [2] - Авторизация" << endl;
	cin >> m_choice;
	checkInput('1', '2');
	if (m_choice == '1') { menuMain(); }
	else if (m_choice == '2') { menuAuth(); }
}

void Chat::menuAuth()
{
	string name, login, pwd;
	cinClearIgnore();
	consoleClear();
	printHeader("[Авторизация]");
	cout << "Вход для зарегистрированных пользователей" << endl;
	cout << "Введите логин:" << endl;
	getline(cin, login);
	cp1251toUtf8(login);
	if (UserBase->logInChk(login))
	{
		cout << "Введите пароль:" << endl;
		getline(cin, pwd);
		cp1251toUtf8(pwd);
		while (!authPwdChk(login, pwd))
		{
			cout << "Неверный пароль, повторите:" << endl;
			getline(cin, pwd);
			cp1251toUtf8(pwd);
		}
		if (authPwdChk(login, pwd))
		{		
			UserBase->setAuthData(UserBase->getUsrUUID(login));
			AllMsgBase->getMsgBase(UserBase->getCurUUID());
			PerMsgBase->getMsgBase(UserBase->getCurUUID());
			menuAuthorized();
		}
	}
	else
	{
		cout << "Неверное имя пользователя" << endl;
		cout << " [1] - вернуться в главное меню\n [2] - повторить\n [Q] - выход из программы" << endl;
		cin >> m_choice;
		checkInput('1', '2', 'q', 'Q');
		if (m_choice == '1') { menuMain(); }
		if (m_choice == '2') { menuAuth(); }
		if (m_choice == 'q' || m_choice == 'Q') { exit(); }
	}
}

void Chat::menuAbout()
{
	consoleClear();
	printHeader("О программе");
	cout << "Программа консольный чат. [0.69]\n© Максаков А.А., 2024\nСборка для " << getOsName() << endl;
	cout << "PID процесса: " << getProcId() << endl;
	cout << " [1] - Вернуться в главное меню\n [2] - Выход из программы" << endl;
	cin >> m_choice;
	checkInput('1', '2');
	if (m_choice == '1') { menuMain(); }
	else if (m_choice == '2') { exit(); }
}

void Chat::menuAuthorized()
{
	consoleClear();
	printHeader("[Главное меню]");
	cout << UserBase->getUsrName(UserBase->getCurUUID()) << " добро пожаловать в консольный чат!" << endl;
	cout << "Выберите действие:\n [1] - Общий чат";
	if (AllMsgBase->countUnreadAM() > 0)
	{
		cout << "\t\t [" << AllMsgBase->countUnreadAM() << "] непроочитанных сообщений";
	}
	cout << "\n [2] - Личные сообщения";
	if (PerMsgBase->countUnreadPM() > 0)
	{
		cout << "\t [" << PerMsgBase->countUnreadPM() << "] непроочитанных сообщений";
	}
	cout << endl;
	if (UserBase->getUsrLogin(UserBase->getCurUUID()) == "admin")
	{
		cout << " [3] - Управление пользователями" << endl;
	}
	else
	{
		cout << " [3] - Изменить пароль" << endl;
	}
	cout << " [4] - Выйти из учетной записи\n [5] - О программе\n [Q] - Выход из программы" << endl;
	cin >> m_choice;
	checkInput('1', '5', 'q', 'Q');
	if (m_choice == '1') { menuAllMsg(); }
	else if (m_choice == '2') { menuPerMsg(); }
	else if (m_choice == '3')
	{
		if (UserBase->getUsrLogin(UserBase->getCurUUID()) == "admin") { menuAdmin(); }
		else { menuChgPwd(UserBase->getUsrId()); }
	}
	else if (m_choice == '4') { UserBase->logOut(); menuMain(); }
	else if (m_choice == '5') { menuAbout(); }
	else if (m_choice == 'q' || m_choice == 'Q') { exit(); }
}

void Chat::menuUserList()
{
	consoleClear();
	printHeader("[Список пользователей]");
	UserBase->printUsrBase();
	printBorder();
	cout << " [1] - вернуться в главное меню\n [2] - Войти\n [Q] - выход из программы\n";
	cin >> m_choice;
	checkInput('1', '2', 'q', 'Q');
	if (m_choice == '1') { menuMain(); }
	else if (m_choice == '2') { menuAuth(); }
	else if (m_choice == 'q' || m_choice == 'Q') { exit(); }
}

void Chat::menuAllMsg()
{
	while (true)
	{
		consoleClear();
		printHeader("[Общий чат]");
		AllMsgBase->printMain();
		cout << " [1] - Отправить сообщение\n [2] - Вернуться в главное меню\n [Q] - Выход из программы" << endl;
		cin >> m_choice;
		checkInput('1', '2', 'q', 'Q');
		cinClearIgnore();
		if (m_choice == '1')
		{
			cout << "Введите сообщение:" << endl;
			string msgText;
			getline(cin, msgText);
			cp1251toUtf8(msgText);
			while (msgText.empty())
			{
				cout << "Некорректный ввод, повторите:" << endl;
				getline(cin, msgText);
				cp1251toUtf8(msgText);
			}
			AllMsgBase->sendMsg(UserBase->getCurUUID(), "ALL",  msgText);
		}
		else if (m_choice == '2') { menuAuthorized(); break; }
		else if (m_choice == 'q' || m_choice == 'Q') { exit(); break; }
	}
}

void Chat::menuPerMsg()
{
	consoleClear();
	printHeader("[Список пользователей]");
	printUserBase();
	printBorder();
	cout << " [1] - вернуться в главное меню\n [2] - Просмотр сообщений\n [Q] - выход из программы\n";
	cin >> m_choice;
	checkInput('1', '2', 'q', 'Q');
	if (m_choice == '1') { menuAuthorized(); }
	else if (m_choice == '2')
	{ 
		int temp;
		cout << "Введите ID пользователя:" << endl;
		cin >> temp;
		while (temp < 0 || temp >= UserBase->getCount() || temp == UserBase->getUsrId())
		{
			incorrect();
			cin >> temp;
		}
		cinClearIgnore();
		while (true)
		{
			consoleClear();
			string header = "[Чат с " + UserBase->getUsrName(temp) + "]";
			printHeader(header);
			PerMsgBase->printPers(UserBase->getUsrUUID(temp), UserBase->getCurUUID());
			cout << " [1] - Вернуться в главное меню\n [2] - Отправка сообщения\n [3] - Вернуться к списку пользователей\n [Q] - Выход из программы" << endl;
			cin >> m_choice;
			checkInput('1', '3', 'q', 'Q');
			if (m_choice == '1') { menuAuthorized(); break; }
			else if (m_choice == '2')
			{
				cout << "Введите сообщение:" << endl;
				string msgText;
				getline(cin, msgText);
				cp1251toUtf8(msgText);
				while (msgText.empty())
				{
					cout << "Некорректный ввод, повторите:" << endl;
					getline(cin, msgText);
					cp1251toUtf8(msgText);
				}
				PerMsgBase->sendMsg(UserBase->getCurUUID(), UserBase->getUsrUUID(temp), msgText);
			}
			else if (m_choice == '3') { menuPerMsg(); break; }
			else if (m_choice == 'q' || m_choice == 'Q') { exit(); break; }
		}
	}
	else if (m_choice == 'q' || m_choice == 'Q') { exit(); }
}

void Chat::menuAdmin()
{
	consoleClear();
	printHeader("Управление пользователями");
	UserBase->printUsrBase();
	cout << "\n Выберите действие: \n [1] - Добавить пользователя \n [2] - Удалить пользователя \n [3] - Сменить пароль пользователя \n [4] - Главное меню" << endl;
	cin >> m_choice;
	checkInput('1', '4');
	if (m_choice == '1') { menuRegUser(); }
	else if (m_choice == '2') { menuDelUser(); }
	else if (m_choice == '3') { menuSetPwd(); }
	else if (m_choice == '4') { menuAuthorized(); }
}

void Chat::menuChgPwd(int id = UserBase->getUsrId())
{
	cinClearIgnore();
	string pwd;
	cout << "Введите новый пароль:" << endl;
	getline(cin, pwd);
	cp1251toUtf8(pwd);
	while (!regPwdChk(pwd))
	{
		getline(cin, pwd);
		cp1251toUtf8(pwd);
	}
	UserBase->setUsrPwd(id, pwd);
	cout << "Пароль пользователя " << UserBase->getUsrName(id) << " успешно изменен." << endl;
	menu_pause();
	if (UserBase->getUsrLogin(UserBase->getCurUUID()) == "admin") { menuAdmin(); }
	else { menuAuthorized(); }
}

void Chat::menuSetPwd()
{
	if (UserBase->getCount() > 1)
	{
		cout << "Изменить пароль учётной записи \"Администратор\" нельзя. Для изменения пароля пользователя введите его ID : " << endl;
		cin >> m_choice;
		checkInput('0', '0' + UserBase->getCount());
		cinClearIgnore();
		menuChgPwd(m_choice);
	}
	else
	{
		cout << "Нет доступных для измениения пользователей." << endl;
		menu_pause();
		menuAdmin();
	}
}

void Chat::menuDelUser()
{
	if (UserBase->getCount() > 1)
	{
		cout << "Учётную запись \"Администратор\" удалить нельзя. Для удаления пользователя введите его ID : " << endl;
		cin >> m_choice;
		checkInput('0', '0' + UserBase->getCount());
		cinClearIgnore();
		cout << "Вы уверены что хотите удалить " << UserBase->getUsrName(m_choice - 48) << "? \n Y - да, N - нет" << endl;
		char temp = m_choice;
		cin >> m_choice;
		while (m_choice != 'y' && m_choice != 'Y' && m_choice != 'n' && m_choice != 'N')
		{
			incorrect();
			cin >> m_choice;
		}
		if (m_choice == 'y' || m_choice == 'Y') { UserBase->delUsr(temp - 48); menuAdmin(); }
		else if (m_choice == 'n' || m_choice == 'N') { menuAdmin(); }	
	}
	else
	{
		cout << "Нет доступных для измениения пользователей." << endl;
		menu_pause();
		menuAdmin();
	}
}

void Chat::printUserBase()
{
	cout << std::format("{:5}", "ID") << std::format("{:12}", "Login") << std::format("{:32}", "Имя пользователя") << endl; 
	for (int i = 0; i < UserBase->getCount(); i++)
	{
		string uuid = UserBase->getUsrUUID(i);
		string login = UserBase->getUsrLogin(i);
		string name = UserBase->getUsrName(i);

		if (UserBase->getCurAuth() && UserBase->getCurUUID() == UserBase->getUsrUUID(login))
			continue;
		if (UserBase->isDeleted(i))
			continue;

		if (authStatChk())
		{
			int msgCount = PerMsgBase->countUnreadPM(uuid);
			if (msgCount > 0)
			{
				cout << std::format("{:<5}", i) << std::format("{:12}", login) << std::format("{:32}", name) << "[" << msgCount << "]" << endl;
			}
			else
			{
				cout << std::format("{:<5}", i) << std::format("{:12}", login) << std::format("{:32}", name) << endl;
			}
		}
		else
		{
			cout << std::format("{:<5}", i) << std::format("{:12}", login) << std::format("{:32}", name) << endl;
		}
	}
}

bool Chat::authStatChk()
{
	if (UserBase->getCurAuth() && UserBase->getUsrName(UserBase->getCurUUID()) != "deleted")
	{
		return true;
	}
	return false;
}

int Chat::exit()
{
	Online->disconnect();
	return 0;
}