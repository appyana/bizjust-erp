#include "addcustomercashreceipt.h"
#include "ui_addcustomercashreceipt.h"

AddCustomerCashReceipt::AddCustomerCashReceipt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddCustomerCashReceipt)
{
    ui->setupUi(this);
    ui->recievedate->setDate(QDate::currentDate());
    ui->detailTable->setColumnCount(4);
    QStringList titles;
    titles <<"Customer"<<"Customer Id"<<"Description"<<"Amount";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->hideColumn(CUSTOMERID);
    //ui->detailTable->setColumnWidth(CUSTOMERNAME,250);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(CUSTOMERNAME, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(DESCRIPTION,200);
    ui->detailTable->setColumnWidth(AMOUNT,100);

    ui->SaveButton->setEnabled(false);
}

AddCustomerCashReceipt::~AddCustomerCashReceipt()
{
    delete ui;
}

void AddCustomerCashReceipt::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddCustomerCashReceipt::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddCustomerCashReceipt::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddCustomerCashReceipt::on_customername_textEdited(const QString &arg1)
{
   QString sql = sch.customers(arg1);
   autocompleter(sql,ui->customername, ui->customerid);
}

void AddCustomerCashReceipt::on_AddButton_clicked()
{
    if(ui->customerid->text().toInt()>0 && ui->amount->value()>0)
    {
        ui->detailTable->insertRow(ui->detailTable->rowCount());
        int row = ui->detailTable->rowCount()-1;
        ui->detailTable->setItem(row,CUSTOMERNAME,new QTableWidgetItem( ui->customername->text() ));
        ui->detailTable->setItem(row,CUSTOMERID,new QTableWidgetItem( ui->customerid->text() ));
        ui->detailTable->setItem(row,DESCRIPTION,new QTableWidgetItem( ui->description->text() ));
        ui->detailTable->setItem(row,AMOUNT,new QTableWidgetItem( erp.DecimalString(ui->amount->value()) ));
        ui->customername->clear();
        ui->customerid->clear();
        ui->description->clear();
        ui->amount->setValue(0.00);
        ui->customername->setFocus();
        checktotal();
    }
}

void AddCustomerCashReceipt::on_removeRowButton_clicked()
{
    if(ui->detailTable->currentRow()>=0)
    {
        ui->detailTable->removeRow(ui->detailTable->currentRow());
        checktotal();
    }
    else
    {
        QMessageBox::critical(this,"Error","Please Select row to remove");
    }
}

void AddCustomerCashReceipt::checktotal()
{
    float totamt=0.00;
    for(int i=0; i<ui->detailTable->rowCount(); i++)
    {
        float amt = ui->detailTable->item(i,AMOUNT)->text().toFloat();
        totamt += amt;
    }
    ui->total->setValue(totamt);
    if(ui->recievedate->text()!="" && ui->total->value()>0)
    {
        ui->SaveButton->setEnabled(true);
    }
    else
    {
        ui->SaveButton->setEnabled(false);
    }
}

void AddCustomerCashReceipt::on_recievedate_userDateChanged(const QDate &date)
{
    checktotal();
}

void AddCustomerCashReceipt::on_SaveButton_clicked()
{
    if(ui->recievedate->text()!="" && ui->total->value()>0)
    {
        QString sql;
        QString recievedate = ui->recievedate->date().toString("yyyy-MM-dd");
        int lines = ui->detailTable->rowCount();
        int line_no=1;

        QString pmt_code = "PMT";
        QString pmt_num = erp.get_num(pmt_code);
        QString vno = pmt_code+"-"+pmt_num;

        QString desc = "Payment Received";
        float total_amount = ui->total->value();

        sql = " INSERT INTO `tblgltransvoucher` "
              " (`voucherno` , `description`,	`entrydate`, `comments`, `companyid`, `total_debit`, `total_credit`,`companytype`,`lines`) "
              " VALUES "
              " ('"+vno+"', '"+desc+"', '"+recievedate+"', 'Payment Received', '', '"+erp.DecimalString(total_amount)+"', '"+erp.DecimalString(total_amount)+"', 'customer', '"+erp.intString(lines)+"') ";
        if(!conn.execQuery(sql))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }

        QString voucher_id = erp.getvoucherid(vno);

        for(int i=0; i<lines; i++)
        {
            QString customername = ui->detailTable->item(i,CUSTOMERNAME)->text();
            QString customerid = ui->detailTable->item(i,CUSTOMERID)->text();
            QString description = ui->detailTable->item(i,DESCRIPTION)->text();
            float amount = ui->detailTable->item(i,AMOUNT)->text().toFloat();
            QString paymeth = "1";
            if(description=="")
            {
                description = desc;
            }

            sql = " INSERT INTO `tblinvoicepaymentdetail` "
                  " (`invoiceid` , `amount` , `voucherid` , `datereceived`, `paymeth`,`customer_id`, `description`) "
                  " VALUES "
                  " ('', '"+erp.DecimalString(amount)+"', '"+vno+"', '"+recievedate+"', '"+paymeth+"', '"+customerid+"', '"+description+"')";
            if(!conn.execQuery(sql))
            {
                QMessageBox::critical(this,"Error","Some problem in record insertion");
                return;
            }

            QString account_ID = erp.getaccountAR("cash");
            sql = " INSERT INTO `tblgltransaction` "
                  " ( `voucherno`, `voucherid`, `glaccountid`, `debit`, `linenumber`, `entrydate`,`companytype`, description) "
                  " VALUES "
                  " ( '"+vno+"', '"+voucher_id+"', '"+account_ID+"', '"+erp.DecimalString(amount)+"',  '"+erp.intString(line_no)+"', '"+recievedate+"','','"+description+"')";
            if(!conn.execQuery(sql))
            {
                QMessageBox::critical(this,"Error","Some problem in record insertion");
                return;
            }

            QString account_ID_AR = erp.getaccountAR("receivables");
            sql = " INSERT INTO `tblgltransaction` "
                  " ( `voucherno`, `voucherid`, `glaccountid`, `credit`, `linenumber`, `entrydate`,`companytype`, description,compid) "
                  " VALUES "
                  " ( '"+vno+"', '"+voucher_id+"', '"+account_ID_AR+"', '"+erp.DecimalString(amount)+"',  '"+erp.intString(line_no)+"', '"+recievedate+"','customer', '"+description+"', '"+customerid+"')";
            if(!conn.execQuery(sql))
            {
                QMessageBox::critical(this,"Error","Some problem in record insertion");
                return;
            }
            line_no++;
        }//end for(int i=0; i<lines; i++)

        erp.update_ids_num(pmt_code);

        QString event = "New";
        QString transtype = "Customer Receipt";
        QString transid = vno;
        erp.AddToEventLog(event,transtype,transid);

        QMessageBox::information(this,"Info","Voucher "+vno+" saved successfully");
        clearForm();


    }//end if(ui->recievedate->text()!="" && ui->total->value()>0)
}

void AddCustomerCashReceipt::clearForm()
{
    ui->detailTable->setRowCount(0);
    checktotal();
    ui->recievedate->setFocus();
}
