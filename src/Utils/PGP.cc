#include "PGP.hh"
#include <unistd.h>
#include <string.h>
#include <streambuf>
#include <assert.h>
#include <fstream>

namespace PGP {
    string GetPathFromFd(int fd){
        char keyfname[TEMP_LEN];
        string linkpath = "/proc/self/fd/" + to_string(fd);
        if(readlink(linkpath.c_str(), keyfname, TEMP_LEN) != -1)
            return string(keyfname);
        return NULL;
    }

    void PGP::ImportKey(string key){
        char temp[] = "/tmp/dataXXXXXX";
        char buf[1024] = "";
        int keyfd = mkstemp(temp);
        write(keyfd, key.c_str(), key.size());
        string path = GetPathFromFd(keyfd);
        close(keyfd);

        string command = "/usr/bin/gpg --import " + path + " 2>&1";
        FILE* pipe = popen(command.c_str(), "r");
        
        do{
            fgets(buf, 1024, pipe);
        }while(strstr(buf, "key ") == NULL);
        string s = string(buf);

        s = s.substr(s.find("key "));
        string info = s.substr(4, 8);

        s = s.substr(s.find('\"'));
        string id = s.substr(1, s.find('\"', 1)-1);

        pclose(pipe);
        setPassInfo(info);
        setUid(id);

        remove(path.c_str());
    }

    void PGP::ImportSecretKey(string key){
        char temp[] = "/tmp/dataXXXXXX";
        int keyfd = mkstemp(temp);
        write(keyfd, key.c_str(), key.size());
        string path = GetPathFromFd(keyfd);
        close(keyfd);

        string command = "/usr/bin/gpg --import " + path + " 2>&1";
        FILE* pipe = popen(command.c_str(), "r");

        pclose(pipe);
        remove(path.c_str());
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
        char temp[] = "/tmp/dataXXXXXX";
        char buf[1024];
        unsigned int size = 0;
        int datafd = mkstemp(temp);
        write(datafd, ct.c_str(), ct.size());
        string path = GetPathFromFd(datafd);
        close(datafd);

        string result = "";
        string command = "echo " + string(passphrase) + " | /usr/bin/gpg --armor --no-tty --passphrase-fd 0 --decrypt " + path + " 2>/dev/null";
        FILE* pipe = popen(command.c_str(), "r");
        do {
            size = fread(buf, 1, 1024, pipe);
            result += string(buf, size);
        } while(size == 1024);

        pclose(pipe);
        remove(path.c_str());
        return result;
    }

    string PGP::Encrypt(string pt) {
        char temp[] = "/tmp/dataXXXXXX";
        int datafd = mkstemp(temp);
        write(datafd, pt.c_str(), pt.size());
        string path = GetPathFromFd(datafd);
        close(datafd);

        string command = "/usr/bin/gpg --armor --trust-model always -r " + passphrase_info + " --encrypt " + path;
        FILE* pipe = popen(command.c_str(), "r");
        pclose(pipe);

        string s = path + ".asc";
        ifstream encfile(s);
        string result((std::istreambuf_iterator<char>(encfile)),
                         std::istreambuf_iterator<char>());
        remove(path.c_str());
        remove(s.c_str());
        return result;
    }
}
