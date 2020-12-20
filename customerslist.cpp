#include "customerslist.h"
#include "ui_customerslist.h"


CustomersList::CustomersList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomersList)
{
    ui->setupUi(this);
}

CustomersList::~CustomersList()
{
    delete ui;
}
