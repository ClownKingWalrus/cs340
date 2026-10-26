#include "databasehandler.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

DatabaseHandler::DatabaseHandler() {}

QList<Email> DatabaseHandler::loadEmails(int limit) {
    QList<Email> emails;
    QSqlQuery query;

    // Uses QSqlQuery to select all emails.
    query.prepare(R"(
        SELECT
            m.messageid,
            m.userid AS sender_id,
            sender.username AS sender_username,
            m.subject,
            m.message,
            STRING_AGG(rec.userid || ':' || rec.username, ',') AS recipients_data
            FROM messages m
            JOIN "User" sender ON m.userid = sender.userid
            LEFT JOIN messagesrecieved mr ON m.messageid = mr.messageid
            LEFT JOIN "User" rec ON mr.receiverid = rec.userid
            GROUP BY m.messageid, m.userid, sender.username, m.subject, m.message
            ORDER BY m.messageid DESC
            LIMIT :limit;
    )");

    // Note: STRING_AGG() puts all recipients with ID and name onto one row for each email.
    // This means that if one person sends an email to ten others, the ten people will appear on one row rather than making ten separate rows.

    // To add a limit to the amount of emails pulled, query.bindValue() binds the limit variable from this code into the SQL.
    query.bindValue(":limit", limit);

    //Error capturing.
    if (!query.exec()) {
        qWarning() << "Database query failed: " << query.lastError().text();
        return emails;
    }

    //A while loop for each query value from the database.
    while (query.next()) {
        //Sets the object public variables equal to the query results.
        Email email;
        email.messageId = query.value("messageid").toInt();
        email.senderId = query.value("sender_id").toInt();
        email.senderUsername = query.value("sender_username").toString();
        email.subject = query.value("subject").toString();
        email.body = query.value("message").toString();

        QString recipients = query.value("recipients_data").toString();
        if (!recipients.isEmpty()) {
            //recipientPairs gathers all the recipients and splits them with a comma for readability.
            const QStringList recipientPairs = recipients.split(',');
            //For every pair (looking at memory) in recipientPairs, it will split the ID and name from each other to add to the email public variable recipients.
            for (int i = 0; i < recipientPairs.size(); i++) {
                const QString &pair = recipientPairs[i];
                const QStringList parts = pair.split(':');
                if (parts.size() == 2) {
                    email.recipients.append({ parts[0].toInt(), parts[1] });
                }
            }
        }
        //Appends all emails.
        emails.append(email);
    }

    return emails;
}