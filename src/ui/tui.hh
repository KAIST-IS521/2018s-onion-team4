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
            pair<string, string> GetUserInfo(char *msg);
            void UserInputLoop(void (*handler)(char *in));
            TUIProvider();
            ~TUIProvider();
    };
}

#endif
