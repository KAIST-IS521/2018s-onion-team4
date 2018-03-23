#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Packet.hh"
namespace Packet {


    Packet* Unserialize(ReadCTX *ctx) {
        Packet *packet;
        int type;
        if (!ctx->aux) {
            CTXRead(ctx, (char *)&type, 1);
            if (type == HANDSHAKE) {
                packet = new HandShake();
            } else if(type == MSG) {
                packet = new Msg();
            } else if(type == IMG) {
                packet = new Img();
            } else {
                return NULL;
            }
        } else {
            packet = static_cast<Packet *>(ctx->aux);
        }
        packet->ContinueBuild(ctx);
        return packet;
    }

    char* Msg::Serialize() {
        size_t lth = 5 + this->length;

        char* packet = (char *) malloc(lth);

        *(uint8_t *) (packet + 0) = MSG;
        *(uint32_t *) (packet + 1) = htonl(this->length);
        if(this->message)
            memcpy(packet + 5, this->message, this->length);

        return packet;
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
            setReady();
            ctx->aux = NULL;
            return;
        }
        ctx->aux = this;
    }

    char *Msg::GetMessage(void) {
        return message;
    }

    int Msg::GetMessageLength(void) {
        return length;
    }

    Msg::~Msg(void) {
        if (message) free(message);
    }

    char * HandShake::Serialize(void) {
        size_t lth = 13 + this->id_length + this->pubkey_length +
                      sizeof(uint32_t) * this->connected_nodes;

        char* packet = (char *) malloc(lth);

        *(uint8_t *) (packet + 0) = HANDSHAKE;

        *(uint32_t *) (packet + 1) = htonl(this->id_length);
        if(this->id)
            memcpy(packet + 5, this->id, this->id_length);

        *(uint32_t *) (packet + 5 + this->id_length) = htonl(this->pubkey_length);
        if(this->pubkey)
            memcpy(packet + 9 + this->id_length, this->pubkey, this->pubkey_length);

        *(uint32_t *) (packet + 9 + this->id_length + this->pubkey_length)
                                                  = htonl(this->connected_nodes);
        if(this->node_ips){
            for(unsigned int i = 0; i < this->connected_nodes; i++){
                *(uint32_t *) (packet + 13 + sizeof(uint32_t) * i + this->id_length + this->pubkey_length)
                     = htonl(this->node_ips[i]);
            }
        }
        return packet;
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
        unsigned int il, kl, cn;
        // state for parse length
        if (state == 0 && CTXGetsz(ctx) >= 4) {
            CTXRead(ctx, (char *)&il, 4);
            id_length = ntohl(il);
            state = 1;
        }
        // state for parse message
        if (state == 1 && CTXGetsz(ctx) >= id_length) {
            id = (char *)calloc(1, id_length + 1);
            CTXRead(ctx, id, id_length);
            state = 2;
        }
        if (state == 2 && CTXGetsz(ctx) >= 4) {
            CTXRead(ctx, (char *)&kl, 4);
            pubkey_length = ntohl(kl);
            state = 3;
        }
        if (state == 3 && CTXGetsz(ctx) >= pubkey_length) { 
            pubkey = (char *)calloc(1, pubkey_length + 1);
            CTXRead(ctx, pubkey, pubkey_length);
            state = 4;
        }
        if (state == 4 && CTXGetsz(ctx) >= 4) {
            CTXRead(ctx, (char *)&cn, 4);
            connected_nodes = ntohl(cn);
            state = 5;
        }
        if (state == 5 && CTXGetsz(ctx) >= connected_nodes * 4){
            node_ips = (char *)calloc(1, connected_nodes * 4);
            for (unsigned int i = 0; i < connected_nodes; i++) {
                unsigned int temp;
                CTXRead(ctx, (char *)&temp, 4);
                node_ips[i] = ntohl(temp);
            }
            // Release unused buffer
            CTXDiscard(ctx);
            setReady();
            ctx->aux = NULL;
            return;
        }
        ctx->aux = this;

    }
    
    char* Img::Serialize() {
        size_t lth = 5 + this->url_length;

        char* packet = (char *) malloc(lth);

        *(uint8_t *) (packet + 0) = IMG;
        *(uint32_t *) (packet + 1) = htonl(this->url_length);
        if(this->url)
            memcpy(packet + 5, this->url, this->url_length);

        return packet;
    }

    void Img::ContinueBuild(ReadCTX *ctx) {
        int l;
        // state for parse length
        if (state == 0 && CTXGetsz(ctx) >= 4) {
            CTXRead(ctx, (char *)&l, 4);
            url_length = ntohl(l);
            state = 1;
        }
        // state for parse message
        if (state == 1 && CTXGetsz(ctx) >= url_length) {
            url = (char *)calloc(1, length + 1);
            CTXRead(ctx, url, url_length);
            // Release unused buffer
            CTXDiscard(ctx);
            setReady();
            ctx->aux = NULL;
            return;
        }
        ctx->aux = this;
    }

    char *Img::GetUrl(void) {
        return url;
    }

    int Img::GetUrlLength(void) {
        return url_length;
    }

    Img::~Img(void) {
        if (url) free(url);
    }
}
