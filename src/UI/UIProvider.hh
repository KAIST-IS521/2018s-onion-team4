#ifndef __UIPROVIDER__
#define __UIPROVIDER__
#include <iostream>

using namespace std;
namespace UI {
    class UIProvider
    {
        public:
            virtual pair<string, string> GetUserInfo(string uid,
                                                     string info, char *msg) = 0;
            virtual void UserInputLoop(string uid, string info,
                    void (*handler)(char *, void *), void *aux) = 0;
            virtual void PushMessage(char *msg) = 0;
            void PushMessage(const char *msg) { PushMessage((char *)msg); };
            void PushMessage(string msg) { PushMessage(msg.c_str()); };
            virtual void PushError(string msg) = 0;
            virtual void PushChat(string user, string msg) = 0;
            virtual void PushNotification(string msg) = 0;
            virtual void Clear(void) = 0;

            virtual void AddUser(string user) = 0;
            virtual void RemoveUser(string user) = 0;
    };
}
#endif
