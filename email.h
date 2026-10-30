#ifndef EMAIL_H
#define EMAIL_H

#include <QString>
#include <QList>

struct Recipient {
    int id;
    QString username;
};

struct Email {
    int messageId;
    int senderId;
    QString senderUsername;
    QString subject;
    QString body;
    QList<Recipient> recipients;
};

#endif // EMAIL_H
