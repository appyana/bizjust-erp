#include "managesalesman.h"
#include "ui_managesalesman.h"

ManageSalesMan::ManageSalesMan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageSalesMan)
{
    ui->setupUi(this);
    loadtable();
    ui->lbl_id->setVisible(false);
}

ManageSalesMan::~ManageSalesMan()
{
    delete ui;
}

void ManageSalesMan::loadtable()
{
    ui->detailtable->setColumnCount(4);
    ui->detailtable->setRowCount(0);
    QStringList titles;
    titles <<"id"<<"Name"<<"Address"<<"City";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ID);
    ui->detailtable->setColumnWidth(NAME,200);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(ADDRESS, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(CITY,100);
    QSqlQuery qry(" select * from tblsalesman order by name asc ");
    while(qry.next())
    {
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,ID, new QTableWidgetItem( qry.value("salesmanid").toString() ));
        ui->detailtable->setItem(row,NAME, new QTableWidgetItem( qry.value("name").toString() ));
        ui->detailtable->setItem(row,ADDRESS, new QTableWidgetItem( qry.value("address1").toString() ));
        ui->detailtable->setItem(row,CITY, new QTableWidgetItem( qry.value("city").toString() ));
    }
}

void ManageSalesMan::on_save_btn_clicked()
{
    QString name = ui->name->text();
    QString address = ui->address->text();
    QString country = ui->country->text();
    QString city = ui->city->text();
    QString phone = ui->phone->text();
    QString sql;
    if(salesmanid=="")
    {
        sql = " INSERT INTO tblsalesman "
              " (name, address1, country, city, phone1) "
              " VALUES "
              " ('"+name+"','"+address+"','"+country+"','"+city+"','"+phone+"') ";
    }
    else
    {
        sql = " UPDATE tblsalesman SET "
              " name     = '"+name+"',"
              " address1 = '"+address+"',"
              " country  = '"+country+"',"
              " city     = '"+city+"',"
              " phone1   = '"+phone+"'"
              " WHERE "
              " salesmanid = '"+salesmanid+"'";
    }
    if(conn.execQuery(sql))
    {
        QMessageBox::information(this,"","Record saved successfully");
        loadtable();
        ui->name->clear();
        ui->address->clear();
        ui->country->setText("Pakistan");
        ui->city->clear();
        ui->phone->clear();
        ui->lbl_id->clear();
        salesmanid = "";
        ui->lbl_id->setVisible(false);
    }
    else
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
    }
}

void ManageSalesMan::on_edit_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        int row = ui->detailtable->currentRow();
        QString id = ui->detailtable->item(row,ID)->text();
        QSqlQuery qry;
        qry.prepare(" select * from tblsalesman WHERE salesmanid = '"+id+"' ");
        if(qry.exec())
        {
            ui->lbl_id->setVisible(true);
            salesmanid = id;
            ui->lbl_id->setText("Sales Man ID: "+id);
            qry.next();
            ui->name->setText( qry.value("name").toString() );
            ui->address->setText( qry.value("address1").toString() );
            ui->country->setText( qry.value("country").toString() );
            ui->city->setText( qry.value("city").toString() );
            ui->phone->setText( qry.value("phone1").toString() );
        }
    }
    else
    {
        QMessageBox::warning(this,"","Select sales man for edit");
    }
}

void ManageSalesMan::on_clear_btn_clicked()
{
    salesmanid = "";
    ui->name->clear();
    ui->address->clear();
    ui->country->setText("Pakistan");
    ui->city->clear();
    ui->phone->clear();
    ui->lbl_id->clear();
    ui->lbl_id->setVisible(false);
}

void ManageSalesMan::on_delete_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {

        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
                int row = ui->detailtable->currentRow();
                QString id = ui->detailtable->item(row,ID)->text();
                if(conn.execQuery(" delete from tblsalesman WHERE salesmanid = '"+id+"' "))
                {
                    QMessageBox::information(this,"","Record deleted successfully");
                    loadtable();
                }
                else
                {
                    QMessageBox::critical(this,"","Some problem in record deletion");
                }
          }
    }
    else
    {
        QMessageBox::warning(this,"","Select sames man for delete");
    }
}
