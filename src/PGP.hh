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

void init_gpgme(gpgme_protocol_t proto);
class PGPEncrypt
{
    private:
        gpgme_ctx_t ctx;
        char* PubKey;
    public:
        char* Encrypt(char *pt);
        PGPEncrypt(char *pubkey);
};

class PGPDecrypt
{
    private:
        gpgme_ctx_t ctx;
        char* PriKey;
        char *PassPhrase;
    public:
        char* Decrypt(char *ct);
        PGPDecrypt(char *prikey);
};
#endif
