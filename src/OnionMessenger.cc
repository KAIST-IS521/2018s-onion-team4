#include "OnionMessenger.hh"
#include "ui/tui.hh"
#include <algorithm>
#include <string>

namespace MessageHandler {
    void handleHandShake(void *aux, Packet::Packet *packet) {
        auto messenger = static_cast<OnionMessenger::OnionMessenger *>(aux);
        auto handshake = static_cast<Packet::HandShake *>(packet);
        messenger->HandleAsync(handshake);
    }

    void handleMsg(void *aux, Packet::Packet *packet) {
        auto messenger = static_cast<OnionMessenger::OnionMessenger *>(aux);
        auto msg = static_cast<Packet::Msg *>(packet);
        messenger->HandleAsync(msg);
    }
}
namespace OnionMessenger {

    bool handleServer(Server *server, ReadCTX *ctx, void *aux) {
        Packet::Packet *packet = Packet::Unserialize(ctx);
        int ret = 1;
        if (packet->isReady()) {
            switch (packet->getType()) {
                case HANDSHAKE:
                    MessageHandler::handleHandShake(aux, packet);
                    break;
                case MSG:
                    MessageHandler::handleMsg(aux, packet);
                    break;
                //case IMG:
                // TODO
                default:
                    ret = 0;
                    break;
            }
        }
        return ret;
    };

    void handleCLI(char *in, void *aux) {
        auto messenger = static_cast<OnionMessenger *>(aux);
        messenger->HandleCommand(in);
    }

    void OnionMessenger::HandleAsync(Packet::Msg *msg) {
        string ct(msg->GetMessage(), msg->GetMessageLength());
        auto future = async([this, &ct] ()
                { provider->PushMessage(pgp->Decrypt(ct)); });
        futureMutex.lock();
        futures.push_back(move(future));
        futureMutex.unlock();
    }

    void OnionMessenger::HandleAsync(Packet::HandShake *hs) {
        // TODO
    }

    void OnionMessenger::CleanFuture(void) {
        futureMutex.lock();
        auto filt =
            [](const future<void>& f)
            { return f.wait_for(chrono::seconds(0)) == future_status::ready; };
        futures.erase(remove_if(futures.begin(), futures.end(), filt));
        futureMutex.unlock();
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

    void OnionMessenger::InitServer(void) {
        server = newServer(1234, handleServer, this);
        serverTh = new thread(ServerLoop, server);
    }

    void OnionMessenger::HandleCommand(char *msg) {
        if (!msg) return;
        string input;
        string cmd(""), id("");
        input.assign(msg);
        auto nptr = input.find(" ");
        if (nptr != string::npos) {
            cmd = input.substr(0, nptr);
            input = input.substr(nptr+1);
            nptr = input.find(" ");
            if (nptr != string::npos) {
                id = input.substr(0, nptr);
                input = input.substr(nptr + 1);
            }
        }

        if (!cmd.compare("/msg")) {
            // TODO: handle msg
        } else if(!cmd.compare("/image")) {
            // TODO: handle image
        } else if(!cmd.compare("/music")) {
            //TODO:handle music
        } else {
            auto err = ("Unknown Command: " + string(msg));
            provider->PushMessage((char *)err.c_str());
        }
    }

    void OnionMessenger::Loop(void) {
        InitServer();
        provider->UserInputLoop(ID, pgp->getPassInfo().substr(0, 8),
                handleCLI, this);
    }
}
