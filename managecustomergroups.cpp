#include "managecustomergroups.h"
#include "ui_managecustomergroups.h"

ManageCustomerGroups::ManageCustomerGroups(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageCustomerGroups)
{
    ui->setupUi(this);
    loadtable();
    ui->lbl_id->setVisible(false);
}

ManageCustomerGroups::~ManageCustomerGroups()
{
    delete ui;
}


void ManageCustomerGroups::loadtable()
{
    ui->detailtable->setColumnCount(2);
    ui->detailtable->setRowCount(0);
    QStringList titles;
    titles <<"id"<<"Customer Group Name";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(NAME, QHeaderView::Stretch);
    QSqlQuery qry(" select * from tblcustomergroups order by groupid asc ");
    while(qry.next())
    {
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,ID, new QTableWidgetItem( qry.value("groupid").toString() ));
        ui->detailtable->setItem(row,NAME, new QTableWidgetItem( qry.value("groupname").toString() ));
    }
}

void ManageCustomerGroups::on_save_btn_clicked()
{
    QString name = ui->name->text();
    QString sql;
    if(editid=="")
    {
        sql = " INSERT INTO tblcustomergroups "
              " (groupname) "
              " VALUES "
              " ('"+name+"') ";
    }
    else
    {
        sql = " UPDATE tblcustomergroups SET "
              " groupname     = '"+name+"'"
              " WHERE "
              " groupid = '"+editid+"'";
    }
    if(conn.execQuery(sql))
    {
        QMessageBox::information(this,"","Record saved successfully");
        loadtable();
        ui->name->clear();
        editid="";
        ui->lbl_id->clear();
        ui->lbl_id->setVisible(false);
    }
    else
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
    }
}

void ManageCustomerGroups::on_edit_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        int row = ui->detailtable->currentRow();
        QString id = ui->detailtable->item(row,ID)->text();
        QString name = ui->detailtable->item(row,NAME)->text();
        QSqlQuery qry;
        qry.prepare(" select * from tblcustomergroups WHERE groupid = '"+id+"' ");
        if(qry.exec())
        {
            ui->lbl_id->setVisible(true);
            editid = id;
            ui->lbl_id->setText("Group Name: "+name);
            qry.next();
            ui->name->setText( qry.value("groupname").toString() );
        }
    }
    else
    {
        QMessageBox::warning(this,"","Select row for edit");
    }
}

void ManageCustomerGroups::on_clear_btn_clicked()
{
    ui->name->clear();
    ui->lbl_id->clear();
    editid="";
    ui->lbl_id->setVisible(false);
}

void ManageCustomerGroups::on_delete_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {

        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
                int row = ui->detailtable->currentRow();
                QString id = ui->detailtable->item(row,ID)->text();
                if(conn.execQuery(" delete from tblcustomergroups WHERE groupid = '"+id+"' "))
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
        QMessageBox::warning(this,"","Select row for delete");
    }
}
