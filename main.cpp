#include "mainwindow.h"
#include "HelperFile.h"
#include <QApplication>

#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Initialize the PostgreSQL driver
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    std::array<std::string, 5> pgInfo = loadEnv();
    db.setHostName(QString::fromStdString(pgInfo[0]));
    db.setPort(std::stoi(pgInfo[1]));
    db.setDatabaseName(QString::fromStdString(pgInfo[2]));
    db.setUserName(QString::fromStdString(pgInfo[3]));
    db.setPassword(QString::fromStdString(pgInfo[4]));


    if (!db.open()) {
        qDebug() << "Connection failed:"
                 << db.lastError().text();

        return 1;
    }

    qDebug() << "Successfully connected!";

    MainWindow w;

    return app.exec();
}
