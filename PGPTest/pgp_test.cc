#include <fstream>
#include <string>
#include "../src/PGP.hh"
#include <unistd.h>

int main(){
    PGP::initGPG();
    auto PGP = new PGP::PGP("./pubkey", "./prikey");
    while(!PGP->Verify_Pass(getpass("Passpharse : ")));
    string cipher = PGP->Encrypt(string("This is test\x00hello world", 24));

    std::cout << "Cipher : " << cipher << "bb" << std::endl;
    std::cout << "Result : " << PGP->Decrypt(cipher) << "aa" << std::endl;
    std::cout << "uid : " << PGP->getUid() << std::endl;
    std::cout << "info : " << PGP->getPassInfo() << std::endl;

    return 0;
}
