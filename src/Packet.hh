#ifndef __PACKET__
#define __PACKET__

#include <stdint.h>

class Packet
{
    public:
        Packet() {}
        ~Packet() {}
        virtual char *Serialize() = 0;
};

class HandShake : public Packet
{
    public:
        uint32_t id_length, pubkey_length, connected_nodes;
        char* id, *pubkey;
        uint32_t* node_ips;
        
        char *Serialize();

        HandShake() : Packet() { }
        ~HandShake() { }
};

class Msg : public Packet
{
    public:
        uint32_t timestamp, length;
        char* message;

        char *Serialize();

        Msg() : Packet() { }
        ~Msg() { }
};

Packet* Deserialize(char*);

#endif
