#include "addglopeningbalance.h"
#include "ui_addglopeningbalance.h"

AddGLOpeningBalance::AddGLOpeningBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddGLOpeningBalance)
{
    ui->setupUi(this);
    loadform();
}

AddGLOpeningBalance::~AddGLOpeningBalance()
{
    delete ui;
}


void AddGLOpeningBalance::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
{
    sch.name_txt = name_txt;
    sch.id_txt = id_txt;
    QMap<int, QString> data = sch.data;
    //conn.connOpen();
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        while(qry.next())
        {
            data[qry.value(0).toInt()] = qry.value(1).toString();
        }
    }
    //conn.connClose();
    /*data[2] = "Moscow";
    data[4] = "London";
    data[6] = "Paris";*/
    QCompleter *completer = new QCompleter(this);
    QStandardItemModel *model = new QStandardItemModel(completer);
    QMapIterator<int, QString> it(data);
    while (it.hasNext())
    {
        it.next();
        int code = it.key();
        QString name = it.value();
        QStandardItem *item = new QStandardItem;
        item->setText(name);
        item->setData(code, Qt::UserRole);
        model->appendRow(item);
    }
    completer->setModel(model);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCurrentRow(0);
    completer->setFilterMode(Qt::MatchContains);
    name_txt->setCompleter(completer);


    connect(completer, SIGNAL(highlighted(QModelIndex)),this,SLOT(onItemHighlighted(QModelIndex)),Qt::QueuedConnection);
    connect(name_txt,SIGNAL(editingFinished()),this,SLOT(editingFinished() ));
}

void AddGLOpeningBalance::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddGLOpeningBalance::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddGLOpeningBalance::on_accountname_textEdited(const QString &arg1)
{
   QString sql = sch.glaccount_generalize(arg1);
   autocompleter(sql,ui->accountname, ui->accountid);
}

void AddGLOpeningBalance::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->detailtable->setColumnCount(5);
    QStringList titles;
    titles <<"Account"<<"Account Id"<<"Description"<<"Debit"<<"Credit";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ACCOUNTID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(ACCOUNTNAME, QHeaderView::Stretch);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(DESCRIPTION, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(DEBIT,100);
    ui->detailtable->setColumnWidth(CREDIT,100);
    ui->btn_save->setEnabled(false);
}

void AddGLOpeningBalance::on_btn_add_clicked()
{
    QString accountname = ui->accountname->text();
    QString accountid = ui->accountid->text();
    float debit = ui->debit->value();
    float credit = ui->credit->value();
    float total = debit+credit;
    if(accountid=="")
    {
        QMessageBox::critical(this,"","Please select Account");
        ui->accountname->setFocus();
        return;
    }
    else if(total<=0)
    {
        QMessageBox::critical(this,"","Please enter debit or credit");
        ui->debit->setFocus();
        return;
    }
    else
    {
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,ACCOUNTNAME, new QTableWidgetItem(accountname));
        ui->detailtable->setItem(row,ACCOUNTID,   new QTableWidgetItem(accountid));
        ui->detailtable->setItem(row,DESCRIPTION,   new QTableWidgetItem(ui->description->text()));
        ui->detailtable->setItem(row,DEBIT,      new QTableWidgetItem( erp.DecimalString(debit) ));
        ui->detailtable->setItem(row,CREDIT,     new QTableWidgetItem( erp.DecimalString(credit) ));
        checktotal();
        ui->accountname->clear();
        ui->accountid->clear();
        ui->debit->setValue(0.00);
        ui->credit->setValue(0.00);
        ui->accountname->setFocus();
    }
}

void AddGLOpeningBalance::checktotal()
{
    float tot_debit=0;
    float tot_credit=0;
    for(int i=0; i<ui->detailtable->rowCount(); i++)
    {
        float debit = ui->detailtable->item(i,DEBIT)->text().toFloat();
        float credit = ui->detailtable->item(i,CREDIT)->text().toFloat();
        tot_debit += debit;
        tot_credit += credit;
    }
    ui->total_debit->setValue(tot_debit);
    ui->total_credit->setValue(tot_credit);
    if(ui->detailtable->rowCount()>0)
    {
        ui->btn_save->setEnabled(true);
    }
    else
    {
        ui->btn_save->setEnabled(false);
    }
}



void AddGLOpeningBalance::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
        checktotal();
    }
}

void AddGLOpeningBalance::on_btn_save_clicked()
{
    QString entrydate = ui->entrydate->date().toString("yyyy-MM-dd");
    float total = ui->total_debit->value() + ui->total_credit->value();
    if(entrydate=="")
    {
        QMessageBox::critical(this,"","Transaction date is required");
        ui->entrydate->setFocus();
        return;
    }
    else if(total==0)
    {
        QMessageBox::critical(this,"","Total Debit or Total Credit must not be 0.00");
        return;
    }
    else
    {
        QString ob_code = "OB";
        QString ob_num = erp.get_num(ob_code);
        QString vno = ob_code+"-"+ob_num;
        erp.update_ids_num(ob_code);
        float total_debit  = ui->total_debit->value();
        float total_credit = ui->total_credit->value();
        float total_amount = total_debit+total_credit;
        QString wherefrom = "3";
        QString description = "gl opening balance";
        QString query_transaction = "INSERT INTO `tblgltransvoucher` "
                                    " (	`voucherno` ,`description` ,`entrydate` ,`total_debit` ,`total_credit` ,`wherefrom`, `financialyearid`) "
                                    " VALUES "
                                    " ('"+vno+"' ,'"+description+"', '"+entrydate+"', '"+erp.DecimalString(total_amount)+"', '"+erp.DecimalString(total_amount)+"', '"+wherefrom+"', '"+erp._yearid+"')";
        if(!conn.execQuery(query_transaction))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString voucherid = erp.getvoucherid(vno);
        QString event = "Add";
        QString transtype = "GL Opening Balance";
        QString transid = vno ;
        erp.AddToEventLog(event,transtype,transid);
        QString voucher = vno;
        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString glaccountid   = ui->detailtable->item(i,ACCOUNTID)->text();
            int ln = 0;
            if(glaccountid!="")
            {
                description = ui->detailtable->item(i,DESCRIPTION)->text();
                float debit = ui->detailtable->item(i,DEBIT)->text().toFloat();
                float credit = ui->detailtable->item(i,CREDIT)->text().toFloat();
                ln++;
                QString query = "insert into tblgltransaction "
                                " ( voucherno, voucherid, glaccountid, description, debit, credit, linenumber, `entrydate`, financialyearid) "
                                " values "
                                " ('"+voucher+"', '"+voucherid+"', '"+glaccountid+"', '"+description+"', '"+erp.DecimalString(debit)+"', '"+erp.DecimalString(credit)+"', '"+erp.intString(ln)+"', '"+entrydate+"', '"+erp._yearid+"')";
                if(!conn.execQuery(query))
                {
                    QMessageBox::critical(this,"","Some problem in record insertion");
                    return;
                }
            }
        }
        QMessageBox::information(this,"","Opening balance "+voucher+" inserted successfully");
        ui->detailtable->setRowCount(0);
        ui->accountname->clear();
        ui->accountid->clear();
        ui->debit->setValue(0.00);
        ui->credit->setValue(0.00);
        ui->entrydate->setFocus();
        checktotal();
    }
}

void AddGLOpeningBalance::on_debit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->credit->setValue(0.00);
    }
}

void AddGLOpeningBalance::on_credit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->debit->setValue(0.00);
    }
}
