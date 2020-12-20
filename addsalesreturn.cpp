#include "addsalesreturn.h"
#include "ui_addsalesreturn.h"

AddSalesReturn::AddSalesReturn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddSalesReturn)
{
    ui->setupUi(this);

    ui->dateEntrydate->setDate(QDate::currentDate());
    erp.loadSalesTypeToComboBox(ui->comboSales_type);
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
    ui->tableWidgetDetail->setColumnWidth(UNITNAME,80);
    ui->tableWidgetDetail->setColumnWidth(UNITID,50);
    ui->tableWidgetDetail->setColumnWidth(PRICE,100);
    ui->tableWidgetDetail->setColumnWidth(AMOUNT,100);
    ui->tableWidgetDetail->setColumnWidth(BUTTON,80);
}

AddSalesReturn::~AddSalesReturn()
{
    delete ui;
}


void AddSalesReturn::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddSalesReturn::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddSalesReturn::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddSalesReturn::on_txt_customername_textEdited(const QString &arg1)
{
    QString str = sch.customers(arg1);
    autocompleter(str, ui->txt_customername, ui->txt_customerid);
}

void AddSalesReturn::checkamt()
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

void AddSalesReturn::checktotal()
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

bool AddSalesReturn::isValid()
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
void AddSalesReturn::clearform()
{
    ui->comboSales_type->setCurrentIndex(0);
    ui->txt_vendorRefno->clear();
    ui->txt_shipped_via->clear();
    ui->comboCarrier_id->setCurrentIndex(0);
    ui->comboLocation->setCurrentIndex(0);
    ui->txt_customername->clear();
    ui->txt_customerid->clear();
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

void AddSalesReturn::on_txt_itemname_textEdited(const QString &arg1)
{
    QString str = sch.items(arg1);
    autocompleter(str, ui->txt_itemname, ui->txt_itemid);
}

void AddSalesReturn::on_txt_qty_valueChanged()
{
    checkamt();
}

void AddSalesReturn::on_txt_price_valueChanged()
{
    checkamt();
}

void AddSalesReturn::on_txt_amt_valueChanged()
{
    checktotal();
}

void AddSalesReturn::on_btn_add_clicked()
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

    ui->tableWidgetDetail->setItem(row,UNITNAME, new QTableWidgetItem(ui->comboUnit->currentText()));
    int uindex = ui->comboUnit->currentIndex();
    QString unitid = ui->comboUnit->itemData(uindex).toString();
    ui->tableWidgetDetail->setItem(row,UNITID, new QTableWidgetItem(unitid));

    ui->tableWidgetDetail->setItem(row,QTY, new QTableWidgetItem(ui->txt_qty->text()));
    ui->tableWidgetDetail->setItem(row,PRICE, new QTableWidgetItem(ui->txt_price->text()));
    ui->tableWidgetDetail->setItem(row,AMOUNT, new QTableWidgetItem(ui->txt_amt->text()));
    ui->tableWidgetDetail->setCellWidget(row,BUTTON, btn1);
    connect(btn1, SIGNAL( clicked() ), this, SLOT( on_btn1_clicked() ) );
    checktotal();
}
void AddSalesReturn::on_btn1_clicked()
{
    ui->tableWidgetDetail->removeRow(ui->tableWidgetDetail->currentRow());
    checktotal();
}

void AddSalesReturn::on_txt_discount_valueChanged()
{
    checktotal();
}

void AddSalesReturn::on_txt_freight_valueChanged()
{
    checktotal();
}

void AddSalesReturn::on_txt_customerid_textChanged()
{
    checktotal();
}

void AddSalesReturn::on_dateEntrydate_userDateChanged()
{
    checktotal();
}

void AddSalesReturn::on_txt_totalamt_valueChanged()
{
    checktotal();
}

void AddSalesReturn::on_txt_totNetAmount_valueChanged()
{
    checktotal();
}

void AddSalesReturn::on_comboSales_type_currentIndexChanged()//int index
{
    checktotal();
}

void AddSalesReturn::on_btn_save_clicked()
{
    if(isValid())
    {
        QString invdate = ui->dateEntrydate->date().toString("yyyy-MM-dd");
        QString sales_type = ui->comboSales_type->itemData(ui->comboSales_type->currentIndex()).toString();
        QString shipped_via = ui->txt_shipped_via->text();
        QString vendor_refno = ui->txt_vendorRefno->text();
        QString customerid = ui->txt_customerid->text();
        QString Carrier_id = ui->comboCarrier_id->itemData(ui->comboCarrier_id->currentIndex()).toString();
        QString notes = ui->txt_notes->text();
        QString ordernum = "0";
        float total_amount = ui->txt_totalamt->value();
        float totalDiscount = ui->txt_discount->value();
        float freight = ui->txt_freight->value();
        float netamount = ui->txt_totNetAmount->value();
        float cashreceived = ui->txt_cashReceived->value();
        float net_total = netamount;
        int lines = ui->tableWidgetDetail->rowCount();
        QString refno = vendor_refno;

        QString ord_code 	= "SR";
        QString si_num 	= erp.get_num(ord_code);
        QString ord_num 	= ord_code+"-"+si_num;

        if(customerid!="" && invdate!="0000-00-00" && invdate!="" && netamount>0 )
        {
            QString query = "insert into tblreturn "
                     "(id, customer_id, retdate, notes,  totalDiscount, invoicetotal, `lines`, `customerbillcode`, `vendorRefno`,`shipped_via`,`freight`, `sales_type`,`Carrier_id`,`AdvancePayment`,`total_amount`, subtotal   )"
                     "values"
                    "('"+ord_num+"', '"+customerid+"', '"+invdate+"', '"+notes+"', '"+erp.DecimalString(totalDiscount)+"', '"+erp.DecimalString(net_total)+"', '"+erp.intString(lines)+"', '"+refno+"', '"+vendor_refno+"', '"+shipped_via+"', '"+erp.DecimalString(freight)+"', '"+sales_type+"','"+Carrier_id+"','"+erp.DecimalString(cashreceived)+"','"+erp.DecimalString(net_total)+"','"+erp.DecimalString(total_amount)+"' ) ";
            QSqlQuery qry;
            qry.prepare(query);
            if(!qry.exec())
            {
                QMessageBox::critical(this,"Error","Some Error in return ");
                return;
            }
                QString order_id = qry.lastInsertId().toString();
                if(erp.update_ids_num(ord_code))
                {
                    QString date = invdate;
                    QString vno = ord_num;

                    QString query_transaction = "INSERT INTO `tblgltransvoucher` ( "
                            "`voucherno` ,"
                            "`voucherno_parent`,"
                            "`description` ,"
                            "`entrydate` ,"
                            "`comments` ,"
                            "`companyid` ,"
                            "`total_debit` ,"
                            "`total_credit` ,"
                            "`wherefrom`,`companytype`)"
                            " VALUES "
                            " ('"+vno+"', '"+ordernum+"' , '"+notes +"', '"+invdate+"', 'Sales Return', '"+customerid+"', '"+erp.DecimalString(net_total)+"', '"+erp.DecimalString(net_total)+"','2','customer')";
                    QSqlQuery qry_transaction;
                    qry_transaction.prepare(query_transaction);
                    if(!qry_transaction.exec())
                    {
                        QMessageBox::critical(this,"Error","Some Error in trans voucher ");
                        return;
                    }
                    QString voucher_id = qry_transaction.lastInsertId().toString();

                    QString TransDesc = "Sales Return";
                    QString query_itemtrans = "insert into tblitemtrans "
                            " ( ITransID, ITransType, ITDate,	SenderRecieverID, DetailedDescription, OrderID, DeliveryDate, Delivered)"
                            " values "
                            " ('"+si_num+"', 'SR', '"+invdate+"', '"+customerid+"', '"+TransDesc+"', '"+ordernum+"', '"+invdate+"', '1')" ;
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


                            QString query_det = "insert into tblreturndetail "
                               " (orderid, itemid, description, itemqtyorder, packingUnit, itempriceach, locationid, linenumber, total) "
                               " values "
                                " ('"+ord_num+"', '"+itemid+"', '"+description+"', '"+erp.intString(itemqtyorder)+"', '"+packingUnit+"', '"+erp.DecimalString(itempriceach)+"',  "
                                " '"+locationid+"', '"+erp.intString(j)+"', '"+erp.DecimalString(total)+"')";
                            QSqlQuery qry_det;
                            qry_det.prepare(query_det);
                            if(!qry_det.exec())
                            {
                                QMessageBox::critical(this,"Error","Some Error in return detail ");
                                return;
                            }

                            QString query_td = "insert into tblitemtransdet "
                            " ( ITransID, ITransType, ItemID, Qty, packingUnit, locationid, ItemPriceEach,  itemcosteach, CalType) "
                             " values "
                            " ('"+si_num+"','SR','"+itemid+"','"+erp.intString(itemqtyorder)+"','"+packingUnit+"','"+locationid+"','"+erp.DecimalString(itempriceach)+"','"+erp.DecimalString(itemcosteach)+"', '+')";
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



                    TransDesc = "Sales Return";
                    QString TransDesc1 = "Sold To "+ui->txt_customername->text();
                    QString rece_acct = erp.getaccountAR("receivables");
                    QString dis_acct = erp.getaccountAR("discount");
                    QString freight_acct = erp.getaccountAR("freight");
                    QString r_acctid = erp.getaccountAR("return");

                    float net_amt = net_total;
                    float tot_amt = total_amount;
                    int ln=1;
                    QString query_dr = "INSERT INTO `tblgltransaction` "
                                       " (`voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, description, `credit` ,`linenumber`, `entrydate`,`companytype`, `compid`)"
                                        " VALUES "
                                       " ('"+vno+"', '"+ordernum+"', '"+voucher_id+"', '"+rece_acct+"' , '"+TransDesc +"', '"+erp.DecimalString(net_amt)+"', '"+erp.intString(ln)+"', '"+invdate+"', 'customer', '"+customerid+"')";
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
                                           " (`voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, description, `credit` ,`linenumber`, `entrydate`,`companytype`)"
                                            " VALUES "
                                           " ('"+vno+"', '"+ordernum+"', '"+voucher_id+"', '"+dis_acct+"' , '"+TransDesc_Disc+"', '"+erp.DecimalString(totalDiscount)+"', '"+erp.intString(ln)+"', '"+invdate+"', '')";
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
                                           " (`voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, description, `debit` ,`linenumber`, `entrydate`,`companytype`)"
                                            " VALUES "
                                           " ('"+vno+"', '"+ordernum+"', '"+voucher_id+"', '"+freight_acct+"' , '"+TransDesc_Disc+"', '"+erp.DecimalString(freight)+"', '"+erp.intString(ln)+"', '"+invdate+"', '')";
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
                                       " (`voucherno`, `voucherno_parent`, `voucherid`, `glaccountid`, description, `debit` ,`linenumber`, `entrydate`,`companytype`)"
                                        " VALUES "
                                       " ('"+vno+"', '"+ordernum+"', '"+voucher_id+"', '"+r_acctid+"' , '"+TransDesc1+"', '"+erp.DecimalString(tot_amt)+"', '"+erp.intString(ln)+"', '"+invdate+"', '')";
                    QSqlQuery qry_cr;
                    qry_cr.prepare(query_cr);
                    if(!qry_cr.exec())
                    {
                        QMessageBox::critical(this,"Error","Some Error in cr transaction ");
                        return;
                    }



                    QString event = "New";
                    QString transtype = "Sales Return";
                    QString transid = ord_num ;
                    erp.AddToEventLog(event,transtype,transid);

                    QString msg = "Sales Return "+ord_num+" added Successfully!";
                    QMessageBox::information(this,"Information",msg);
                    clearform();

                }//end if(erp.update_ids_num(ord_code))

        }//end if(customerid!="" && invdate!="0000-00-00" && invdate!="" && netamount>0 )


    }//end if(isValid())
}

void AddSalesReturn::on_txt_itemid_textChanged(const QString &arg1)
{
    erp.loadItemUnits(ui->comboUnit,arg1);
    QString price = erp.getItemField(arg1,"price1");
    ui->txt_price->setValue(price.toFloat());
    checkamt();
}
