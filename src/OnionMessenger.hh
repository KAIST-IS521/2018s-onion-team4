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

#ifndef PORT
#define PORT 1234
#endif

using namespace std;
void dbg(string s);

namespace OnionMessenger {
    class UserRepresentation {
        private:
            string id;
            uint32_t ip;
            uint16_t port;
            int fd;
        public:
            PGP::PGP *pgp;
            string GetId(void) { return id; };
            uint32_t GetIp(void) { return ip; };
            uint16_t GetPort(void) { return port; };
            int GetFd(void) { return fd; };
            string Encrypt(string msg) { 
                FILE* fp = fopen(msg.c_str(), "a+");
                fclose(fp);
                return pgp->Encrypt(msg); };
            UserRepresentation(string pk, string id, uint32_t ip,
                               uint16_t port, int fd);
            ~UserRepresentation();
    };

    class OnionMessenger
    {
        private:
            UI::UIProvider *provider;
            PGP::PGP *pgp;
            Server *server;

            thread *serverTh;
            mutex serverMutex;
            mutex serverWriteMutex;

            mutex futureMutex;

            uint16_t port;

            map<string, UserRepresentation *> users;
            vector<future<void>> futures;

            string LoginUser(void);
            string ID;
            void InitServer(void);
            void SendPacket(Packet::Packet *packet, int fd);
            bool SendMsgAsync(string msg, string user);
            bool SendImageAsync(string url, string user);
        public:
            OnionMessenger(bool usetui, string priv, string pub, uint16_t port);
            void Loop(void);
            void HandleCommand(char *msg);
            bool RecvHandShake(Packet::HandShake *hs);
            void RecvMsgAsync(Packet::Msg *msg);
            void RecvImageAsync(Packet::Img *img);
            void CleanFuture(void);
            void HandShake(uint32_t ip, uint16_t port);
            void HandShake(string ip, uint16_t port);
    };
}

#endif
