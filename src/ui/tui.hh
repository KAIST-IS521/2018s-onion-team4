#ifndef __UI__
#define __UI__

#define MAX_ID_LEN 30

struct inputForm
{
    char *id;
    char *pass;
};

void initUI(void);
struct inputForm *drawPassPhraseUI(void);
void drawOnionChatUI(char *uid, char *keyid);

#endif
