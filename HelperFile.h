#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <qdatetime.h>

#include <QDebug>

#include "picosha2.h" //Sha256 hasher https://github.com/okdshin/PicoSHA2

// this assumes in order pgHostName, pgPort, pgDatabase, pgUser, pgPass
class Utils {
private:
    static inline const QString connectionName = "QPSQL";

public:
    static bool InitSql() {
        std::string pgHostName;
        std::string pgPort;
        std::string pgDatabase;
        std::string pgUser;
        std::string pgPass;

        std::ifstream file("../../.env"); // put your .env file in same dir as cpp and .h the ../ is required due to build file being being its own entity

        if (!file.is_open()) {
            std::cerr << "Error: Could not open the file!" << std::endl;
        }

        std::string line;
        int i = 0;

        // Read line by line until the end of the file (apprently size_t is required cause npos is a massive number)
        while (std::getline(file, line)) {
            size_t pos = line.find('=');

            //npos just means not found or no match
            if (pos != std::string::npos) {
                // Erase from index 0 up to (pos + 1) to include the '=' sign
                line.erase(0, pos + 1);
                if (i == 0) { pgHostName = line; }
                if (i == 1) { pgPort = line; }
                if (i == 2) { pgDatabase = line; }
                if (i == 3) { pgUser = line; }
                if (i == 4) { pgPass = line; }
                i++;
            }
        }

        file.close();

        if (i < 5) {
            qDebug() << "missing some postgres config check .env file!";
            return false;
        }

        //build the items to be returned
        QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionName); //makes the connection name QPSQL

        db.setHostName(QString::fromStdString(pgHostName));
        db.setPort(std::stoi(pgPort));
        db.setDatabaseName(QString::fromStdString(pgDatabase));
        db.setUserName(QString::fromStdString(pgUser));
        db.setPassword(QString::fromStdString(pgPass));


        if (!db.open()) {
            qDebug() << "Connection failed:" << db.lastError().text();
            return false;
        }

        qDebug() << "Successfully connected to PostgreSQL!";
        return true;
    };

    static QSqlDatabase GetConn() {
        return QSqlDatabase::database(connectionName, false);
    }

    static void Close() {
        if (!QSqlDatabase::contains(connectionName)) {
            return;
        }

        {
            QSqlDatabase db = QSqlDatabase::database(connectionName, false);
            db.close();
        }

        QSqlDatabase::removeDatabase(connectionName);

        qDebug() << "PostgreSQL connection closed!";
    }

    static void CreateProfile(std::string UserName, std::string UserPassword, std::string Email) {

        try {
            //create the query
            QSqlDatabase db = Utils::GetConn();
            QSqlQuery query(db);

            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            int min = 1000;
            int max = 999999999;
            int randomNumber = min + (std::rand() % (max - min + 1));

            std::string salt = UserName;
            salt += std::to_string(randomNumber);
            salt += UserPassword;
            salt = picosha2::hash256_hex_string(salt);

            std::string hashedPass = picosha2::hash256_hex_string((UserPassword+salt));
            UserPassword.clear(); //no except this cannot fail

            query.prepare("INSERT INTO public.\"User\" (username, datejoined, password, salt, email)" // public.\"User\" is required since in psql its a keyword
                          "VALUES (?, ?, ?, ?, ?)");
            query.bindValue(0, QString::fromStdString(UserName));
            query.bindValue(1, QDateTime::currentDateTimeUtc());
            query.bindValue(2, QString::fromStdString(hashedPass));
            query.bindValue(3, QString::fromStdString(salt));
            query.bindValue(4, QString::fromStdString(Email));

            if (!query.exec()) {
                Utils::PrintSqlError(query.lastError());
            }

            hashedPass.clear();


        }
        catch (const std::exception& e) {
            std::cerr << "C++ error: " << e.what() << std::endl;
        }
    }

    static void PrintSqlError(const QSqlError& error) {
        qDebug() << "PostgreSQL Error:" << error.text();
        qDebug() << "Database Error:" << error.databaseText();
        qDebug() << "Driver Error:" << error.driverText();
        qDebug() << "Error Code:" << error.nativeErrorCode();
    }

    static bool UsernameChecker(std::string UN) {

        std::cout << "in usernameCheck\n";
        //set Username
        std::string UserName = UN;

        if (UserName == "") {
            return false; //bad string
        }

        try {
            QSqlDatabase db = Utils::GetConn();
            QSqlQuery query(db);

            //this statement should be optimized this is essentially a select * statement
            if (!query.prepare( //apprently select 1 just checks whether it exist or not rather than retrieving the whole user db
                    "SELECT 1 FROM public.\"User\" WHERE LOWER(username) = LOWER(?) LIMIT 1"
                    )) {
                qDebug() << "UsernameChecker: Prepare failed";
                Utils::PrintSqlError(query.lastError());
                return false;
            }
            query.addBindValue(QString::fromStdString(UserName));

            if (!query.exec()) {
                Utils::PrintSqlError(query.lastError());
                return false;
            }

            return query.next();
        }
        catch (const std::exception& e) {
                std::cerr << "C++ error: " << e.what() << std::endl;
                return false;
            }
        return false; //assuming somthing failed we will return false
    }

    static bool EmailChecker(std::string EM) {

        //initlize user and pass
        std::string Email = EM;

        if (Email == "") {
            return false; //bad string
        }

        try {
            QSqlDatabase db = Utils::GetConn();
            QSqlQuery query(db);

            if (!query.prepare(
                    "SELECT 1 FROM public.\"User\" WHERE LOWER(email) = LOWER(?) LIMIT 1"
                    )) {
                qDebug() << "Prepare failed:";
                Utils::PrintSqlError(query.lastError());
                return false;
            }

            query.addBindValue(QString::fromStdString(Email));

            if (!query.exec()) {
                qDebug() << "Execute failed:";
                Utils::PrintSqlError(query.lastError());
                return false;
            }

            return query.next();
        }
        catch (const std::exception& e) {
            std::cerr << "C++ error: " << e.what() << std::endl;
            return false;
        }

        return false; //assuming somthing failed we will return false
    }
}; //End of class Utils
