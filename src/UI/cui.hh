#ifndef __CUI__
#define __CUI__

#define MAX_ID_LEN 30
#include <iostream>
#include <mutex>
#include "UIProvider.hh"

using namespace std;

namespace CUI
{
    class CUIProvider : public UI::UIProvider
    {
        private:
            mutex lock;
        public:
            pair<string, string> GetUserInfo(string uid, string info, char *msg);
            void UserInputLoop(string uid, string keyid,
                    void (*handler)(char *, void *), void *aux);

            // add Lock!
            void PushMessage(char *msg);
            void PushError(string msg);
            void PushChat(string user, string msg);
            void PushNotification(string msg);
            void Clear(void);

            // TODO
            void AddUser(string user) { };
            void RemoveUser(string user) { };

            CUIProvider(void);
            ~CUIProvider(void);
    };
}
#endif
