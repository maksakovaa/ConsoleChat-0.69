#include "navigation.h"

std::unique_ptr<Net> Online(new Net);
std::unique_ptr<UsrBase> UserBase(new UsrBase);
std::unique_ptr<MsgBase> AllMsgBase(new MsgBase);
std::unique_ptr<MsgBase> PerMsgBase(new MsgBase);

int main()
{
    Chat navigation;
    navigation.menuMain();
    return 0;
}