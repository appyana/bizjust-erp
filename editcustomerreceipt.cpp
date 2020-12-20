#include "editcustomerreceipt.h"
#include "ui_editcustomerreceipt.h"

EditCustomerReceipt::EditCustomerReceipt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditCustomerReceipt)
{
    ui->setupUi(this);
    ui->recievedate->setDate(QDate::currentDate());
    ui->duedate->setDate(QDate::currentDate());
    loadpaymenth();
    ui->SaveButton->setEnabled(false);
    loadsearchtable();
}

EditCustomerReceipt::~EditCustomerReceipt()
{
    delete ui;
}

void EditCustomerReceipt::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditCustomerReceipt::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditCustomerReceipt::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditCustomerReceipt::on_customername_textEdited(const QString &arg1)
{
    QString str = sch.customers(arg1);
    autocompleter(str,ui->customername, ui->customerid);
}

void EditCustomerReceipt::loadpaymenth()
{
    QSqlQuery qry;
    qry.prepare("select * from tblpaymeth where status = '1' and id in('2','5') order by seqno");
    if(qry.exec())
    {
        while(qry.next())
        {
            ui->paymeth->addItem(qry.value("paymeth").toString(), QVariant(qry.value("id")) );
        }
    }
}

void EditCustomerReceipt::on_paymeth_currentIndexChanged(int index)
{
    int paymeth = ui->paymeth->itemData(index).toInt();
    if(paymeth==5)
    {
        ui->cheque_td->setText("Purchie #:");
        ui->customer_bank_th->setText("Purchie Details:");
        ui->cheque_date_th->setText("Purchie Date:");
        ui->our_bank_th->setText("Purchie Account:");
        ui->depositdescription->setText("");
        ui->deposit->setText("");
    }
    else
    {
        ui->cheque_td->setText("Cheque #:");
        ui->customer_bank_th->setText("Customer Bank:");
        ui->cheque_date_th->setText("Cheque Date:");
        ui->our_bank_th->setText("Our Bank:");
        ui->depositdescription->setText("");
        ui->deposit->setText("");
    }
}

void EditCustomerReceipt::on_depositdescription_textEdited(const QString &arg1)
{
    QString sql = sch.glaccount_generalize(arg1);
    autocompleter(sql,ui->depositdescription, ui->deposit);
}

void EditCustomerReceipt::on_customerid_textChanged(const QString &arg1)
{
    ui->label_balance->setText(erp.customerBalance(arg1,QDate::currentDate().toString("yyyy-MM-dd")));
}

void EditCustomerReceipt::on_SaveButton_clicked()
{
    if(isValid())
    {
        QString voucherno = ui->lbl_vno->text();
        QString recievedate     = ui->recievedate->date().toString("yyyy-MM-dd");
        QString customerid      = ui->customerid->text();
        float amount            = ui->amount->value();
        int index               = ui->paymeth->currentIndex();
        QString paymeth         = ui->paymeth->itemData(index).toString();
        QString chequenum       = ui->chequenum->text();
        QString bank            = ui->bank->text();
        QString dueDate         = ui->duedate->date().toString("yyyy-MM-dd");
        QString deposit         = ui->deposit->text();
        QString description     = ui->description->text();

        QString pmt_code = "PMT";
        //QString pmt_num = voucherno.split(QRegExp("-")).value(1);// erp.get_num(pmt_code);
        QString vno = voucherno;//pmt_code+"-"+pmt_num;

        QString sql = "UPDATE `tblgltransvoucher` "
                      " SET "
                      "`description`    = '"+description+"',	"
                      "`entrydate`      = '"+recievedate+"', "
                      "`comments`       = 'Payment Received', "
                      "`companyid`      = '"+customerid+"', "
                      "`total_debit`    = '"+erp.DecimalString(amount)+"', "
                      "`total_credit`   = '"+erp.DecimalString(amount)+"',"
                      "`companytype`    = 'customer' "
                      " WHERE "
                      " `voucherno` = '"+vno+"' ";
        QSqlQuery qry;
        qry.prepare(sql);
        if(!qry.exec())
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }
        QString voucher_id = erp.getvoucherid(vno);// qry.lastInsertId().toString();
        QString account_ID = deposit;

        QString sql2 = "UPDATE `tblinvoicepaymentdetail` "
                       " SET "
                       "`invoiceid`     = '', "
                       "`amount`        = '"+erp.DecimalString(amount)+"', "
                       "`datereceived`  = '"+recievedate+"', "
                       "`paymeth`       = '"+paymeth+"', "
                       "`checkNum`      = '"+chequenum+"', "
                       "`bank`          = '"+bank+"', "
                       "`dueDate`       = '"+dueDate+"', "
                       "`bankdepositid` = '"+deposit+"',"
                       "`customer_id`   = '"+customerid+"', "
                       "`ourbank`       = '"+deposit+"', "
                       "`description`   = '"+description+"'  "
                       " WHERE "
                       " `voucherid`     = '"+vno+"'  ";
        if(!conn.execQuery(sql2))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }


        if(!conn.execQuery(" DELETE FROM tblgltransaction WHERE voucherno = '"+vno+"' "))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }


        int line_no=1;
        sql = "INSERT INTO `tblgltransaction` "
              " ( `voucherno`, `voucherid`, `glaccountid`, `debit`, `linenumber`, `entrydate`,`companytype`, description) "
              " VALUES "
              " ( '"+vno+"', '"+voucher_id+"', '"+account_ID+"', '"+erp.DecimalString(amount)+"',  '"+erp.intString(line_no)+"', '"+recievedate+"','','"+description+"')";
        if(!conn.execQuery(sql))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }
        line_no++;
        QString account_ID_AR = erp.getaccountAR("receivables");
        sql = "INSERT INTO `tblgltransaction` "
              " ( `voucherno`, `voucherid`, `glaccountid`, `credit`, `linenumber`, `entrydate`,`companytype`, description,compid) "
              " VALUES "
              " ( '"+vno+"', '"+voucher_id+"', '"+account_ID_AR+"', '"+erp.DecimalString(amount)+"',  '"+erp.intString(line_no)+"', '"+recievedate+"','customer', '"+description+"', '"+customerid+"')";
        if(!conn.execQuery(sql))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }

        erp.update_ids_num(pmt_code);

        QString event = "Update";
        QString transtype = "Customer Payment";
        QString transid = vno ;
        erp.AddToEventLog(event,transtype,transid);

        QString msg = "Customer Payment "+vno+" saved Successfully!";
        QMessageBox::information(this,"Information",msg);
    }
}

void EditCustomerReceipt::clearform()
{
    ui->customername->clear();
    ui->customerid->clear();
    ui->amount->clear();
    ui->description->clear();
    ui->bank->clear();
    ui->chequenum->clear();
    ui->depositdescription->clear();
    ui->deposit->clear();
    ui->recievedate->setFocus();
    ui->lbl_vno->clear();
    ui->SaveButton->setEnabled(false);
}

bool EditCustomerReceipt::isValid()
{
    if(ui->recievedate->text()=="")
    {
        QMessageBox::critical(this,"Validation Error","Date Received is required");
        ui->customername->setFocus();
        return false;
    }
    else if(ui->customerid->text().toInt()==0)
    {
        QMessageBox::critical(this,"Validation Error","Please Select Customer");
        ui->customername->setFocus();
        return false;
    }
    else if(ui->amount->value()==0)
    {
        QMessageBox::critical(this,"Validation Error","Please enter amount");
        ui->amount->setFocus();
        return false;
    }
    else if(ui->chequenum->text()=="" || ui->bank->text()=="" || ui->duedate->text()=="" || ui->deposit->text()=="" )
    {
        QMessageBox::critical(this,"Validation Error","All Fields are required");
        ui->amount->setFocus();
        return false;
    }
    else
    {
        return true;
    }
}

void EditCustomerReceipt::on_search_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->search_customername, ui->search_customerid);
}
void EditCustomerReceipt::loadsearchtable()
{
    ui->searchTable->setColumnCount(4);
    QStringList titles;
    titles <<"Voucher #"<<"Voucher Date"<<"Customer"<<"Amount";
    ui->searchTable->setHorizontalHeaderLabels(titles);
    ui->searchTable->setColumnWidth(VOUCHER,80);
    ui->searchTable->setColumnWidth(VDATE,80);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(CUSTOMER, QHeaderView::Stretch);
    ui->searchTable->setColumnWidth(AMOUNT,100);
}
void EditCustomerReceipt::on_search_btn_clicked()
{
    clearform();
    ui->searchTable->setRowCount(0);
    QString sql = "select * from tblgltransvoucher tv "
            " inner join tblinvoicepaymentdetail pd on (tv.voucherno = pd.voucherid) "
            " where voucherno like 'pmt%' "
            " and pd.cancel =0 and pd.paymeth !='1'";
    if(ui->search_vno->text()!="")
    {
        sql += " and (tv.voucherno='"+ui->search_vno->text()+"' OR tv.voucherno='PMT-"+ui->search_vno->text()+"') ";
    }
    if(ui->search_customerid->text()!="")
    {
        sql += " and tv.companyid = '"+ui->search_customerid->text()+"' ";
    }

    sql += " group by tv.voucherid order by tv.voucherid DESC ";
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        while(qry.next())
        {
            ui->searchTable->insertRow(ui->searchTable->rowCount());
            int row = ui->searchTable->rowCount()-1;
            ui->searchTable->setItem(row, VOUCHER,   new QTableWidgetItem( qry.value("voucherno").toString() ));
            ui->searchTable->setItem(row, VDATE,     new QTableWidgetItem( qry.value("entrydate").toDate().toString("dd/MM/yyyy") ));
            ui->searchTable->setItem(row, CUSTOMER,  new QTableWidgetItem( erp.customerName(qry.value("companyid").toString()) ));
            ui->searchTable->setItem(row, AMOUNT,    new QTableWidgetItem( erp.DecimalString(qry.value("total_debit").toFloat()) ));
        }
    }
}

void EditCustomerReceipt::on_EditButton_clicked()
{
    if(ui->searchTable->currentRow()<0)
    {
        QMessageBox::critical(this,"Error","Please select voucher to edit");
    }
    else
    {
        QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VOUCHER)->text();
        ui->lbl_vno->setText(vno);
        QSqlQuery qry;
        qry.prepare(" select * from tblinvoicepaymentdetail where voucherid = '"+vno+"' ");
        qry.exec();
        qry.next();
        ui->recievedate->setDate(qry.value("datereceived").toDate());
        ui->customername->setText( erp.customerName(qry.value("customer_id").toString()) );
        ui->customerid->setText( qry.value("customer_id").toString() );
        ui->amount->setValue( qry.value("amount").toFloat() );
        ui->description->setText( qry.value("description").toString() );
        erp.setComboBoxSelected(ui->paymeth, qry.value("paymeth").toInt() );
        ui->chequenum->setText( qry.value("checkNum").toString() );
        ui->bank->setText( qry.value("bank").toString() );
        ui->duedate->setDate( qry.value("duedate").toDate() );
        ui->deposit->setText( qry.value("bankdepositid").toString() );
        ui->depositdescription->setText( erp.getAccount( qry.value("bankdepositid").toString() ) );
        ui->SaveButton->setEnabled(true);
    }
}

void EditCustomerReceipt::on_DeleteButton_clicked()
{
    if(ui->searchTable->currentRow()<0)
    {
        QMessageBox::critical(this,"Error","Please select voucher to delete");
    }
    else
    {
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes)
          {
                QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VOUCHER)->text();
                if(!conn.execQuery(" DELETE FROM tblgltransvoucher WHERE voucherno = '"+vno+"' "))
                {
                    QMessageBox::critical(this,"Error","Some problem in record insertion");
                    return;
                }
                else if(!conn.execQuery(" DELETE FROM tblgltransaction WHERE voucherno = '"+vno+"' "))
                {
                    QMessageBox::critical(this,"Error","Some problem in record insertion");
                    return;
                }
                else if(!conn.execQuery(" DELETE FROM tblinvoicepaymentdetail WHERE voucherid = '"+vno+"' "))
                {
                    QMessageBox::critical(this,"Error","Some problem in record insertion");
                    return;
                }
                else
                {
                    QMessageBox::information(this,"Info","Voucher deleted successfully");
                    on_search_btn_clicked();
                }
          }
    }

}
