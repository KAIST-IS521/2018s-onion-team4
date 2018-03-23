#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Packet.hh"
namespace Packet {
    class PacketBuilder
    {
        private:
            char *buf = NULL;
            size_t size = 0;
        public:
            PacketBuilder& operator<<(uint32_t x) {
                int width = sizeof(uint32_t);
                buf = (char *)realloc(buf, size + width);
                memcpy(buf + size, &x, width);
                size += width;
                return *this;
            };
            PacketBuilder& operator<<(int x) {
                int width = sizeof(int);
                buf = (char *)realloc(buf, size + width);
                memcpy(buf + size, &x, width);
                size += width;
                return *this;
            };
            PacketBuilder& operator<<(uint8_t x) {
                int width = sizeof(uint8_t);
                buf = (char *)realloc(buf, size + width);
                memcpy(buf + size, &x, width);
                size += width;
                return *this;
            };
            PacketBuilder& operator<<(string x) {
                int width = x.size();
                buf = (char *)realloc(buf, size + width);
                memcpy(buf + size, x.c_str(), width);
                size += width;
                return *this;
            };

            pair<char *, size_t> Finalize(void) {
                return pair<char *, size_t>(buf, size);
            };
    };



    Packet* Unserialize(ReadCTX *ctx) {
        Packet *packet;
        int type;
        if (!ctx->aux) {
            CTXRead(ctx, (char *)&type, 1);
            if (type == HANDSHAKE) {
                packet = new HandShake(ctx->fd);
            } else if(type == MSG) {
                packet = new Msg(ctx->fd);
            } else {
                return NULL;
            }
        } else {
            packet = static_cast<Packet *>(ctx->aux);
        }
        packet->ContinueBuild(ctx);
        return packet;
    }

    pair<char *, size_t> Msg::Serialize(void) {
        PacketBuilder builder;
        builder << (uint8_t) MSG << htonl(length) << string(message, length);
        return builder.Finalize();
    }

    void Msg::ContinueBuild(ReadCTX *ctx) {
        int l;
        // state for parse length
        if (state == 0 && CTXGetsz(ctx) >= 4) {
            CTXRead(ctx, (char *)&l, 4);
            length = ntohl(l);
            state = 1;
        }
        // state for parse message
        if (state == 1 && CTXGetsz(ctx) >= length) {
            message = (char *)calloc(1, length+1);
            CTXRead(ctx, message, length);
            // Release unused buffer
            CTXDiscard(ctx);
            SetReady();
            ctx->aux = NULL;
            return;
        }
        ctx->aux = this;
    }

    string Msg::GetMessage(void) {
        return string(message, length);
    }

    Msg::~Msg(void) {
        if (message) free(message);
    }

    pair<char *, size_t> HandShake::Serialize(void) {
        PacketBuilder builder;
        builder << (uint8_t) HANDSHAKE
                << htonl(id_length)
                << string(id, id_length)
                << htonl(pubkey_length)
                << string(pubkey, pubkey_length)
                << htonl(connected_nodes);

        for (unsigned int i = 0; i < connected_nodes; i++) {
            builder << htonl(node_ips[i]);
        }
        return builder.Finalize();
    }

    void HandShake::ContinueBuild(ReadCTX *ctx) {
        // TODO: Fill me
        /*
        HandShake* hs = new HandShake();

        hs->id_length = ntohl(*(uint32_t *) (recv + 1));
        hs->id = (char *) calloc(1, sizeof(hs->id_length));
        memcpy(hs->id, recv + 5, hs->id_length);

        hs->pubkey_length = ntohl(*(uint32_t *) (recv + 5 + hs->id_length));
        hs->pubkey = (char *) calloc(1, sizeof(hs->pubkey_length));
        memcpy(hs->pubkey, recv + 9 + hs->id_length, hs->pubkey_length);

        hs->connected_nodes = ntohl(*(uint32_t *) (recv + 9 + hs->id_length +
                    hs->pubkey_length));
        hs->node_ips = (uint32_t *) calloc(hs->connected_nodes, sizeof(uint32_t));
        for(unsigned int i = 0; i < hs->connected_nodes; i++){
            uint32_t temp = ntohl(*(uint32_t *)
                    (recv + 13 + 4 * i + hs->id_length + hs->pubkey_length));
            hs->node_ips[i] = temp;
        }
        */
    }
}
