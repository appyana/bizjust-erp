#include "customerreceipt.h"
#include "ui_customerreceipt.h"

CustomerReceipt::CustomerReceipt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerReceipt)
{
    ui->setupUi(this);
    ui->recievedate->setDate(QDate::currentDate());
    ui->duedate->setDate(QDate::currentDate());
    loadpaymenth();
}

CustomerReceipt::~CustomerReceipt()
{
    delete ui;
}


void CustomerReceipt::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void CustomerReceipt::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomerReceipt::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomerReceipt::on_customername_textEdited(const QString &arg1)
{
    QString str = sch.customers(arg1);
    autocompleter(str,ui->customername, ui->customerid);
}

void CustomerReceipt::loadpaymenth()
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

void CustomerReceipt::on_paymeth_currentIndexChanged(int index)
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

void CustomerReceipt::on_depositdescription_textEdited(const QString &arg1)
{
    QString sql = sch.glaccount_generalize(arg1);
    autocompleter(sql,ui->depositdescription, ui->deposit);
}

void CustomerReceipt::on_customerid_textChanged(const QString &arg1)
{
    ui->label_balance->setText(erp.customerBalance(arg1,QDate::currentDate().toString("yyyy-MM-dd")));
}

void CustomerReceipt::on_pushButton_clicked()
{
    if(isValid())
    {
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
        QString pmt_num = erp.get_num(pmt_code);
        QString vno = pmt_code+"-"+pmt_num;

        QString sql = "INSERT INTO `tblgltransvoucher` "
                      " (`voucherno` , `description`,	`entrydate`, `comments`, `companyid`, `total_debit`, `total_credit`,`companytype`) "
                      " VALUES "
                      " ('"+vno+"', '"+description+"', '"+recievedate+"', 'Payment Received', '"+customerid+"', '"+erp.DecimalString(amount)+"', '"+erp.DecimalString(amount)+"', 'customer')";
        QSqlQuery qry;
        qry.prepare(sql);
        if(!qry.exec())
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }
        QString voucher_id = qry.lastInsertId().toString();
        QString account_ID = deposit;

        QString sql2 = "INSERT INTO `tblinvoicepaymentdetail` "
                       "(`invoiceid` , `amount` , `voucherid` , `datereceived`, `paymeth`, `checkNum`, `bank`, `dueDate`, `bankdepositid`,`customer_id`, `ourbank`, `description`) "
                       " VALUES "
                       "('', '"+erp.DecimalString(amount)+"', '"+vno+"', '"+recievedate+"', '"+paymeth+"', '"+chequenum+"', '"+bank+"', '"+dueDate+"', '"+deposit+"', '"+customerid+"', '"+deposit+"', '"+description+"')";
        if(!conn.execQuery(sql2))
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

        QString event = "New";
        QString transtype = "Customer Payment";
        QString transid = vno ;
        erp.AddToEventLog(event,transtype,transid);

        QString msg = "Customer Payment "+vno+" saved Successfully!";
        QMessageBox::information(this,"Information",msg);
        clearform();
    }
}

void CustomerReceipt::clearform()
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
}

bool CustomerReceipt::isValid()
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
