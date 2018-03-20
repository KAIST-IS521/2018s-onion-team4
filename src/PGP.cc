#include "PGP.hh"
#include <unistd.h>
#include <string.h>

//gpgme_ctx_t ctx;
gpgme_error_t passphrase_cb(void *pass, const char *uid_hint,
            const char *passphrase_info, int last_was_bad, int fd){
  gpgme_io_writen(fd, (char*)pass, strlen((char*)pass));
}

void init_gpgme(gpgme_protocol_t proto){
  gpgme_error_t err;

  gpgme_check_version (NULL);
  setlocale (LC_ALL, "");
  gpgme_set_locale (NULL, LC_CTYPE, setlocale (LC_CTYPE, NULL));

  err = gpgme_engine_check_version (proto);
  fail_if_err (err);

}

char* Data2String(gpgme_data_t data){
  gpgme_data_seek(data, 0, SEEK_SET);
  size_t size = gpgme_data_seek(data, 0, SEEK_END);
  char* result = (char*) malloc(size + 1);
  gpgme_data_seek(data, 0, SEEK_SET);
  gpgme_data_read(data, result, size);
  gpgme_data_release(data);
  return result;
}

PGPDecrypt::PGPDecrypt(char *prikey) {
  gpgme_error_t err;
  err = gpgme_new(&ctx);
  fail_if_err(err);
  //gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
  PassPhrase = getpass("Enter passphrase: ");
  gpgme_set_passphrase_cb(ctx, passphrase_cb, (void*)PassPhrase);
  gpgme_set_armor(ctx, 1);
  PriKey = prikey;
}

PGPEncrypt::PGPEncrypt(char *pubkey){
  gpgme_error_t err;
  err = gpgme_new(&ctx);
  fail_if_err(err);
  gpgme_set_armor(ctx, 1);
  PubKey = pubkey;
}

char* PGPDecrypt::Decrypt(char* ct){
  gpgme_data_t cipher_text, plain_text;
  gpgme_data_t keydata;
  gpgme_import_result_t import_result;
  gpgme_error_t err;
  gpgme_key_t key[2] = {NULL, NULL};
  char* temp="\0";
  char pass_temp[100]= "\0";
  void * cb = NULL;
  int flag=0;
  int count=2;
  err = gpgme_data_new_from_mem(&keydata, PriKey, strlen(PriKey), 1);
  fail_if_err(err);

  err = gpgme_op_import(ctx, keydata);
  fail_if_err(err);

  import_result = gpgme_op_import_result(ctx);
  err = gpgme_get_key(ctx, import_result->imports->fpr, &key[0], 1);
  fail_if_err(err);

  gpgme_get_passphrase_cb(ctx,NULL,&cb);
  while(flag==0){
    strcpy(pass_temp,(char*)cb);
    temp = getpass("Enter passphrase: ");
    if(strcmp(pass_temp,temp)==0)
    {
      flag=1;
    }
    else{
      strcpy((char*)cb,pass_temp);
      printf("PassPhrase is wrong (%d/3)\n",count);
      if(count==0)
        exit(0);
      count-=1;
    }
  }

  gpgme_data_new_from_mem(&cipher_text, ct, strlen(ct), 1);
	gpgme_data_new(&plain_text);
  gpgme_op_decrypt(ctx, cipher_text, plain_text);
  char* result = Data2String(plain_text);
  return result;
}

char* PGPEncrypt::Encrypt(char* pt){
  gpgme_error_t err;
  gpgme_data_t keydata;
  gpgme_data_t cipher_text, plain_text;
  gpgme_import_result_t import_result;
  gpgme_key_t key[2] = {NULL, NULL};

  err = gpgme_data_new_from_mem(&keydata, PubKey, strlen(PubKey), 1);
  fail_if_err(err);

  err = gpgme_op_import(ctx, keydata);
  fail_if_err(err);

  import_result = gpgme_op_import_result(ctx);
  err = gpgme_get_key(ctx, import_result->imports->fpr, &key[0], 0);
  fail_if_err(err);

  gpgme_data_new_from_mem(&plain_text, pt, strlen(pt), 0);
	gpgme_data_new(&cipher_text);
  err = gpgme_op_encrypt(ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, plain_text, cipher_text);
  fail_if_err(err);

  char* result = Data2String(cipher_text);
  return result;
}
