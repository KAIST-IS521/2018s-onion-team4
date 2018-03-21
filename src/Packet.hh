#ifndef __PACKET__
#define __PACKET__

class Packet
{
    public:
        virtual char *Serialize() = 0;
};

class HandShake : Packet
{
    public:
        uint32_t id_length, pubkey_length, connected_nodes;
        
        char *Serialize();
};

class Msg : Packet
{
    public:
        uint32_t timestamp, length;
        char* message;

        char *Serialize();
};

#endif
