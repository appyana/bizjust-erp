#include "editchequeparchieencashed.h"
#include "ui_editchequeparchieencashed.h"

EditChequeParchieEncashed::EditChequeParchieEncashed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditChequeParchieEncashed)
{
    ui->setupUi(this);
    loadForm();
    loadSearchForm();
}

EditChequeParchieEncashed::~EditChequeParchieEncashed()
{
    delete ui;
}


void EditChequeParchieEncashed::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditChequeParchieEncashed::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditChequeParchieEncashed::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditChequeParchieEncashed::loadForm()
{
    ui->recievedate->setDate(QDate::currentDate());
    ui->duedate->setDate(QDate::currentDate());
    QSqlQuery qry;
    qry.prepare(" SELECT * FROM tblpaymeth WHERE id in(2,5) ORDER BY seqno ");
    if(qry.exec())
    {
        ui->paymeth->addItem("Select", QVariant(0) );
        while(qry.next())
        {
            ui->paymeth->addItem(qry.value("paymeth").toString(), QVariant(qry.value("id")) );
        }
    }

    QSqlQuery qry2;
    qry2.prepare(" SELECT * FROM tblpaymeth WHERE id in(1,2) ORDER BY seqno ");
    if(qry2.exec())
    {
        ui->method->addItem("Select", QVariant(0) );
        while(qry2.next())
        {
            ui->method->addItem(qry2.value("paymeth").toString(), QVariant(qry2.value("id")) );
        }
    }
    on_method_currentIndexChanged(0);
}

void EditChequeParchieEncashed::loadSearchForm()
{
    ui->searchTable->setColumnCount(4);
    QStringList titles;
    titles <<"Voucher #"<<"Invoice Date"<<"Customer"<<"Amount";
    ui->searchTable->setHorizontalHeaderLabels(titles);
    ui->searchTable->setColumnWidth(VNO,80);
    ui->searchTable->setColumnWidth(DATE,80);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(CUSTOMER, QHeaderView::Stretch);
    ui->searchTable->setColumnWidth(AMOUNT,80);
}

void EditChequeParchieEncashed::on_depositdescription_textEdited(const QString &arg1)
{
    QString sql = sch.glaccount_generalize(arg1);
    autocompleter(sql,ui->depositdescription,ui->deposit);
}

void EditChequeParchieEncashed::on_paymeth_currentIndexChanged(int index)
{
    QString paymeth = ui->paymeth->itemData(index).toString();
    erp.loadChequePurchieVouchers(ui->voucherno,paymeth);
    ui->method->setCurrentIndex(0);
}

void EditChequeParchieEncashed::on_voucherno_currentIndexChanged(int index)
{
    QString vno = ui->voucherno->itemData(index).toString();
    QSqlQuery qry1;
    qry1.prepare(" select sum(amount) as amount from tblencashpaymentdetail where pmtno='"+vno+"' ");
    qry1.exec();
    qry1.next();
    QSqlQuery qry;
    qry.prepare(" select * from tblinvoicepaymentdetail where voucherid = '"+vno+"' ");
    qry.exec();
    qry.next();
    float balance = qry.value("amount").toFloat() - qry1.value("amount").toFloat();
    ui->customername->setText( erp.customerName(qry.value("customer_id").toString()) );
    ui->customerid->setText( qry.value("customer_id").toString() );
    ui->balance->setText( erp.DecimalString(balance) );
    ui->desc->setText( qry.value("description").toString() );
    ui->desc2->setText( erp.customerName(qry.value("customer_id").toString())+" "+vno );
}

void EditChequeParchieEncashed::on_method_currentIndexChanged(int index)
{
    int method = ui->method->itemData(index).toInt();
    if(method==2)
    {
        ui->lbl_checkNum->setVisible(true);
        ui->checkNum->setVisible(true);
        ui->checkNum->clear();
        ui->lbl_bank->setVisible(true);
        ui->bank->setVisible(true);
        ui->bank->clear();
        ui->lbl_duedate->setVisible(true);
        ui->duedate->setVisible(true);
        ui->duedate->setDate(QDate::currentDate());
        ui->lbl_deposit->setVisible(true);
        ui->depositdescription->clear();
        ui->deposit->clear();
        ui->depositdescription->setVisible(true);
        ui->deposit->setVisible(true);
    }
    else
    {
        ui->lbl_checkNum->setVisible(false);
        ui->checkNum->setVisible(false);
        ui->checkNum->clear();
        ui->lbl_bank->setVisible(false);
        ui->bank->setVisible(false);
        ui->bank->clear();
        ui->lbl_duedate->setVisible(false);
        ui->duedate->setVisible(false);
        ui->duedate->setDate(QDate::currentDate());
        ui->lbl_deposit->setVisible(false);
        ui->depositdescription->clear();
        ui->deposit->clear();
        ui->depositdescription->setVisible(false);
        ui->deposit->setVisible(false);
    }
}

void EditChequeParchieEncashed::on_saveButton_clicked()
{
    bool valid = false;
    if(ui->lbl_vno->text()=="" )
    {
        valid = false;
        QMessageBox::critical(this,"Validation Error","Please select voucher to edit");
        return;
    }
    else if(ui->method->itemData(ui->method->currentIndex()).toInt()==0 )
    {
        valid = false;
        QMessageBox::critical(this,"Validation Error","Please select payment mode");
        ui->method->setFocus();
        return;
    }
    else if(ui->method->itemData(ui->method->currentIndex()).toInt()==1 )
    {
        if(ui->customerid->text().toInt()==0)
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Customer is required");
            return;
        }
        else if(ui->recievedate->text()=="")
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Date is required");
            return;
        }
        else if(ui->ammt->value()==0)
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Amount is required");
            return;
        }
        else if(ui->ammt->value()>ui->balance->text().toDouble())
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Amount must be equal or less than voucher balance");
            return;
        }
        else
        {
            valid = true;
        }
    }
    else
    {
        if(ui->customerid->text().toInt()==0)
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Customer is required");
            return;
        }
        else if(ui->recievedate->text()=="")
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Date is required");
            return;
        }
        else if(ui->ammt->value()==0)
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Ammount is required");
            return;
        }
        else if(ui->ammt->value()>ui->balance->text().toDouble())
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Amount must be equal or less than voucher balance");
            return;
        }
        else if(ui->checkNum->text()=="")
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Cheque # bank is required");
            return;
        }
        else if(ui->bank->text()=="")
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Customer bank is required");
            return;
        }
        else if(ui->duedate->text()=="")
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Cheque Date is required");
            return;
        }
        else if(ui->deposit->text()=="")
        {
            valid = false;
            QMessageBox::critical(this,"Validation Error","Our bank is required");
            return;
        }
        else
        {
            valid = true;
        }

    }//end else


    if(valid==true)
    {
        QString sql;
        QString customername = ui->customername->text();
        QString recievedate = ui->recievedate->date().toString("yyyy-MM-dd");
        QString customerid = ui->customerid->text();
        float amount = ui->ammt->value();
        int paymeth = ui->method->itemData(ui->method->currentIndex()).toInt();
        QString checkNum = ui->checkNum->text();
        QString bank = ui->bank->text();
        QString payment_no = ui->voucherno->itemData(ui->voucherno->currentIndex()).toString();
        float payment_amount = ui->balance->text().toFloat();
        QString dueDate = ui->duedate->date().toString("yyyy-MM-dd");
        QString deposit = ui->deposit->text();
        QString description = customername+" "+payment_no;
        if(ui->desc2->text()!="")
        {
            description = ui->desc2->text();
        }
        QString vno = ui->lbl_vno->text();

        sql = " UPDATE `tblgltransvoucher` "
              " SET "
              "`description` = '"+description+"',	"
              "`entrydate` = '"+recievedate+"', "
              "`comments` = 'Payment Received', "
              "`companyid` = '"+customerid+"', "
              "`total_debit` = '"+erp.DecimalString(amount)+"', "
              "`total_credit` = '"+erp.DecimalString(amount)+"',"
              "`companytype` = 'customer' "
              " WHERE "
              " `voucherno` = '"+vno+"' ";
        if(!conn.execQuery(sql))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }
        QString voucher_id = erp.getvoucherid(vno);
        QString account_ID;

        if(paymeth==1)
        {
            account_ID = erp.getaccountAR("cash");
        }
        else
        {
            account_ID = deposit;
        }

        sql = " UPDATE `tblencashpaymentdetail` "
              " SET "
              "`pmtno`          = '"+payment_no+"', "
              "`amount`         = '"+erp.DecimalString(amount)+"', "
              "`datereceived`   = '"+recievedate+"', "
              "`paymeth`        = '"+erp.intString(paymeth)+"', "
              "`checkNum`       = '"+checkNum+"', "
              "`bank`           = '"+bank+"', "
              "`dueDate`        = '"+dueDate+"', "
              "`bankdepositid`  = '"+deposit+"',"
              "`customer_id`    = '"+customerid+"', "
              "`ourbank`        = '"+deposit+"', "
              "`description`    = '"+description+"' "
              " WHERE "
              " `voucherid` = '"+vno+"' ";
        if(!conn.execQuery(sql))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }

        if(!conn.execQuery(" delete from tblgltransaction where voucherno = '"+vno+"' "))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }

        int line_no=1;

        sql = " INSERT INTO `tblgltransaction` "
              " (`voucherno`, `voucherid`, `glaccountid`, `debit`, `linenumber`, `entrydate`, description) "
              " VALUES "
              " ( '"+vno+"', '"+voucher_id+"', '"+account_ID+"', '"+erp.DecimalString(amount)+"',  '"+erp.intString(line_no)+"', '"+recievedate+"','"+description+"') ";
        if(!conn.execQuery(sql))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }
        line_no++;
        QString account_ID_Purchie = erp.getAccountGlStd("purchie");
        sql = " INSERT INTO `tblgltransaction` "
                      " (`voucherno`, `voucherid`, `glaccountid`, `credit`, `linenumber`, `entrydate`, description) "
                      " VALUES "
                      " ( '"+vno+"', '"+voucher_id+"', '"+account_ID_Purchie+"', '"+erp.DecimalString(amount)+"',  '"+erp.intString(line_no)+"', '"+recievedate+"', '"+description+"') ";
        if(!conn.execQuery(sql))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }

        QString event = "Update";
        QString transtype = "Cheque/Purchie Encashed";
        QString transid = vno;
        erp.AddToEventLog(event,transtype,transid);
        QString status;
        if(amount==payment_amount)
        {
            status = "2";
        }
        else if(amount<payment_amount)
        {
            status = "1";
        }
        else
        {
            status = "3";
        }
        conn.execQuery(" update tblinvoicepaymentdetail set status='"+status+"' where voucherid='"+payment_no+"' ");
        QMessageBox::information(this,"Info","Cheque/Purchie Encashed No. "+ vno +" Saved Successfully");
        clearForm();

    }//end if(valid==true)


}

void EditChequeParchieEncashed::clearForm()
{
    ui->paymeth->setCurrentIndex(0);
    ui->ammt->setValue(0.00);
    ui->lbl_vno->clear();
}

void EditChequeParchieEncashed::on_search_btn_clicked()
{
    clearForm();
    ui->searchTable->setRowCount(0);
    QString sql = " select * from tblencashpaymentdetail where id >0 ";
    if(ui->search_vno->text()!="")
    {
        sql += " and (voucherid='"+ui->search_vno->text()+"' OR voucherid='ENC-"+ui->search_vno->text()+"') ";
    }
    if(ui->search_customerid->text()!="")
    {
        sql += " and customer_id = '"+ui->search_customerid->text()+"' ";
    }
    sql += " group by voucherid order by voucherid desc ";
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        while(qry.next())
        {
            ui->searchTable->insertRow(ui->searchTable->rowCount());
            int row = ui->searchTable->rowCount()-1;
            ui->searchTable->setItem(row,VNO, new QTableWidgetItem( qry.value("voucherid").toString() ) );
            ui->searchTable->setItem(row,DATE, new QTableWidgetItem( qry.value("datereceived").toDate().toString("dd/MM/yyyy") ) );
            ui->searchTable->setItem(row,CUSTOMER, new QTableWidgetItem( erp.customerName(qry.value("customer_id").toString()) ) );
            ui->searchTable->setItem(row,AMOUNT, new QTableWidgetItem( erp.DecimalString(qry.value("amount").toFloat()) ) );
        }
    }
}

void EditChequeParchieEncashed::on_search_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->search_customername,ui->search_customerid);
}

void EditChequeParchieEncashed::on_EditButton_clicked()
{
    if(ui->searchTable->currentRow()<0)
    {
        QMessageBox::critical(this,"Error","Please select voucher to edit");
        return;
    }
    QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VNO)->text();
    ui->lbl_vno->setText(vno);
    QString sql;

    sql = " select pmtno,amount, datereceived,customer_id, duedate,bankdepositid, paymeth,checknum,bank,ourbank,description from  tblencashpaymentdetail where voucherid='"+vno+"' group by voucherid ";
    QSqlQuery qry;
    qry.prepare(sql);
    qry.exec();
    qry.next();

    QSqlQuery qryenc;
    qryenc.prepare("select sum(amount) as amount from tblencashpaymentdetail where pmtno= '"+ qry.value("pmtno").toString() +"' and voucherid != '"+vno+"' ");
    qryenc.exec();
    qryenc.next();

    QSqlQuery qryinv;
    qryinv.prepare("select customer_id,amount,description,paymeth  from  tblinvoicepaymentdetail where  voucherid= '"+ qry.value("pmtno").toString() +"'");
    qryinv.exec();
    qryinv.next();
    erp.setComboBoxSelected(ui->paymeth, qryinv.value("paymeth").toInt() );
    ui->voucherno->clear();
    ui->voucherno->addItem(qry.value("pmtno").toString(), QVariant(qry.value("pmtno")) );

    ui->customername->setText( erp.customerName(qryinv.value("customer_id").toString()) );
    ui->customerid->setText( qryinv.value("customer_id").toString() );
    float amount = qryinv.value("amount").toFloat() - qryenc.value("amount").toFloat();
    QString desc = qryinv.value("description").toString();

    ui->balance->setText( erp.DecimalString(amount) );
    ui->desc->setText( desc );

    erp.setComboBoxSelected(ui->method, qry.value("paymeth").toInt() );
    ui->recievedate->setDate( qry.value("datereceived").toDate() );
    ui->ammt->setValue( qry.value("amount").toFloat() );
    ui->desc2->setText( qry.value("description").toString() );
    ui->checkNum->setText( qry.value("checknum").toString() );
    ui->bank->setText( qry.value("bank").toString() );
    ui->duedate->setDate( qry.value("duedate").toDate() );
    ui->deposit->setText( qry.value("ourbank").toString() );
    ui->depositdescription->setText( erp.getAccount(qry.value("ourbank").toString()) );

}

void EditChequeParchieEncashed::on_DeleteButton_clicked()
{
    if(ui->searchTable->currentRow()<0)
    {
        QMessageBox::critical(this,"Error","Please select voucher to delete");
        return;
    }

    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes)
      {
            QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VNO)->text();
            QSqlQuery qry;
            qry.prepare(" select * from tblencashpaymentdetail WHERE voucherid = '"+vno+"' ");
            qry.exec();
            qry.next();
            QString payment_no = qry.value("pmtno").toString();
            if(!conn.execQuery(" DELETE FROM tblgltransaction WHERE voucherno = '"+vno+"' "))
            {
                QMessageBox::critical(this,"Error","Some problem in record insertion");
                return;
            }
            if(!conn.execQuery(" DELETE FROM tblgltransvoucher WHERE voucherno = '"+vno+"' "))
            {
                QMessageBox::critical(this,"Error","Some problem in record insertion");
                return;
            }
            if(!conn.execQuery(" DELETE FROM tblencashpaymentdetail WHERE voucherid = '"+vno+"' "))
            {
                QMessageBox::critical(this,"Error","Some problem in record insertion");
                return;
            }


            QSqlQuery qry2;
            qry2.prepare("select `amount` from `tblinvoicepaymentdetail` where `voucherid`='"+payment_no+"' ");
            qry2.exec();
            qry2.next();
            float pmt_amt = qry2.value("amount").toFloat();

            QSqlQuery qry3;
            qry3.prepare("select sum(`amount`) as encashed_amt from tblencashpaymentdetail where pmtno='"+payment_no+"' ");
            qry3.exec();
            qry3.next();
            float amt_encashed = qry3.value("encashed_amt").toFloat();
            QString status;
            if(amt_encashed==pmt_amt)
            {
                status = "2";
            }
            else if(amt_encashed<pmt_amt)
            {
                status = "1";
            }
            else if(amt_encashed==0)
            {
                status = "0";
            }
            else
            {
                status = "3";
            }

            conn.execQuery("update tblinvoicepaymentdetail set status='$status' where voucherid='"+payment_no+"' ");


            QMessageBox::information(this,"Info","Voucher deleted successfully");
            on_search_btn_clicked();
    }//end if (reply == QMessageBox::Yes)

}
