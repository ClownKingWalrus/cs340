#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehandler.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //Loads the DatabaseHandler and loads emails. The limit is 5. Once the database is fully completed, we can expand the limit.
    DatabaseHandler dbHandler;
    QList<Email> emails = dbHandler.loadEmails(5);

    std::cout << "Loaded Emails:\n";
    //For every email, print the ID, From, Subject, Body, and To values.
    for (int i = 0; i < emails.size(); ++i) {
        const Email &email = emails[i];
        std::cout << "ID: " << email.messageId << "\n";
        std::cout << "From: " << email.senderUsername.toStdString() << " (ID: " << email.senderId << ")\n";
        //If the subject is NOT empty, print the value. Else, print Null.
        if (email.subject.size() != 0) {
            std::cout << "Subject: " << email.subject.toStdString() << "\n";
        }
        else {
            std::cout << "Subject: Null\n";
        }
        std::cout << "Body: " << email.body.toStdString() << "\n";
        std::cout << "To: ";
        if (email.recipients.size() != 0) {
            //For all the recipients of an email, print them and their ID.
            for (int j = 0; j < email.recipients.size(); ++j) {
                std::cout << email.recipients[j].username.toStdString() << " (ID: " << email.recipients[i].id << ")";
                if (j < email.recipients.size() - 1) {
                    std::cout << ", ";
                }
            }
        }
        else {
            std::cout << "Null";
        }
        std::cout << "\n";
        std::cout << std::endl;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
