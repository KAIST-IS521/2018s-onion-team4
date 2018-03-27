#ifndef __PACKET__
#define __PACKET__

#include <cstdint>
#include <string>
#include <vector>

#include "../SelectServer/src/context.h"

#define HANDSHAKE 0
#define MSG 1

using namespace std;
void d(string s);


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
            virtual string Serialize(void) = 0;
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
            uint16_t* node_ports = NULL;
        public:
            string Serialize(void);
            string GetId(void);
            string GetPubKey(void);
            vector<uint32_t> GetConnectedNodeIps(void);
            vector<uint16_t> GetConnectedNodePorts(void);
            void ContinueBuild(ReadCTX *ctx);

            HandShake(int t) : Packet(HANDSHAKE, t) { };
            HandShake(string id, vector<uint32_t> cIps,
                      vector<uint16_t> cPorts, string pubkey);
            ~HandShake(void);
    };

    class Msg : public Packet
    {
        private:
            int state = 0;
            uint32_t length;
            char *ct = NULL;
        public:
            string Serialize(void);
            string GetCT(void);
            void ContinueBuild(ReadCTX *ctx);
            Msg(int t) : Packet(MSG, t) { };
            Msg(string msg);
            ~Msg(void);
    };

    Packet *Unserialize(ReadCTX *ctx);
}
#endif
