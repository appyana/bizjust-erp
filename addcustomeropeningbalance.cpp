#include "addcustomeropeningbalance.h"
#include "ui_addcustomeropeningbalance.h"

AddCustomerOpeningBalance::AddCustomerOpeningBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddCustomerOpeningBalance)
{
    ui->setupUi(this);
    loadform();
}

AddCustomerOpeningBalance::~AddCustomerOpeningBalance()
{
    delete ui;
}


void AddCustomerOpeningBalance::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddCustomerOpeningBalance::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddCustomerOpeningBalance::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddCustomerOpeningBalance::on_customername_textEdited(const QString &arg1)
{
   QString sql = sch.customers(arg1);
   autocompleter(sql,ui->customername, ui->customerid);
}

void AddCustomerOpeningBalance::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->detailtable->setColumnCount(3);
    QStringList titles;
    titles <<"Customer"<<"Customer Id"<<"Amount";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(CUSTOMERID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(CUSTOMERNAME, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(AMOUNT,100);
    ui->btn_save->setEnabled(false);
}

void AddCustomerOpeningBalance::on_btn_add_clicked()
{
    QString customername = ui->customername->text();
    QString customerid = ui->customerid->text();
    float amount = ui->amount->value();
    if(customerid=="")
    {
        QMessageBox::critical(this,"","Please select customer");
        ui->customername->setFocus();
        return;
    }
    else if(amount==0)
    {
        QMessageBox::critical(this,"","Please enter amount");
        ui->amount->setFocus();
        return;
    }
    else
    {
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,CUSTOMERNAME, new QTableWidgetItem(customername));
        ui->detailtable->setItem(row,CUSTOMERID, new QTableWidgetItem(customerid));
        ui->detailtable->setItem(row,AMOUNT, new QTableWidgetItem( erp.DecimalString(amount) ));
        checktotal();
        ui->customername->clear();
        ui->customerid->clear();
        ui->amount->setValue(0.00);
        ui->customername->setFocus();
    }
}

void AddCustomerOpeningBalance::checktotal()
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



void AddCustomerOpeningBalance::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
        checktotal();
    }
}

void AddCustomerOpeningBalance::on_btn_save_clicked()
{
    QString entrydate = ui->entrydate->date().toString("yyyy-MM-dd");
    float total = ui->total->value();
    if(entrydate=="")
    {
        QMessageBox::critical(this,"","Transaction date is required");
        ui->entrydate->setFocus();
        return;
    }
    else if(total==0)
    {
        QMessageBox::critical(this,"","Total must not be 0.00");
        return;
    }
    else
    {
        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString ob_code = "OB";
            QString ob_num = erp.get_num(ob_code);
            QString vno = ob_code+"-"+ob_num;
            erp.update_ids_num(ob_code);
            QString description = "customer opening balance";
            QString customerid = ui->detailtable->item(i,CUSTOMERID)->text();
            float total_amount = ui->detailtable->item(i,AMOUNT)->text().toFloat();
            //QString wherefrom = "1";
            int ln = 0;
            if(total_amount!=0 && customerid!="")
            {
                QString query_transaction = "INSERT INTO `tblgltransvoucher` "
                                            " (	`voucherno` ,`voucherno_parent`,`description` ,`entrydate` ,`comments` ,`companyid` ,`total_debit` ,`total_credit` ,`wherefrom`,`companytype`, financialyearid) "
                                            " VALUES "
                                            " ('"+vno+"', '' ,'"+description+"', '"+entrydate+"', '"+description+"', '"+customerid+"', '"+erp.DecimalString(total_amount)+"', '"+erp.DecimalString(total_amount)+"','1','customer', '"+erp._yearid+"')";
                if(!conn.execQuery(query_transaction))
                {
                    QMessageBox::critical(this,"","Some problem in record insertion");
                    return;
                }
                QString voucher_id = erp.getvoucherid(vno);

                QString event = "Add";
                QString transtype = "Customer Opening Balance";
                QString transid = vno ;
                erp.AddToEventLog(event,transtype,transid);
                if(voucher_id.toInt()>0)
                {
                    QString a = "debit";
                    float amount = total_amount;
                    if(amount<0)
                    {
                        a = "credit";
                        amount = amount * (-1);
                    }
                    QString p_acct = erp.getaccountAR("receivables");
                    ln++;
                    QString query_cr = "INSERT INTO `tblgltransaction` "
                                       " ( `voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, "+a+", `linenumber`, `entrydate`,`companytype`,`compid`) "
                                       " VALUES "
                                       " ( '"+vno+"', '' ,'"+voucher_id+"', '"+p_acct+"', '"+erp.DecimalString(amount)+"','"+erp.intString(ln)+"', '"+entrydate+"','customer', '"+customerid+"')";
                    if(!conn.execQuery(query_cr))
                    {
                        QMessageBox::critical(this,"","Some problem in record insertion");
                        return;
                    }
                    if(a=="credit"){a="debit";}else{a="credit";}
                    QString glaccountid = erp.getaccountAR("suspect");
                    ln++;
                    QString query_dr = "INSERT INTO `tblgltransaction` "
                                       " ( `voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, "+a+", `linenumber`, `entrydate`,`companytype`) "
                                       " VALUES "
                                       " ( '"+vno+"', '' ,'"+voucher_id+"', '"+glaccountid+"', '"+erp.DecimalString(amount)+"','"+erp.intString(ln)+"', '"+entrydate+"','')";
                    if(!conn.execQuery(query_dr))
                    {
                        QMessageBox::critical(this,"","Some problem in record insertion");
                        return;
                    }
                }
            }
        }

        QMessageBox::information(this,"","Customer opening balance inserted successfully");
        ui->detailtable->setRowCount(0);
        ui->customername->clear();
        ui->customerid->clear();
        ui->amount->setValue(0.00);
        ui->entrydate->setFocus();

    }
}
