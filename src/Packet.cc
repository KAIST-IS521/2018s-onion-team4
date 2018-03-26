#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <time.h>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "Packet.hh"

namespace Socket {
  int ConnectServerInternal(int port, uint32_t ip) {
      int client_fd;
      struct sockaddr_in client_addr;

      client_fd = socket(AF_INET, SOCK_STREAM, 0);

      client_addr.sin_addr.s_addr = ip;
      client_addr.sin_family = AF_INET;
      client_addr.sin_port = htons(port);

      if (connect(client_fd, (struct sockaddr *)&client_addr,
                  sizeof(client_addr)) == -1)
      {
          close(client_fd);
          return -1;
      }

      return client_fd;
  }

  int ConnectTo(int port, string ip) {
      return ConnectServerInternal(port, inet_addr(ip.c_str()));
  }

  int ConnectTo(int port, uint32_t ip) {
      return ConnectServerInternal(port, htonl(ip));
  }

  uint32_t GetIPaddr(int fd) {
    struct sockaddr_in temp_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    getpeername(fd, (struct sockaddr *)&temp_addr, &len);

    return ntohl(temp_addr.sin_addr.s_addr);
  }

  uint16_t GetPort(int fd) {
    struct sockaddr_in temp_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    getpeername(fd, (struct sockaddr *)&temp_addr, &len);

    return ntohs(temp_addr.sin_port);
  }
}

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
        char type;
        if (!ctx->aux) {
            CTXRead(ctx, (char *)&type, 1);
            cout << type << endl;
            if (type == HANDSHAKE) {
                packet = new HandShake(ctx->fd);
            } else if(type == MSG) {
                packet = new Msg(ctx->fd);
            } else if(type == IMG) {
                packet = new Img(ctx->fd);
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
        d(string(s.first, s.second));
        d(to_string(write(fd, s.first, s.second)));
        ServerWrite(server, fd, s.first, s.second);
        free(s.first);
        delete this;
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

    Msg::Msg(string msg) : Packet(MSG) {
        message = strdup(msg.c_str());
        length = msg.size();
    }

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
            }
            // Release unused buffer
            CTXDiscard(ctx);
            SetReady();
            ctx->aux = NULL;
            return;
        }
        ctx->aux = this;

    }

    Img::Img(string url) : Packet(IMG) {
        url_length = url.size();
        url = strdup(url.c_str());
    }

    pair<char *, size_t> Img::Serialize() {
        PacketBuilder builder;
        builder << (uint8_t) IMG
                << htonl(url_length)
                << string(url, url_length);
        return builder.Finalize();
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
            url = (char *)calloc(1, url_length + 1);
            CTXRead(ctx, url, url_length);
            // Release unused buffer
            CTXDiscard(ctx);
            SetReady();
            ctx->aux = NULL;
            return;
        }
        ctx->aux = this;
    }

    string Img::GetUrl(void) {
        return string(url, url_length);
    }

    Img::~Img(void) {
        if (url) free(url);
    }
}
