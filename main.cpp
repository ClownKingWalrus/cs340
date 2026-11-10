#include "mainwindow.h"
#include "signup.h"
#include "HelperFile.h"
#include <QApplication>

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!Utils::InitSql()) {
        std::cout << "Sql is down\n";
        // return 1;
    }

    SignUp window;

    window.show();



    Utils::Close();

    return app.exec();
}
