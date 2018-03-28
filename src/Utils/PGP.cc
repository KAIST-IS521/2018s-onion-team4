#include "PGP.hh"
#include <unistd.h>
#include <string.h>
#include <streambuf>
#include <assert.h>
#include <fstream>

namespace PGP {
    void PGP::ImportKey(string key){
        char buf[1024] = "";
        string keyfname = tmpnam(NULL);
        ofstream keyfile(keyfname);
        keyfile << key;
        keyfile.close();

        string command = "/usr/bin/gpg --import " + keyfname + " 2>&1";
        FILE* pipe = popen(command.c_str(), "r");

        fgets(buf, 1024, pipe);
        string s = string(buf);
        s = s.substr(s.find("key "));
        string info = s.substr(4, 8);

        s = s.substr(s.find('\"'));
        string id = s.substr(1, s.find('\"', 1)-1);

        pclose(pipe);
        setPassInfo(info);
        setUid(id);

        unlink(keyfname.c_str());
    }

    void PGP::ImportSecretKey(string key){
        string keyfname = tmpnam(NULL);
        ofstream keyfile(keyfname);
        keyfile << key;
        keyfile.close();

        string command = "/usr/bin/gpg --import " + keyfname + " 2>&1";
        FILE* pipe = popen(command.c_str(), "r");

        pclose(pipe);
        unlink(keyfname.c_str());
    }


    void PGP::InitPrikey(string prikey) {
        priArmored.assign(prikey);
        ImportSecretKey(prikey);
    }

    void PGP::InitPubkey(string pubkey) {
        pubArmored.assign(pubkey);
        ImportKey(pubkey);
    }


    // This provide enc, dec method
    PGP::PGP(string pubkey, string prikey) {
        InitPubkey(pubkey);
        InitPrikey(prikey);
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
        if (pass) {
            passphrase = strdup(pass);
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
        char buf[1024];
        unsigned int size = 0;
        string result = "";
        string datafname = tmpnam(NULL);
        ofstream datafile(datafname);
        datafile << ct;
        datafile.close();

        string command = "echo " + string(passphrase) + " | /usr/bin/gpg --armor --no-tty --passphrase-fd 0 --decrypt " + datafname + " 2>/dev/null";
        FILE* pipe = popen(command.c_str(), "r");
        do {
            size = fread(buf, 1, 1024, pipe);
            result += string(buf, size);
        } while(size == 1024);

        pclose(pipe);
        unlink(datafname.c_str());
        return result;
    }

    string PGP::Encrypt(string pt) {
        string datafname = tmpnam(NULL);
        ofstream datafile(datafname);
        datafile << pt;
        datafile.close();

        string command = "/usr/bin/gpg --armor --trust-model always -r " + passphrase_info + " --encrypt " + datafname;
        FILE* pipe = popen(command.c_str(), "r");
        pclose(pipe);

        string s = datafname + ".asc";
        ifstream encfile(s);
        string result((std::istreambuf_iterator<char>(encfile)),
                         std::istreambuf_iterator<char>());
        unlink(datafname.c_str());
        unlink(s.c_str());
        return result;
    }
}
