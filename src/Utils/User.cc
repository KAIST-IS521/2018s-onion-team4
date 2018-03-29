#include "User.hh"

namespace User {
    Rep::Rep(string pubkey, string _id, uint32_t _ip, uint16_t _port, int _fd) {
        pgp = new PGP::PGP(pubkey);
        id.assign(_id);
        ip = _ip;
        fd = _fd;
        port = _port;
    }

    Rep::~Rep(void) {
        delete pgp;
    }
}
