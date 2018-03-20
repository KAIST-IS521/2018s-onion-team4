#include <stdlib.h>
#include <stdio.h>
#include "../PGP.hh"
#include <string.h>
int main(){
	PGP* p = new PGP("./prikey");
	
  FILE* pub_fp = fopen("./pubkey", "r");
  fseek(pub_fp, 0, SEEK_END);
  int pub_size = ftell(pub_fp);
  rewind(pub_fp);
  char* pubkey = (char*) malloc(pub_size + 1);
  fread(pubkey, pub_size, 1, pub_fp);
  fclose(pub_fp);

  char *plain = "This is plain text\n";
  char *cipher = p->Encrypt(plain, pubkey);

  printf("Cipher : %s", cipher);
  printf("Result : %s", p->Decrypt(cipher));

  return 0;
}
