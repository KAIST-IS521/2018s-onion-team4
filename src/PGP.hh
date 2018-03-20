#ifndef __PGP__
#define __PGP__
#include <stdlib.h>
#include <gpgme.h>
#include <locale.h>
#include <errno.h>

#define fail_if_err(err)          \
  do                \
    {               \
      if (err)              \
        {             \
          fprintf (stderr, "%s:%d: %s: %s\n",     \
                   __FILE__, __LINE__, gpgme_strsource (err), \
       gpgme_strerror (err));     \
          exit (1);           \
        }             \
    }               \
  while (0)

class PGP{
	private:
		gpgme_ctx_t ctx;
		gpgme_key_t prikey;
		char* passphrase;
	public:
		PGP(char* prikey_path);
		char* Encrypt(char* pt, char* pubkey);
		char* Decrypt(char* ct);
		bool Verify_Pass();
};
#endif
