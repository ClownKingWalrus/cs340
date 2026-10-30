#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "email.h"
#include <QList>
#include <QSqlDatabase>

class DatabaseHandler {
public:
    DatabaseHandler();

    // This line fetches X amount of emails.
    QList<Email> loadEmails(int limit);
};

#endif // DATABASEHANDLER_H
