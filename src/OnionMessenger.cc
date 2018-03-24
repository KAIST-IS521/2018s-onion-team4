#include "OnionMessenger.hh"
#include "ui/tui.hh"
#include "ui/cui.hh"
#include <algorithm>
#include <string>

namespace MessageHandler {
    bool handleHandShake(void *aux, Packet::Packet *packet) {
        auto messenger = static_cast<OnionMessenger::OnionMessenger *>(aux);
        auto handshake = static_cast<Packet::HandShake *>(packet);
        return messenger->RecvHandShake(handshake);
    }

    void handleMsg(void *aux, Packet::Packet *packet) {
        auto messenger = static_cast<OnionMessenger::OnionMessenger *>(aux);
        auto msg = static_cast<Packet::Msg *>(packet);
        messenger->RecvMsgAsync(msg);
    }

    void handleImg(void *aux, Packet::Packet *packet) {
        auto messenger = static_cast<OnionMessenger::OnionMessenger *>(aux);
        auto img = static_cast<Packet::Img *>(packet);
        messenger->RecvImageAsync(img);
    }
}

namespace OnionMessenger {
    bool handleServer(Server *server, ReadCTX *ctx, void *aux) {
        Packet::Packet *packet = Packet::Unserialize(ctx);
        int ret = 1;
        if (packet->IsReady()) {
            switch (packet->GetType()) {
                case HANDSHAKE:
                    ret = MessageHandler::handleHandShake(aux, packet);
                    break;
                case MSG:
                    MessageHandler::handleMsg(aux, packet);
                    break;
                case IMG:
                    MessageHandler::handleImg(aux, packet);
                    break;
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

    UserRepresentation::UserRepresentation(string pubkey, string _id,
                                           int _ip, int _fd) {
        pgp = new PGP::PGP(pubkey);
        id.assign(_id);
        ip = _ip;
        fd = _fd;
    }

    UserRepresentation::~UserRepresentation() {
        delete pgp;
    }

    void OnionMessenger::RecvMsgAsync(Packet::Msg *msg) {
        string ct = msg->GetMessage();
        auto future = async([this, &ct] ()
                { provider->PushMessage(pgp->Decrypt(ct)); });
        futureMutex.lock();
        futures.push_back(move(future));
        futureMutex.unlock();
    }

    void OnionMessenger::RecvImageAsync(Packet::Img *img) {
        string ct = img->GetUrl();
        auto future = async([this, &ct] ()
                { auto img = Features::DisplayAArt(pgp->Decrypt(ct));
                  provider->PushMessage(img); });
        futureMutex.lock();
        futures.push_back(move(future));
        futureMutex.unlock();
    }

    void OnionMessenger::SendPacket(Packet::Packet *packet, int fd) {
        serverMutex.lock();
        packet->SendFd(server, fd);
        serverMutex.unlock();
    }

    bool OnionMessenger::SendMsgAsync(string msg, string user) {
        if (users.find(user) != users.end()) {
            auto rep = users[user];
            auto future = async([this, rep, &msg] ()
                    { auto packet = new Packet::Msg(rep->Encrypt(msg));
                      SendPacket(packet, rep->GetFd()); });
            futureMutex.lock();
            futures.push_back(move(future));
            futureMutex.unlock();
            return true;
        }
        return false;
    }

    bool OnionMessenger::SendImageAsync(string img, string user) {
        if (users.find(user) != users.end()) {
            auto rep = users[user];
            auto future = async([this, rep, &img] ()
                    { auto packet = new Packet::Img(rep->Encrypt(img));
                      SendPacket(packet, rep->GetFd()); });
            futureMutex.lock();
            futures.push_back(move(future));
            futureMutex.unlock();
            return true;
        }
        return false;
    }

    bool OnionMessenger::RecvHandShake(Packet::HandShake *hs) {
        string id = hs->GetId();
        // assert no user
        bool find;
        if (users.find(id) == users.end()) {
            find = false;
            for (auto s : hs->GetConnectedNodes()) {
                for (auto u : users) {
                    if (u.second->GetIp() == s) {
                        find = true;
                        break;
                    }
                }
                if (!find) {
                    HandShake(s);
                }
            }
            auto ip = Socket::GetIPaddr(hs->GetFd());
            auto user = new UserRepresentation(hs->GetId(), hs->GetPubKey(),
                                               ip, hs->GetFd());
            users[hs->GetId()] = user;
            return true;
        }
        return false;
    }

    void OnionMessenger::HandShake(string ip) {
        vector<uint32_t> cNodes;

        for (auto u : users) {
            cNodes.push_back(u.second->GetIp());
        }
        int fd = Socket::ConnectTo(PORT, ip);
        // Not a thread-safe add lock!
        serverMutex.lock();
        ServerFDadd(server, fd);
        serverMutex.unlock();
        auto hs = new Packet::HandShake(ID, cNodes, pgp->GetPub());
        SendPacket(hs, fd);
    }

    void OnionMessenger::HandShake(uint32_t ip) {
        vector<uint32_t> cNodes;

        for (auto u : users) {
            cNodes.push_back(u.second->GetIp());
        }
        int fd = Socket::ConnectTo(PORT, ip);
        serverMutex.lock();
        ServerFDadd(server, fd);
        serverMutex.unlock();
        auto hs = new Packet::HandShake(ID, cNodes, pgp->GetPub());
        SendPacket(hs, fd);
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
            provider = new CUI::CUIProvider();
        }
        ID = LoginUser();
        InitServer();
    }

    void OnionMessenger::InitServer(void) {
        server = newServer(PORT, handleServer, this);
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
            SendMsgAsync(input, id);
        } else if(!cmd.compare("/image")) {
            SendImageAsync(input, id);
        } else {
            auto err = ("Unknown Command: " + string(msg));
            provider->PushMessage((char *)err.c_str());
        }
    }

    void OnionMessenger::Loop(void) {
        provider->UserInputLoop(ID, pgp->getPassInfo().substr(0, 8),
                                handleCLI, this);
    }
}
