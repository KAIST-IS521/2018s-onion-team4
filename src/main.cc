#include "OnionMessenger.hh"
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>

FILE* log_f;
bool debug = false;
void d(string s){
    if (debug) {
        fwrite(s.c_str(), s.size(), 1, log_f);
        fflush(log_f);
    }
}

using namespace std;
void usage(char *progname) {
    cout << "Usage: " << progname << " [Options]" << endl
         << endl
         << "Required arguments" << endl
         << "  --priv, -s : Private key file" << endl
         << "  --pub,  -b : Public key file" << endl
         << "  --port, -p : Server listen port" << endl
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
    uint16_t nport;
    uint16_t port;
    size_t token;
    static struct option long_options[] =
    {
        {"notui", no_argument, &useTUI, 0},
        {"node",  required_argument, 0, 'n'},
        {"priv",  required_argument, 0, 's'},
        {"pub",   required_argument, 0, 'b'},
        {"port",  required_argument, 0, 'p'},
        {"help",  no_argument, 0, 'h'},
        {"debug", no_argument, 0, 'g'},
        {0, 0, 0, 0}
    };
    while (-1 != (c = getopt_long(argc, argv, "n:s:b:p:h:g",
                                  long_options, NULL))) {
        switch (c) {
            case 0:
                break;
            case 'n':
                node = optarg;
                token = node.find(":");
                if (token != string::npos) {
                    nport = (uint16_t)stoi(node.substr(token+1));
                    node = node.substr(0, token);
                } else {
                    nport = 1234;
                }
                break;
            case 's':
                priv = optarg;
                break;
            case 'b':
                pub = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'g':
                debug = true;
                break;
            case 'h':
            case '?':
                usage(argv[0]);
                break;
        }
    }
    ifstream privkey(priv);
    ifstream pubkey(pub);

    if (!port) {
        cerr << argv[0] << " : port argument is not supplied." << endl;
        exit(1);
    } else if (priv.empty() || pub.empty()) {
        usage(argv[0]);
    } else if (!privkey.is_open()) {
        cerr << argv[0] << ": " << priv << " : No such file." << endl;
        exit(1);
    } else if (!pubkey.is_open()) {
        cerr << argv[0] << ": " << priv << " : No such file." << endl;
        exit(1);
    }
    if (debug) {
        log_f = fopen("./log", "a+");
    }
    string privData((std::istreambuf_iterator<char>(privkey)),
                     std::istreambuf_iterator<char>());
    string pubData((std::istreambuf_iterator<char>(pubkey)),
                     std::istreambuf_iterator<char>());
    auto onion =
        new OnionMessenger::OnionMessenger(useTUI, privData, pubData, port);
    if (!node.empty()) {
        onion->HandShake(node, nport);
    }
    onion->Loop();
}
