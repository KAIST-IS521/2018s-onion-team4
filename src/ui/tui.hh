#ifndef __UI__
#define __UI__

struct inputForm
{
    char *id;
    char *pass;
};

void initUI(void);
struct inputForm *drawPassPhraseUI(void);
void drawOnionChatUI(char *uid, char *keyid);

#endif
