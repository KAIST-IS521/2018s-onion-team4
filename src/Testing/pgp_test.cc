#include <stdlib.h>
#include <stdio.h>
#include "../PGP.hh"
#include <string.h>
int main(){
  init_gpgme(GPGME_PROTOCOL_OpenPGP);
  FILE* pri_fp = fopen("./prikey", "r");
  FILE* pub_fp = fopen("./pubkey", "r");
  fseek(pri_fp, 0, SEEK_END);
  fseek(pub_fp, 0, SEEK_END);
  int pri_size = ftell(pri_fp);
  int pub_size = ftell(pub_fp);
  rewind(pri_fp);
  rewind(pub_fp);
  char* prikey = (char*) malloc(pri_size + 1);
  char* pubkey = (char*) malloc(pub_size + 1);
  fread(prikey, pri_size, 1, pri_fp);
  fread(pubkey, pub_size, 1, pub_fp);
  fclose(pri_fp);
  fclose(pub_fp);

  PGPEncrypt *enc = new PGPEncrypt(pubkey);
  PGPDecrypt *dec = new PGPDecrypt(prikey);
  
  char *plain = "This is plain text\n";
  //char *plain = (char*) malloc(strlen(p));
  //memcpy(plain, p, strlen(p));
  char *cipher = enc->Encrypt(plain);

  printf("Cipher : %s", cipher);
  printf("Result : %s", dec->Decrypt(cipher));

  return 0;
}
