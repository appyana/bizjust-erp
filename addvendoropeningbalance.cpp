#include "addvendoropeningbalance.h"
#include "ui_addvendoropeningbalance.h"

AddVendorOpeningBalance::AddVendorOpeningBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddVendorOpeningBalance)
{
    ui->setupUi(this);
    loadform();
}

AddVendorOpeningBalance::~AddVendorOpeningBalance()
{
    delete ui;
}


void AddVendorOpeningBalance::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddVendorOpeningBalance::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddVendorOpeningBalance::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddVendorOpeningBalance::on_vendorname_textEdited(const QString &arg1)
{
   QString sql = sch.vendors(arg1);
   autocompleter(sql,ui->vendorname, ui->vendorid);
}

void AddVendorOpeningBalance::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->detailtable->setColumnCount(4);
    QStringList titles;
    titles <<"Vendor"<<"Vendor Id"<<"Debit"<<"Credit";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(VENDORID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(VENDORNAME, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(DEBIT,100);
    ui->detailtable->setColumnWidth(CREDIT,100);
    ui->btn_save->setEnabled(false);
}

void AddVendorOpeningBalance::on_btn_add_clicked()
{
    QString vendorname = ui->vendorname->text();
    QString vendorid = ui->vendorid->text();
    float debit = ui->debit->value();
    float credit = ui->credit->value();
    float total = debit+credit;
    if(vendorid=="")
    {
        QMessageBox::critical(this,"","Please select vendor");
        ui->vendorname->setFocus();
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
        ui->detailtable->setItem(row,VENDORNAME, new QTableWidgetItem(vendorname));
        ui->detailtable->setItem(row,VENDORID,   new QTableWidgetItem(vendorid));
        ui->detailtable->setItem(row,DEBIT,      new QTableWidgetItem( erp.DecimalString(debit) ));
        ui->detailtable->setItem(row,CREDIT,     new QTableWidgetItem( erp.DecimalString(credit) ));
        checktotal();
        ui->vendorname->clear();
        ui->vendorid->clear();
        ui->debit->setValue(0.00);
        ui->credit->setValue(0.00);
        ui->vendorname->setFocus();
    }
}

void AddVendorOpeningBalance::checktotal()
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



void AddVendorOpeningBalance::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
        checktotal();
    }
}

void AddVendorOpeningBalance::on_btn_save_clicked()
{
    QString entrydate = ui->entrydate->text();
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
        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString ob_code = "OB";
            QString ob_num = erp.get_num(ob_code);
            QString vno = ob_code+"-"+ob_num;
            erp.update_ids_num(ob_code);
            QString description = "vendor opening balance";
            QString vendorid   = ui->detailtable->item(i,VENDORID)->text();
            float total_debit  = ui->detailtable->item(i,DEBIT)->text().toFloat();
            float total_credit = ui->detailtable->item(i,CREDIT)->text().toFloat();
            float total_amount = total_debit+total_credit;
            int ln = 0;
            if(total_amount!=0 && vendorid!="")
            {
                QString query_transaction = "INSERT INTO `tblgltransvoucher` "
                                            " (	`voucherno` ,`voucherno_parent`,`description` ,`entrydate` ,`comments` ,`companyid` ,`total_debit` ,`total_credit` ,`wherefrom`,`companytype`, `financialyearid`) "
                                            " VALUES "
                                            " ('"+vno+"', '' ,'"+description+"', '"+entrydate+"', '"+description+"', '"+vendorid+"', '"+erp.DecimalString(total_amount)+"', '"+erp.DecimalString(total_amount)+"', '1', 'vendor', '"+erp._yearid+"')";
                if(!conn.execQuery(query_transaction))
                {
                    QMessageBox::critical(this,"","Some problem in record insertion");
                    return;
                }
                QString voucher_id = erp.getvoucherid(vno);

                QString event = "Add";
                QString transtype = "Vendor Opening Balance";
                QString transid = vno ;
                erp.AddToEventLog(event,transtype,transid);
                if(voucher_id.toInt()>0)
                {
                    float debit = total_debit;
                    float credit = total_credit;
                    QString a = "credit";
                    float amount=credit;
                    if(debit>0)
                    {
                        a="debit";
                        amount=debit;
                    }

                    QString p_acct = erp.getaccountAP("payable");
                    ln++;
                    QString query_cr = "INSERT INTO `tblgltransaction` "
                                       " ( `voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, "+a+", `linenumber`, `entrydate`,`companytype`,`compid`) "
                                       " VALUES "
                                       " ( '"+vno+"', '' ,'"+voucher_id+"', '"+p_acct+"', '"+erp.DecimalString(amount)+"','"+erp.intString(ln)+"', '"+entrydate+"','vendor', '"+vendorid+"')";
                    if(!conn.execQuery(query_cr))
                    {
                        QMessageBox::critical(this,"","Some problem in record insertion");
                        return;
                    }
                    if(a=="credit"){ a="debit"; }else{ a="credit"; }
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

        QMessageBox::information(this,"","Vendor opening balance inserted successfully");
        ui->detailtable->setRowCount(0);
        ui->vendorname->clear();
        ui->vendorid->clear();
        ui->debit->setValue(0.00);
        ui->credit->setValue(0.00);
        ui->entrydate->setFocus();
        checktotal();

    }
}

void AddVendorOpeningBalance::on_debit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->credit->setValue(0.00);
    }
}

void AddVendorOpeningBalance::on_credit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->debit->setValue(0.00);
    }
}
