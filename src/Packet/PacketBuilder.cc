#include "PacketBuilder.hh"
#include <cstring>

namespace PacketBuilder {
    Builder& Builder::operator<<(uint32_t x) {
        int width = sizeof(uint32_t);
        buf = (char *)realloc(buf, size + width);
        memcpy(buf + size, &x, width);
        size += width;
        return *this;
    }
    Builder& Builder::operator<<(int x) {
        int width = sizeof(int);
        buf = (char *)realloc(buf, size + width);
        memcpy(buf + size, &x, width);
        size += width;
        return *this;
    };
    Builder& Builder::operator<<(uint8_t x) {
        int width = sizeof(uint8_t);
        buf = (char *)realloc(buf, size + width);
        memcpy(buf + size, &x, width);
        size += width;
        return *this;
    };
    Builder& Builder::operator<<(string x) {
        int width = x.size();
        buf = (char *)realloc(buf, size + width);
        memcpy(buf + size, x.c_str(), width);
        size += width;
        return *this;
    };
    pair<char *, size_t> Builder::Finalize(void) {
        return pair<char *, size_t>(buf, size);
    };
    string Builder::ToString(void) {
        return string(buf, size);
    }
    Builder::~Builder(void) {
        if (buf) free(buf);
    }
}
