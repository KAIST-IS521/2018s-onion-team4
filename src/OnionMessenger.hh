#ifndef __ONION__
#define __ONION__
#include <map>
#include <iostream>

#include "PGP.hh"
#include "ui/UIProvider.hh"

using namespace std;
namespace OnionMessenger {
class OnionMessenger
    {
        private:
            UI::UIProvider *provider;
            void CLI(void);
        public:
            void Loop(void);
            OnionMessenger(bool usetui, string priv);
    };
}
#endif
