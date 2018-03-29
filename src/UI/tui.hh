#ifndef __TUI__
#define __TUI__

#define MAX_ID_LEN 30
#include <iostream>
#include <list>
#include <mutex>
#include "UIProvider.hh"

using namespace std;

namespace TUI
{
    class TUIProvider : public UI::UIProvider
    {
        private:
            string uid;
            mutex msgLock;
            mutex userLock;
            list<string> users;
            void RenderUsers(void);
        public:
            pair<string, string> GetUserInfo(string uid, string info, char *msg);
            void UserInputLoop(string uid, string keyid,
                    void (*handler)(char *, void *), void *aux);

            void PushMessage(char *msg);
            void PushError(string msg);
            void PushChat(string user, string msg);
            void PushNotification(string msg);
            void Clear(void);
            void AddUser(string user);
            void RemoveUser(string user);
            TUIProvider(void);
            ~TUIProvider(void);
    };
}

#endif
