#include "createprofile.h"
#include "ui_createprofile.h"

CreateProfile::CreateProfile(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CreateProfile)
{
    ui->setupUi(this);
}

CreateProfile::~CreateProfile()
{
    delete ui;
}
