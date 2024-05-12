#pragma once
#include "MsgBase.h"

void consoleClear();
void localeSetup();
void cinClearIgnore();
void incorrect();
bool nonLatinChk(string& text);
bool noSpacesChk(string& text);
bool lengthChk(string& text);
bool regLoginChk(string& _login);
bool regPwdChk(string& _pwd);
bool authPwdChk(string& _login, string& _pwd);
string getOsName();
int getProcId();
string middle(const string& str);
void printHeader(const string& header);
void printBorder();
void cp1251toUtf8(string& str);
string curDateTime();
string logTimeStamp();
void menu_pause();