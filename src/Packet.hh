#ifndef __PACKET__
#define __PACKET__

#include <cstdint>
#include <string>
#include <vector>

#include "SelectServer/src/context.h"

#define HANDSHAKE 0
#define MSG 1
#define IMG 2

using namespace std;

namespace Socket {
  int ConnectTo(int port, string ip);
  int ConnectTo(int port, uint32_t ip);
  uint32_t GetIPaddr(int fd);
}

namespace Packet {
    class Packet
    {
        private:
            bool ready;
            int type;
            int fd;
        public:
            Packet(int t, int fd) : ready(false), type(t), fd(fd) { };
            Packet(int t) : ready(false), type(t) { };
            virtual ~Packet(void) { };
            virtual pair<char *, size_t> Serialize(void) = 0;
            virtual void ContinueBuild(ReadCTX *ctx) = 0;
            int GetType(void) { return type; };
            int GetFd(void) { return fd; };
            int IsReady(void) { return ready; };
            void SetReady(void) { ready = true; };
            void SendFd(Server *server, int fd);
    };

    class HandShake : public Packet
    {
        private:
            int state = 0;
            uint32_t id_length, pubkey_length, connected_nodes;
            char *id = NULL;
            char *pubkey = NULL;
            uint32_t* node_ips = NULL;
        public:
            pair<char *, size_t> Serialize(void);
            string GetId(void);
            string GetPubKey(void);
            vector<uint32_t> GetConnectedNodes(void);
            void ContinueBuild(ReadCTX *ctx);

            HandShake(int t) : Packet(HANDSHAKE, t) { };
            HandShake(string id, vector<uint32_t> cNodes, string pubkey);
            ~HandShake(void);
    };

    class Msg : public Packet
    {
        private:
            int state = 0;
            uint32_t length;
            char *message = NULL;
        public:
            pair<char *, size_t> Serialize(void);
            string GetMessage(void);
            void ContinueBuild(ReadCTX *ctx);
            Msg(int t) : Packet(MSG, t) { };
            Msg(string msg);
            ~Msg(void);
    };

    class Img : public Packet
    {
        private:
            int state = 0;
            uint32_t url_length;
            char *url = NULL;
        public:
            pair<char *, size_t> Serialize(void);
            char *GetUrl(void);
            int GetUrlLength(void);
            void ContinueBuild(ReadCTX *ctx);
            Img() : Packet(IMG) {};
            ~Img();
    };

    Packet *Unserialize(ReadCTX *ctx);
}
#endif
