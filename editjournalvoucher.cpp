#include "editjournalvoucher.h"
#include "ui_editjournalvoucher.h"

EditJournalVoucher::EditJournalVoucher(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditJournalVoucher)
{
    ui->setupUi(this);
    loadform();
}

EditJournalVoucher::~EditJournalVoucher()
{
    delete ui;
}


void EditJournalVoucher::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditJournalVoucher::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditJournalVoucher::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditJournalVoucher::on_accountname_textEdited(const QString &arg1)
{
    //1 = cistomer, 2 = vendor, 3 = glaccount
    int type = ui->companytype->currentIndex();
    QString sql="";
    if(type==0)
    {
        sql="";
    }
    else if(type==1)
    {
        sql = sch.customers(arg1);
    }
    else if(type==2)
    {
        sql = sch.vendors(arg1);
    }
    else
    {
        sql = sch.glaccount_generalize(arg1);
    }
    autocompleter(sql,ui->accountname, ui->accountid);
}

void EditJournalVoucher::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->detailtable->setColumnCount(6);
    QStringList titles;
    titles <<"Cust/Vend/\nGL Acct"<<"Account"<<"Account Id"<<"Description"<<"Debit"<<"Credit";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ACCOUNTID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(ACCOUNTNAME, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(COMPANYTYPE,100);
    ui->detailtable->setColumnWidth(DESCRIPTION,200);
    ui->detailtable->setColumnWidth(DEBIT,90);
    ui->detailtable->setColumnWidth(CREDIT,90);
    //ui->btn_save->setEnabled(false);
    ui->companytype->addItem("Select",QVariant("select"));
    ui->companytype->addItem("Customer",QVariant("customer"));
    ui->companytype->addItem("Vendor",QVariant("vendor"));
    ui->companytype->addItem("GL Account",QVariant("glaccount"));
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

void EditJournalVoucher::on_btn_add_clicked()
{
    QString accountname = ui->accountname->text();
    QString accountid = ui->accountid->text();
    QString description = ui->description->text();
    float debit = ui->debit->value();
    float credit = ui->credit->value();
    float tot = debit + credit;
    if(accountid=="")
    {
        QMessageBox::critical(this,"","Please select account");
        ui->accountname->setFocus();
        return;
    }
    else if(tot<=0)
    {
        QMessageBox::critical(this,"","Please enter value to debit or credit");
        ui->debit->setFocus();
        return;
    }
    else
    {
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,COMPANYTYPE, new QTableWidgetItem(ui->companytype->currentText()));
        ui->detailtable->setItem(row,ACCOUNTNAME, new QTableWidgetItem(accountname));
        ui->detailtable->setItem(row,ACCOUNTID, new QTableWidgetItem(accountid));
        ui->detailtable->setItem(row,DESCRIPTION, new QTableWidgetItem(description));
        ui->detailtable->setItem(row,DEBIT, new QTableWidgetItem( erp.DecimalString(debit) ));
        ui->detailtable->setItem(row,CREDIT, new QTableWidgetItem( erp.DecimalString(credit) ));
        checktotal();
        ui->companytype->setCurrentIndex(0);
        ui->accountname->clear();
        ui->accountid->clear();
        ui->description->clear();
        ui->debit->setValue(0.00);
        ui->credit->setValue(0.00);
        ui->companytype->setFocus();
    }
}

void EditJournalVoucher::checktotal()
{
    float total_debit=0;
    float total_credit=0;
    for(int i=0; i<ui->detailtable->rowCount(); i++)
    {
        float debit = ui->detailtable->item(i,DEBIT)->text().toFloat();
        float credit = ui->detailtable->item(i,CREDIT)->text().toFloat();
        total_debit += debit;
        total_credit += credit;
    }
    ui->total_debit->setValue(total_debit);
    ui->total_credit->setValue(total_credit);
    if(total_debit>0 && total_debit==total_credit)
    {
        ui->btn_save->setEnabled(true);
    }
    else
    {
        ui->btn_save->setEnabled(false);
    }
}



void EditJournalVoucher::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
        checktotal();
    }
}

void EditJournalVoucher::on_btn_save_clicked()
{
    QString entrydate = ui->entrydate->date().toString("yyyy-MM-dd");
    float total_debit = ui->total_debit->value();
    float total_credit = ui->total_credit->value();
    if(entrydate=="")
    {
        QMessageBox::critical(this,"","Transaction date is required");
        ui->entrydate->setFocus();
        return;
    }
    else if(total_debit<=0)
    {
        QMessageBox::critical(this,"","Total must be greater than 0.00");
        return;
    }
    else
    {
        QString voucher = ui->lbl_vno->text();
        int lines = ui->detailtable->rowCount();
        QString wherefrom = "3";
        QString query = "UPDATE tblgltransvoucher "
                        " SET "
                        " entrydate       = '"+entrydate+"',"
                        " total_debit     = '"+erp.DecimalString(total_debit)+"',"
                        " total_credit    = '"+erp.DecimalString(total_credit)+"',"
                        " wherefrom       = '"+wherefrom+"',"
                        " description     = '',"
                        " financialyearid = '"+erp._yearid+"',"
                        " `lines`         = '"+erp.intString(lines)+"' "
                        " WHERE "
                        " voucherno = '"+voucher+"' ";
        if(!conn.execQuery(query))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString voucherid = erp.getvoucherid(voucher);
        if(!conn.execQuery("delete from tblgltransaction where voucherno = '"+voucher+"' "))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString str = ui->detailtable->item(i,COMPANYTYPE)->text().toLower();
            QString description = ui->detailtable->item(i,DESCRIPTION)->text();
            QString glaccountid = ui->detailtable->item(i,ACCOUNTID)->text();
            float debit = ui->detailtable->item(i,DEBIT)->text().toFloat();
            float credit = ui->detailtable->item(i,CREDIT)->text().toFloat();
            QString acct = str.replace(" ","");
            QString account_ID;
            if(acct=="customer")
            {
                account_ID = erp.getaccountAR("receivables");
            }
            else if(acct=="vendor")
            {
                account_ID = erp.getaccountAP("payable");
            }
            else
            {
                account_ID = glaccountid;
            }
            int j=i+1;
            query = "insert into tblgltransaction "
                    " ( voucherno, voucherid, glaccountid, description, debit, credit, linenumber, `entrydate`, financialyearid , compid , companytype) "
                    " values "
                    " ('"+voucher+"', '"+voucherid+"', '"+account_ID+"', '"+description+"', '"+erp.DecimalString(debit)+"', '"+erp.DecimalString(credit)+"', '"+erp.intString(j)+"', '"+entrydate+"', '"+erp._yearid+"' , '"+glaccountid+"' , '"+acct+"')";
            if(!conn.execQuery(query))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
        }
        QString event = "Update";
        QString transtype = "Journal Voucher";
        QString transid = voucher;
        erp.AddToEventLog(event,transtype,transid);
        QMessageBox::information(this,"","Voucher  "+ voucher +" is updated successfully.");
        on_btn_search_clicked();
    }
}

void EditJournalVoucher::on_companytype_currentIndexChanged(int index)
{
    if(index==0)
    {
        ui->accountname->clear();
        ui->accountid->clear();
    }
    else
    {
        ui->accountname->clear();
        ui->accountid->clear();
    }
}

void EditJournalVoucher::on_debit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->credit->setValue(0.00);
    }
}

void EditJournalVoucher::on_credit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->debit->setValue(0.00);
    }
}


void EditJournalVoucher::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString vno = ui->search_voucherno->text();
    QString begindate = ui->search_begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->search_enddate->date().toString("yyyy-MM-dd");
    QString sql = " select * from tblgltransvoucher where voucherno like 'jv-%' and entrydate BETWEEN '"+begindate+"' AND '"+enddate+"' ";
    if(vno!="")
    {
        sql += " and (voucherno = '"+vno+"' or voucherno = 'jv-"+vno+"') ";
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

void EditJournalVoucher::on_btn_edit_clicked()
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
            ui->detailtable->setItem(row,COMPANYTYPE, new QTableWidgetItem( acct ));
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

void EditJournalVoucher::on_btn_delete_clicked()
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
                QString transtype = "Journal Voucher";
                QString transid = vno;
                erp.AddToEventLog(event,transtype,transid);
                QMessageBox::information(this,"","Voucher "+ vno +" is deleted successfully.");
                ui->detailtable->setRowCount(0);
                ui->companytype->setCurrentIndex(0);
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
