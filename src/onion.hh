#ifndef __ONION__
#define __ONION__
#include <map>
#include "PGP.hh"

using namespace std;
class Onion
{
    private:
        map<int, PGPEncrypt> encrypters;
        PGPDecrypt decrypter;
        void CLI(void);
    public:
        void Loop(void);
        Onion();
};
#endif
