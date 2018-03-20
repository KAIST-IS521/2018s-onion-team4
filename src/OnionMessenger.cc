#include "OnionMessenger.hh"
#include "ui/tui.hh"
#include <string>

namespace OnionMessenger {
    OnionMessenger::OnionMessenger(bool usetui, string priv) {
        if (usetui) {
           provider = new TUI::TUIProvider();
        } else {
            //provider = new CUIProvider();
        }
        pair<string, string> info = provider->GetUserInfo(NULL);
        string githubID = info.first;
        string pubkey = info.second;
    }
}
