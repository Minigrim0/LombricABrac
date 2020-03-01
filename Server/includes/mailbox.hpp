#include <iostream>

#define MAX_MAILS 4
#define MAX_SIZE_MAILS 100

class MailBox{
    public:
        MailBox();
        ~MailBox();

    private:
        char m_mails[MAX_MAILS][MAX_SIZE_MAILS];
};
