#include "editcustomercashreceipt.h"
#include "ui_editcustomercashreceipt.h"

EditCustomerCashReceipt::EditCustomerCashReceipt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditCustomerCashReceipt)
{
    ui->setupUi(this);
    loadEditForm();
    loadSearchForm();
}

EditCustomerCashReceipt::~EditCustomerCashReceipt()
{
    delete ui;
}


void EditCustomerCashReceipt::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditCustomerCashReceipt::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditCustomerCashReceipt::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}


void EditCustomerCashReceipt::loadEditForm()
{
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

void EditCustomerCashReceipt::loadSearchForm()
{
    ui->searchTable->setColumnCount(3);
    QStringList titles;
    titles <<"Voucher No"<<"Date"<<"Amount";
    ui->searchTable->setHorizontalHeaderLabels(titles);
    ui->searchTable->setColumnWidth(VNO,100);
    //ui->searchTable->setColumnWidth(VDATE,100);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(VDATE, QHeaderView::Stretch);
    ui->searchTable->setColumnWidth(VAMOUNT,100);
}


void EditCustomerCashReceipt::on_customername_textEdited(const QString &arg1)
{
   QString sql = sch.customers(arg1);
   autocompleter(sql,ui->customername, ui->customerid);
}

void EditCustomerCashReceipt::on_AddButton_clicked()
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

void EditCustomerCashReceipt::on_removeRowButton_clicked()
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

void EditCustomerCashReceipt::checktotal()
{
    float totamt=0.00;
    for(int i=0; i<ui->detailTable->rowCount(); i++)
    {
        float amt = ui->detailTable->item(i,AMOUNT)->text().toFloat();
        totamt += amt;
    }
    ui->total->setValue(totamt);
    if(ui->recievedate->text()!="" && ui->total->value()>0 && ui->lbl_vno->text()!="")
    {
        ui->SaveButton->setEnabled(true);
    }
    else
    {
        ui->SaveButton->setEnabled(false);
    }
}

void EditCustomerCashReceipt::on_recievedate_userDateChanged(const QDate &date)
{
    checktotal();
}

void EditCustomerCashReceipt::on_SaveButton_clicked()
{
    if(ui->recievedate->text()!="" && ui->total->value()>0 && ui->lbl_vno->text()!="")
    {
        QString sql;
        QString recievedate = ui->recievedate->date().toString("yyyy-MM-dd");
        int lines = ui->detailTable->rowCount();
        int line_no=1;

        QString vno = ui->lbl_vno->text();

        QString desc = "Payment Received";
        float total_amount = ui->total->value();

        sql = " UPDATE `tblgltransvoucher` "
              " SET "
              "`description` = '"+desc+"',	"
              "`entrydate` = '"+recievedate+"', "
              "`comments` = 'Payment Received', "
              "`companyid` = '', "
              "`total_debit` = '"+erp.DecimalString(total_amount)+"', "
              "`total_credit` = '"+erp.DecimalString(total_amount)+"',"
              "`companytype` = 'customer',"
              "`lines` = '"+erp.intString(lines)+"' "
              " WHERE "
              " `voucherno` = '"+vno+"' ";
        if(!conn.execQuery(sql))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }

        QString voucher_id = erp.getvoucherid(vno);

        if(!conn.execQuery(" delete from tblinvoicepaymentdetail where voucherid = '"+vno+"' "))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }
        if(!conn.execQuery(" delete from tblgltransaction where voucherno = '"+vno+"' "))
        {
            QMessageBox::critical(this,"Error","Some problem in record insertion");
            return;
        }

        for(int i=0; i<lines; i++)
        {
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

        QString event = "Update";
        QString transtype = "Customer Receipt";
        QString transid = vno;
        erp.AddToEventLog(event,transtype,transid);

        QMessageBox::information(this,"Info","Voucher "+vno+" saved successfully");
        //clearForm();


    }//end if(ui->recievedate->text()!="" && ui->total->value()>0)
}

void EditCustomerCashReceipt::clearForm()
{
    ui->detailTable->setRowCount(0);
    ui->lbl_vno->clear();
    checktotal();
    ui->recievedate->setFocus();
}

void EditCustomerCashReceipt::on_search_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->search_customername, ui->search_customerid);
}

void EditCustomerCashReceipt::on_search_btn_clicked()
{
    ui->searchTable->setRowCount(0);
    clearForm();
    QString sql = " select * from tblgltransvoucher tv inner join tblinvoicepaymentdetail pd "
                  " on (tv.voucherno = pd.voucherid) where tv.voucherno like 'pmt%' and pd.cancel =0 "
                  " and pd.paymeth ='1' ";
    if(ui->search_vno->text()!="")
    {
        sql += " and (tv.voucherno='"+ui->search_vno->text()+"' OR tv.voucherno='PMT-"+ui->search_vno->text()+"') ";
    }
    if(ui->search_customerid->text().toInt()>0)
    {
        sql += " and pd.customer_id='"+ui->search_customerid->text()+"'  ";
    }
    sql += " group by tv.voucherid order by tv.voucherid DESC ";

    QSqlQuery res;
    res.prepare(sql);
    if(res.exec())
    {
        while(res.next())
        {
            ui->searchTable->insertRow(ui->searchTable->rowCount());
            int row = ui->searchTable->rowCount()-1;
            ui->searchTable->setItem(row,VNO, new QTableWidgetItem( res.value("voucherno").toString() ));
            ui->searchTable->setItem(row,VDATE, new QTableWidgetItem( res.value("entrydate").toDate().toString("dd/MM/yyyy") ));
            ui->searchTable->setItem(row,VAMOUNT, new QTableWidgetItem( erp.DecimalString(res.value("total_debit").toFloat()) ));
        }
    }
}

void EditCustomerCashReceipt::on_EditButton_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VNO)->text();
        QSqlQuery qry;
        qry.prepare(" SELECT * FROM tblgltransvoucher WHERE voucherno = '"+vno+"' ");
        qry.exec();
        qry.next();
        ui->lbl_vno->setText(vno);
        ui->recievedate->setDate( qry.value("entrydate").toDate() );
        ui->detailTable->setRowCount(0);
        QSqlQuery det;
        det.prepare(" SELECT * FROM tblinvoicepaymentdetail WHERE voucherid = '"+vno+"' ORDER BY id ASC ");
        if(det.exec())
        {
            while(det.next())
            {
                ui->detailTable->insertRow(ui->detailTable->rowCount());
                int row = ui->detailTable->rowCount()-1;
                ui->detailTable->setItem(row, CUSTOMERNAME,  new QTableWidgetItem( erp.customerName(det.value("customer_id").toString()) ) );
                ui->detailTable->setItem(row, CUSTOMERID,    new QTableWidgetItem( det.value("customer_id").toString() ) );
                ui->detailTable->setItem(row, DESCRIPTION,   new QTableWidgetItem( det.value("description").toString() ) );
                ui->detailTable->setItem(row, AMOUNT,        new QTableWidgetItem( erp.DecimalString(det.value("amount").toFloat()) ) );
            }
        }
        checktotal();
    }
    else
    {
        QMessageBox::critical(this,"Error","Please select voucher to edit");
    }
}

void EditCustomerCashReceipt::on_DeleteButton_clicked()
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
                QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VNO)->text();
                if(!conn.execQuery(" DELETE FROM tblgltransaction WHERE voucherno = '"+vno+"' "))
                {
                    QMessageBox::critical(this,"Error","Some problem in record insertion");
                    return;
                }
                if(!conn.execQuery(" DELETE FROM tblinvoicepaymentdetail WHERE voucherid = '"+vno+"' "))
                {
                    QMessageBox::critical(this,"Error","Some problem in record insertion");
                    return;
                }
                if(!conn.execQuery(" DELETE FROM tblgltransvoucher WHERE voucherno = '"+vno+"' "))
                {
                    QMessageBox::critical(this,"Error","Some problem in record insertion");
                    return;
                }
                QMessageBox::information(this,"Info","Voucher deleted successfully");
                on_search_btn_clicked();
        }//end if (reply == QMessageBox::Yes)
    }//end else
}
