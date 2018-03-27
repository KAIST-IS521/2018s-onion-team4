#include "PGP.hh"
#include <unistd.h>
#include <string.h>
#include <streambuf>
#include <assert.h>
#include <fstream>

namespace PGP {
    gpgme_error_t passphrase_cb(void *pass, const char *uid_hint,
            const char *passphrase_info, int last_was_bad, int fd) {
        gpgme_io_writen(fd, (char *)pass, strlen((const char *)pass));
        gpgme_io_writen(fd, "\n", 1);
        return GPG_ERR_NO_ERROR;
    }

    gpgme_error_t get_passphrase_info_cb(void *pass, const char *uid_hint,
            const char *passphrase_info, int last_was_bad, int fd) {
        auto pgp = static_cast<PGP *>(pass);
        pgp->setUid(uid_hint);
        pgp->setPassInfo(passphrase_info);
        gpgme_io_writen(fd, "\n", 1);
        return GPG_ERR_NO_ERROR;
    }

    void initGPG(void) {
        cout << "Initalizing GPGME...";
        cout.flush();
        gpg_error_t err;
        gpgme_check_version (NULL);
        setlocale (LC_ALL, "");
        gpgme_set_locale (NULL, LC_CTYPE, setlocale (LC_CTYPE, NULL));
        err = gpgme_engine_check_version (GPGME_PROTOCOL_OpenPGP);
        fail_if_err (err);
        cout << " Done." << endl;
    }

    void PGP::get_passphrase_info(void) {
        gpgme_error_t err;
        gpgme_ctx_t ctx;
        gpgme_data_t cipher_text, plain_text;
        gpgme_data_t keydata;
        gpgme_import_result_t import_result;

        err = gpgme_new(&ctx);
        fail_if_err(err);
        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
        gpgme_set_armor(ctx, 1);
        gpgme_set_passphrase_cb(ctx, get_passphrase_info_cb, (void *)this);

        err = gpgme_data_new_from_mem(&keydata,
                priArmored.c_str(), priArmored.size(), 0);
        fail_if_err(err);

        err = gpgme_op_import(ctx, keydata);
        fail_if_err(err);
        import_result = gpgme_op_import_result(ctx);
        err = gpgme_get_key(ctx, import_result -> imports -> fpr, &privkey, 1);
        string t = Encrypt("a");
        gpgme_data_new_from_mem(&cipher_text, t.c_str(), t.size(), 1);
        gpgme_data_new(&plain_text);
        gpgme_op_decrypt(ctx, cipher_text, plain_text);
    }

    void PGP::InitPrikey(string prikey) {
        priArmored.assign(prikey);

    }

    void PGP::InitPubkey(string pubkey) {
        pubArmored.assign(pubkey);
    }


    // This provide enc, dec method
    PGP::PGP(string pubkey, string prikey) {
        InitPubkey(pubkey);
        InitPrikey(prikey);
        get_passphrase_info();
    }

    // This only provide enc
    PGP::PGP(string pubkey) {
        InitPubkey(pubkey);
    }

    void PGP::setUid(string uid) {
        uid_hint = uid;
    }

    string PGP::getUid(void) {
        return uid_hint;
    }

    void PGP::setPassInfo(string passinfo) {
        passphrase_info = passinfo;
    }

    string PGP::getPassInfo(void) {
        return passphrase_info;
    }


    bool PGP::Verify_Pass(const char *pass) {
        gpgme_ctx_t ctx;
        gpgme_error_t err;
        err = gpgme_new(&ctx);
        fail_if_err(err);
        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
        gpgme_set_armor(ctx, 1);

        if (pass) {
            passphrase = strdup(pass);
            gpgme_set_passphrase_cb(ctx, passphrase_cb, passphrase);
            string t = "Verify Pass\n";
            string res = Decrypt(Encrypt(t));
            if (!res.empty() && !res.compare(t)) {
                return true;
            } else {
                std::cout << "Wrong Passphrase" << std::endl;
                free(passphrase);
                passphrase = 0;
                return false;
            }
        }
        return false;
    }

    string PGP::Decrypt(string ct) {
        gpgme_ctx_t ctx;
        gpgme_data_t cipher_text, plain_text;
        gpgme_data_t keydata;
        gpgme_import_result_t import_result;
        gpgme_error_t err;

        err = gpgme_new(&ctx);
        fail_if_err(err);
        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
        gpgme_set_armor(ctx, 1);
        gpgme_set_passphrase_cb(ctx, passphrase_cb, passphrase);

        err = gpgme_data_new_from_mem(&keydata,
                priArmored.c_str(), priArmored.size(), 0);
        fail_if_err(err);

        err = gpgme_op_import(ctx, keydata);
        fail_if_err(err);
        import_result = gpgme_op_import_result(ctx);
        err = gpgme_get_key(ctx, import_result -> imports -> fpr, &privkey, 1);
        gpgme_data_new_from_mem(&cipher_text, ct.c_str(), ct.size(), 1);
        gpgme_data_new(&plain_text);
        gpgme_op_decrypt(ctx, cipher_text, plain_text);

        size_t size;
        char* result = gpgme_data_release_and_get_mem(plain_text, &size);
        return string(result, size);
    }

    string PGP::Encrypt(string pt) {
        gpgme_error_t err;
        gpgme_data_t keydata, cipher_text, plain_text;
        gpgme_import_result_t import_result;
        gpgme_key_t key[2] = {NULL, NULL};
        gpgme_ctx_t ctx;

        err = gpgme_new(&ctx);
        fail_if_err(err);
        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
        gpgme_set_armor(ctx, 1);

        err = gpgme_data_new_from_mem(&keydata,
                pubArmored.c_str(), pubArmored.size(), 0);
        fail_if_err(err);

        err = gpgme_op_import(ctx, keydata);
        fail_if_err(err);

        import_result = gpgme_op_import_result(ctx);
        err = gpgme_get_key(ctx, import_result->imports->fpr, &key[0], 0);
        fail_if_err(err);

        err = gpgme_data_new_from_mem(&plain_text, pt.c_str(), pt.size(), 0);
        fail_if_err(err);

        gpgme_data_new(&cipher_text);
        err = gpgme_op_encrypt(ctx, key,
                GPGME_ENCRYPT_ALWAYS_TRUST, plain_text, cipher_text);
        fail_if_err(err);

        size_t size;
        char* result = gpgme_data_release_and_get_mem(cipher_text, &size);
        return string(result, size);
    }
}
