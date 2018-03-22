#include <fstream>
#include <string>
#include "../src/PGP.hh"

int main(){
	auto PGP = new PGP::PGP("./pubkey", "./prikey");

  string cipher = PGP->Encrypt(string("This is test\x00hello world", 24));

  std::cout << "Cipher : " << cipher << "bb" << std::endl;
  std::cout << "Result : " << PGP->Decrypt(cipher) << "aa" << std::endl;

  return 0;
}
