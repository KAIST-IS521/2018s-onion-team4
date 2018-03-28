#include "OnionMessenger.hh"
#include "UI/tui.hh"
#include "UI/cui.hh"
#include <algorithm>
#include <string>
#include <ncurses.h>
#include <unistd.h>
#include "Packet/Socket.hh"

namespace MessageHandler {
    void handleCLI(char *in, void *aux) {
        auto messenger = static_cast<OnionMessenger::OnionMessenger *>(aux);
        messenger->HandleCommand(in);
    }

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

    bool handleServer(Server *server, ReadCTX *ctx, void *aux) {
        Packet::Packet *packet = Packet::Unserialize(ctx);
        int ret = 1;
        if (packet->IsReady()) {
            switch (packet->GetType()) {
                case HANDSHAKE:
                    ret = handleHandShake(aux, packet);
                    break;
                case MSG:
                    handleMsg(aux, packet);
                    break;
                default:
                    ret = 0;
                    break;
            }
        }
        return ret;
    };
}

namespace OnionMessenger {
    // XXX:Reciever side logic
    bool OnionMessenger::RecvHandShake(Packet::HandShake *hs) {
        string id = hs->GetId();
        // assert no user
        bool find;
        if (users.find(id) == users.end()) {
            find = false;
            auto ipVec = hs->GetConnectedNodeIps();
            auto portVec = hs->GetConnectedNodePorts();
            auto ip = ipVec.begin();
            auto port = portVec.begin();
            for (;ip != ipVec.end(); ip++, port++) {
                for (auto u : users) {
                    if (u.second->GetIp() == *ip && u.second->GetPort() == *port) {
                        find = true;
                        break;
                    }
                }
                if (!find) {
                    HandShake(*ip, *port);
                }
            }
            auto tip = Socket::GetIPaddr(hs->GetFd());
            auto tport = hs->GetPort();

            // XXX: send my handshake packet to user;
            vector<uint32_t> cIps;
            vector<uint16_t> cPorts;

            for (auto u : users) {
                cIps.push_back(u.second->GetIp());
                cPorts.push_back(u.second->GetPort());
            }
            auto nhs = new Packet::HandShake(PORT, ID, cIps, cPorts, pgp->GetPub());
            SendPacket(nhs, hs->GetFd());
            provider->PushNotification("[*] New user :\t" + hs->GetId());
            auto user = new User::Rep(hs->GetPubKey(), hs->GetId(),
                                         tip, tport, hs->GetFd());
            users[hs->GetId()] = user;
            return true;
        }
        return true;
    }

    void OnionMessenger::Relay(Message::OnionLayer *msg) {
        string user = msg->GetNextDst();
        if (user == ID) {
            auto data = Message::Unserialize(pgp->Decrypt(msg->GetData()));
            switch (data->GetType()) {
                case Message::ONIONLAYER:
                    Relay(static_cast<Message::OnionLayer *>(data));
                    break;
                case Message::MSGLAYER:
                    HandleMessage(static_cast<Message::MsgLayer *>(data));
                    break;
                case Message::IMGLAYER:
                    HandleAArt(static_cast<Message::ImgLayer *>(data));
                    break;
                default:
                    // XXX: NOTREACHABLE
                    exit(0);
                    break;
            }
        } else if (users.find(user) != users.end()) {
            auto rep = users[user];
            auto pkt = new Packet::Msg(msg->GetData());
            SendPacket(pkt, rep->GetFd());
        }
        delete msg;
    }

    void OnionMessenger::HandleMessage(Message::MsgLayer *msg) {
        auto sender = msg->GetSender();
        auto text = msg->GetData();
        // TODO: ADD provider->PushChat(user, text);
        provider->PushChat(sender, text);
        delete msg;
    }

    void OnionMessenger::HandleAArt(Message::ImgLayer *msg) {
        auto sender = msg->GetSender();
        auto url = msg->GetUrl();
        provider->PushMessage(Features::DisplayAArt(url));
        delete msg;
    }

    void OnionMessenger::RecvMsgAsync(Packet::Msg *msg) {
        string ct = msg->GetCT();
        thread([this, ct](){
                auto msg = Message::Unserialize(pgp->Decrypt(ct));
                if (msg) {
                    switch (msg->GetType()) {
                        case Message::ONIONLAYER:
                            Relay(static_cast<Message::OnionLayer *>(msg));
                            break;
                        case Message::MSGLAYER:
                            HandleMessage(static_cast<Message::MsgLayer *>(msg));
                            break;
                        case Message::IMGLAYER:
                            HandleAArt(static_cast<Message::ImgLayer *>(msg));
                            break;
                        default:
                            // XXX: NOTREACHABLE
                            exit(0);
                            break;
                        }
                    }
            }).detach();
    }

    // XXX: Sender side logic
    void OnionMessenger::DoOnionRouting(Message::MsgBody *bd, User::Rep *rep) {
        thread([this, rep, bd]() {
                if(users.size() > 1){
                    Message::OnionLayer *layer = bd->AddLayer(rep);
                    User::Rep *nrep, *prev;
                    for(int i = 0; i < rand() % 5 + 1; i++) {//rand() % 10; i++) {
                        do{
                            auto item = users.begin();
                            advance(item, rand() % users.size());
                            nrep = (*item).second;
                        }while(prev == nrep);
                        layer = layer->AddLayer(nrep);
                        prev = nrep;
                    }
                    auto ser = layer->Serialize(nrep);
                    SendPacket(new Packet::Msg(ser), nrep->GetFd());
                    delete layer; // XXX: Last layer is not serialized
                }
                else{
                    auto ser = bd->Serialize(rep);
                    SendPacket(new Packet::Msg(ser), rep->GetFd());
                }
                }).detach();
    }

    bool OnionMessenger::SendMsgAsync(string msg, string user) {
        if (users.find(user) != users.end()) {
            auto rep = users[user];
            auto layer = new Message::MsgLayer(ID, msg);
            DoOnionRouting(layer, rep);
            return true;
        }
        provider->PushError("[*] No such User :\t" + user);
        return false;
    }

    bool OnionMessenger::SendImgAsync(string url, string user) {
        if (users.find(user) != users.end()) {
            auto rep = users[user];
            auto layer = new Message::ImgLayer(ID, url);
            DoOnionRouting(layer, rep);
            return true;
        }
        return false;
    }

    // XXX: Helper
    void OnionMessenger::HandShake(string ip, uint16_t port) {
        vector<uint32_t> cIps;
        vector<uint16_t> cPorts;

        for (auto u : users) {
            cIps.push_back(u.second->GetIp());
            cPorts.push_back(u.second->GetPort());
        }
        int fd = Socket::ConnectTo(port, ip);
        serverMutex.lock();
        ServerFDadd(server, fd);
        serverMutex.unlock();
        auto hs = new Packet::HandShake(PORT, ID, cIps, cPorts, pgp->GetPub());
        SendPacket(hs, fd);
    }

    void OnionMessenger::HandShake(uint32_t ip, uint16_t port) {
        vector<uint32_t> cIps;
        vector<uint16_t> cPorts;

        for (auto u : users) {
            cIps.push_back(u.second->GetIp());
            cPorts.push_back(u.second->GetPort());
        }
        int fd = Socket::ConnectTo(port, ip);
        serverMutex.lock();
        ServerFDadd(server, fd);
        serverMutex.unlock();
        auto hs = new Packet::HandShake(PORT, ID, cIps, cPorts, pgp->GetPub());
        SendPacket(hs, fd);
    }

    void OnionMessenger::SendPacket(Packet::Packet *packet, int fd) {
        serverMutex.lock();
        packet->SendFd(server, fd);
        serverMutex.unlock();
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
        else {
            cmd = input;
        }

        if (!cmd.compare("/msg") || !cmd.compare("/m")) {
            SendMsgAsync(input, id);
            // Push sended message to client side
        } else if(!cmd.compare("/img") || !cmd.compare("/i")) {
            SendImgAsync(input, id);
            // Push sended message to client side
        } else if(!cmd.compare("/help") || !cmd.compare("/h")) {
            Help();
        } else if(!cmd.compare("/list") || !cmd.compare("/l")) {
            List();
        } else {
            auto err = ("[*] Unknown Command :\t" + string(msg));
            provider->PushError((char *)err.c_str());
        }
    }
    void OnionMessenger::Help(void) {
        provider->PushNotification("Usage: /[COMMAND] {[TARGET] [MSG || [URL]}\n");
        
        provider->PushNotification("COMMAND");
        provider->PushNotification("  msg,\tm : Send message to other");
        provider->PushNotification("  img,\ti : Send asciiart image to other");
        provider->PushNotification("  list,\tl : List online users");
        provider->PushNotification("  help,\th : See this information\n");

        provider->PushNotification("Example");
        provider->PushNotification("  /msg gildong Hello");
        provider->PushNotification("  /i simsim2 http://cfile28.uf.tistory.com/image/176C5E494E0B556E043D7F\n");
    }
    
    void OnionMessenger::List(void) {
        provider->PushNotification("Online");
        provider->PushNotification("--------------------------------");
        provider->PushNotification(ID + " [ me ]");
        for (auto u : users) {
            provider->PushNotification(u.first);            
        }
    }
    
    // XXX: Init
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

    OnionMessenger::OnionMessenger(bool usetui, string priv,
                                   string pub, uint16_t _port) {
        srand(time(nullptr));
        PORT = _port;
        cout << "Initalizing private key...";
        cout.flush();
        pgp = new PGP::PGP(pub, priv);
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
        server = newServer(PORT, MessageHandler::handleServer, this);
        serverTh = new thread(ServerLoop, server);
    }

    void OnionMessenger::Loop(void) {
        provider->UserInputLoop(ID, pgp->getPassInfo().substr(0, 8),
                                MessageHandler::handleCLI, this);
    }
}
