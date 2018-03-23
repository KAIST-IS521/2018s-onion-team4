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
namespace Packet {
    class Packet
    {
        private:
            bool ready;
            int type;
            int fd;
        public:
            Packet(int t, int fd) : ready(false), type(t), fd(fd) { };
            ~Packet() {}
            virtual char *Serialize() = 0;
            virtual void ContinueBuild(ReadCTX *ctx) = 0;
            int GetType() { return type; };
            int GetFd() { return fd; };
            int IsReady() { return ready; };
            void SetReady() { ready = true; };
    };

    class HandShake : public Packet
    {
        private:
            int state = 0;
            uint32_t id_length, pubkey_length, connected_nodes;
            char* id, *pubkey;
            uint32_t* node_ips;
            // TODO: We need ip of the client.
            // TODO: Add ip field into packet.
            uint32_t ip;
        public:
            char *Serialize(void);
            string GetId(void);
            string GetPubKey(void);
            int GetIp(void);
            vector<string> GetConnectedNodes(void);
            void ContinueBuild(ReadCTX *ctx);

            HandShake(int t) : Packet(HANDSHAKE, t) { }
            ~HandShake() { }
    };

    class Msg : public Packet
    {
        private:
            int state = 0;
            uint32_t length;
            char *message = NULL;
        public:
            char *Serialize(void);
            string GetMessage(void);
            void ContinueBuild(ReadCTX *ctx);
            Msg(int t) : Packet(MSG, t) {};
            ~Msg();
    };

    Packet *Unserialize(ReadCTX *ctx);
}
#endif
