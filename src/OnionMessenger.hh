#ifndef __ONION__
#define __ONION__
#include <map>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

#include "SelectServer/src/server.h"
#include "SelectServer/src/context.h"
#include "Packet/Packet.hh"
#include "Packet/OnionMessage.hh"
#include "Utils/Features.hh"
#include "Utils/User.hh"
#include "Utils/PGP.hh"
#include "Ui/UIProvider.hh"

using namespace std;
void dbg(string s);

namespace OnionMessenger {
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

            map<string, User::Rep *> users;

            string LoginUser(void);
            string ID;
            void InitServer(void);
            void SendPacket(Packet::Packet *packet, int fd);
            bool SendMsgAsync(string msg, string user);
            bool SendImgAsync(string msg, string user);
            void Relay(Message::OnionLayer *msg);
            void HandleMessage(Message::MsgLayer *msg);
            void HandleAArt(Message::ImgLayer *msg);
            void DoOnionRouting(Message::MsgBody *bd, User::Rep *rep);
        public:
            OnionMessenger(bool usetui, string priv, string pub, uint16_t port);
            void Loop(void);
            void HandleCommand(char *msg);
            bool RecvHandShake(Packet::HandShake *hs);
            void RecvMsgAsync(Packet::Msg *msg);
            void HandShake(uint32_t ip, uint16_t port);
            void HandShake(string ip, uint16_t port);
    };
}

#endif
