#include "editglopeningbalance.h"
#include "ui_editglopeningbalance.h"

EditGLOpeningBalance::EditGLOpeningBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditGLOpeningBalance)
{
    ui->setupUi(this);
    loadform();
}

EditGLOpeningBalance::~EditGLOpeningBalance()
{
    delete ui;
}



void EditGLOpeningBalance::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditGLOpeningBalance::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditGLOpeningBalance::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditGLOpeningBalance::on_accountname_textEdited(const QString &arg1)
{
   QString sql = sch.glaccount_generalize(arg1);
   autocompleter(sql,ui->accountname, ui->accountid);
}

void EditGLOpeningBalance::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->detailtable->setColumnCount(5);
    QStringList titles;
    titles <<"Account"<<"Account Id"<<"Description"<<"Debit"<<"Credit";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ACCOUNTID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(ACCOUNTNAME, QHeaderView::Stretch);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(DESCRIPTION, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(DEBIT,100);
    ui->detailtable->setColumnWidth(CREDIT,100);
    //ui->btn_save->setEnabled(false);
    ui->search_begindate->setDate(QDate::currentDate().addMonths(-2));
    ui->search_enddate->setDate(QDate::currentDate());
    QStringList labels;
    labels <<"Voucher No"<<"Entry Date";
    ui->searchTable->setColumnCount(2);
    ui->searchTable->setHorizontalHeaderLabels(labels);
    ui->searchTable->setColumnWidth(VNO,90);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(DATE, QHeaderView::Stretch);
    checktotal();
}

void EditGLOpeningBalance::on_btn_add_clicked()
{
    QString accountname = ui->accountname->text();
    QString accountid = ui->accountid->text();
    float debit = ui->debit->value();
    float credit = ui->credit->value();
    float total = debit+credit;
    if(accountid=="")
    {
        QMessageBox::critical(this,"","Please select Account");
        ui->accountname->setFocus();
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
        ui->detailtable->setItem(row,ACCOUNTNAME, new QTableWidgetItem(accountname));
        ui->detailtable->setItem(row,ACCOUNTID,   new QTableWidgetItem(accountid));
        ui->detailtable->setItem(row,DESCRIPTION,   new QTableWidgetItem(ui->description->text()));
        ui->detailtable->setItem(row,DEBIT,      new QTableWidgetItem( erp.DecimalString(debit) ));
        ui->detailtable->setItem(row,CREDIT,     new QTableWidgetItem( erp.DecimalString(credit) ));
        checktotal();
        ui->accountname->clear();
        ui->accountid->clear();
        ui->debit->setValue(0.00);
        ui->credit->setValue(0.00);
        ui->accountname->setFocus();
    }
}

void EditGLOpeningBalance::checktotal()
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



void EditGLOpeningBalance::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
        checktotal();
    }
}

void EditGLOpeningBalance::on_btn_save_clicked()
{
    QString entrydate = ui->entrydate->date().toString("yyyy-MM-dd");
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
        //QString ob_code = "OB";
        //QString ob_num = erp.get_num(ob_code);
        QString vno = ui->lbl_vno->text();//ob_code+"-"+ob_num;
        //erp.update_ids_num(ob_code);
        float total_debit  = ui->total_debit->value();
        float total_credit = ui->total_credit->value();
        float total_amount = total_debit+total_credit;
        QString wherefrom = "3";
        QString description = "gl opening balance";
        QString query_transaction = "UPDATE `tblgltransvoucher` "
                                    " SET "
                                    " "
                                    " `description` = '"+description+"',"
                                    " `entrydate` = '"+entrydate+"',"
                                    " `total_debit` = '"+erp.DecimalString(total_amount)+"',"
                                    " `total_credit` = '"+erp.DecimalString(total_amount)+"',"
                                    " `wherefrom` = '"+wherefrom+"',"
                                    " `financialyearid` = '"+erp._yearid+"' "
                                    " WHERE "
                                    " `voucherno` = '"+vno+"' ";
        if(!conn.execQuery(query_transaction))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString voucherid = erp.getvoucherid(vno);
        QString voucher = vno;
        if(!conn.execQuery(" delete from tblgltransaction where voucherno = '"+voucher+"' "))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString glaccountid   = ui->detailtable->item(i,ACCOUNTID)->text();
            int ln = 0;
            if(glaccountid!="")
            {
                description = ui->detailtable->item(i,DESCRIPTION)->text();
                float debit = ui->detailtable->item(i,DEBIT)->text().toFloat();
                float credit = ui->detailtable->item(i,CREDIT)->text().toFloat();
                ln++;
                QString query = "insert into tblgltransaction "
                                " ( voucherno, voucherid, glaccountid, description, debit, credit, linenumber, `entrydate`, financialyearid) "
                                " values "
                                " ('"+voucher+"', '"+voucherid+"', '"+glaccountid+"', '"+description+"', '"+erp.DecimalString(debit)+"', '"+erp.DecimalString(credit)+"', '"+erp.intString(ln)+"', '"+entrydate+"', '"+erp._yearid+"')";
                if(!conn.execQuery(query))
                {
                    QMessageBox::critical(this,"","Some problem in record insertion");
                    return;
                }
            }
        }
        QString event = "Update";
        QString transtype = "GL Opening Balance";
        QString transid = vno ;
        erp.AddToEventLog(event,transtype,transid);
        QMessageBox::information(this,"","Opening balance "+voucher+" updated successfully");
//        ui->detailtable->setRowCount(0);
//        ui->accountname->clear();
//        ui->accountid->clear();
//        ui->debit->setValue(0.00);
//        ui->credit->setValue(0.00);
//        ui->entrydate->setFocus();
//        checktotal();
        on_btn_search_clicked();
    }
}

void EditGLOpeningBalance::on_debit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->credit->setValue(0.00);
    }
}

void EditGLOpeningBalance::on_credit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->debit->setValue(0.00);
    }
}



void EditGLOpeningBalance::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString vno = ui->search_voucherno->text();
    QString begindate = ui->search_begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->search_enddate->date().toString("yyyy-MM-dd");
    QString sql = " select * from tblgltransvoucher where voucherno like 'ob-%'  and wherefrom='3' and entrydate BETWEEN '"+begindate+"' AND '"+enddate+"' ";
    if(vno!="")
    {
        sql += " and (voucherno = '"+vno+"' or voucherno = 'ob-"+vno+"') ";
    }
    sql += " order by voucherid ASC ";
    QSqlQuery info(sql);
    while(info.next())
    {
        int row = ui->searchTable->rowCount();
        ui->searchTable->insertRow(row);
        ui->searchTable->setItem(row,VNO,new QTableWidgetItem( info.value("voucherno").toString() ));
        ui->searchTable->setItem(row,DATE,new QTableWidgetItem( info.value("entrydate").toDate().toString("dd/MM/yyyy") ));
    }
}

void EditGLOpeningBalance::on_btn_edit_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        ui->detailtable->setRowCount(0);
        QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VNO)->text();
        ui->lbl_vno->setText(vno);
        QSqlQuery qry(" select * from tblgltransvoucher where voucherno = '"+vno+"' ");
        qry.next();
        ui->entrydate->setDate( qry.value("entrydate").toDate() );
        QSqlQuery info(" select * from tblgltransaction where voucherno = '"+vno+"' order by linenumber ");
        while(info.next())
        {
            QString ctype = info.value("companytype").toString();
            QString accountid;
            QString accountname;
            QString acct;
            if(ctype=="customer")
            {
                acct = "Customer";
                accountid = info.value("compid").toString();
                accountname = erp.customerName(accountid);
            }
            else if(ctype=="vendor")
            {
                acct = "Vendor";
                accountid = info.value("compid").toString();
                accountname = erp.vendorname(accountid);
            }
            else
            {
                acct = "GL Account";
                accountid = info.value("glaccountid").toString();
                accountname = erp.getAccount(accountid);
            }
            int row = ui->detailtable->rowCount();
            ui->detailtable->insertRow(row);
            ui->detailtable->setItem(row,ACCOUNTNAME, new QTableWidgetItem( accountname ));
            ui->detailtable->setItem(row,ACCOUNTID, new QTableWidgetItem( accountid ));
            ui->detailtable->setItem(row,DESCRIPTION, new QTableWidgetItem( info.value("description").toString() ));
            ui->detailtable->setItem(row,DEBIT, new QTableWidgetItem( erp.DecimalString( info.value("debit").toFloat() ) ));
            ui->detailtable->setItem(row,CREDIT, new QTableWidgetItem( erp.DecimalString( info.value("credit").toFloat() ) ));
            checktotal();
        }
        ui->editFrame->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,"","Please select voucher to edit");
    }
}

void EditGLOpeningBalance::on_btn_delete_clicked()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
            QString vno = ui->lbl_vno->text();
            if(conn.execQuery(" delete from tblgltransvoucher WHERE voucherno = '"+vno+"' ") && conn.execQuery(" delete from tblgltransaction WHERE voucherno = '"+vno+"' "))
            {
                on_btn_search_clicked();
                QString event = "Delete";
                QString transtype = "GL Opening Balance";
                QString transid = vno;
                erp.AddToEventLog(event,transtype,transid);
                QMessageBox::information(this,"","Voucher "+ vno +" is deleted successfully.");
                ui->detailtable->setRowCount(0);
                ui->accountname->clear();
                ui->accountid->clear();
                ui->description->clear();
                ui->debit->setValue(0.00);
                ui->credit->setValue(0.00);
                ui->entrydate->setDate(QDate::currentDate());
                ui->lbl_vno->clear();
                ui->editFrame->setEnabled(false);
            }
            else
            {
                QMessageBox::critical(this,"","Some problem in record deletion");
            }
      }
}
