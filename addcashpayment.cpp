#include "addcashpayment.h"
#include "ui_addcashpayment.h"

AddCashPayment::AddCashPayment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddCashPayment)
{
    ui->setupUi(this);
    loadform();
}

AddCashPayment::~AddCashPayment()
{
    delete ui;
}


void AddCashPayment::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddCashPayment::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddCashPayment::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddCashPayment::on_accountname_textEdited(const QString &arg1)
{
   QString sql = sch.glaccount_generalize(arg1);
   autocompleter(sql,ui->accountname, ui->accountid);
}

void AddCashPayment::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->detailtable->setColumnCount(4);
    QStringList titles;
    titles <<"Account"<<"Account Id"<<"Description"<<"Amount";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ACCOUNTID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(ACCOUNTNAME, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(DESCRIPTION,250);
    ui->detailtable->setColumnWidth(AMOUNT,100);
    ui->btn_save->setEnabled(false);
}

void AddCashPayment::on_btn_add_clicked()
{
    QString accountname = ui->accountname->text();
    QString accountid = ui->accountid->text();
    QString description = ui->description->text();
    float amount = ui->amount->value();
    if(accountid=="")
    {
        QMessageBox::critical(this,"","Please select account");
        ui->accountname->setFocus();
        return;
    }
    else if(amount<=0)
    {
        QMessageBox::critical(this,"","Please enter amount");
        ui->amount->setFocus();
        return;
    }
    else
    {
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,ACCOUNTNAME, new QTableWidgetItem(accountname));
        ui->detailtable->setItem(row,ACCOUNTID, new QTableWidgetItem(accountid));
        ui->detailtable->setItem(row,DESCRIPTION, new QTableWidgetItem(description));
        ui->detailtable->setItem(row,AMOUNT, new QTableWidgetItem( erp.DecimalString(amount) ));
        checktotal();
        ui->accountname->clear();
        ui->accountid->clear();
        ui->description->clear();
        ui->amount->setValue(0.00);
        ui->accountname->setFocus();
    }
}

void AddCashPayment::checktotal()
{
    float tot_amt=0;
    for(int i=0; i<ui->detailtable->rowCount(); i++)
    {
        float amt = ui->detailtable->item(i,AMOUNT)->text().toFloat();
        tot_amt += amt;
    }
    ui->total->setValue(tot_amt);
    if(ui->detailtable->rowCount()>0)
    {
        ui->btn_save->setEnabled(true);
    }
    else
    {
        ui->btn_save->setEnabled(false);
    }
}



void AddCashPayment::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
        checktotal();
    }
}

void AddCashPayment::on_btn_save_clicked()
{
    QString entrydate = ui->entrydate->date().toString("yyyy-MM-dd");
    float total_amount = ui->total->value();
    if(entrydate=="")
    {
        QMessageBox::critical(this,"","Transaction date is required");
        ui->entrydate->setFocus();
        return;
    }
    else if(total_amount<=0)
    {
        QMessageBox::critical(this,"","Total must be greater than 0.00");
        return;
    }
    else
    {
        QString jv_code = "cpv";
        QString jv_num = erp.get_num(jv_code);
        QString voucher = jv_code+"-"+jv_num;
        int lines = ui->detailtable->rowCount();
        float total_debit = total_amount;
        float total_credit = total_amount;
        QString wherefrom = "3";
        QString query = "insert into tblgltransvoucher "
                        " (voucherno, entrydate, total_debit, total_credit, wherefrom, description, financialyearid,`lines`) "
                        " values "
                        " ('"+voucher+"', '"+entrydate+"', '"+erp.DecimalString(total_debit)+"', '"+erp.DecimalString(total_credit)+"', '"+wherefrom+"','', '"+erp._yearid+"','"+erp.intString(lines)+"')";
        if(!conn.execQuery(query))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString voucherid = erp.getvoucherid(voucher);
        erp.update_ids_num(jv_code);
        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString description = ui->detailtable->item(i,DESCRIPTION)->text();
            QString glaccountid = ui->detailtable->item(i,ACCOUNTID)->text();
            float amount = ui->detailtable->item(i,AMOUNT)->text().toFloat();
            int ln = 1;
            query = "insert into tblgltransaction "
                    " ( voucherno, voucherid, description, glaccountid, debit, credit, linenumber, `entrydate`,`companytype`, financialyearid) "
                    " values"
                    " ('"+voucher+"', '"+voucherid+"', '"+description+"', '"+glaccountid+"', '"+erp.DecimalString(amount)+"', '0.00', '"+erp.intString(ln)+"', '"+entrydate+"','glpayment', '"+erp._yearid+"')";
            if(!conn.execQuery(query))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
            ln++;
            glaccountid = erp.getaccountAR("cash");
            description = "System account";
            query = "insert into tblgltransaction"
                    " ( voucherno, voucherid, description, glaccountid, debit, credit, linenumber, `entrydate`,`companytype`, financialyearid) "
                    " values "
                    " ('"+voucher+"', '"+voucherid+"', '"+description+"', '"+glaccountid+"', '0.00', '"+erp.DecimalString(amount)+"', '"+erp.intString(ln)+"', '"+entrydate+"','','"+erp._yearid+"')";
            if(!conn.execQuery(query))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
        }
        QString event = "New";
        QString transtype = "Cash Payment";
        QString transid = voucher;
        erp.AddToEventLog(event,transtype,transid);
        QMessageBox::information(this,"","Voucher  "+ voucher +" is inserted successfully.");
        ui->detailtable->setRowCount(0);
        ui->accountname->clear();
        ui->accountid->clear();
        ui->description->clear();
        ui->amount->setValue(0.00);
        ui->entrydate->setDate(QDate::currentDate());
        ui->entrydate->setFocus();
    }
}
