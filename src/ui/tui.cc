#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ncurses.h>
#include "ui.hh"

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

WINDOW *LogoWin, *passWin, *okWin, *cancelWin, *infoWin;
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

static char *inputBoxBuilder(int len, int state) {
    for (int i = 0; i < 50; i++) {
        mvwaddstr(passWin, 0, 18+i, i < len ? "*" : "_");
    }
    wmove(passWin, 0, len + 18);
    mvwaddstr(okWin, 0, 16, "<OK>");
    wrefresh(okWin);
    mvwaddstr(cancelWin, 0, 14, "<Cancel>");
    wrefresh(cancelWin);
    switch (state) {
        case 0:
            wmove(passWin, 0, len + 18);
            wrefresh(passWin);
            break;
        case 1:
            wattron(okWin, COLOR_PAIR(11));
            mvwaddstr(okWin, 0, 16, "<OK>");
            wmove(okWin, 0, 16);
            wattroff(okWin, COLOR_PAIR(11));
            wrefresh(okWin);
            break;
        case 2:
            wattron(cancelWin, COLOR_PAIR(11));
            mvwaddstr(cancelWin, 0, 14, "<Cancel>");
            wmove(cancelWin, 0, 14);
            wattroff(cancelWin, COLOR_PAIR(11));
            wrefresh(cancelWin);
            break;
    }
}

static char *handlePasswordInput() {
    int len = 0;
    int state = 0;
    char *pass = 0;
    int ch;

    wcursyncup(passWin);
    inputBoxBuilder(len, state);

    while ((ch = getch()) != '\n') {
        // Backspace
        if (ch == 8 || ch == 127 || ch == KEY_BACKSPACE) {
            len = len ? len-1 : 0;
            pass = (char *)realloc(pass, len);
        } else if (ch == KEY_RESIZE) {
            continue;
        } else if (ch == KEY_LEFT) {
            state = state ? state-1 : 0;
        } else if (ch == KEY_RIGHT) {
            state = state == 2 ? 2 : state + 1;
        } else if (ch == KEY_UP) {
            state = state ? 0 : state;
        } else if (ch == KEY_DOWN) {
            state = state == 0 ? 1 : state;
        } else if (ch != ERR) {
            pass = (char *)realloc(pass, ++len);
            pass[len-1] = ch & 0xff;
        }
        if (len >= 50) {
            inputBoxBuilder((len+5) % 50, state);
        } else {
            inputBoxBuilder(len, state);
        }
    }
    if (state == 2) {
        exit(0);
    }
    return pass;
}

char *drawPassPhraseUI(void) {
    drawLogo();
    infoWin = subwin(mainWin, 9, 72, LINES/2+1, COLS/2 - 36);
    passWin = derwin(infoWin, 1, 68, 5, 2);
    okWin = derwin(infoWin, 1, 35, 7, 1);
    cancelWin = derwin(infoWin, 1, 35, 7, 36);
    box(infoWin, 0, 0);
    mvwaddstr(infoWin, 1, 2,
            "Please enter the passphrase to unlock the OpenPGP secret key:");
    mvwaddstr(passWin, 0, 0, "Enter Passphrase: ");
    return handlePasswordInput();
}

int main()
{
    initUI();
    char *pass = drawPassPhraseUI();
    pass = drawPassPhraseUI();
    endwin();
}

