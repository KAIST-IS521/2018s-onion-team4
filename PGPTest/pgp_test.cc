#include <fstream>
#include <string>
#include "../src/Utils/PGP.hh"
#include <unistd.h>

int main(){
    std::ifstream pri("./prikey");
    std::ifstream pub("./pubkey");
    string pubkey((std::istreambuf_iterator<char>(pub)),
                     std::istreambuf_iterator<char>());
    string prikey((std::istreambuf_iterator<char>(pri)),
                     std::istreambuf_iterator<char>());
    auto PGP = new PGP::PGP(pubkey, prikey);
    PGP->Verify_Pass((const char*)getpass(""));
    string cipher = PGP->Encrypt(string("This\nis test\x00hello world", 24));

    //std::cout << "Cipher : " << cipher << "bb" << std::endl;

    string plain = PGP->Decrypt(cipher);
    
    std::cout << "Plain : " << plain << "cc" << std::endl;

    return 0;
}
