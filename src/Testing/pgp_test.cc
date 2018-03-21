#include <fstream>
#include <string>
#include "../PGP.hh"

int main(){
	auto PGP = new PGP::PGP("./pubkey", "./prikey");

  char* plain = "This is plain text\n";
  char* cipher = PGP->Encrypt(plain);

  std::cout << "Cipher : " << cipher << "bb" << std::endl;
  std::cout << "Result : " << PGP->Decrypt(cipher) << "aa" << std::endl;

  return 0;
}
