#ifndef __PGP__
#define __PGP__
#include <iostream>
#include <string>
#include <stdlib.h>
#include <gpgme.h>
#include <locale.h>
#include <errno.h>

using namespace std;

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
		class Data
		{
			private:
				char* data;
				size_t size;
			public:
				Data(char* data, size_t size);
				char* getData();
				size_t getSize();
		};

    class PGP
    {
        private:
            string pubArmored;
            string priArmored;
            string uid_hint;
            string passphrase_info;

            gpgme_ctx_t ctx;
            gpgme_key_t privkey;
            char* passphrase;
            void get_passphrase_info();
            void InitPrikey(string prikey);
            void InitPubkey(string pubkey);
            void InitCTX();
						void InitPassPhrase();
						void setPass();
        public:
            PGP(string pubkey, string prikey);
            PGP(string pubkey);
						string Encrypt(string pt);
						string Encrypt(Data d);
            string Decrypt(string ct);
						Data Decrypt_as_Data(string ct);
            bool Verify_Pass(const char *pass);
            void setUid(string uid);
            string getUid();
            void setPassInfo(string passinfo);
            string getPassInfo();
    };
}
#endif
