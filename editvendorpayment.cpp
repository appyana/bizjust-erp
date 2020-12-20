#include "editvendorpayment.h"
#include "ui_editvendorpayment.h"

EditVendorPayment::EditVendorPayment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditVendorPayment)
{
    ui->setupUi(this);
    ui->editFrame->setVisible(false);
    ui->entrydate->setDate(QDate::currentDate());
    ui->searchTable->setColumnCount(4);
    QStringList titles;
    titles <<"Voucher\n#"<<"Voucher Date"<<"Vendor"<<"Amount";
    ui->searchTable->horizontalHeader()->setSectionResizeMode(VENDOR, QHeaderView::Stretch);
    ui->searchTable->setColumnWidth(VNO,80);
    ui->searchTable->setColumnWidth(DATE,80);
    ui->searchTable->setColumnWidth(AMOUNT,80);
}

EditVendorPayment::~EditVendorPayment()
{
    delete ui;
}


void EditVendorPayment::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditVendorPayment::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditVendorPayment::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditVendorPayment::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->vendorname,ui->vendorid);
}

void EditVendorPayment::on_btn_save_clicked()
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
    //QString pmt_code = "vp";
    //QString pmt_num = erp.get_num(pmt_code);
    QString vno = ui->lbl_vno->text();//pmt_code+"-"+pmt_num;
    //erp.update_ids_num(pmt_code);
    QString query_transaction = "UPDATE `tblgltransvoucher` "
                                " SET "
                                " `description` = '"+description+"',"
                                " `entrydate` = '"+transdate+"',"
                                " `comments` = '',"
                                " `companyid` = '"+vendorid+"',"
                                " `total_debit` = '"+erp.DecimalString(amount)+"',"
                                " `total_credit` = '"+erp.DecimalString(amount)+"',"
                                " `wherefrom` = '"+wherefrom+"',"
                                " `companytype` = 'vendor',"
                                " `financialyearid` = '"+erp._yearid+"' "
                                " WHERE "
                                " `voucherno` = '"+vno+"' ";
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
    QString query_paydet ="UPDATE `tblpurchasepaymentdetail` "
                          " SET "
                          " `amount`        = '"+erp.DecimalString(amount)+"',"
                          " `datereceived`  = '"+transdate+"',"
                          " `paymeth`       = '"+erp.intString(paymeth)+"',"
                          " `checkNum`      = '"+checkNum+"',"
                          " `bank`          = '"+bank+"',"
                          " `dueDate`       = '"+dueDate+"',"
                          " `bank_id`       = '"+glaccountid1+"',"
                          " `vendorid`      = '"+vendorid+"',"
                          " `description`   = '"+description+"' "
                          " WHERE "
                          " `voucherno` = '"+vno+"' ";
    if(!conn.execQuery(query_paydet))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }

    if(!conn.execQuery(" delete from tblgltransaction where voucherno = '"+vno+"' "))
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
    QString event = "Update";
    QString transtype = "Vendor Payment";
    QString transid = vno;
    erp.AddToEventLog(event,transtype,transid);
    QMessageBox::information(this,"","Vendor Payment No. "+vno+" updated Successfully");
//    ui->vendorname->clear();
//    ui->vendorid->clear();
//    ui->amount->setValue(0.00);
//    ui->description->clear();
//    ui->entrydate->setDate(QDate::currentDate());
//    ui->vendorname->setFocus();
}

void EditVendorPayment::on_search_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->search_vendorname,ui->search_vendorid);
}

void EditVendorPayment::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString vno = ui->search_vno->text();
    QString vendorid = ui->search_vendorid->text();
    QString sql = " select * from tblpurchasepaymentdetail where id>'0' ";
    if(vno!="")
    {
        sql += " and (voucherno = '"+vno+"' or voucherno = 'vp-"+vno+"') ";
    }
    if(vendorid!="")
    {
        sql += " and vendorid = '"+vendorid+"' ";
    }
    sql += " order by id desc ";
    QSqlQuery info(sql);
    while(info.next())
    {
        int row = ui->searchTable->rowCount();
        ui->searchTable->insertRow(row);
        ui->searchTable->setItem(row,VNO,new QTableWidgetItem( info.value("voucherno").toString() ));
        ui->searchTable->setItem(row,DATE,new QTableWidgetItem( erp.DDMMYYDateFromSQL(info.value("datereceived").toString()) ));
        ui->searchTable->setItem(row,VENDOR,new QTableWidgetItem( erp.vendorname(info.value("vendorid").toString()) ));
        ui->searchTable->setItem(row,AMOUNT,new QTableWidgetItem( erp.amountString(info.value("amount").toFloat()) ));
    }
}

void EditVendorPayment::on_btn_edit_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VNO)->text();
        QSqlQuery info(" select * from tblpurchasepaymentdetail where voucherno = '"+vno+"' ");
        info.next();
        ui->vendorid->setText( info.value("vendorid").toString() );
        ui->vendorname->setText( erp.vendorname(info.value("vendorid").toString()) );
        ui->amount->setValue( info.value("amount").toFloat() );
        ui->description->setText( info.value("description").toString() );
        ui->entrydate->setDate( info.value("datereceived").toDate() );
        ui->lbl_vno->setText(vno);
        ui->searchFrame->setVisible(false);
        ui->editFrame->setVisible(true);
    }
    else
    {
        QMessageBox::warning(this,"","Please select voucher to edit");
    }
}

void EditVendorPayment::on_btn_delete_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
              QString vno = ui->searchTable->item(ui->searchTable->currentRow(),VNO)->text();
              if(!conn.execQuery(" delete from  tblgltransvoucher Where `voucherno` = '"+vno+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in deletion");
                  return;
              }
              if(!conn.execQuery(" delete from  tblgltransaction Where `voucherno` = '"+vno+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in deletion");
                  return;
              }
              if(!conn.execQuery(" delete from  tblpurchasepaymentdetail Where `voucherno` = '"+vno+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in deletion");
                  return;
              }
              QString event = "Delete";
              QString transtype = "Vendor Payment";
              QString transid = vno;
              erp.AddToEventLog(event,transtype,transid);
              QMessageBox::information(this,"","Vendor Payment "+vno+" deleted successfully");
              on_btn_search_clicked();
          }
    }
    else
    {
        QMessageBox::warning(this,"","Please select voucher to delete");
    }
}

void EditVendorPayment::on_btn_back_clicked()
{
    on_btn_search_clicked();
    ui->editFrame->setVisible(false);
    ui->searchFrame->setVisible(true);
}
