#include "setupserialdialog.h"
#include "ui_setupserialdialog.h"

setupSerialDialog::setupSerialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setupSerialDialog)
    ui->setupUi(this);
}

setupSerialDialog::~setupSerialDialog()
{
    delete ui;
}
