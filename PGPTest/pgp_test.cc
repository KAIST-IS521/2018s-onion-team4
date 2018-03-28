#include <fstream>
#include <string>
#include "../src/PGP.hh"
#include <unistd.h>

int main(){
    PGP::initGPG();
    std::ifstream pri("./prikey");
    std::ifstream pub("./pubkey");
    string pubkey((std::istreambuf_iterator<char>(pub)),
                     std::istreambuf_iterator<char>());
    string prikey((std::istreambuf_iterator<char>(pri)),
                     std::istreambuf_iterator<char>());
    auto PGP = new PGP::PGP(pubkey);
    string cipher = PGP->Encrypt(string("This is test\x00hello world", 24));

    std::cout << "Cipher : " << cipher << "bb" << std::endl;

    return 0;
}
