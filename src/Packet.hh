#ifndef __PACKET__
#define __PACKET__

class Packet
{
    public:
        virtual char *Serialize() = 0;
};

class HandShake : Packet
{
};

class Msg : Packet
{
};

#endif
