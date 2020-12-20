#include "managecarrier.h"
#include "ui_managecarrier.h"

ManageCarrier::ManageCarrier(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageCarrier)
{
    ui->setupUi(this);
    loadtable();
    ui->lbl_id->setVisible(false);
}

ManageCarrier::~ManageCarrier()
{
    delete ui;
}


void ManageCarrier::loadtable()
{
    ui->detailtable->setColumnCount(2);
    ui->detailtable->setRowCount(0);
    QStringList titles;
    titles <<"id"<<"Carrier Description";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(NAME, QHeaderView::Stretch);
    QSqlQuery qry(" select * from tblcarrier order by id asc ");
    while(qry.next())
    {
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,ID, new QTableWidgetItem( qry.value("id").toString() ));
        ui->detailtable->setItem(row,NAME, new QTableWidgetItem( qry.value("description").toString() ));
    }
}

void ManageCarrier::on_save_btn_clicked()
{
    QString name = ui->name->text();
    QString sql;
    if(carrierid=="")
    {
        sql = " INSERT INTO tblcarrier "
              " (description) "
              " VALUES "
              " ('"+name+"') ";
    }
    else
    {
        sql = " UPDATE tblcarrier SET "
              " description     = '"+name+"'"
              " WHERE "
              " id = '"+carrierid+"'";
    }
    if(conn.execQuery(sql))
    {
        QMessageBox::information(this,"","Record saved successfully");
        loadtable();
        ui->name->clear();
        carrierid="";
        ui->lbl_id->clear();
        ui->lbl_id->setVisible(false);
    }
    else
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
    }
}

void ManageCarrier::on_edit_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        int row = ui->detailtable->currentRow();
        QString id = ui->detailtable->item(row,ID)->text();
        QString name = ui->detailtable->item(row,NAME)->text();
        QSqlQuery qry;
        qry.prepare(" select * from tblcarrier WHERE id = '"+id+"' ");
        if(qry.exec())
        {
            ui->lbl_id->setVisible(true);
            carrierid = id;
            ui->lbl_id->setText("Carrier: "+name);
            qry.next();
            ui->name->setText( qry.value("description").toString() );
        }
    }
    else
    {
        QMessageBox::warning(this,"","Select carrier for edit");
    }
}

void ManageCarrier::on_clear_btn_clicked()
{
    ui->name->clear();
    ui->lbl_id->clear();
    carrierid="";
    ui->lbl_id->setVisible(false);
}

void ManageCarrier::on_delete_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {

        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
                int row = ui->detailtable->currentRow();
                QString id = ui->detailtable->item(row,ID)->text();
                if(conn.execQuery(" delete from tblcarrier WHERE id = '"+id+"' "))
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
        QMessageBox::warning(this,"","Select carrier for delete");
    }
}
