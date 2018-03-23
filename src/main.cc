#include "OnionMessenger.hh"
#include "PGP.hh"
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>

using namespace std;
void usage(char *progname) {
    cout << "Usage: " << progname << " [Options]" << endl
         << endl
         << "Required arguments" << endl
         << "  --priv, -s : Private key file" << endl
         << "  --pub,  -p : Public key file" << endl
         << endl
         << "Optional arguments" << endl
         << "  --notui    : Not use TUI" << endl
         << "  --node, -n : Another client's IP" << endl
         << "  --help, -h : Show this message" << endl
         << endl;
    exit(0);
}

int main(int argc, char **argv) {
    int useTUI = true;
    string node;
    string priv;
    string pub;
    int c;
    static struct option long_options[] =
    {
        {"notui", no_argument, &useTUI, 0},
        {"node",  required_argument, 0, 'n'},
        {"priv",  required_argument, 0, 's'},
        {"pub",   required_argument, 0, 'p'},
        {"help",  no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    while (-1 != (c = getopt_long(argc, argv, "n:s:p:h", long_options, NULL))) {
        switch (c) {
            case 0:
                break;
            case 'n':
                node = optarg;
                break;
            case 's':
                priv = optarg;
                break;
            case 'p':
                pub = optarg;
                break;
            case 'h':
            case '?':
                usage(argv[0]);
                break;
        }
    }
    ifstream privkey(priv);
    ifstream pubkey(pub);

    if (priv.empty() || pub.empty()) {
        usage(argv[0]);
    } else if (!privkey.is_open()) {
        cerr << argv[0] << ": " << priv << " : No such file.";
    } else if (!pubkey.is_open()) {
        cerr << argv[0] << ": " << priv << " : No such file.";
    }
    string privData((std::istreambuf_iterator<char>(privkey)),
                     std::istreambuf_iterator<char>());
    string pubData((std::istreambuf_iterator<char>(pubkey)),
                     std::istreambuf_iterator<char>());
    PGP::initGPG();

    auto onion =
        new OnionMessenger::OnionMessenger(useTUI, privData, pubData);
    onion->Loop();
}
