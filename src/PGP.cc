#include "PGP.hh"
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <streambuf>

namespace PGP {
	gpgme_error_t passphrase_cb(void *pass, const char *uid_hint,
			const char *passphrase_info, int last_was_bad, int fd) {
		gpgme_io_writen(fd, (char*)pass, strlen((char*)pass));
		gpgme_io_writen(fd, "\n", 1);
		return GPG_ERR_NO_ERROR;
	}

	void __attribute((constructor)) initGPG() {
		gpg_error_t err;
		gpgme_check_version (NULL);
		setlocale (LC_ALL, "");
		gpgme_set_locale (NULL, LC_CTYPE, setlocale (LC_CTYPE, NULL));
		err = gpgme_engine_check_version (GPGME_PROTOCOL_OpenPGP);
		fail_if_err (err);
	}


	void PGP::InitPrikey(string prikey) {
		ifstream pri(prikey);

		priArmored.assign((istreambuf_iterator<char>(pri)),
				std::istreambuf_iterator<char>());
	}

	void PGP::InitPubkey(string pubkey) {
		ifstream pub(pubkey);
		pubArmored.assign((istreambuf_iterator<char>(pub)),
				std::istreambuf_iterator<char>());
	}

	void PGP::InitCTX() {
		gpgme_error_t err;
		gpgme_data_t keydata;
		gpgme_import_result_t import_result;

		err = gpgme_new(&ctx);
		fail_if_err(err);
		gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
		gpgme_set_armor(ctx, 1);
		fail_if_err(err);

		err = gpgme_data_new_from_mem(&keydata, priArmored.c_str(), priArmored.size(), 0);
		fail_if_err(err);

		err = gpgme_op_import(ctx, keydata);
		fail_if_err(err);

		import_result = gpgme_op_import_result(ctx);
		err = gpgme_get_key(ctx, import_result -> imports -> fpr, &prikey, 1);
		fail_if_err(err);
	}

	void PGP::InitPassPhrase(){
		passphrase = getpass("Enter passphrase: ");
		gpgme_set_passphrase_cb(ctx, passphrase_cb, passphrase);

		while(!this->Verify_Pass()){
			std::cout << "Wrong Passphrase!" << std::endl;
			passphrase = getpass("Enter passphrase: ");
			gpgme_set_passphrase_cb(ctx, passphrase_cb, passphrase);
		}
	}

	// This provide enc, dec method
	PGP::PGP(string pubkey, string prikey) {
		InitPubkey(pubkey);
		InitPrikey(prikey);
		InitCTX();
		InitPassPhrase();
	}

	// This only provide enc
	PGP::PGP(string pubkey) {
		InitPubkey(pubkey);
		InitCTX();
	}

	bool PGP::Verify_Pass(){
		char* pt = "This is plain text\n";
		char* ct = this->Encrypt(pt);
		char* dt = this->Decrypt(ct);
		if(dt == NULL)
			return false;
		return !strcmp(pt, dt);
	}

	char* PGP::Decrypt(char* ct){
		gpgme_data_t cipher_text, plain_text;

		gpgme_data_new_from_mem(&cipher_text, ct, strlen(ct), 0);
		gpgme_data_new(&plain_text);
		gpgme_op_decrypt(ctx, cipher_text, plain_text);

		size_t size;
		char* result = gpgme_data_release_and_get_mem(plain_text, &size);
		return result;
	}

	char* PGP::Encrypt(char* pt){
		gpgme_error_t err;
		gpgme_data_t keydata;
		gpgme_data_t cipher_text, plain_text;
		gpgme_import_result_t import_result;
		gpgme_key_t key[2] = {NULL, NULL};

		err = gpgme_data_new_from_mem(&keydata, pubArmored.c_str(), pubArmored.size(), 0);
		fail_if_err(err);

		err = gpgme_op_import(ctx, keydata);
		fail_if_err(err);

		import_result = gpgme_op_import_result(ctx);
		err = gpgme_get_key(ctx, import_result->imports->fpr, &key[0], 0);
		fail_if_err(err);

		err = gpgme_data_new_from_mem(&plain_text, pt, strlen(pt)+1, 0);
		fail_if_err(err);

		gpgme_data_new(&cipher_text);
		err = gpgme_op_encrypt(ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, plain_text, cipher_text);
		fail_if_err(err);

		size_t size;
		char* result = gpgme_data_release_and_get_mem(cipher_text, &size);
		return result;
	}
}
