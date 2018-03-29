#ifndef __USER__
#define __USER__

#include "PGP.hh"
#include <string>

namespace User {
    using namespace std;
        class Rep {
            private:
                string id;
                uint32_t ip;
                uint16_t port;
                int fd;
            public:
                PGP::PGP *pgp;
                string GetId(void) { return id; };
                uint32_t GetIp(void) { return ip; };
                uint16_t GetPort(void) { return port; };
                int GetFd(void) { return fd; };
                string Encrypt(string msg) { return pgp->Encrypt(msg); };
                Rep(string pk, string id, uint32_t ip, uint16_t port, int fd);
                ~Rep(void);
        };
}
#endif
