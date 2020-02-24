#include <iostream>

#define MAX_MAILS 100
#define MAX_SIZE_MAILS 100

class MailBox{
    public:
        MailBox();
        ~MailBox();

    private:
        char m_mails[MAX_MAILS][MAX_SIZE_MAILS];
};
