#include "vendorslist.h"
#include "ui_vendorslist.h"

VendorsList::VendorsList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VendorsList)
{
    ui->setupUi(this);
    loadform();
}

VendorsList::~VendorsList()
{
    delete ui;
}

void VendorsList::loadform()
{
    ui->tableWidget->setColumnCount(6);
    QStringList titles;
    titles <<"ID"<<"Company Name"<<"Address"<<"City"<<"Phone"<<"Contact Person";
    ui->tableWidget->setHorizontalHeaderLabels(titles);
    ui->tableWidget->setColumnWidth(VENDORID,80);
    ui->tableWidget->setColumnWidth(VENDORNAME,250);
    ui->tableWidget->setColumnWidth(CITY,140);
    ui->tableWidget->setColumnWidth(PHONE,200);
    ui->tableWidget->setColumnWidth(CONTACTPERSON,200);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(ADDRESS,QHeaderView::Stretch);
    QSqlQuery info("select * from tblvendors where dl = '0'");
    while(info.next())
    {
        QString contact = info.value("firstname").toString()+" "+info.value("middlename").toString()+" "+info.value("lastname").toString();
        QString phones = info.value("phone1").toString()+" "+info.value("phone2").toString()+" "+info.value("phone3").toString();
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,VENDORID, new QTableWidgetItem( info.value("vendorid").toString() ));
        ui->tableWidget->setItem(row,VENDORNAME, new QTableWidgetItem( info.value("vendorname").toString() ));
        ui->tableWidget->setItem(row,ADDRESS, new QTableWidgetItem( info.value("address1").toString() ));
        ui->tableWidget->setItem(row,CITY, new QTableWidgetItem( info.value("city").toString() ));
        ui->tableWidget->setItem(row,PHONE, new QTableWidgetItem( phones ));
        ui->tableWidget->setItem(row,CONTACTPERSON, new QTableWidgetItem( contact ));
    }
}
