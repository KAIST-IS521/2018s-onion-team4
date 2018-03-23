#ifndef __ONION__
#define __ONION__
#include <map>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

#include "SelectServer/src/server.h"
#include "SelectServer/src/context.h"
#include "PGP.hh"
#include "Packet.hh"
#include "Features.hh"
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

            mutex serverWriteMutex;
            mutex futureMutex;
            vector<future<void>> futures;

            string LoginUser(void);
            string ID;
            void InitServer(void);
        public:
            OnionMessenger(bool usetui, string priv, string pub);
            void Loop(void);
            void HandleCommand(char *msg);
            void HandleAsync(Packet::Msg *msg);
            void HandleAsync(Packet::HandShake *hs);
            void CleanFuture(void);
    };
}
#endif
