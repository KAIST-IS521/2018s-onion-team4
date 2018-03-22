#include "cui.hh"

namespace CUI {
    CUIProvider::CUIProvider() {
    }

    CUIProvider::~CUIProvider() {
    }

    pair<string, string> CUIProvider::GetUserInfo(string uid, string infos,
                                                  char *msg) {
    }

    void CUIProvider::UserInputLoop(string uid, string keyid,
            void (*handler)(char *, void *), void *aux) {
    }

    void CUIProvider::PushMessage(char *msg) {
    }

    void CUIProvider::PushError(char *msg) {
    }
}
