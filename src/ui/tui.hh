#ifndef __UI__
#define __UI__

#define MAX_ID_LEN 30
#include <iostream>
#include "UIProvider.hh"

using namespace std;

namespace TUI
{
    class TUIProvider : public UI::UIProvider
    {
        public:
            pair<string, string> GetUserInfo(string uid, string info, char *msg);
            void UserInputLoop(string uid, string keyid,
                    void (*handler)(char *, void *), void *aux);

            // add Lock!
            void PushMessage(char *msg);
            void PushError(char *msg);
            TUIProvider();
            ~TUIProvider();
    };
}

#endif
