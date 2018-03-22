#include "OnionMessenger.hh"
#include "ui/tui.hh"
#include <string>
#include <unistd.h>

namespace OnionMessenger {

    bool handleServer(Server *server, ReadCTX *ctx) {
        return 1;
    };

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

    void OnionMessenger::InitServer(void) {
        server = newServer(1234, handleServer);
        serverTh = new thread(ServerLoop, server);
    }

    void OnionMessenger::handleCommand(char *msg) {
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
          this->Imagehandle(input)
        } else {
            auto err = ("Unknown Command: " + string(msg));
            provider->PushMessage((char *)err.c_str());
        }
    }


    //mdkir ~/go
    //export GOPATH=~/go
    //export PATH=$PATH:$GOPATH/bin
    //go get github.com/zyxar/image2ascii
    //image2ascii [image_file]
    void OnionMessenger::Imagehandle(string url){
      pid_t childpid;
      int status;
      char* imagepath;
      imagepath="image.jpg";
      char * urls = new char[str.size() + 1];
      std::copy(str.begin(), str.end(), urls);
      urls[str.size()] = '\0';
      childpid = fork();
      if(childpid == -1)
      {
      }
      else if(childpid == 0)
      {
           execlp("wget","wget","-q",urls,"-O",imagepath,NULL);
      }
      else
      {
        wait(&status);
        childpid = fork();
        if(childpid == -1)
        {
        }
        else if(childpid == 0)
        {
             execlp("image2ascii","image2ascii",imagepath,NULL);
        }
        else
        {
          wait(&status);
        }
      }
    }

    void OnionMessenger::Loop(void) {
        InitServer();
        provider->UserInputLoop(ID, pgp->getPassInfo().substr(0, 8),
                handleCLI, this);
    }
}
