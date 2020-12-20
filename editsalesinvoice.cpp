#include "editsalesinvoice.h"
#include "ui_editsalesinvoice.h"
#include <QMapIterator>
EditSalesInvoice::EditSalesInvoice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditSalesInvoice)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    searchFormLoad();
    editFormLoad();
}



EditSalesInvoice::~EditSalesInvoice()
{
    delete ui;
}

void EditSalesInvoice::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditSalesInvoice::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditSalesInvoice::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditSalesInvoice::searchFormLoad()
{
    QStringList titles;
    ui->tableWidgetSearch->setColumnCount(5);
    titles <<"ID"<<"Invoice #"<<"Amount"<<"Date"<<"Customer";
    ui->tableWidgetSearch->setHorizontalHeaderLabels(titles);
    ui->tableWidgetSearch->hideColumn(INVOICEID);
    ui->tableWidgetSearch->setColumnWidth(INVOICEID,80);
    ui->tableWidgetSearch->setColumnWidth(INVOICENO,100);
    ui->tableWidgetSearch->setColumnWidth(INVAMOUNT,100);
    ui->tableWidgetSearch->setColumnWidth(INVDATE,100);
    ui->tableWidgetSearch->horizontalHeader()->setSectionResizeMode(CUSTOMER,QHeaderView::Stretch);
}

void EditSalesInvoice::editFormLoad()
{
    ui->dateEntrydate->setDate(QDate::currentDate());
    erp.loadSalesTypeToComboBox(ui->comboSales_type);
    erp.loadCustomersGroupToComboBox(ui->comboCust_group);
    erp.loadCarrierToComboBox(ui->comboCarrier_id);
    erp.loadLocationsToComboBox(ui->comboLocation);
    QStringList titles;
    ui->tableWidgetDetail->setColumnCount(10);
    titles <<"Item Description"<<"Item Id"<<"Location"<<"Location Id"<<"Quantity"<<"Unit"<<"Unit Id"<<"Price"<<"Amount"<<"";
    ui->tableWidgetDetail->setHorizontalHeaderLabels(titles);
    ui->tableWidgetDetail->hideColumn(ITEMID);
    ui->tableWidgetDetail->hideColumn(LOCATIONID);
    ui->tableWidgetDetail->hideColumn(UNITID);
    //ui->tableWidgetDetail->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableWidgetDetail->setColumnWidth(0,300);
    ui->tableWidgetDetail->horizontalHeader()->setSectionResizeMode(ITEM, QHeaderView::Stretch);
    ui->tableWidgetDetail->setColumnWidth(ITEMID,50);
    ui->tableWidgetDetail->setColumnWidth(LOCATION,150);
    ui->tableWidgetDetail->setColumnWidth(LOCATIONID,80);
    ui->tableWidgetDetail->setColumnWidth(QTY,80);
    ui->tableWidgetDetail->setColumnWidth(UNITNAME,100);
    ui->tableWidgetDetail->setColumnWidth(UNITID,50);
    ui->tableWidgetDetail->setColumnWidth(PRICE,100);
    ui->tableWidgetDetail->setColumnWidth(AMOUNT,100);
    ui->tableWidgetDetail->setColumnWidth(BUTTON,80);
}


void EditSalesInvoice::on_txt_customername_textEdited()
{
    QString str = sch.customers(ui->txt_customername->text());
    autocompleter(str, ui->txt_customername, ui->txt_customerid);
}

void EditSalesInvoice::on_txt_itemname_textEdited()
{
    QString str = sch.items(ui->txt_itemname->text());
    autocompleter(str, ui->txt_itemname, ui->txt_itemid);
}

void EditSalesInvoice::on_txt_itemid_textChanged()
{
    QString itemid = ui->txt_itemid->text();
    erp.loadItemUnits(ui->comboUnit,itemid);
    QString price = erp.getItemField(itemid,"price1");
    ui->txt_price->setValue(price.toFloat());
    checkamt();
}

void EditSalesInvoice::on_txt_qty_valueChanged()
{
    checkamt();
}

void EditSalesInvoice::on_txt_price_valueChanged()
{
    checkamt();
}

void EditSalesInvoice::on_txt_amt_valueChanged()
{
    checktotal();
}

void EditSalesInvoice::checkamt()
{
    int itemid = ui->txt_itemid->text().toInt();
    int qty = ui->txt_qty->text().toInt();
    double price = ui->txt_price->text().toDouble();
    double amount = qty*price;
    ui->txt_amt->setValue(erp.DecimalString(amount).toFloat());
    if(itemid>0 && amount>0)
    {
        ui->btn_add->setEnabled(true);
    }
    else
    {
        ui->btn_add->setEnabled(false);
    }
}

void EditSalesInvoice::on_btn_add_clicked()
{

    for(int i=0; i<ui->tableWidgetDetail->rowCount(); i++)
    {
        int itemid = ui->tableWidgetDetail->item(i,ITEMID)->text().toInt();
        int iid2 = ui->txt_itemid->text().toInt();

        int locationid = ui->tableWidgetDetail->item(i,LOCATIONID)->text().toInt();
        int ind = ui->comboLocation->currentIndex();
        int lid2 = ui->comboLocation->itemData(ind).toInt();

        int unitid = ui->tableWidgetDetail->item(i,UNITID)->text().toInt();
        int uind = ui->comboUnit->currentIndex();
        int uid2 = ui->comboUnit->itemData(uind).toInt();

        float price = ui->tableWidgetDetail->item(i,PRICE)->text().toFloat();
        float pr2 = ui->txt_price->text().toFloat();
        //QMessageBox::information(this,"info", QString::number(itemid) + "=" + QString::number(iid2) +" , "+ QString::number(locationid) +"="+ QString::number(lid2) + " , " + QString::number(pr2) +" = "+ ui->tableWidgetDetail->item(i,PRICE)->text());
        if(iid2==itemid && lid2==locationid && pr2==price && uid2==unitid)
        {
            int cqty = ui->tableWidgetDetail->item(i,QTY)->text().toInt()+ui->txt_qty->text().toInt();
            ui->tableWidgetDetail->removeCellWidget(i,QTY);
            ui->tableWidgetDetail->setItem(i,QTY,new QTableWidgetItem(erp.intString(cqty)));

            float amt1 = ui->tableWidgetDetail->item(i,QTY)->text().toInt()*pr2;
            ui->tableWidgetDetail->removeCellWidget(i,AMOUNT);
            ui->tableWidgetDetail->setItem(i,AMOUNT,new QTableWidgetItem(erp.DecimalString(amt1)));
            checktotal();
            return;
        }
    }


    QPushButton *btn1 = new QPushButton();
    btn1->setParent(ui->tableWidgetDetail);
    btn1->setText("Delete");
    btn1->setVisible(true);
    ui->tableWidgetDetail->insertRow(ui->tableWidgetDetail->rowCount());
    int row = ui->tableWidgetDetail->rowCount()-1;
    ui->tableWidgetDetail->setItem(row,ITEM, new QTableWidgetItem(ui->txt_itemname->text()));
    ui->tableWidgetDetail->setItem(row,ITEMID, new QTableWidgetItem(ui->txt_itemid->text()));
    ui->tableWidgetDetail->setItem(row,LOCATION, new QTableWidgetItem(ui->comboLocation->currentText()));
    int index = ui->comboLocation->currentIndex();
    QString locationid = ui->comboLocation->itemData(index).toString();
    ui->tableWidgetDetail->setItem(row,LOCATIONID, new QTableWidgetItem(locationid));
    ui->tableWidgetDetail->setItem(row,QTY, new QTableWidgetItem(ui->txt_qty->text()));
    ui->tableWidgetDetail->setItem(row,UNITNAME, new QTableWidgetItem(ui->comboUnit->currentText()));
    int unitindex = ui->comboUnit->currentIndex();
    QString unitid = ui->comboUnit->itemData(unitindex).toString();
    ui->tableWidgetDetail->setItem(row,UNITID, new QTableWidgetItem(unitid));
    ui->tableWidgetDetail->setItem(row,PRICE, new QTableWidgetItem(ui->txt_price->text()));
    ui->tableWidgetDetail->setItem(row,AMOUNT, new QTableWidgetItem(ui->txt_amt->text()));
    ui->tableWidgetDetail->setCellWidget(row,BUTTON, btn1);
    connect(btn1, SIGNAL( clicked() ), this, SLOT( on_btn1_clicked() ) );
    checktotal();
}
void EditSalesInvoice::on_btn1_clicked()
{
    ui->tableWidgetDetail->removeRow(ui->tableWidgetDetail->currentRow());
    checktotal();
}
void EditSalesInvoice::checktotal()
{
    double total=0.00;
    double nettotal=0.00;
    double discount = ui->txt_discount->text().toFloat();
    double freight = ui->txt_freight->text().toFloat();
    for(int i=0; i<ui->tableWidgetDetail->rowCount(); i++)
    {
        total += ui->tableWidgetDetail->item(i,AMOUNT)->text().toFloat();
    }
    ui->txt_totalamt->setValue(erp.DecimalString(total).toFloat());
    nettotal = total-discount+freight;
    ui->txt_totNetAmount->setValue(erp.DecimalString(nettotal).toFloat());
    if(ui->comboSales_type->itemData(ui->comboSales_type->currentIndex()).toInt()==1)
    {
        ui->txt_cashReceived->setValue(ui->txt_totNetAmount->value());
        ui->txt_cashReceived->setEnabled(true);
    }
    else
    {
        ui->txt_cashReceived->setValue(0.00);
        ui->txt_cashReceived->setEnabled(false);
    }

    if(ui->txt_customerid->text().toInt()>0 && nettotal>0 && ui->dateEntrydate->text()!="")
    {
        ui->btn_save->setEnabled(true);
    }
    else
    {
        ui->btn_save->setEnabled(false);
    }
}

void EditSalesInvoice::on_txt_discount_valueChanged()
{
    checktotal();
}

void EditSalesInvoice::on_txt_freight_valueChanged()
{
    checktotal();
}

void EditSalesInvoice::on_txt_customerid_textChanged()
{
    checktotal();
}

void EditSalesInvoice::on_dateEntrydate_userDateChanged()
{
    checktotal();
}

void EditSalesInvoice::on_txt_totalamt_valueChanged()
{
    checktotal();
}

void EditSalesInvoice::on_txt_totNetAmount_valueChanged()
{
    checktotal();
}

void EditSalesInvoice::on_comboSales_type_currentIndexChanged()//int index
{
    checktotal();
}

void EditSalesInvoice::on_btn_save_clicked()
{
    if(isValid())
    {
        QString invno = ui->tableWidgetSearch->item(ui->tableWidgetSearch->currentRow(),INVOICENO)->text();


        QString invdate = ui->dateEntrydate->date().toString("yyyy-MM-dd");
        QString sales_type = ui->comboSales_type->itemData(ui->comboSales_type->currentIndex()).toString();
        //QString groupid = ui->comboCust_group->itemData(ui->comboCust_group->currentIndex()).toString();
        QString shipped_via = ui->txt_shipped_via->text();
        QString vendor_refno = ui->txt_vendorRefno->text();
        QString customerid = ui->txt_customerid->text();
        QString Carrier_id = ui->comboCarrier_id->itemData(ui->comboCarrier_id->currentIndex()).toString();
        QString notes = ui->txt_notes->text();
        QString shipto = ui->txt_shipto->text();
        QString ordernum = "0";
        float total_amount = ui->txt_totalamt->value();
        float totalDiscount = ui->txt_discount->value();
        float freight = ui->txt_freight->value();
        float netamount = ui->txt_totNetAmount->value();
        float cashreceived = ui->txt_cashReceived->value();
        float net_total = netamount;
        int lines = ui->tableWidgetDetail->rowCount();
        QString refno = vendor_refno;

        QString ord_code 	= invno.split(QRegExp("-")).value(0);//"SI";
        QString si_num 	= invno.split(QRegExp("-")).value(1);//erp.get_num(ord_code);
        QString ord_num 	= ord_code+"-"+si_num;

        if(customerid!="" && invdate!="0000-00-00" && invdate!="" && netamount>0 )
        {
            QString query =  "UPDATE tblinvoice "
                             " SET "
                             "customerid        = '"+customerid+"', "
                             "invoicedate       = '"+invdate+"', "
                             "notes             = '"+notes+"',  "
                             "totalDiscount     = '"+erp.DecimalString(totalDiscount)+"', "
                             "invoicetotal      = '"+erp.DecimalString(net_total)+"', "
                             "`lines`           = '"+erp.intString(lines)+"', "
                             "`customerbillcode` = '"+refno+"', "
                             "`vendorRefno`     = '"+vendor_refno+"',"
                             "`shipped_via`     = '"+shipped_via+"',"
                             "`freight`         = '"+erp.DecimalString(freight)+"', "
                             "`sales_type`      = '"+sales_type+"',"
                             "`Carrier_id`      = '"+Carrier_id+"',"
                             "`AdvancePayment`  = '"+erp.DecimalString(cashreceived)+"',"
                             "`net_total`       = '"+erp.DecimalString(net_total)+"' , "
                             "userid            = '"+loginid+"', "
                             "`shipto`          = '"+shipto+"' "
                             " WHERE "
                             " invoicenumber = '"+ord_num+"' ";
            QSqlQuery qry;
            qry.prepare(query);
            if(!qry.exec())
            {
                QMessageBox::critical(this,"Error","Some Error in invoice ");
                return;
            }
                QString order_id = erp.getInvoiceId(ord_num);
                QString vno = ord_num;
                QString query_transaction = "UPDATE `tblgltransvoucher` "
                        " SET "
                        "`voucherno_parent` = '"+ordernum+"',"
                        "`description`      = '"+notes + shipto+"',"
                        "`entrydate`        = '"+invdate+"',"
                        "`comments`         = 'Sales Invoice',"
                        "`companyid`        = '"+customerid+"',"
                        "`total_debit`      = '"+erp.DecimalString(net_total)+"',"
                        "`total_credit`     = '"+erp.DecimalString(net_total)+"',"
                        "`wherefrom`        = '2',"
                        "`companytype`      = 'customer'"
                        " WHERE "
                        " `voucherno` = '"+vno+"' ";
                QSqlQuery qry_transaction;
                qry_transaction.prepare(query_transaction);
                if(!qry_transaction.exec())
                {
                    QMessageBox::critical(this,"Error","Some Error in trans voucher ");
                    return;
                }
                QString voucher_id = erp.getvoucherid(vno);

                if(!conn.execQuery("delete from tblgltransaction where voucherno='"+ord_num+"'")){
                    QMessageBox::critical(this,"Error", "Some problem in updation");
                }
                if(!conn.execQuery("delete from tblinvoicepaymentdetail where invoiceid='"+ord_num+"'")){
                    QMessageBox::critical(this,"Error", "Some problem in updation");
                }
                if(!conn.execQuery("DELETE FROM tblinvoicedetail WHERE invoiceid = '"+ord_num+"'")){
                    QMessageBox::critical(this,"Error", "Some problem in updation");
                }
                if(!conn.execQuery("DELETE FROM tblitemtransdet WHERE ITransType = 'si' AND ITransID = '"+si_num+"'")){
                    QMessageBox::critical(this,"Error", "Some problem in updation");
                }


                QString TransDesc = "";
                QString query_itemtrans = "UPDATE tblitemtrans "
                                          " SET "
                                          "ITDate               = '"+invdate+"',	"
                                          "SenderRecieverID     = '"+customerid+"', "
                                          "DetailedDescription  = '"+TransDesc+"', "
                                          "OrderID              = '"+ordernum+"', "
                                          "DeliveryDate         = '"+invdate+"', "
                                          "Delivered            = '1' "
                                          " WHERE "
                                          " ITransID    = '"+si_num+"' AND  "
                                          " ITransType  = 'si' " ;
                QSqlQuery qry_itemtrans;
                qry_itemtrans.prepare(query_itemtrans);
                if(!qry_itemtrans.exec())
                {
                    QMessageBox::critical(this,"Error","Some Error in item trans ");
                    return;
                }

                int j=1;
                for(int i=0; i<ui->tableWidgetDetail->rowCount(); i++)
                {
                    QString itemid = ui->tableWidgetDetail->item(i,ITEMID)->text();
                    QString locationid = ui->tableWidgetDetail->item(i,LOCATIONID)->text();
                    int itemqtyorder = ui->tableWidgetDetail->item(i,QTY)->text().toInt();
                    float itempriceach = ui->tableWidgetDetail->item(i,PRICE)->text().toFloat();
                    float total = ui->tableWidgetDetail->item(i,AMOUNT)->text().toFloat();
                    if(itemid!="" && total>0)
                    {
                        float itemcosteach = erp.itemcost(itemid);
                        QString description = erp.itemname(itemid);
                        QString packingUnit = ui->tableWidgetDetail->item(i,UNITID)->text();//erp.getItemField(itemid,"masterpackingunitid");


                        QString query_det = "insert into tblinvoicedetail "
                           " (invoiceid, itemid, itemdescription, qty, packingUnit, priceach, locationid, linenumber, amount) "
                           " values "
                            "('"+ord_num+"', '"+itemid+"', '"+description+"', '"+erp.intString(itemqtyorder)+"', '"+packingUnit+"', '"+erp.DecimalString(itempriceach)+"',  "
                            " '"+locationid+"', '"+erp.intString(j)+"', '"+erp.DecimalString(total)+"')";
                        QSqlQuery qry_det;
                        qry_det.prepare(query_det);
                        if(!qry_det.exec())
                        {
                            QMessageBox::critical(this,"Error","Some Error in invoice detail ");
                            return;
                        }

                        QString query_td = "insert into tblitemtransdet"
                        "( ITransID, ITransType, ItemID, Qty, packingUnit, locationid, ItemPriceEach,  itemcosteach, CalType) "
                         "values"
                        "('"+si_num+"','si','"+itemid+"','"+erp.intString(itemqtyorder)+"','"+packingUnit+"','"+locationid+"','"+erp.DecimalString(itempriceach)+"','"+erp.DecimalString(itemcosteach)+"', '-')";
                        QSqlQuery qry_td;
                        qry_td.prepare(query_td);
                        if(!qry_td.exec())
                        {
                            QMessageBox::critical(this,"Error","Some Error in item trans detail ");
                            return;
                        }


                    }//end if(itemid!="" && amount>0)

                        j++;
                 }//end for(i=0; i<ui->tableWidgetDetail->rowCount(); i++)



                    TransDesc = "Sales";
                    QString TransDesc1 = "Sold To "+ui->txt_customername->text();
                    QString rece_acct = erp.getaccountAR("receivables");
                    QString dis_acct = erp.getaccountAR("discount");
                    QString freight_acct = erp.getaccountAR("freight");
                    QString income_acct = erp.getaccountAR("income");

                    float net_amt = net_total;
                    float tot_amt = total_amount;
                    int ln=1;
                    QString query_dr = "INSERT INTO `tblgltransaction` "
                                       "(`voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, description, `debit` ,`linenumber`, `entrydate`,`companytype`, `compid`)"
                                        "VALUES "
                                       "('"+vno+"', '"+ordernum+"', '"+voucher_id+"', '"+rece_acct+"' , '"+TransDesc + shipto+"', '"+erp.DecimalString(net_amt)+"', '"+erp.intString(ln)+"', '"+invdate+"', 'customer', '"+customerid+"')";
                    QSqlQuery qry_dr;
                    qry_dr.prepare(query_dr);
                    if(!qry_dr.exec())
                    {
                        QMessageBox::critical(this,"Error","Some Error in dr transaction ");
                        return;
                    }
                    if(totalDiscount>0)
                    {
                        ln++;
                        QString TransDesc_Disc = "Discount To "+ui->txt_customername->text();

                        QString query_dr2 = "INSERT INTO `tblgltransaction` "
                                           "(`voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, description, `debit` ,`linenumber`, `entrydate`,`companytype`)"
                                            "VALUES "
                                           "('"+vno+"', '"+ordernum+"', '"+voucher_id+"', '"+dis_acct+"' , '"+TransDesc_Disc+"', '"+erp.DecimalString(totalDiscount)+"', '"+erp.intString(ln)+"', '"+invdate+"', '')";
                        QSqlQuery qry_dr2;
                        qry_dr2.prepare(query_dr2);
                        if(!qry_dr2.exec())
                        {
                            QMessageBox::critical(this,"Error","Some Error in dr2 transaction ");
                            return;
                        }
                    }

                    if(freight>0)
                    {
                        ln++;
                        QString TransDesc_Disc = "Freight IN Against "+vno;

                        QString query_dr3 = "INSERT INTO `tblgltransaction` "
                                           "(`voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, description, `credit` ,`linenumber`, `entrydate`,`companytype`)"
                                            "VALUES "
                                           "('"+vno+"', '"+ordernum+"', '"+voucher_id+"', '"+freight_acct+"' , '"+TransDesc_Disc+"', '"+erp.DecimalString(freight)+"', '"+erp.intString(ln)+"', '"+invdate+"', '')";
                        QSqlQuery qry_dr3;
                        qry_dr3.prepare(query_dr3);
                        if(!qry_dr3.exec())
                        {
                            QMessageBox::critical(this,"Error","Some Error in dr3 transaction ");
                            return;
                        }
                    }


                    ln++;

                    QString query_cr = "INSERT INTO `tblgltransaction` "
                                       "(`voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, description, `credit` ,`linenumber`, `entrydate`,`companytype`)"
                                        "VALUES "
                                       "('"+vno+"', '"+ordernum+"', '"+voucher_id+"', '"+income_acct+"' , '"+TransDesc1+shipto+"', '"+erp.DecimalString(tot_amt)+"', '"+erp.intString(ln)+"', '"+invdate+"', '')";
                    QSqlQuery qry_cr;
                    qry_cr.prepare(query_cr);
                    if(!qry_cr.exec())
                    {
                        QMessageBox::critical(this,"Error","Some Error in cr transaction ");
                        return;
                    }



                    QString event = "Update";
                    QString transtype = "Sales Invoice";
                    QString transid = ord_num ;
                    erp.AddToEventLog(event,transtype,transid);

                    if(cashreceived>0)
                    {
                        QString pmt_description = "Customer Payment";
                        QString pmtno = vno;
                        QString query_paydet ="INSERT INTO `tblinvoicepaymentdetail` "
                                              "(`invoiceid`,`amount` , `voucherid` , `datereceived`, `paymeth`,  `customer_id`,  `description`)"
                                            "VALUES "
                                              "('"+vno+"','"+erp.DecimalString(cashreceived)+"', '"+pmtno+"', '"+invdate+"', '1', '"+customerid+"',  'invoice cash payment')";
                        QSqlQuery qry_paydet;
                        qry_paydet.prepare(query_paydet);
                        if(!qry_paydet.exec())
                        {
                            QMessageBox::critical(this,"Error","Some Error in invoice payment ");
                            return;
                        }


                        QString r_acct72 = erp.getaccountAR("cash");
                        QString query_cash_dr = "INSERT INTO `tblgltransaction` "
                                           "(`voucherno`, `voucherid`, `glaccountid`, `debit`, `linenumber`, `entrydate`,`companytype`,`description`) 	"
                                            " VALUES "
                                           "('"+pmtno+"', '"+voucher_id+"', '"+r_acct72+"', '"+erp.DecimalString(cashreceived)+"', '1', '"+invdate+"','','"+pmt_description+"')";
                        QSqlQuery qry_cash_dr;
                        qry_cash_dr.prepare(query_cash_dr);
                        if(!qry_cash_dr.exec())
                        {
                            QMessageBox::critical(this,"Error","Some Error in Cash receive dr ");
                            return;
                        }

                        QString query_cash_cr = "INSERT INTO `tblgltransaction` "
                                           "(`voucherno`, `voucherid`, `glaccountid`, `credit`, `linenumber`, `entrydate`,`companytype`,compid,`description`)     "
                                            "VALUES "
                                           "('"+pmtno+"', '"+voucher_id+"', '"+rece_acct+"', '"+erp.DecimalString(cashreceived)+"', '2', '"+invdate+"','customer','"+customerid+"', '"+pmt_description+shipto+"')";
                        QSqlQuery qry_cash_cr;
                        qry_cash_cr.prepare(query_cash_cr);
                        if(!qry_cash_cr.exec())
                        {
                            QMessageBox::critical(this,"Error","Some Error in Cash receive cr ");
                            return;
                        }

                        QString event = "New";
                        QString transtype = "Customer Payment";
                        QString transid = pmtno ;
                        erp.AddToEventLog(event,transtype,transid);

                    }//end if(cashreceived>0)

                    QString msg = "Sales Invoice "+ord_num+" Updated Successfully!";
                    QMessageBox::information(this,"Information",msg);
                    //clearform();

        }//end if(customerid!="" && invdate!="0000-00-00" && invdate!="" && netamount>0 )


    }//end if(isValid())
}

bool EditSalesInvoice::isValid()
{
    if(ui->dateEntrydate->text()=="")
    {
        QMessageBox::critical(this,"Validation Error","Invoice Date is required");
        ui->dateEntrydate->setFocus();
        return false;
    }
    if(ui->txt_customerid->text().toInt()<=0)
    {
        QMessageBox::critical(this,"Validation Error","Customer is required");
        ui->txt_customername->setFocus();
        return false;
    }
    if(ui->tableWidgetDetail->rowCount()<=0)
    {
        QMessageBox::critical(this,"Validation Error","Please add an item for invoice");
        ui->txt_itemname->setFocus();
        return false;
    }
    if(ui->txt_totNetAmount->value()<=0)
    {
        QMessageBox::critical(this,"Validation Error","Total Amount is 0");
        ui->txt_itemname->setFocus();
        return false;
    }
    return true;
}
void EditSalesInvoice::clearform()
{
    ui->comboSales_type->setCurrentIndex(0);
    ui->txt_vendorRefno->clear();
    ui->txt_shipped_via->clear();
    ui->comboCarrier_id->setCurrentIndex(0);
    ui->comboCust_group->setCurrentIndex(0);
    ui->comboLocation->setCurrentIndex(0);
    ui->txt_customername->clear();
    ui->txt_customerid->clear();
    ui->txt_shipto->clear();
    ui->txt_notes->clear();
    ui->txt_itemname->clear();
    ui->txt_itemid->clear();
    ui->txt_qty->clear();
    ui->txt_price->clear();
    ui->txt_amt->clear();
    ui->tableWidgetDetail->setRowCount(0);
    ui->txt_totalamt->clear();
    ui->txt_discount->clear();
    ui->txt_freight->clear();
    ui->txt_totNetAmount->clear();
    ui->txt_cashReceived->clear();
    ui->dateEntrydate->setFocus();
}

void EditSalesInvoice::on_search_customername_textEdited(const QString &arg1)
{
    QString str = sch.customers(arg1);
    autocompleter(str, ui->search_customername, ui->search_customer_id);
}

void EditSalesInvoice::on_btn_next_clicked()
{
    ui->tableWidgetSearch->setRowCount(0);
    QString invno = ui->search_invno->text();
    QString refno = ui->search_refno->text();
    QString cid = ui->search_customer_id->text();
    QString sql = "select id,invoicenumber,net_total,invoicedate,customerid from tblinvoice where invoicenumber like '%SI-%' ";
    if(invno!="")
    {
        sql += " and invoicenumber = '"+invno+"' ";
    }
    if(refno!="")
    {
        sql += " and customerbillcode = '"+refno+"' ";
    }
    if(cid!="")
    {
        sql += " and customerid = '"+cid+"' ";
    }
    sql += " order by id DESC ";
    QSqlQuery qry;
    qry.prepare(sql);
    qry.exec();
    int i=0;
    while(qry.next())
    {
        ui->tableWidgetSearch->insertRow(ui->tableWidgetSearch->rowCount());
        ui->tableWidgetSearch->setItem(i,INVOICEID, new QTableWidgetItem(qry.value("id").toString()));
        ui->tableWidgetSearch->setItem(i,INVOICENO, new QTableWidgetItem(qry.value("invoicenumber").toString()));
        ui->tableWidgetSearch->setItem(i,INVAMOUNT, new QTableWidgetItem( erp.DecimalString( qry.value("net_total").toFloat() )) );
        ui->tableWidgetSearch->setItem(i,INVDATE, new QTableWidgetItem( erp.DDMMYYDateFromSQL( qry.value("invoicedate").toString()) ) );
        ui->tableWidgetSearch->setItem(i,CUSTOMER, new QTableWidgetItem( erp.customerName( qry.value("customerid").toString()) ) );
        i++;
    }
}

void EditSalesInvoice::on_pushButton_clicked()
{
    int searchrow = ui->tableWidgetSearch->currentRow();
    if(searchrow>=0)
    {
        int id = ui->tableWidgetSearch->item(searchrow,INVOICEID)->text().toInt();
        loadInvoiceInForm(id);
    }
    else
    {
        QMessageBox::warning(this,"Warning","Please Select invoice to edit");
    }

}

void EditSalesInvoice::loadInvoiceInForm(int id)
{
    QString sql = "SELECT * FROM tblinvoice WHERE id = '"+erp.intString(id)+"' ";
    QSqlQuery qry;
    qry.prepare(sql);
    qry.exec();
    qry.next();

    ui->dateEntrydate->setDate( erp.SQLDateToQtDate(qry.value("invoicedate").toString()) );
    erp.setComboBoxSelected(ui->comboSales_type,qry.value("sales_type").toInt() );
    ui->txt_shipped_via->setText( qry.value("shipped_via").toString() );
    ui->txt_vendorRefno->setText( qry.value("vendorRefno").toString() );
    erp.setComboBoxSelected(ui->comboCarrier_id, qry.value("Carrier_id").toInt() );

    ui->txt_customername->setText( erp.customerName( qry.value("customerid").toString()) );
    ui->txt_customerid->setText( qry.value("customerid").toString() );
    ui->txt_notes->setText(qry.value("notes").toString());
    ui->txt_shipto->setText(qry.value("shipto").toString());

    ui->txt_discount->setValue(qry.value("totalDiscount").toFloat());
    ui->txt_freight->setValue(qry.value("freight").toFloat());
    ui->txt_cashReceived->setValue(0.00);

    QString sql2 = " SELECT * FROM tblinvoicedetail WHERE invoiceid = '"+qry.value("invoicenumber").toString()+"' ORDER BY linenumber ASC ";
    QSqlQuery qry2;
    qry2.prepare(sql2);
    if(qry2.exec())
    {
        ui->tableWidgetDetail->setRowCount(0);
        while(qry2.next())
        {
            QPushButton *btn1 = new QPushButton();
            btn1->setParent(ui->tableWidgetDetail);
            btn1->setText("Delete");
            btn1->setVisible(true);
            ui->tableWidgetDetail->insertRow(ui->tableWidgetDetail->rowCount());
            int row = ui->tableWidgetDetail->rowCount()-1;
            ui->tableWidgetDetail->setItem(row, ITEMID,     new QTableWidgetItem( qry2.value("itemid").toString() ) );
            ui->tableWidgetDetail->setItem(row, ITEM,       new QTableWidgetItem( erp.itemname(qry2.value("itemid").toString()) ) );
            ui->tableWidgetDetail->setItem(row, LOCATION,   new QTableWidgetItem( erp.locationName(qry2.value("locationid").toString()) ) );
            ui->tableWidgetDetail->setItem(row, LOCATIONID, new QTableWidgetItem( qry2.value("locationid").toString() ));
            ui->tableWidgetDetail->setItem(row, UNITNAME,   new QTableWidgetItem( erp.unitName(qry2.value("packingUnit").toString()) ) );
            ui->tableWidgetDetail->setItem(row, UNITID,     new QTableWidgetItem( qry2.value("packingUnit").toString() ));
            ui->tableWidgetDetail->setItem(row, QTY,        new QTableWidgetItem( qry2.value("qty").toString() ));
            ui->tableWidgetDetail->setItem(row, PRICE,      new QTableWidgetItem( erp.DecimalString(qry2.value("priceach").toFloat()) ) );
            ui->tableWidgetDetail->setItem(row, AMOUNT,     new QTableWidgetItem( erp.DecimalString(qry2.value("priceach").toFloat()) ) );
            ui->tableWidgetDetail->setCellWidget(row, BUTTON, btn1);
            connect(btn1, SIGNAL( clicked() ), this, SLOT( on_btn1_clicked() ) );
        }
    }
    ui->lbl_invoice_no->setText("Invoice No. "+ qry.value("invoicenumber").toString() );
    checktotal();


    ui->stackedWidget->setCurrentIndex(1);
}



void EditSalesInvoice::on_btn_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void EditSalesInvoice::on_pushButton_Delete_clicked()
{
    if(ui->tableWidgetSearch->currentRow()>=0)
    {

        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
                int crow = ui->tableWidgetSearch->currentRow();
                //QString id = ui->tableWidgetSearch->item(crow,INVOICEID)->text();
                QString ord_num = ui->tableWidgetSearch->item(crow,INVOICENO)->text();
                QRegExp separator("-");
                QStringList list = ord_num.split(separator);
                QString si_code = list.value(0);//+":"+list.value(1);
                QString si_num = list.value(1);
                QString sql1 = "DELETE FROM tblgltransaction WHERE voucherno = '"+ord_num+"'";
                if(!conn.execQuery(sql1))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 1");
                    return;
                }
                QString sql2 = "DELETE FROM tblgltransvoucher WHERE voucherno = '"+ord_num+"'";
                if(!conn.execQuery(sql2))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 2");
                    return;
                }
                QString sql3 = "DELETE FROM tblitemtransdet WHERE ITransID = '"+si_num+"' AND ITransType = '"+si_code+"' ";
                if(!conn.execQuery(sql3))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 3");
                    return;
                }
                QString sql4 = "DELETE FROM tblitemtrans WHERE ITransID = '"+si_num+"' AND ITransType = '"+si_code+"' ";
                if(!conn.execQuery(sql4))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 4");
                    return;
                }
                QString sql5 = "DELETE FROM tblinvoicedetail WHERE invoiceid = '"+ord_num+"'";
                if(!conn.execQuery(sql5))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 5");
                    return;
                }
                QString sql6 = "DELETE FROM tblinvoice WHERE invoicenumber = '"+ord_num+"'";
                if(!conn.execQuery(sql6))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 6");
                    return;
                }

                QString query_getpmt="select voucherid from tblinvoicepaymentdetail where invoiceid='"+ord_num+"'";
                QSqlQuery qry_getpmt;
                qry_getpmt.prepare(query_getpmt);
                qry_getpmt.exec();
                while (qry_getpmt.next()) {
                    QString pmt_no = qry_getpmt.value("voucherid").toString();
                    conn.execQuery("delete from tblgltransaction where voucherno='"+pmt_no+"' ");
                    conn.execQuery("delete from tblgltransvoucher where voucherno='"+pmt_no+"' ");
                    conn.execQuery("delete from tblinvoicepaymentdetail where voucherid='"+pmt_no+"' ");

                    QString event = "Delete";
                    QString transtype = "Customer Payment";
                    QString transid = pmt_no;
                    erp.AddToEventLog(event,transtype,transid);
                }

                QString event = "Delete";
                QString transtype = "Sale Invoice";
                QString transid = ord_num;
                erp.AddToEventLog(event,transtype,transid);
                QMessageBox::information(this,"Info","Invoice "+ord_num+" Deleted Successfully");
                on_btn_next_clicked();

          }//end if (reply == QMessageBox::Yes) {
    }//end if(ui->tableWidgetSearch->currentRow()>=0)
    else
    {
        QMessageBox::warning(this, "Warning","Please Select Invoice to delete");
    }
}
