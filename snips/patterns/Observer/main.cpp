#include "impl.h"
#include <iostream>

int main()
{
    std::string bla = "World";
    Observer<Notification<std::string>, StringMessageClient, std::string> mainOb;
    mainOb.insert_notification(bla);
    mainOb.insert_notification("Real World Shit");
    mainOb.insert_client();
    mainOb.insert_client();
    mainOb.insert_client();
    mainOb.insert_client();
    mainOb.Notify();
}
