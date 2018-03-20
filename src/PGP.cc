#include "PGP.hh"
#include <unistd.h>
#include <string.h>
#include <iostream>

gpgme_error_t passphrase_cb(void *pass, const char *uid_hint,
    const char *passphrase_info, int last_was_bad, int fd){
	gpgme_io_writen(fd, (char*)pass, strlen((char*)pass));
	gpgme_io_writen(fd, "\n", 1);
}

bool PGP::Verify_Pass(){
  FILE* pub_fp = fopen("./pubkey", "r");
  fseek(pub_fp, 0, SEEK_END);
  int pub_size = ftell(pub_fp);
  rewind(pub_fp);
  char* key = (char*) malloc(pub_size + 1);
  fread(key, pub_size, 1, pub_fp);
  fclose(pub_fp);
	char* pt = "This is plain text\n";
	char* ct = this->Encrypt(pt, key);
	char* dt = this->Decrypt(ct);
	
	return !strcmp(pt, dt);
}
	
PGP::PGP(char* prikey_path){
  gpgme_error_t err;
	gpgme_data_t keydata;
	gpgme_import_result_t import_result;
  
	gpgme_check_version (NULL);
  setlocale (LC_ALL, "");
  gpgme_set_locale (NULL, LC_CTYPE, setlocale (LC_CTYPE, NULL));
  err = gpgme_engine_check_version (GPGME_PROTOCOL_OpenPGP);
  fail_if_err (err);

	err = gpgme_new(&ctx);
  fail_if_err(err);
  gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
  gpgme_set_armor(ctx, 1);
  fail_if_err(err);
  
	passphrase = getpass("Enter passphrase: ");
  gpgme_set_passphrase_cb(ctx, passphrase_cb, passphrase);

  err = gpgme_data_new_from_file(&keydata, prikey_path, 1);
  fail_if_err(err);

	err = gpgme_op_import(ctx, keydata);
  fail_if_err(err);

	import_result = gpgme_op_import_result(ctx);
	err = gpgme_get_key(ctx, import_result -> imports -> fpr, &prikey, 1);
  fail_if_err(err);

	while(!this->Verify_Pass()){
		std::cout << "Wrong Passphrase!" << std::endl;
		passphrase = getpass("Enter passphrase: ");
		gpgme_set_passphrase_cb(ctx, passphrase_cb, passphrase);
	}
	
}

char* PGP::Decrypt(char* ct){
  gpgme_data_t cipher_text, plain_text;
  gpgme_data_t keydata;
  gpgme_error_t err;
	gpgme_passphrase_cb_t f;
	void* hook;
	gpgme_get_passphrase_cb(ctx, &f, &hook);

  gpgme_data_new_from_mem(&cipher_text, ct, strlen(ct), 1);
	gpgme_data_new(&plain_text);
  gpgme_op_decrypt(ctx, cipher_text, plain_text);

	size_t size;
	char* result = gpgme_data_release_and_get_mem(plain_text, &size);
  return result;
}

char* PGP::Encrypt(char* pt, char* pubkey){
  gpgme_error_t err;
  gpgme_data_t keydata;
  gpgme_data_t cipher_text, plain_text;
  gpgme_import_result_t import_result;
  gpgme_key_t key[2] = {NULL, NULL};

  err = gpgme_data_new_from_mem(&keydata, pubkey, strlen(pubkey), 1);
  fail_if_err(err);

  err = gpgme_op_import(ctx, keydata);
  fail_if_err(err);

  import_result = gpgme_op_import_result(ctx);
  err = gpgme_get_key(ctx, import_result->imports->fpr, &key[0], 0);
  fail_if_err(err);

  err = gpgme_data_new_from_mem(&plain_text, pt, strlen(pt), 0);
  fail_if_err(err);

	gpgme_data_new(&cipher_text);
  err = gpgme_op_encrypt(ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, plain_text, cipher_text);
  fail_if_err(err);

	size_t size;
	char* result = gpgme_data_release_and_get_mem(cipher_text, &size);
  return result;
}
