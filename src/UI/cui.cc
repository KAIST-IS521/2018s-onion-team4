#include <cstdlib>
#include <string>
#include <unistd.h>
#include "cui.hh"

namespace CUIHelper {
    void initialize(string uid, string infos, char *msg) {
        cout << "Please enter the Github ID and user information to unlock"
             << endl << "the OpenPGP secret key for user:" << endl
             << "\"" << uid << "\", " << infos << endl;
        if (msg)
            cout << msg << endl;
    }

    string getID(void) {
        string input_id = "";
        do {
            cout << "Enter the Github Id:";
            cout.flush();
            getline(cin, input_id);
        } while (input_id.empty() || input_id.length() > MAX_ID_LEN);

        return input_id;
    }

    string getPW(void)  {
        char* input_pw;
        input_pw = getpass("Enter Passphrase: ");
        return input_pw;
    }
}

namespace CUI {
    CUIProvider::CUIProvider() {
    }

    CUIProvider::~CUIProvider() {
    }

    pair<string, string> CUIProvider::GetUserInfo(string uid, string infos,
            char *msg) {
        CUIHelper::initialize(uid, infos, msg);

        string id = CUIHelper::getID();
        string pw = CUIHelper::getPW();

        return pair<string, string>(id, pw);
    }

    void CUIProvider::UserInputLoop(string uid, string keyid,
            void (*handler)(char *, void *), void *aux) {
        while (true) {
            string input;
            string prompt = uid + " @ [" + keyid + "]: ";
            cout << prompt;
            getline(cin, input);
            cout.flush();
            if (!input.empty()) {
                handler((char *)input.c_str(), aux);
            }
        }
    }

    void CUIProvider::PushMessage(char *msg) {
        lock.lock();
        cout << msg << endl;
        lock.unlock();
    }

    void CUIProvider::PushError(string msg) {
        lock.lock();
        cout << msg << endl;
        lock.unlock();
    }

    void CUIProvider::PushChat(string sender, string msg) {
        lock.lock();
        cout << "[ " << sender << " ]\t" << msg << endl;
        lock.unlock();
    }

    void CUIProvider::PushNotification(string msg) {
        lock.lock();
        cout << msg << endl;
        lock.unlock();
    }

    void CUIProvider::Clear(void) {
        system("/usr/bin/clear");
    }
}
