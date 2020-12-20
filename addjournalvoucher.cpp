#include "addjournalvoucher.h"
#include "ui_addjournalvoucher.h"

AddJournalVoucher::AddJournalVoucher(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddJournalVoucher)
{
    ui->setupUi(this);
    loadform();
}

AddJournalVoucher::~AddJournalVoucher()
{
    delete ui;
}


void AddJournalVoucher::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddJournalVoucher::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddJournalVoucher::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddJournalVoucher::on_accountname_textEdited(const QString &arg1)
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

void AddJournalVoucher::loadform()
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
    ui->btn_save->setEnabled(false);
    ui->companytype->addItem("Select",QVariant("select"));
    ui->companytype->addItem("Customer",QVariant("customer"));
    ui->companytype->addItem("Vendor",QVariant("vendor"));
    ui->companytype->addItem("GL Account",QVariant("glaccount"));
}

void AddJournalVoucher::on_btn_add_clicked()
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

void AddJournalVoucher::checktotal()
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



void AddJournalVoucher::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
        checktotal();
    }
}

void AddJournalVoucher::on_btn_save_clicked()
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
        QString jv_code = "jv";
        QString jv_num = erp.get_num(jv_code);
        QString voucher = jv_code+"-"+jv_num;
        int lines = ui->detailtable->rowCount();
        QString wherefrom = "3";
        QString query = "insert into tblgltransvoucher "
                        " (voucherno, entrydate, total_debit, total_credit, wherefrom, description, financialyearid,`lines`) "
                        " values "
                        " ('"+voucher+"', '"+entrydate+"', '"+erp.DecimalString(total_debit)+"', '"+erp.DecimalString(total_credit)+"', '"+wherefrom+"','', '"+erp._yearid+"','"+erp.intString(lines)+"')";
        if(!conn.execQuery(query))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString voucherid = erp.getvoucherid(voucher);
        erp.update_ids_num(jv_code);
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
        QString event = "New";
        QString transtype = "Journal Voucher";
        QString transid = voucher;
        erp.AddToEventLog(event,transtype,transid);
        QMessageBox::information(this,"","Voucher  "+ voucher +" is inserted successfully.");
        ui->detailtable->setRowCount(0);
        ui->accountname->clear();
        ui->accountid->clear();
        ui->description->clear();
        ui->debit->setValue(0.00);
        ui->credit->setValue(0.00);
        ui->entrydate->setDate(QDate::currentDate());
        ui->entrydate->setFocus();
    }
}

void AddJournalVoucher::on_companytype_currentIndexChanged(int index)
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

void AddJournalVoucher::on_debit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->credit->setValue(0.00);
    }
}

void AddJournalVoucher::on_credit_valueChanged(double arg1)
{
    if(arg1>0)
    {
        ui->debit->setValue(0.00);
    }
}
