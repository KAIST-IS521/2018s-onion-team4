#ifndef __PGP__
#define __PGP__

class PGPEncrypt
{
    private:
         char *PubKey;
    public:
        char *Encrypt(char *pt);
        PGPEncrypt(char *pubkey);
};

class PGPDecrypt
{
    private:
        char *PriKey;
        char *PassPhrase;
    public:
        char *Decrypt(char *ct);
        PGPDecrypt(char *prikey);
};
#endif
