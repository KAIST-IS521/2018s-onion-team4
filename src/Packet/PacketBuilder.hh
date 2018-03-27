#ifndef __PACKET_BUILDER__
#define __PACKET_BUILDER__
#include <iostream>

using namespace std;

namespace PacketBuilder {
    class Builder
    {
        private:
            char *buf = NULL;
            size_t size = 0;
        public:
            Builder& operator<<(uint32_t x);
            Builder& operator<<(int x);
            Builder& operator<<(uint16_t x);
            Builder& operator<<(uint8_t x);
            Builder& operator<<(string x);
            string Finalize(void);
            ~Builder(void);
    };
}
#endif
