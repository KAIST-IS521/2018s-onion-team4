#ifndef __UIPROVIDER__
#define __UIPROVIDER__
#include <iostream>

using namespace std;
namespace UI {
    class UIProvider
    {
        public:
            virtual pair<string, string> GetUserInfo(char *msg) = 0;
            virtual void UserInputLoop(void (*handler)(char *in)) = 0;
    };
}
#endif
