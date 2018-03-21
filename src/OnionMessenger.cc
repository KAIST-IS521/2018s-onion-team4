#include "OnionMessenger.hh"
#include "ui/tui.hh"
#include <string>
#include <thread>
#include <unistd.h>
#include <ncurses.h>

namespace OnionMessenger {
    void OnionMessenger::LoginUser() {
        int idx = 0;
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
            sprintf(msg, "Wrong passphrase! (Try: %d/3)", ++idx);
        } while (!pgp->Verify_Pass(pass.c_str()));
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
        LoginUser();
    }
}
