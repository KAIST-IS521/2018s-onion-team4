#ifndef __PGP__
#define __PGP__
#include <iostream>
#include <string>
#include <stdlib.h>
#include <gpgme.h>
#include <locale.h>
#include <errno.h>
#include <mutex>
#include <fcntl.h>

using namespace std;

#define TEMP_LEN 16

namespace PGP {
    #define fail_if_err(err) do {\
        if (err) { \
            cerr << __FILE__ << ":" \
            << __LINE__ << ": " \
            << gpgme_strsource (err) << ": " \
            << gpgme_strerror (err) << endl; \
            exit(1); \
        } \
    } while(0);
    class PGP
    {
        private:
            string pubArmored;
            string priArmored;
            string uid_hint;
            string passphrase_info;
            gpgme_key_t privkey;
            char* passphrase;

            void ImportKey(string pubkey);
            void ImportSecretKey(string prikey);
            void InitPrikey(string prikey);
            void InitPubkey(string pubkey);

        public:
            PGP(string pubkey, string prikey);
            PGP(string pubkey);
            string Encrypt(string pt);
            string Decrypt(string ct);
            string GetPub(void) { return pubArmored; };
            bool Verify_Pass(const char *pass);
            void setUid(string uid);
            string getUid(void);
            void setPassInfo(string passinfo);
            string getPassInfo(void);
    };
}
#endif
