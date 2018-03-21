#include "OnionMessenger.hh"
#include "ui/tui.hh"
#include <string>
#include <thread>
#include <unistd.h>
#include <ncurses.h>

namespace OnionMessenger {
    void handleCLI(char *in, void *aux) {
        auto messenger = static_cast<OnionMessenger *>(aux);
        messenger->handleCommand(in);
    }

    string OnionMessenger::LoginUser(void) {
        int idx = 1;
        char msg[30] = {0, };
        string githubID;
        string pass;
        do {
            if (idx > 3) {
                exit(0);
            }
            pair<string, string> info =
                provider->GetUserInfo(pgp->getUid(), pgp->getPassInfo(), msg);
            githubID = info.first;
            pass = info.second;
            sprintf(msg, "Bad passphrase (try %d of 3)", idx++);
        } while (!pgp->Verify_Pass(pass.c_str()));
        return githubID;
    }

    OnionMessenger::OnionMessenger(bool usetui, string priv, string pub) {
        cout << "Initalizing private key...";
        cout.flush();
        pgp = new PGP::PGP(priv, pub);
        cout << " Done." << endl;

        if (usetui) {
           provider = new TUI::TUIProvider();
        } else {
            //provider = new CUIProvider();
        }
        ID = LoginUser();
    }

    void OnionMessenger::handleCommand(char *msg) {
    }

    void OnionMessenger::Loop(void) {
        // TODO: Launch server thread here.
        provider->UserInputLoop(ID, pgp->getPassInfo().substr(0, 8),
                handleCLI, provider);
    }

}
