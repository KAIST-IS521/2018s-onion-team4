#include <cstdlib>
#include <string>
#include "cui.hh"

namespace CUIHelper {

	void initialize(string uid, string infos, char *msg) {
		cout << "Please enter the Github ID and user information to unlock" << endl;
		cout << "the OpenPGP secret key for user:" << endl;
		cout << "\"" << uid << "\", " << infos << endl;
		if(msg)
			cout << msg << endl;
	}

	string getID() {
		string input_id = "";
		do {
			cout << "Enter the Github Id:";
			getline(cin, input_id);
            cout.flush();
		} while (input_id.empty() || input_id.length() > MAX_ID_LEN);

		return input_id;
	}

	string getPw()  {
		char* input_pw = "";
		do {
            input_pw = getpass("Enter Passphrase: ");
		} while (strlen(input_pw) <= 0);

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
		string pw = CUIHelper::getPw();

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
			if(input)
				handler(input.c_str(), aux);
		}
    }

    void CUIProvider::PushMessage(char *msg) {
		lock.lock();
		cout << msg << endl;
		lock.unlock();
    }

    void CUIProvider::PushError(char *msg) {
		lock.lock();
		cout << msg << endl;
		lock.unlock();
    }
}
