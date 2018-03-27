#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <time.h>
#include <fcntl.h>
#include <string>
#include <unistd.h>

#include "Packet.hh"
#include "PacketBuilder.hh"

namespace Packet {
    // XXX: PACKET INTERFACE
    Packet* Unserialize(ReadCTX *ctx) {
        Packet *packet;
        char type;
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

    void Packet::SendFd(Server *server, int fd) {
        auto s = Serialize();
        ServerWrite(server, fd, (char *)s.c_str(), s.size());
        delete this;
    }

    // XXX: SECTION FOR MSG
    string Msg::Serialize(void) {
        PacketBuilder::Builder builder;
        builder << (uint8_t) MSG << htonl(length) << string(ct, length);
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
            ct = (char *)calloc(1, length+1);
            CTXRead(ctx, ct, length);
            // Release unused buffer
            CTXDiscard(ctx);
            SetReady();
            ctx->aux = NULL;
            return;
        }
        ctx->aux = this;
    }

    string Msg::GetCT(void) {
        return string(ct, length);
    }

    Msg::~Msg(void) {
        if (ct) free(ct);
    }

    Msg::Msg(string msg) : Packet(MSG) {
        ct = strdup(msg.c_str());
        length = msg.size();
    }

    // XXX: SECTION FOR HANDSHAKE
    HandShake::HandShake(string _id, vector<uint32_t> cIps,
            vector<uint16_t>cPorts, string pk) : Packet(HANDSHAKE) {
            id = strdup(_id.c_str());
            id_length = _id.size();
            pubkey = strdup(pk.c_str());
            pubkey_length = pk.size();
            connected_nodes = cIps.size();
            node_ips = (uint32_t *)calloc(sizeof(uint32_t), connected_nodes);
            node_ports = (uint16_t *)calloc(sizeof(uint16_t), connected_nodes);
            std::copy(cIps.begin(), cIps.end(), node_ips);
            std::copy(cPorts.begin(), cPorts.end(), node_ports);
    }

    HandShake::~HandShake(void) {
        if (id) free(id);
        if (pubkey) free(pubkey);
        if (node_ips) free(node_ips);
    }

    string HandShake::Serialize(void) {
        PacketBuilder::Builder builder;
        builder << (uint8_t) HANDSHAKE
                << htonl(id_length)
                << string(id, id_length)
                << htonl(pubkey_length)
                << string(pubkey, pubkey_length)
                << htonl(connected_nodes);

        for (unsigned int i = 0; i < connected_nodes; i++) {
            builder << htonl(node_ips[i]);
            builder << htons(node_ports[i]);
        }
        return builder.Finalize();
    }

    string HandShake::GetId(void) {
        return string(id, id_length);
    }

    string HandShake::GetPubKey(void) {
        return string(pubkey, pubkey_length);
    }

    vector<uint32_t> HandShake::GetConnectedNodeIps(void) {
        return vector<uint32_t>(node_ips, node_ips + connected_nodes);
    }

    vector<uint16_t> HandShake::GetConnectedNodePorts(void) {
        return vector<uint16_t>(node_ports, node_ports + connected_nodes);
    }

    void HandShake::ContinueBuild(ReadCTX *ctx) {
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
        if (state == 5 && CTXGetsz(ctx) >= connected_nodes *
                                        (sizeof(uint16_t) + sizeof(uint32_t))) {
            node_ips = (uint32_t *)calloc(sizeof(uint32_t), connected_nodes);
            node_ports = (uint16_t *)calloc(sizeof(uint16_t), connected_nodes);
            for (unsigned int i = 0; i < connected_nodes; i++) {
                uint32_t temp32;
                uint16_t temp16;
                CTXRead(ctx, (char *)&temp32, 4);
                CTXRead(ctx, (char *)&temp16, 2);
                node_ips[i] = ntohl(temp32);
                node_ports[i] = ntohs(temp16);
                printf("%d\n", ntohs(temp16));
            }
            // Release unused buffer
            CTXDiscard(ctx);
            SetReady();
            ctx->aux = NULL;
            return;
        }
        ctx->aux = this;
    }
}
