#include "financialyear.h"
#include "ui_financialyear.h"

FinancialYear::FinancialYear(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FinancialYear)
{
    ui->setupUi(this);
    loadtable();
    ui->lbl_id->setVisible(false);
    ui->status->setChecked(true);
}

FinancialYear::~FinancialYear()
{
    delete ui;
}


void FinancialYear::loadtable()
{
    ui->detailtable->setColumnCount(6);
    ui->detailtable->setRowCount(0);
    QStringList titles;
    titles <<"id"<<"Title"<<"Starting"<<"Ending"<<"Description"<<"Status";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(DESCRIPTION, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(TITLE,120);
    ui->detailtable->setColumnWidth(STARTING,90);
    ui->detailtable->setColumnWidth(ENDING,90);
    ui->detailtable->setColumnWidth(STATUS,90);
    QSqlQuery qry(" select * from tblfinancialyear order by id desc ");
    while(qry.next())
    {
        QString status="";
        if(qry.value("yearstatus").toInt()==1)
        {
            status = "Open";
        }
        else
        {
            status = "Close";
        }
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,ID, new QTableWidgetItem( qry.value("id").toString() ));
        ui->detailtable->setItem(row,TITLE, new QTableWidgetItem( qry.value("title").toString() ));
        ui->detailtable->setItem(row,STARTING, new QTableWidgetItem( qry.value("startdate").toDate().toString("dd/MM/yyyy") ));
        ui->detailtable->setItem(row,ENDING, new QTableWidgetItem( qry.value("enddate").toDate().toString("dd/MM/yyyy") ));
        ui->detailtable->setItem(row,DESCRIPTION, new QTableWidgetItem( qry.value("description").toString() ));
        ui->detailtable->setItem(row,STATUS, new QTableWidgetItem( status ));
    }
}

void FinancialYear::on_save_btn_clicked()
{
    QString description = ui->description->text();
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString beginyear = ui->begindate->date().toString("yyyy");
    QString endyear = ui->enddate->date().toString("yyyy");
    QString status = "0";
    if(ui->status->isChecked()==true)
    {
        status = "1";
    }
    QString title = beginyear+"-"+endyear;
    if(beginyear==endyear)
    {
        title = beginyear;
    }
    QString sql;
    if(editid=="")
    {
        QSqlQuery qry(" SELECT max(id) from tblfinancialyear ");
        qry.next();
        int yearid = qry.value(0).toInt() + 1;
        sql = " INSERT INTO tblfinancialyear "
              " (yearid, title, description, startdate, enddate, yearstatus) "
              " VALUES "
              " ('"+erp.intString(yearid)+"','"+title+"','"+description+"','"+begindate+"','"+enddate+"','"+status+"') ";
    }
    else
    {
        sql = " UPDATE tblfinancialyear SET "
              " title       = '"+title+"',"
              " description = '"+description+"',"
              " startdate   = '"+begindate+"',"
              " enddate     = '"+enddate+"',"
              " yearstatus  = '"+status+"'"
              " WHERE "
              " id = '"+editid+"'";
    }
    if(conn.execQuery(sql))
    {
        QMessageBox::information(this,"","Record saved successfully");
        loadtable();
        ui->description->clear();
        ui->begindate->setDate(QDate::currentDate());
        ui->enddate->setDate(QDate::currentDate());
        ui->status->setChecked(true);
        editid="";
        ui->lbl_id->clear();
        ui->lbl_id->setVisible(false);
    }
    else
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
    }
}

void FinancialYear::on_edit_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        int row = ui->detailtable->currentRow();
        QString id = ui->detailtable->item(row,ID)->text();
        QString title = ui->detailtable->item(row,TITLE)->text();
        QSqlQuery qry;
        qry.prepare(" select * from tblfinancialyear WHERE id = '"+id+"' ");
        if(qry.exec())
        {
            ui->lbl_id->setVisible(true);
            editid = id;
            ui->lbl_id->setText("Year: "+title);
            qry.next();
            ui->description->setText( qry.value("description").toString() );
            ui->begindate->setDate( qry.value("startdate").toDate() );
            ui->enddate->setDate( qry.value("enddate").toDate() );
            ui->status->setChecked( qry.value("yearstatus").toBool() );
        }
    }
    else
    {
        QMessageBox::warning(this,"","Select row for edit");
    }
}

void FinancialYear::on_clear_btn_clicked()
{
    ui->description->clear();
    ui->begindate->setDate(QDate::currentDate());
    ui->enddate->setDate(QDate::currentDate());
    ui->status->setChecked(true);
    ui->lbl_id->clear();
    editid="";
    ui->lbl_id->setVisible(false);
}
