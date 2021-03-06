#include "OnionMessage.hh"
#include "PacketBuilder.hh"
#include <ncurses.h>
#include <cstring>

namespace Message {
    OnionLayer *MsgBody::AddLayer(User::Rep *rep) {
        auto msg = new OnionLayer(rep->GetId(), Serialize(rep));
        delete this;
        return msg;
    }

    // XXX: Section for OnionLayer
    string OnionLayer::Serialize(User::Rep *rep) {
        PacketBuilder::Builder builder;
        builder << (uint8_t)ONIONLAYER
                << htonl(id.size())
                << id
                << data;
        return rep->Encrypt(builder.Finalize());
    }

    OnionLayer::OnionLayer(string _id, string _body) : MsgBody(ONIONLAYER) {
        id = _id;
        data = _body;
    }

    // XXX: Section for MsgLayer
    string MsgLayer::Serialize(User::Rep *rep) {
        PacketBuilder::Builder builder;
        builder << (uint8_t)MSGLAYER
                << htonl(sender.size())
                << sender
                << data;
        return rep->Encrypt(builder.Finalize());
    }

    MsgLayer::MsgLayer(string _sender, string msg) : MsgBody(MSGLAYER) {
        sender = _sender;
        data = msg;
    }

    // XXX: Section for ImgLayer
    string ImgLayer::Serialize(User::Rep *rep) {
        PacketBuilder::Builder builder;
        builder << (uint8_t)IMGLAYER
                << htonl(sender.size())
                << sender
                << url;
        return rep->Encrypt(builder.Finalize());
    }

    ImgLayer::ImgLayer(string _sender, string _url) : MsgBody(IMGLAYER) {
        sender = _sender;
        url = _url;
    }

    MsgBody *Unserialize(string data) {
        uint8_t ty = data.c_str()[0];
        uint32_t length;
        memcpy(&length, data.substr(1, 4).c_str(), 4);
        length = ntohl(length);
        string id = data.substr(5, length);
        string left = data.substr(5+length);

        switch (ty) {
            case ONIONLAYER:
                return new OnionLayer(id, left);
            case MSGLAYER:
                return new MsgLayer(id, left);
            case IMGLAYER:
                return new ImgLayer(id, left);
            default:
                return NULL;
        }
    }
}
