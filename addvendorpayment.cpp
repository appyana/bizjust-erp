#include "addvendorpayment.h"
#include "ui_addvendorpayment.h"

AddVendorPayment::AddVendorPayment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddVendorPayment)
{
    ui->setupUi(this);
    ui->entrydate->setDate(QDate::currentDate());
}

AddVendorPayment::~AddVendorPayment()
{
    delete ui;
}


void AddVendorPayment::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
{
    sch.name_txt = name_txt;
    sch.id_txt = id_txt;
    QMap<int, QString> data = sch.data;
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        while(qry.next())
        {
            data[qry.value(0).toInt()] = qry.value(1).toString();
        }
    }
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

void AddVendorPayment::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddVendorPayment::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddVendorPayment::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->vendorname,ui->vendorid);
}

void AddVendorPayment::on_btn_save_clicked()
{
    if(ui->vendorid->text()=="")
    {
        QMessageBox::critical(this,"","Please select vendor");
        ui->vendorname->setFocus();
        return;
    }
    if(ui->amount->value()==0)
    {
        QMessageBox::critical(this,"","Amount must be greater than 0.00");
        ui->amount->setFocus();
        return;
    }
    if(ui->entrydate->text()=="")
    {
        QMessageBox::critical(this,"","Entry Date is required");
        ui->entrydate->setFocus();
        return;
    }
    QString transdate = ui->entrydate->date().toString("yyyy-MM-dd");
    QString vendorid = ui->vendorid->text();
    float amount = ui->amount->value();
    int paymeth = 1;
    QString checkNum = "";
    QString bank = "";
    QString dueDate = "";
    QString glaccountid1 = "";
    QString description = "Vendor Payment";
    QString wherefrom = "1";
    if(ui->description->text()!="")
    {
        description = ui->description->text();
    }
    QString pmt_code = "vp";
    QString pmt_num = erp.get_num(pmt_code);
    QString vno = pmt_code+"-"+pmt_num;
    erp.update_ids_num(pmt_code);
    QString query_transaction = "INSERT INTO `tblgltransvoucher` "
                                " (`voucherno`, `description`, `entrydate`, `comments`, `companyid`, `total_debit`, `total_credit`, `wherefrom`, `companytype`, `financialyearid`) "
                                " VALUES "
                                " ('"+vno+"', '"+description+"', '"+transdate+"', '', '"+vendorid+"', '"+erp.DecimalString(amount)+"', '"+erp.DecimalString(amount)+"', '"+wherefrom+"', 'vendor', '"+erp._yearid+"')";
    if(!conn.execQuery(query_transaction))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString voucher_id = erp.getvoucherid(vno);
    QString account_ID = glaccountid1;
    if(paymeth == 1)
    {
        account_ID = erp.getaccountAR("cash");
    }
    QString query_paydet ="INSERT INTO `tblpurchasepaymentdetail` (`amount`, `voucherno`, `datereceived`, `paymeth`, `checkNum`, `bank`, `dueDate`, `bank_id`, `vendorid`, `description`) "
                          " VALUES "
                          " ('"+erp.DecimalString(amount)+"', '"+vno+"', '"+transdate+"', '"+erp.intString(paymeth)+"', '"+checkNum+"', '"+bank+"', '"+dueDate+"', '"+glaccountid1+"', '"+vendorid+"', '"+description+"')";
    if(!conn.execQuery(query_paydet))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString acct_pay = erp.getaccountAP("payable");
    int line_no = 1;
    QString query_dr = "INSERT INTO `tblgltransaction` "
                       " (`voucherno`, `voucherid`, `glaccountid`, `description`, `debit`, `linenumber`, `entrydate`, `companytype`,`compid`) "
                       " VALUES "
                       " ('"+vno+"', '"+voucher_id+"', '"+acct_pay+"', '"+description+"', '"+erp.DecimalString(amount)+"', '"+erp.intString(line_no)+"', '"+transdate+"', 'vendor','"+vendorid+"')";
    if(!conn.execQuery(query_dr))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    line_no++;
    QString query_cr = "INSERT INTO `tblgltransaction` "
                       " (`voucherno`, `voucherid`, `glaccountid`, `description`, `credit`, `linenumber`, `entrydate`,`companytype`) "
                       " VALUES "
                       " ('"+vno+"', '"+voucher_id+"', '"+account_ID+"', '"+description+"', '"+erp.DecimalString(amount)+"', '"+erp.intString(line_no)+"', '"+transdate+"','')";
    if(!conn.execQuery(query_cr))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString event = "New";
    QString transtype = "Vendor Payment";
    QString transid = vno;
    erp.AddToEventLog(event,transtype,transid);
    QMessageBox::information(this,"","Vendor Payment No. "+vno+" Saved Successfully");
    ui->vendorname->clear();
    ui->vendorid->clear();
    ui->amount->setValue(0.00);
    ui->description->clear();
    ui->entrydate->setDate(QDate::currentDate());
    ui->vendorname->setFocus();
}

