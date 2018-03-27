#ifndef __ONION_MESSAGE__
#define __ONION_MESSAGE_
#include <string>
#include "../Utils/User.hh"
#include "Packet.hh"

using namespace std;

namespace message {
    typedef enum {ONIONLAYER=0, MSGLAYER=1, IMGLAYER=2} msgType;

    class MsgBody
    {
        private:
            msgType ty;
        public:
            MsgBody *AddLayer(User::Rep *rep);
            virtual string Serialize(User::Rep *rep) = 0;
            MsgBody(msgType t) : ty(t) { };
            virtual ~MsgBody(void) { };
    };

    class OnionLayer: public MsgBody
    {
        private:
            string id;
            string data;
        public:
            string Serialize(User::Rep *rep);
            OnionLayer(string id, string body);
            ~OnionLayer(void) { };
    };

    class MsgLayer: public MsgBody
    {
        private:
            string sender;
            string data;
        public:
            string Serialize(User::Rep *rep);
            MsgLayer(string sender, string msg);
            ~MsgLayer(void) { };
    };

    class ImgLayer: public MsgBody
    {
        private:
            string sender;
            string url;
        public:
            string Serialize(User::Rep *rep);
            ImgLayer(string sender, string url);
            ~ImgLayer(void) { };
    };

    MsgBody *Unserialize(string data);
}
#endif
