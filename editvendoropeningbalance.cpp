#include "editvendoropeningbalance.h"
#include "ui_editvendoropeningbalance.h"

EditVendorOpeningBalance::EditVendorOpeningBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditVendorOpeningBalance)
{
    ui->setupUi(this);
    ui->edit_frame->setEnabled(false);
    loadform();
}

EditVendorOpeningBalance::~EditVendorOpeningBalance()
{
    delete ui;
}


void EditVendorOpeningBalance::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditVendorOpeningBalance::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditVendorOpeningBalance::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditVendorOpeningBalance::on_vendorname_textEdited(const QString &arg1)
{
   QString sql = sch.vendors(arg1);
   autocompleter(sql,ui->vendorname, ui->vendorid);
}

void EditVendorOpeningBalance::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->searchTable->setColumnCount(2);
    QStringList searchtitles;
    searchtitles <<"Voucher No"<<"Vendor";
    ui->searchTable->setHorizontalHeaderLabels(searchtitles);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(VENDOR, QHeaderView::Stretch);
    ui->searchTable->setColumnWidth(VOUCHER,100);
}

void EditVendorOpeningBalance::on_btn_save_clicked()
{
    QString entrydate = ui->entrydate->text();
    QString vno = ui->lbl_vno->text();
    QString description = "vendor opening balance";
    QString vendorid = ui->vendorid->text();
    float total_debit = ui->debit->value();
    float total_credit = ui->credit->value();
    float total_amount = total_debit + total_credit;
    if(vendorid=="")
    {
        QMessageBox::critical(this,"","Vendor is required");
        ui->vendorname->setFocus();
        return;
    }
    if(entrydate=="")
    {
        QMessageBox::critical(this,"","Transaction date is required");
        ui->entrydate->setFocus();
        return;
    }
    else if(total_amount==0)
    {
        QMessageBox::critical(this,"","Debit or Credit must be greater than 0.00");
        ui->debit->setFocus();
        return;
    }
    else
    {
        int ln = 0;
        if(total_amount!=0 && vendorid!="")
        {
            QString query_transaction = "UPDATE `tblgltransvoucher` "
                                        " SET "
                                        " `description`   = '"+description+"',"
                                        " `entrydate`     = '"+entrydate+"',"
                                        " `comments`      = '"+description+"',"
                                        " `companyid`     = '"+vendorid+"',"
                                        " `total_debit`   = '"+erp.DecimalString(total_amount)+"',"
                                        " `total_credit`  = '"+erp.DecimalString(total_amount)+"',"
                                        " `wherefrom`     = '1',"
                                        " `companytype`   = 'vendor', "
                                        " financialyearid = '"+erp._yearid+"' "
                                        " WHERE "
                                        " `voucherno`     = '"+vno+"' ";
            if(!conn.execQuery(query_transaction))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
            QString voucher_id = erp.getvoucherid(vno);
            if(!conn.execQuery(" delete from tblgltransaction where voucherno = '"+vno+"' "))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }

            QString event = "Update";
            QString transtype = "Vendor Opening Balance";
            QString transid = vno ;
            erp.AddToEventLog(event,transtype,transid);
            if(voucher_id.toInt()>0)
            {
                QString a = "debit";
                float amount = ui->debit->value();
                if(ui->credit->value()>0)
                {
                    a = "credit";
                    amount = ui->credit->value();
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

        QMessageBox::information(this,"","Vendor opening balance '"+vno+"' updated successfully");
        //ui->customername->clear();
        //ui->customerid->clear();
        //ui->amount->setValue(0.00);
        ui->entrydate->setFocus();

    }
}

void EditVendorOpeningBalance::on_search_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->search_vendorname,ui->search_vendorid);
}

void EditVendorOpeningBalance::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString vno = ui->search_voucherno->text();
    QString vendorid = ui->search_vendorid->text();
    QString query = " select * from tblgltransvoucher where  voucherno like '%ob-%' and voucherid>0 and companytype='vendor'" ;
    if(vno!="")
    {
        query += " and (voucherno = '"+vno+"' or voucherno = 'ob-"+vno+"') ";
    }
    if(vendorid!="")
    {
        query += " and companyid = '"+vendorid+"' ";
    }
    QSqlQuery info(query);
    while(info.next())
    {
        int row = ui->searchTable->rowCount();
        ui->searchTable->insertRow(row);
        ui->searchTable->setItem(row,VOUCHER, new QTableWidgetItem( info.value("voucherno").toString() ));
        ui->searchTable->setItem(row,VENDOR, new QTableWidgetItem( erp.vendorname(info.value("companyid").toString()) ));
    }
}

void EditVendorOpeningBalance::on_btn_edit_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VOUCHER)->text();
        QSqlQuery qry(" select * from tblgltransaction where voucherno = '"+vno+"' and companytype='vendor' ");
        qry.next();
        QString vendorid   = qry.value("compid").toString();
        QString vendorname = ui->searchTable->item(ui->searchTable->currentRow(),VENDOR)->text();
        QString entrydate    = qry.value("entrydate").toString();
        float debit         = qry.value("debit").toFloat();
        float credit         = qry.value("credit").toFloat();
        ui->entrydate->setDate(erp.SQLDateToQtDate(entrydate));
        ui->vendorname->setText(vendorname);
        ui->vendorid->setText(vendorid);
        ui->debit->setValue(debit);
        ui->credit->setValue(credit);
        ui->lbl_vno->setText(vno);
        ui->edit_frame->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,"","Select row for edit");
    }
}

void EditVendorOpeningBalance::on_btn_delete_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
                int row = ui->searchTable->currentRow();
                QString vno = ui->searchTable->item(row,VOUCHER)->text();
                if(conn.execQuery(" delete from tblgltransvoucher WHERE voucherno = '"+vno+"' ") && conn.execQuery(" delete from tblgltransaction WHERE voucherno = '"+vno+"' "))
                {
                    on_btn_search_clicked();
                    ui->vendorid->clear();
                    ui->vendorname->clear();
                    ui->debit->setValue(0.00);
                    ui->credit->setValue(0.00);
                    ui->lbl_vno->clear();
                    ui->edit_frame->setEnabled(false);
                    QMessageBox::information(this,"","Record deleted successfully");
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
        return;
    }
}

void EditVendorOpeningBalance::on_debit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->credit->setValue(0.00);
    }
}

void EditVendorOpeningBalance::on_credit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->debit->setValue(0.00);
    }
}
