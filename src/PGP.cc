#include "PGP.hh"
#include <unistd.h>

PGPDecrypt::PGPDecrypt(char *prikey) {
    do {
        PassPhrase = getpass("Enter passphrase: ");
    } while (CheckPassPhrase() != false);
}
