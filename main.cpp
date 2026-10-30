#include "mainwindow.h"
#include "HelperFile.h"
#include <QApplication>

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!Utils::InitSql()) {
        return 1;
    }

    MainWindow window;

    window.show();

    Utils::Close();

    MainWindow w;

    return app.exec();
}
