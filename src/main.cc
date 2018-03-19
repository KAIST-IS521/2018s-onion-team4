#include "onion.hh"

int main(int argc, char **argv) {
    // Parse Opt here. --node [nodeip] --prikey [prikey.pub]
    Onion *onion = new Onion();
    onion->Loop();
}
