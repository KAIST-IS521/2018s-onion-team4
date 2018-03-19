#include "PGP.hh"
#include <unistd.h>

PGPDecrypt::PGPDecrypt(char *prikey) {
    PassPhrase = getpass("Enter passphrase: ");
}
