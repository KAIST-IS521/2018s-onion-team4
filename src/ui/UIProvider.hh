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
            virtual void PushError(char *msg) = 0;
    };
}
#endif
