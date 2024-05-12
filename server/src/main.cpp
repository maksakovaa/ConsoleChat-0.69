#include "netd.h"

std::unique_ptr<Settings> Config{new Settings};
std::shared_ptr<Logger> Log{new Logger};
std::unique_ptr<UsrBase> UserBase{new UsrBase};
std::unique_ptr<MsgBase> AllMsgBase{new MsgBase};
std::unique_ptr<MsgBase> PerMsgBase{new MsgBase};
std::unique_ptr<SQL_DB> SQL_DataBase{new SQL_DB};
std::unique_ptr<Net> Online{new Net};

int main()
{
    Online->transmission();
    return 0;
}