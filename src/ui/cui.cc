#include <cstdlib>
#include <string>
#include "cui.hh"

namespace CUIHelper {

	void initialize(string uid, string infos, char *msg) {
		cout << "Please enter the Github ID and user information to unlock" << endl;
		cout << "the OpenPGP secret key for user:" << endl;
		cout << "\"" << uid << "\", " << infos << endl;
		cout << msg << endl;
	}

	string getID() {
		string input_id == "";
		do {
			cout << "Enter the Github Id:";
			getline(cin, input_id);
		} while (input_id.empty() || input_id.length()) > MAX_ID_LEN);

		return input_id;
	}

	string getPw()  {
		string input_pw == "";
		do {
			cout << "Enter Passphrase: " ;
			getline(cin, input_pw);
		} while (input_pw.empty());

		return input_pw;
	}

	string getInput() {

	}

}

namespace CUI {
    CUIProvider::CUIProvider() {
		ios::sync_with_stdio(false);
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
    }

    void CUIProvider::PushMessage(char *msg) {
		lock.lock();
		cout << msg;
		lock.unlock();
    }

    void CUIProvider::PushError(char *msg) {
		lock.lock();
		cout << msg;
		lock.unlock();
    }
}
