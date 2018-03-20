#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ncurses.h>
#include "tui.hh"

typedef enum {INPUT_ID, INPUT_PASS, INPUT_OK, INPUT_CANCEL} startInputFormState;

const char *prompt = "[Onion] ";
const char *user = "hestati63 @ [3CB44CA1]";

WINDOW *mainWin;
void colors() {
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
   init_pair(10, COLOR_BLACK, COLOR_BLUE);
   init_pair(11, -1, COLOR_CYAN);
}

/*
WINDOW *mainWin, *inputWin, *chatWin, *infoWin;
WINDOW *topLine, *botLine;
Draw chat box and window
void drawChatWin() {
    topLine = subwin(mainWin, 1, COLS, 0, 0);
    botLine = subwin(mainWin, 1, COLS, LINES-2, 0);
    wbkgd(topLine, COLOR_PAIR(9));
    wbkgd(botLine, COLOR_PAIR(10));
   mvwaddstr(botLine, 0, COLS - strlen(user), user);

   chatWin = subwin(mainWin, LINES - 2, COLS, 1, 0);

   mvwaddstr(topLine, 0, 0, " Onion Messenger");

   // Enable text scrolling
   scrollok(chatWin, TRUE);
   wrefresh(chatWin);
   wrefresh(topLine);
   wrefresh(botLine);
   wrefresh(mainWin);
}

void drawInputWin() {
   // Create input box and window
   inputWin = subwin(mainWin, 1, COLS, LINES-1, strlen(prompt));
   mvwaddstr(mainWin, LINES-1, 0, prompt);
   wrefresh(inputWin);
   wrefresh(mainWin);
}




void initializeCurses() {
   drawChatWin();
   drawInputWin();

}
*/
void end() {
    endwin();
}
// https://github.com/mgeitz/tbdchat/blob/master/client/chat_client.c
// https://github.com/mgeitz/tbdchat/blob/master/client/visual.c

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

static char *inputBoxBuilder(int idlen, int passlen, int state) {
    for (int i = 0; i < 50; i++) {
        mvwaddstr(passWin, 0, 18+i, i < passlen ? "*" : "_");
    }
    wmove(passWin, 0, 18+passlen);
    wrefresh(passWin);

    mvwaddstr(okWin, 0, 16, "<OK>");
    wrefresh(okWin);
    mvwaddstr(cancelWin, 0, 14, "<Cancel>");
    wrefresh(cancelWin);
    switch (state) {
        case INPUT_ID:
            wmove(idWin, 0, idlen + 18);
            wrefresh(idWin);
            break;
        case INPUT_PASS:
            wmove(passWin, 0, passlen + 18);
            wrefresh(passWin);
            break;
        case INPUT_OK:
            wattron(okWin, COLOR_PAIR(11));
            mvwaddstr(okWin, 0, 16, "<OK>");
            wmove(okWin, 0, 16);
            wattroff(okWin, COLOR_PAIR(11));
            wrefresh(okWin);
            break;
        case INPUT_CANCEL:
            wattron(cancelWin, COLOR_PAIR(11));
            mvwaddstr(cancelWin, 0, 14, "<Cancel>");
            wmove(cancelWin, 0, 14);
            wattroff(cancelWin, COLOR_PAIR(11));
            wrefresh(cancelWin);
            break;
    }
}

void drawInputFormBox(void) {
    infoWinBox = subwin(mainWin, 11, 72, LINES/2+1, COLS/2 - 36);
    infoWin = subwin(mainWin, 9, 68, LINES/2+2, COLS/2 - 35);
    box(infoWinBox, 0, 0);
    wrefresh(infoWinBox);
    wrefresh(infoWin);
}

struct inputForm *handleUserInfo() {
    int state = INPUT_ID;

    int idlen = 0;
    char *id = 0;

    int passlen = 0;
    char *pass = 0;

    char *ptr = id;
    int *ptrlen = &idlen;
    int ch;

    wcursyncup(idWin);
    inputBoxBuilder(0, 0, state);

    while ((ch = getch()) != '\n') {
        // Backspace
        if (ch == 8 || ch == 127 || ch == KEY_BACKSPACE) {
            /* TODO
            *len = *len ? *len - 1 : 0;
            ptr = (char *)realloc(ptr, *len);
            */
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
                wcursyncup(passWin);
                state = INPUT_PASS;
            }
        } else if (ch == KEY_UP) {
            if (state == INPUT_OK || state == INPUT_CANCEL) {
                wcursyncup(passWin);
                state = INPUT_PASS;
            } else if (state == INPUT_PASS) {
                wcursyncup(idWin);
                state = INPUT_ID;
            }
        } else if (ch == KEY_DOWN) {
            if (state == INPUT_ID) {
                state = INPUT_PASS;
            } else if (state == INPUT_PASS) {
                state = INPUT_OK;
            }
        } else if (ch != ERR) {
            /* TODO
            *len = *len ? 
            pass = (char *)realloc(pass, ++len);
            pass[len-1] = ch & 0xff;
            */
        }
        if (passlen >= 50) {
            inputBoxBuilder(idlen, (passlen+5) % 50, state);
        } else {
            inputBoxBuilder(idlen, passlen, state);
        }
    }
    if (state == 2) {
        exit(0);
    }
}

struct inputForm *drawPassPhraseUI(void) {
    drawLogo();
    drawInputFormBox();

    idWin = derwin(infoWin, 1, 66, 4, 1);
    passWin = derwin(infoWin, 1, 66, 6, 1);
    okWin = derwin(infoWin, 1, 34, 8, 0);
    cancelWin = derwin(infoWin, 1, 34, 8, 34);
    mvwaddstr(infoWin, 0, 2,
            "Please enter the user information to unlock " \
            "the OpenPGP secret key and connect onion messenger.");
    mvwaddstr(idWin, 0, 0, "Enter Github ID:  " \
            "______________________________");
    mvwaddstr(passWin, 0, 0, "Enter Passphrase: ");
    wmove(idWin, 0, 18);
    wmove(passWin, 0, 18);
    wrefresh(idWin);
    wrefresh(passWin);
    return handleUserInfo();
}

int main()
{
    initUI();
    auto pass = drawPassPhraseUI();
    pass = drawPassPhraseUI();
    endwin();
}

