#include "OnionMessenger.hh"
#include "ui/tui.hh"
#include <string>
#include <stdio.h>
#include <curl/curl.h>
#include <unistd.h>

namespace OnionMessenger {
    size_t callbackfunction(void *ptr, size_t size, size_t nmemb, void* userdata)
    {
        FILE* stream = (FILE*)userdata;
        if (!stream){
            //no stream
            return 0;
          }

          size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
          return written;
    }

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
          this->Downloadimag(input);
        } else {
            auto err = ("Unknown Command: " + string(msg));
            provider->PushMessage((char *)err.c_str());
        }
    }

    void OnionMessenger::Downloadimag(string urls)
    {
        char *filepath;
        filepath="out.jpg";
        FILE* fp = fopen(filepath, "wb");
        char *url=&urls[0];
        if (!fp)
        {
            //file open error
            return false;
        }

        CURL* curlCtx = curl_easy_init();
        curl_easy_setopt(curlCtx, CURLOPT_URL, url);
        curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
        curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

        CURLcode rc = curl_easy_perform(curlCtx);
        if (rc)
        {
            //failed to download
            return false;
        }

        long res_code = 0;
        curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
        if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
        {
            //page error
        }

        curl_easy_cleanup(curlCtx);

        fclose(fp);
        this.Asciiart(filepath);
    }

    void OnionMessenger::Asciiart(char *filepath){
      pid_t pid;
      int status;

      pid = fork();
      if (pid == -1) { /* Error occured, Exception required */ }
      else if(pid == 0) { // child
        execlp("image2ascii", "image2ascii", filepath, NULL); //docker
      }
      // parent
      wait(&status);

  // Done
  // verification is required
}

    void OnionMessenger::Loop(void) {
        InitServer();
        provider->UserInputLoop(ID, pgp->getPassInfo().substr(0, 8),
                handleCLI, this);
    }
}
