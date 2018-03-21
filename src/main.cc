#include "OnionMessenger.hh"
#include "PGP.hh"
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <getopt.h>

using namespace std;
void usage(char *progname) {
    cerr << "Usage: TODO" << endl;
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
    auto onion =
        new OnionMessenger::OnionMessenger(useTUI, "./user.pub", "./user.asc");
}
