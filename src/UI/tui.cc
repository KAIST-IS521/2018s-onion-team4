#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ncurses.h>
#include "tui.hh"

namespace TUIImplement {
    struct inputForm
    {
        char *id;
        char *pass;
    };

    typedef enum {INPUT_ID, INPUT_PASS, INPUT_OK, INPUT_CANCEL} startInputFormState;

    const char *prompt = "[Onion] ";

    WINDOW *mainWin;
    static void colors() {
       start_color();
       use_default_colors();
       init_pair(1, -1, -1);
       init_pair(2, COLOR_CYAN, -1);
       init_pair(3, COLOR_YELLOW, -1);
       init_pair(4, COLOR_RED, -1);
       init_pair(5, COLOR_BLUE, -1);
       init_pair(6, COLOR_MAGENTA, -1);
       init_pair(7, COLOR_GREEN, -1);
       init_pair(8, COLOR_WHITE, COLOR_RED);
       init_pair(9, COLOR_WHITE, COLOR_BLUE);
       init_pair(11, -1, COLOR_CYAN);
    }

    static void end() {
        endwin();
    }

    void initUI(void) {
        if ((mainWin = initscr()) == NULL) {
            exit(0);
        }
        noecho();
        cbreak();
        keypad(mainWin, TRUE);
        colors();
        atexit(end);
    }

    WINDOW *LogoWin, *passWin, *okWin, *cancelWin, *infoWin, *infoWinBox, *idWin;
    static void drawLogo(void) {
       LogoWin = subwin(mainWin, LINES/2, COLS, 0, 0);
       mvwaddstr(LogoWin, LINES/2 - 9, COLS/2 - 30,
               "   ___                   _ _ _");
       mvwaddstr(LogoWin, LINES/2 - 8, COLS/2 - 30,
               "  /  ()     o           ( / ) )");
       mvwaddstr(LogoWin, LINES/2 - 7, COLS/2 - 30,
               " /   /_ _  ,  __ _ _     / / / _  (   (   _  _ _   _,  _  _");
       mvwaddstr(LogoWin, LINES/2 - 6, COLS/2 - 30,
               "(___// / /_(_(_)/ / /_  / / (_(/_/_)_/_)_(/_/ / /_(_)_(/_/ (_");
       mvwaddstr(LogoWin, LINES/2 - 5, COLS/2 - 30,
               "                                                   /|");
       mvwaddstr(LogoWin, LINES/2 - 4, COLS/2 - 30,
               "                                                  (/");
       mvwaddstr(LogoWin, LINES/2 - 3, COLS/2 - 30,
               "                                         -by IS521-Team4-VIM");
       wrefresh(LogoWin);
    }

    static void inputBoxBuilder(char *id, int passlen, int state) {
        // Build id string
        for (int i = 0; i < 30; i++) {
            mvwaddstr(idWin, 0, 18+i, "_");
        }
        wrefresh(idWin);
        if (id) {
            mvwaddstr(idWin, 0, 18, id);
            wrefresh(idWin);
        }

        // Build pw string
        for (int i = 0; i < 50; i++) {
            mvwaddstr(passWin, 0, 18+i, i < passlen ? "*" : "_");
        }
        wrefresh(passWin);

        WINDOW *t = NULL;
        if (state == INPUT_OK) {
            t = okWin;
        } else if (state == INPUT_CANCEL) {
            t = cancelWin;
        }

        if (t) wattron(t, COLOR_PAIR(11));
        mvwaddstr(okWin, 0, 16, "<OK>");
        mvwaddstr(cancelWin, 0, 14, "<Cancel>");
        if (t) wcursyncup(t);
        if (t) wattroff(t, COLOR_PAIR(11));
        wrefresh(okWin);
        wrefresh(cancelWin);

        if (state == INPUT_ID) {
            wmove(idWin, 0, id ? 18 + strlen(id) : 18);
            wcursyncup(idWin);
            wrefresh(idWin);
        } else if (state == INPUT_PASS) {
            wmove(passWin, 0, 18 + passlen);
            wcursyncup(passWin);
            wrefresh(passWin);
        }
    }

    static void drawInputFormBox(void) {
        infoWinBox = subwin(mainWin, 12, 72, LINES/2+1, COLS/2 - 36);
        infoWin = subwin(mainWin, 10, 68, LINES/2+2, COLS/2 - 35);
        box(infoWinBox, 0, 0);
        wrefresh(infoWinBox);
        wrefresh(infoWin);
    }

    static struct inputForm *handleUserInfo() {
        int state = INPUT_ID;

        int idlen = 0;
        char *id = 0;

        int passlen = 0;
        char *pass = 0;
        int ch;

        inputBoxBuilder(id, 0, state);
        while ((ch = getch()) != '\n') {
            // Backspace
            if (ch == 8 || ch == 127 || ch == KEY_BACKSPACE) {
                if (state == INPUT_ID) {
                    if (idlen > 0) {
                        id = (char *)realloc(id, --idlen + 1);
                        id[idlen] = 0;
                    }
                } else if (state == INPUT_PASS) {
                    if (passlen > 0) {
                        pass = (char *)realloc(pass, --passlen + 1);
                        pass[passlen] = 0;
                    }
                }
            } else if (ch == KEY_RESIZE) {
                continue;
            } else if (ch == KEY_LEFT) {
                if (state == INPUT_CANCEL) {
                    state = INPUT_OK;
                } else if (state == INPUT_OK) {
                    state = INPUT_PASS;
                } else if (state == INPUT_PASS) {
                    wcursyncup(idWin);
                    state = INPUT_ID;
                }
            } else if (ch == KEY_RIGHT) {
                if (state == INPUT_OK) {
                    state = INPUT_CANCEL;
                } else if (state == INPUT_PASS) {
                    state = INPUT_OK;
                } else if (state == INPUT_ID) {
                    state = INPUT_PASS;
                }
            } else if (ch == KEY_UP) {
                if (state == INPUT_OK || state == INPUT_CANCEL) {
                    state = INPUT_PASS;
                } else if (state == INPUT_PASS) {
                    state = INPUT_ID;
                }
            } else if (ch == KEY_DOWN) {
                if (state == INPUT_ID) {
                    state = INPUT_PASS;
                } else if (state == INPUT_PASS) {
                    state = INPUT_OK;
                }
            } else if (ch >= 0 && ch < 0x100) {
                if (state == INPUT_ID && idlen < MAX_ID_LEN) {
                    id = (char *)realloc(id, ++idlen + 1);
                    id[idlen-1] = ch & 0xff;
                    id[idlen] = 0;
                } else if(state == INPUT_PASS) {
                    pass = (char *)realloc(pass, ++passlen + 1);
                    pass[passlen-1] = ch & 0xff;
                    pass[passlen] = 0;
                }
            }
            if (passlen < 48) {
                inputBoxBuilder(id, passlen, state);
            } else if(passlen >= 48) {
                inputBoxBuilder(id, (passlen - 48) % 43 + 5, state);
            }
        }
        if (state == INPUT_CANCEL) {
            exit(0);
        }
        struct inputForm *form = (struct inputForm *) malloc(sizeof(inputForm));
        form->id = id;
        form->pass = pass;
        return form;
    }

    struct inputForm *drawPassPhraseUI(const char *uid,
                                       const char *info, char *msg) {
        drawLogo();
        drawInputFormBox();

        idWin = derwin(infoWin, 1, 66, 5, 1);
        passWin = derwin(infoWin, 1, 66, 7, 1);
        okWin = derwin(infoWin, 1, 34, 9, 0);
        cancelWin = derwin(infoWin, 1, 34, 9, 34);
        mvwaddstr(infoWin, 0, 2,
                "Please enter the Github ID and user information to unlock " \
                "the OpenPGP secret key for user:");
        
        mvwaddstr(infoWin, 2, 2, "\"");
        mvwaddstr(infoWin, 2, 3, uid);
        mvwaddstr(infoWin, 2, 3 + strlen(uid), "\", ");
        mvwaddstr(infoWin, 2, 6 + strlen(uid), "ID ");
        mvwaddnstr(infoWin, 2, 9 + strlen(uid), info, 8);

        if (msg) {
            wattron(infoWin, COLOR_PAIR(4));
            mvwaddstr(infoWin, 4, 1, msg);
            wattroff(infoWin, COLOR_PAIR(4));
        }
        mvwaddstr(idWin, 0, 0, "Enter Github ID:  " \
                "______________________________");
        mvwaddstr(passWin, 0, 0, "Enter Passphrase: ");
        wrefresh(infoWin);
        return handleUserInfo();
    }

    WINDOW *inputWin, *chatWin, *infoLineWin;
    WINDOW *topLine, *botLine;

    static void drawChatWin(char *user) {
        topLine = subwin(mainWin, 1, COLS, 0, 0);
        botLine = subwin(mainWin, 1, COLS, LINES-2, 0);
        wbkgd(topLine, COLOR_PAIR(9));
        wbkgd(botLine, COLOR_PAIR(9));
        mvwaddstr(botLine, 0, COLS - strlen(user), user);

       chatWin = subwin(mainWin, LINES - 3, COLS, 1, 0);

       mvwaddstr(topLine, 0, 0, " Onion Messenger");

       // Enable text scrolling
       scrollok(chatWin, TRUE);
       wrefresh(chatWin);
       wrefresh(topLine);
       wrefresh(botLine);
       wrefresh(mainWin);
    }

    static void writeChat(char *data) {
        wprintw(chatWin, "%s\n", data);
        wrefresh(chatWin);
        wcursyncup(inputWin);
        wrefresh(inputWin);
        beep();
    }
    static void writeChat(string data) {
        wprintw(chatWin, "%s\n", data.c_str());
        wrefresh(chatWin);
        wcursyncup(inputWin);
        wrefresh(inputWin);
        beep();
    }

    static void drawInputWin() {
       // Create input box and window
       inputWin = subwin(mainWin, 1, COLS, LINES-1, 0);
       mvwaddstr(inputWin, 0, 0, prompt);
       wcursyncup(inputWin);
       wrefresh(inputWin);
       wmove(inputWin, 0, strlen(prompt));
       wrefresh(inputWin);
       wrefresh(mainWin);
    }

    static void buildInputLine(char *msg) {
        wclear(inputWin);
        mvwaddstr(inputWin, 0, 0, prompt);
        if (msg) {
            mvwaddstr(inputWin, 0, strlen(prompt), msg);
            wmove(inputWin, 0, strlen(msg) + strlen(prompt));
        }
        wrefresh(inputWin);
    }

    static char *handleInput()
    {
        char *input = 0;
        int len = 0;
        int ch;
        buildInputLine(input);
        while ((ch = getch()) != '\n') {
            // Backspace
            if (ch == 8 || ch == 127 || ch == KEY_BACKSPACE) {
                if (len > 0) {
                    input = (char *)realloc(input, --len + 1);
                    input[len] = 0;
                }
            } else if (ch == KEY_RESIZE) {
                continue;
            } else if (ch == KEY_LEFT) {
            } else if (ch == KEY_RIGHT) {
            } else if (ch == KEY_UP) {
            } else if (ch == KEY_DOWN) {
            } else if (ch != ERR) {
                input = (char *)realloc(input, ++len + 1);
                input[len-1] = ch & 0xff;
                input[len] = 0;
            }
            buildInputLine(input);
        }
        return input;
    }

    void drawOnionChatUI(const char *uid, const char *keyid,
            void (*handler)(char *, void *), void *aux)
    {
        wclear(mainWin);
        wrefresh(mainWin);
        char msg[100];
        memset(msg, 0, 100);
        snprintf(msg, 100, "%s @ [%s]", uid, keyid);
        drawChatWin(msg);
        drawInputWin();
        wcursyncup(inputWin);
        wrefresh(inputWin);
        while (true) {
            char *userin = handleInput();
            if (userin) {
                handler(userin, aux);
                free(userin);
            }
        }
    }
}

namespace TUI
{
    TUIProvider::TUIProvider() {
        TUIImplement::initUI();
    }

    TUIProvider::~TUIProvider() {
        TUIImplement::end();
    }

    pair<string, string> TUIProvider::GetUserInfo(string uid, string infos,
                                                  char *msg) {
        auto info =
            TUIImplement::drawPassPhraseUI(uid.c_str(), infos.c_str(), msg);
        string id = info->id ? info->id : "";
        string pass = info->pass ? info->pass : "";
        if (info->id) free(info->id);
        if (info->pass) free(info->pass);
        free(info);
        return pair<string, string>(id, pass);
    }

    void TUIProvider::UserInputLoop(string uid, string keyid,
            void (*handler)(char *, void *), void *aux) {
        TUIImplement::drawOnionChatUI(uid.c_str(), keyid.c_str(), handler, aux);
    }

    void TUIProvider::PushMessage(char *msg) {
        msgLock.lock();
        TUIImplement::writeChat(msg);
        msgLock.unlock();
    }
    
    void TUIProvider::PushChat(string sender, string msg){
        msgLock.lock();
        string m = "[ " + sender + " ]\t" + msg;
        TUIImplement::writeChat(m);
        msgLock.unlock();
    }

    void TUIProvider::PushNotification(string msg){
        msgLock.lock();
        wattron(TUIImplement::chatWin, COLOR_PAIR(3));
        TUIImplement::writeChat(msg);
        wattron(TUIImplement::chatWin, COLOR_PAIR(1));
        msgLock.unlock();
    }

    void TUIProvider::PushError(string msg) {
        msgLock.lock();
        wattron(TUIImplement::chatWin, COLOR_PAIR(4));
        TUIImplement::writeChat(msg);
        wattron(TUIImplement::chatWin, COLOR_PAIR(1));
        msgLock.unlock();
    }
}
