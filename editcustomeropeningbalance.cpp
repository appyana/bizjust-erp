#include "editcustomeropeningbalance.h"
#include "ui_editcustomeropeningbalance.h"

EditCustomerOpeningBalance::EditCustomerOpeningBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditCustomerOpeningBalance)
{
    ui->setupUi(this);
    ui->edit_frame->setEnabled(false);
    loadform();
}

EditCustomerOpeningBalance::~EditCustomerOpeningBalance()
{
    delete ui;
}


void EditCustomerOpeningBalance::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditCustomerOpeningBalance::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditCustomerOpeningBalance::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditCustomerOpeningBalance::on_customername_textEdited(const QString &arg1)
{
   QString sql = sch.customers(arg1);
   autocompleter(sql,ui->customername, ui->customerid);
}

void EditCustomerOpeningBalance::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->searchTable->setColumnCount(2);
    QStringList searchtitles;
    searchtitles <<"Voucher No"<<"Customer";
    ui->searchTable->setHorizontalHeaderLabels(searchtitles);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(CUSTOMER, QHeaderView::Stretch);
    ui->searchTable->setColumnWidth(VOUCHER,100);
}

void EditCustomerOpeningBalance::on_btn_save_clicked()
{
    QString entrydate = ui->entrydate->text();
    QString vno = ui->lbl_vno->text();
    QString description = "customer opening balance";
    QString customerid = ui->customerid->text();
    float total_amount = ui->amount->value();
    if(customerid=="")
    {
        QMessageBox::critical(this,"","Customer is required");
        ui->customername->setFocus();
        return;
    }
    if(entrydate=="")
    {
        QMessageBox::critical(this,"","Transaction date is required");
        ui->entrydate->setFocus();
        return;
    }
    else if(ui->amount->value()==0)
    {
        QMessageBox::critical(this,"","Amount must not be 0.00");
        ui->amount->setFocus();
        return;
    }
    else
    {


        int ln = 0;
        if(total_amount!=0 && customerid!="")
        {
            QString query_transaction = "UPDATE `tblgltransvoucher` "
                                        " SET "
                                        " `description`   = '"+description+"',"
                                        " `entrydate`     = '"+entrydate+"',"
                                        " `comments`      = '"+description+"',"
                                        " `companyid`     = '"+customerid+"',"
                                        " `total_debit`   = '"+erp.DecimalString(total_amount)+"',"
                                        " `total_credit`  = '"+erp.DecimalString(total_amount)+"',"
                                        " `wherefrom`     = '1',"
                                        " `companytype`   = 'customer', "
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

        QMessageBox::information(this,"","Customer opening balance '"+vno+"' updated successfully");
        //ui->customername->clear();
        //ui->customerid->clear();
        //ui->amount->setValue(0.00);
        ui->entrydate->setFocus();

    }
}

void EditCustomerOpeningBalance::on_search_customername_textEdited(const QString &arg1)
{
    autocompleter(sch.customers(arg1),ui->search_customername,ui->search_customerid);
}

void EditCustomerOpeningBalance::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString vno = ui->search_voucherno->text();
    QString customerid = ui->search_customerid->text();
    QString query = " select * from tblgltransvoucher where  description = 'customer opening balance' and voucherid>0 and companytype='customer'" ;
    if(vno!="")
    {
        query += " and (voucherno = '"+vno+"' or voucherno = 'ob-"+vno+"') ";
    }
    if(customerid!="")
    {
        query += " and companyid = '"+customerid+"' ";
    }
    QSqlQuery info(query);
    while(info.next())
    {
        int row = ui->searchTable->rowCount();
        ui->searchTable->insertRow(row);
        ui->searchTable->setItem(row,VOUCHER, new QTableWidgetItem( info.value("voucherno").toString() ));
        ui->searchTable->setItem(row,CUSTOMER, new QTableWidgetItem( erp.customerName(info.value("companyid").toString()) ));
    }
}

void EditCustomerOpeningBalance::on_btn_edit_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VOUCHER)->text();
        QSqlQuery qry(" select * from tblgltransvoucher where voucherno = '"+vno+"' ");
        qry.next();
        QString customerid   = qry.value("companyid").toString();
        QString customername = ui->searchTable->item(ui->searchTable->currentRow(),CUSTOMER)->text();
        QString entrydate    = qry.value("entrydate").toString();
        float amount         = qry.value("total_debit").toFloat();
        ui->entrydate->setDate(erp.SQLDateToQtDate(entrydate));
        ui->customername->setText(customername);
        ui->customerid->setText(customerid);
        ui->amount->setValue(amount);
        ui->lbl_vno->setText(vno);
        ui->edit_frame->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,"","Select row for edit");
    }
}

void EditCustomerOpeningBalance::on_btn_delete_clicked()
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
                    ui->customerid->clear();
                    ui->customername->clear();
                    ui->amount->setValue(0.00);
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
