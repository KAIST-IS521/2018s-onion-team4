#ifndef __PACKET__
#define __PACKET__

#include <cstdint>
#include "SelectServer/src/context.h"

#define HANDSHAKE 0
#define MSG 1
namespace Packet {
    class Packet
    {
        private:
            bool ready;
            int type;
        public:
            Packet(int t) : ready(false), type(t) { };
            ~Packet() {}
            virtual char *Serialize() = 0;
            virtual void ContinueBuild(ReadCTX *ctx) = 0;
            int getType() { return type; };
            int isReady() { return ready; };
            void setReady() { ready = true; };

    };

    class HandShake : public Packet
    {
        public:
            uint32_t id_length, pubkey_length, connected_nodes;
            char* id, *pubkey;
            uint32_t* node_ips;

            char *Serialize();
            void ContinueBuild(ReadCTX *ctx);

            HandShake() : Packet(HANDSHAKE) { }
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
            char *GetMessage(void);
            int GetMessageLength(void);
            void ContinueBuild(ReadCTX *ctx);
            Msg() : Packet(MSG) {};
            ~Msg();
    };

    Packet *Unserialize(ReadCTX *ctx);
}
#endif
