#include "chequepurchieencashed.h"
#include "ui_chequepurchieencashed.h"

ChequePurchieEncashed::ChequePurchieEncashed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChequePurchieEncashed)
{
    ui->setupUi(this);
    loadForm();
}

ChequePurchieEncashed::~ChequePurchieEncashed()
{
    delete ui;
}

void ChequePurchieEncashed::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void ChequePurchieEncashed::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void ChequePurchieEncashed::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void ChequePurchieEncashed::loadForm()
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

void ChequePurchieEncashed::on_depositdescription_textEdited(const QString &arg1)
{
    QString sql = sch.glaccount_generalize(arg1);
    autocompleter(sql,ui->depositdescription,ui->deposit);
}

void ChequePurchieEncashed::on_paymeth_currentIndexChanged(int index)
{
    QString paymeth = ui->paymeth->itemData(index).toString();
    erp.loadChequePurchieVouchers(ui->voucherno,paymeth);
    ui->method->setCurrentIndex(0);
}

void ChequePurchieEncashed::on_voucherno_currentIndexChanged(int index)
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

void ChequePurchieEncashed::on_method_currentIndexChanged(int index)
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

void ChequePurchieEncashed::on_saveButton_clicked()
{
    bool valid = false;
    if(ui->method->itemData(ui->method->currentIndex()).toInt()==0 )
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
        QString pmt_code = "ENC";
        QString pmt_num = erp.get_num(pmt_code);
        QString vno = pmt_code+"-"+pmt_num;

        sql = " INSERT INTO `tblgltransvoucher` "
              " (`voucherno` , `description`,	`entrydate`, `comments`, `companyid`, `total_debit`, `total_credit`,`companytype`) "
              " VALUES "
              " ('"+vno+"', '"+description+"', '"+recievedate+"', 'Payment Received', '"+customerid+"', '"+erp.DecimalString(amount)+"', '"+erp.DecimalString(amount)+"', 'customer') ";
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

        sql = " INSERT INTO `tblencashpaymentdetail` (`pmtno` , `amount` , `voucherid` , `datereceived`, `paymeth`, `checkNum`, `bank`, `dueDate`, `bankdepositid`,`customer_id`, `ourbank`, `description`) "
              " VALUES "
              " ('"+payment_no+"', '"+erp.DecimalString(amount)+"', '"+vno+"', '"+recievedate+"', '"+erp.intString(paymeth)+"', '"+checkNum+"', '"+bank+"', '"+dueDate+"', '"+deposit+"', '"+customerid+"', '"+deposit+"', '"+description+"') ";
        if(!conn.execQuery(sql))
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
        erp.update_ids_num(pmt_code);
        QString event = "New";
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

void ChequePurchieEncashed::clearForm()
{
    ui->paymeth->setCurrentIndex(0);
    ui->ammt->setValue(0.00);
}
