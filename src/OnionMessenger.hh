#ifndef __ONION__
#define __ONION__
#include <map>
#include <iostream>
#include <thread>

#include "PGP.hh"
#include "SelectServer/src/server.h"
#include "SelectServer/src/context.h"

#include "ui/UIProvider.hh"

using namespace std;
namespace OnionMessenger {
class OnionMessenger
    {
        private:
            UI::UIProvider *provider;
            PGP::PGP *pgp;
            Server *server;
            thread *serverTh;

            string LoginUser(void);
            string ID;
            void InitServer(void);
        public:
            OnionMessenger(bool usetui, string priv, string pub);
            void Loop(void);
            void handleCommand(char *msg);
            void Downloadmusic(string urls);
            void Downloadfile(string urls);
            void Asciiart(char *filepath);
            void Exemusic(char *filepath);

    };
}
#endif
