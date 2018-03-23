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

#define PORT 1234

using namespace std;
namespace OnionMessenger {
    class UserRepresentation {
        private:
            PGP::PGP *pgp;
            string id;
            uint32_t ip;
            int fd;
        public:
            string GetId(void) { return id; };
            uint32_t GetIp(void) { return ip; };
            int GetFd(void) { return fd; };
            string Encrypt(string msg) { return pgp->Encrypt(msg); };
            UserRepresentation(string pk, string id, int ip, int fd);
            ~UserRepresentation();
    };

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

            map<string, UserRepresentation *> users;

            string LoginUser(void);
            string ID;
            void InitServer(void);
            void HandShake(uint32_t ip);
            void SendPacket(Packet::Packet *packet, int fd);
            bool HandleChatAsync(string msg, string user);
        public:
            OnionMessenger(bool usetui, string priv, string pub);
            void Loop(void);
            void HandleCommand(char *msg);
            void HandleAsync(Packet::Msg *msg);
            bool HandleHandShake(Packet::HandShake *hs);
            void CleanFuture(void);
    };
}
#endif
