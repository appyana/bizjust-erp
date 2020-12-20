#include "editpurchaseinvoice.h"
#include "ui_editpurchaseinvoice.h"

EditPurchaseInvoice::EditPurchaseInvoice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditPurchaseInvoice)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadForm()) );
    timer->start(5);

    ui->podate->setDate(QDate::currentDate());
    erp.loadSalesTypeToComboBox(ui->purchase_type);
    erp.loadLocationsToComboBox(ui->location);
    QStringList titles;
    ui->detailTable->setColumnCount(9);
    titles <<"Item Description"<<"Item Id"<<"Location"<<"Location Id"<<"Quantity"<<"Unit"<<"Unit Id"<<"Price"<<"Amount";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->hideColumn(ITEMID);
    ui->detailTable->hideColumn(LOCATIONID);
    ui->detailTable->hideColumn(UNITID);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(ITEM, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(LOCATION,150);
    ui->detailTable->setColumnWidth(QTY,80);
    ui->detailTable->setColumnWidth(UNITNAME,80);
    ui->detailTable->setColumnWidth(PRICE,100);
    ui->detailTable->setColumnWidth(AMOUNT,100);
}

EditPurchaseInvoice::~EditPurchaseInvoice()
{
    delete ui;
}



void EditPurchaseInvoice::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditPurchaseInvoice::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditPurchaseInvoice::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditPurchaseInvoice::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->vendorname,ui->vendorid);
}

void EditPurchaseInvoice::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void EditPurchaseInvoice::on_itemid_textChanged(const QString &arg1)
{
    erp.loadItemUnits(ui->unit,arg1);
    QString price = erp.getItemField(arg1,"itemcosteach");
    ui->price->setValue(price.toFloat());
}

void EditPurchaseInvoice::on_btn_add_clicked()
{
    if(ui->itemid->text()=="")
    {
        QMessageBox::warning(this,"","Please select Item");
        ui->itemname->setFocus();
        return;
    }
    if(ui->location->itemData(ui->location->currentIndex()).toInt()==0 )
    {
        QMessageBox::warning(this,"","Please select Location");
        ui->location->setFocus();
        return;
    }
    if(ui->qty->value()==0)
    {
        QMessageBox::warning(this,"","Please enter quantity");
        ui->qty->setFocus();
        return;
    }
    if(ui->unit->itemData(ui->unit->currentIndex()).toInt()==0 )
    {
        QMessageBox::warning(this,"","Please select Unit");
        ui->unit->setFocus();
        return;
    }
    if(ui->price->value()==0)
    {
        QMessageBox::warning(this,"","Please enter price");
        ui->price->setFocus();
        return;
    }
    if(ui->amount->value()==0)
    {
        QMessageBox::warning(this,"","Amount must be greater than 0");
        return;
    }
    int row = ui->detailTable->rowCount();
    ui->detailTable->insertRow(row);
    ui->detailTable->setItem(row,ITEM,new QTableWidgetItem( ui->itemname->text() ));
    ui->detailTable->setItem(row,ITEMID,new QTableWidgetItem( ui->itemid->text() ));
    ui->detailTable->setItem(row,LOCATION,new QTableWidgetItem( ui->location->currentText() ));
    ui->detailTable->setItem(row,LOCATIONID,new QTableWidgetItem( ui->location->itemData(ui->location->currentIndex()).toString() ));
    ui->detailTable->setItem(row,QTY,new QTableWidgetItem( erp.intString(ui->qty->value()) ));
    ui->detailTable->setItem(row,UNITNAME,new QTableWidgetItem( ui->unit->currentText() ));
    ui->detailTable->setItem(row,UNITID,new QTableWidgetItem( ui->unit->itemData(ui->unit->currentIndex()).toString() ));
    ui->detailTable->setItem(row,PRICE,new QTableWidgetItem( erp.DecimalString(ui->price->value()) ));
    ui->detailTable->setItem(row,AMOUNT,new QTableWidgetItem( erp.DecimalString(ui->amount->value()) ));
    checktotal();
    ui->itemname->clear();
    ui->itemid->clear();
    ui->qty->setValue(0);
    ui->price->setValue(0.00);
    ui->location->setCurrentIndex(0);
    ui->itemname->setFocus();
}

void EditPurchaseInvoice::checktotal()
{
    float total=0.00;
    for(int i=0; i<ui->detailTable->rowCount(); i++)
    {
        total += ui->detailTable->item(i,AMOUNT)->text().toFloat();
    }
    ui->nettotal->setValue(total);
}

bool EditPurchaseInvoice::isValid()
{
    if(ui->podate->text()=="")
    {
        QMessageBox::warning(this,"","Purchase Date is required");
        ui->podate->setFocus();
        return false;
    }
    if(ui->vendorid->text()=="")
    {
        QMessageBox::warning(this,"","Vendor is required");
        ui->vendorname->setFocus();
        return false;
    }
    if(ui->nettotal->value()==0)
    {
        QMessageBox::warning(this,"","Total Amount must be greater than 0.00");
        ui->vendorname->setFocus();
        return false;
    }
    return true;
}

void EditPurchaseInvoice::clearform()
{
    ui->podate->setDate(QDate::currentDate());
    ui->VendorInvNo->clear();
    ui->vendorname->clear();
    ui->vendorid->clear();
    ui->notes->clear();
    ui->detailTable->setRowCount(0);
    checktotal();
    ui->podate->setFocus();
}

void EditPurchaseInvoice::on_btn_delete_row_clicked()
{
    if(ui->detailTable->currentRow()>=0)
    {
        ui->detailTable->removeRow(ui->detailTable->currentRow());
        checktotal();
    }
    else
    {
        QMessageBox::warning(this,"","please select row to delete");
    }
}

void EditPurchaseInvoice::on_btn_save_clicked()
{
    QString phid_code = "pi";
    QString phid_num = editvno.split(QRegExp("-")).value(1);//erp.get_num( phid_code );
    QString phid = phid_code+"-"+phid_num;
    QString vendorid = ui->vendorid->text();
    QString ponum="0";
    QString date = ui->podate->date().toString("yyyy-MM-dd");
    QString notes = ui->notes->text();
    float totalprice = ui->nettotal->value();
    QString comments="purchased items return";
    QString VendorInvNo = ui->VendorInvNo->text();
    QString vendorname = erp.vendorname(vendorid);
    QString purchase_type = ui->purchase_type->itemData(ui->purchase_type->currentIndex()).toString();
    QString TransDesc = "Items Purchased From "+vendorname+" against His Invoice # "+VendorInvNo;
    QString query_transaction = "UPDATE `tblgltransvoucher` "
                                " SET "
                                " `description`   = '"+TransDesc+"',"
                                " `entrydate`     = '"+date+"',"
                                " `comments`      = '"+comments+"',"
                                " `companyid`     = '"+vendorid+"',"
                                " `total_debit`   = '"+erp.DecimalString(totalprice)+"',"
                                " `total_credit`  = '"+erp.DecimalString(totalprice)+"',"
                                " companytype     = 'vendor',"
                                " financialyearid = '"+erp._yearid+"' "
                                " WHERE "
                                " `voucherno` = '"+phid+"' ";
    if(!conn.execQuery(query_transaction))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString voucher_id = erp.getvoucherid(phid);
    if(!conn.execQuery(" delete from tblgltransaction where voucherno = '"+phid+"' "))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString p_acct = erp.getaccountAP("purchaseid");
    int ln=1;
    QString query_cr = "INSERT INTO `tblgltransaction` "
                       " (`voucherno`, `voucherid`, `glaccountid`, `debit`, `entrydate`, `linenumber`, `companytype`, `financialyearid`, `description`) "
                       " VALUES "
                       " ('"+phid+"', '"+voucher_id+"', '"+p_acct+"', '"+erp.DecimalString(totalprice)+"', '"+date+"', '"+erp.intString(ln)+"', '', '"+erp._yearid+"', '"+TransDesc+"')";
    if(!conn.execQuery(query_cr))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString glaccountid = erp.getaccountAP("payable");
    ln++;
    QString query_dr = "INSERT INTO `tblgltransaction` "
                       " ( `voucherno`,`voucherid`,`glaccountid`,`credit`,`entrydate`,`linenumber`,`companytype`,`compid`,`financialyearid`,`description`) "
                       " VALUES "
                       " ( '"+phid+"', '"+voucher_id+"', '"+glaccountid+"', '"+erp.DecimalString(totalprice)+"', '"+date+"', '"+erp.intString(ln)+"', 'vendor', '"+vendorid+"', '"+erp._yearid+"','"+TransDesc+"')";
    if(!conn.execQuery(query_dr))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    int lines = ui->detailTable->rowCount();
    QString query = "UPDATE tblinvporcv "
                    " SET "
                    " `ponumber`      = '"+ponum+"',"
                    " `description`   = '"+notes+"',"
                    " `rcvdate`       = '"+date+"',"
                    " `vendorid`      = '"+vendorid+"',"
                    " `status`        =  '1',"
                    " `total`         = '"+erp.DecimalString(totalprice)+"',"
                    " `lines`         = '"+erp.intString(lines)+"',"
                    " `vendor_inv_no` = '"+VendorInvNo+"',"
                    " `PurchaseType`  = '"+purchase_type+"' "
                    " WHERE "
                    " `voucherno` = '"+phid+"' ";
    if(!conn.execQuery(query))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString rcvid_num = erp.getrcvid(phid);

    if(!conn.execQuery(" delete from tblitemtrans where ITransID = '"+phid_num+"' and ITransType = 'pi' "))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    if(!conn.execQuery(" delete from tblitemtransdet where ITransID = '"+phid_num+"' and ITransType = 'pi' "))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    if(!conn.execQuery(" delete from tblinvporcvdetail where rcvid = '"+rcvid_num+"' "))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }

    QString query_itemtrans = "insert into tblitemtrans "
                              " ( ITransID, ITransType, ITDate, DetailedDescription, RecieveDate, Recieved) "
                              " values "
                              " ('"+phid_num+"', 'pi', '"+date+"', '"+TransDesc+"', '"+date+"', '1')" ;
    if(!conn.execQuery(query_itemtrans))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }

    for(int i=0; i<ui->detailTable->rowCount(); i++)
    {
        QString itemid = ui->detailTable->item(i,ITEMID)->text();
        QString item_desc = ui->detailTable->item(i,ITEM)->text();
        int qty = ui->detailTable->item(i,QTY)->text().toInt();
        float priceach = ui->detailTable->item(i,PRICE)->text().toFloat();
        float total = ui->detailTable->item(i,AMOUNT)->text().toFloat();
        QString locationid = ui->detailTable->item(i,LOCATIONID)->text();
        QString packingUnit = ui->detailTable->item(i,UNITID)->text();
        query = "INSERT INTO `tblinvporcvdetail` "
                " (`rcvid`,`ponumber`,`itemdescription`,`itempriceach`,`itemid`,`qtyrcv`,`packingUnit`,`locationid`, `Total`) "
                " VALUES "
                " ('"+rcvid_num+"','"+ponum+"','"+item_desc+"','"+erp.DecimalString(priceach)+"','"+itemid+"','"+erp.intString(qty)+"','"+packingUnit+"', '"+locationid+"','"+erp.DecimalString(total)+"')";
        if(!conn.execQuery(query))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString query_transdet;
        query_transdet = "insert into tblitemtransdet "
                         " ( ITransID, ITransType, ItemID, Qty, ItemPriceEach, ItemCostEach, CalType, packingUnit, locationid) "
                         " values "
                         " ('"+phid_num+"', 'pi', '"+itemid+"', '"+erp.intString(qty)+"', '"+erp.DecimalString(priceach)+"','"+erp.DecimalString(priceach)+"', '+', '"+packingUnit+"','"+locationid+"')" ;
        if(!conn.execQuery(query_transdet))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
    }
    QString event = "Update";
    QString transtype = "Purchase Invoice";
    QString transid = phid;
    erp.AddToEventLog(event,transtype,transid);

    QSqlQuery infovp(" select voucherno from tblpurchasepaymentdetail where rcvid = '"+phid+"' ");
    infovp.next();
    QString vpno = infovp.value("voucherno").toString();
    if(!conn.execQuery(" select * from tblgltransvoucher where voucherno = '"+vpno+"' "))
    {
        QMessageBox::critical(this,"","Some problem in vendor payment deletion");
        return;
    }
    if(!conn.execQuery(" select * from tblgltransaction where voucherno = '"+vpno+"' "))
    {
        QMessageBox::critical(this,"","Some problem in vendor payment deletion");
        return;
    }
    if(!conn.execQuery(" select * from tblpurchasepaymentdetail where voucherno = '"+vpno+"' "))
    {
        QMessageBox::critical(this,"","Some problem in vendor payment deletion");
        return;
    }


    if(purchase_type.toInt()==1)
    {
        QString pmt_code = "vp";
        QString pmt_num = erp.get_num( pmt_code );
        QString vno = pmt_code+"-"+pmt_num;
        erp.update_ids_num(pmt_code);
        QString query_transaction = "INSERT INTO `tblgltransvoucher` "
                                    " (`voucherno`, `description`, `entrydate`, `comments`, `companyid`, `total_debit`, `total_credit`, `companytype`, `financialyearid`) "
                                    " VALUES "
                                    " ('"+vno+"', '"+notes+"', '"+date+"', '', '"+vendorid+"', '"+erp.DecimalString(totalprice)+"', '"+erp.DecimalString(totalprice)+"', 'vendor', '"+erp._yearid+"')";
        if(!conn.execQuery(query_transaction))
        {
            QMessageBox::critical(this,"","Some problem in vendor payment");
            return;
        }
        voucher_id = erp.getvoucherid(vno);
        QString account_ID = erp.getaccountAP("glcashid");
        QString query_paydet ="INSERT INTO `tblpurchasepaymentdetail` "
                              " (`amount`, `voucherno`, `rcvid`, `datereceived`, `paymeth`, `bank_id`, `vendorid`, `description`) "
                              " VALUES "
                              " ('"+erp.DecimalString(totalprice)+"', '"+vno+"',  '"+phid+"','"+date+"', '"+purchase_type+"', '"+account_ID+"', '"+vendorid+"', '"+notes+"')";
        if(!conn.execQuery(query_paydet))
        {
            QMessageBox::critical(this,"","Some problem in vendor payment");
            return;
        }
        QString acct_pay = erp.getaccountAP("payable");
        int line_no=1;
        query_dr = "INSERT INTO `tblgltransaction` "
                   " (`voucherno`, `voucherid`, `glaccountid`, `description`, `debit`, `linenumber`, `entrydate`, `companytype`,`compid`) "
                   " VALUES "
                   " ('"+vno+"', '"+voucher_id+"', '"+acct_pay+"', '"+notes+"', '"+erp.DecimalString(totalprice)+"', '"+erp.intString(line_no)+"', '"+date+"', 'vendor','"+vendorid+"')";
        if(!conn.execQuery(query_dr))
        {
            QMessageBox::critical(this,"","Some problem in vendor payment");
            return;
        }
        line_no++;
        query_cr = "INSERT INTO `tblgltransaction`"
                   " (`voucherno`, `voucherid`, `glaccountid`, `description`, `credit`, `linenumber`, `entrydate`,`companytype`) "
                   " VALUES "
                   " ('"+vno+"', '"+voucher_id+"', '"+account_ID+"', '"+notes+"', '"+erp.DecimalString(totalprice)+"', '"+erp.intString(line_no)+"', '"+date+"','')";
        if(!conn.execQuery(query_cr))
        {
            QMessageBox::critical(this,"","Some problem in vendor payment");
            return;
        }
        event = "New";
        transtype = "Vendor Payment";
        transid = vno;
        erp.AddToEventLog(event,transtype,transid);
    }
    QMessageBox::information(this,"","Purchase Invoice "+phid+" updated Successfully!");
    //clearform();
}

void EditPurchaseInvoice::on_qty_valueChanged(int arg1)
{
    float amt=0.00;
    int qty = arg1;
    float price = ui->price->value();
    amt = qty * price;
    ui->amount->setValue(amt);
}

void EditPurchaseInvoice::on_price_valueChanged(double arg1)
{
    float amt=0.00;
    int qty = ui->qty->value();
    float price = arg1;
    amt = qty * price;
    ui->amount->setValue(amt);
}

void EditPurchaseInvoice::loadForm()
{
    timer->stop();
    ui->editScrollArea->setVisible(false);
    ui->search_begindate->setDate(erp.reportBeginDate());
    ui->search_enddate->setDate(QDate::currentDate());

    QStringList titles;
    ui->searchTable->setColumnCount(4);
    titles <<"Invoice\n#"<<"Vendor Name"<<"Entry Date"<<"Amount";
    ui->searchTable->setHorizontalHeaderLabels(titles);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(VENDORNAME, QHeaderView::Stretch);
    ui->searchTable->setColumnWidth(PINUMBER,80);
    ui->searchTable->setColumnWidth(ENTRYDATE,80);
    ui->searchTable->setColumnWidth(SEARCHAMOUNT,80);
}

void EditPurchaseInvoice::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString begindate = ui->search_begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->search_enddate->date().toString("yyyy-MM-dd");
    QString vendorid = ui->search_vendorid->text();
    QString vno = ui->search_vno->text();
    QString query = "SELECT * FROM `tblinvporcv` WHERE voucherno !='' AND ponumber='0' and `rcvdate` BETWEEN "
                    " '"+begindate+"' AND '"+enddate+"' ";
    if(vendorid!="")
    {
        query += " and vendorid = '"+vendorid+"' ";
    }
    if(vno!="")
    {
        query += " and (`voucherno` = '"+vno+"' OR `voucherno` = 'pi-"+vno+"') ";
    }
    query += " order by rcvid desc ";
    QSqlQuery info(query);
    while(info.next())
    {
        int row = ui->searchTable->rowCount();
        ui->searchTable->insertRow(row);
        ui->searchTable->setItem(row,PINUMBER,new QTableWidgetItem( info.value("voucherno").toString() ));
        ui->searchTable->setItem(row,VENDORNAME,new QTableWidgetItem( erp.vendorname( info.value("vendorid").toString() ) ));
        ui->searchTable->setItem(row,ENTRYDATE,new QTableWidgetItem( info.value("rcvdate").toDate().toString("dd/MM/yyyy") ));
        ui->searchTable->setItem(row,SEARCHAMOUNT,new QTableWidgetItem( erp.amountString( info.value("total").toFloat() ) ));
    }
}

void EditPurchaseInvoice::on_btn_edit_clicked()
{
    if(ui->searchTable->currentRow()<0)
    {
        QMessageBox::warning(this,"","Please select invoice to edit");
        return;
    }
    ui->detailTable->setRowCount(0);
    QString vno = ui->searchTable->item(ui->searchTable->currentRow(),PINUMBER)->text();
    editvno = vno;
    ui->lbl_vno->setText(vno);
    QSqlQuery info(" select * from tblinvporcv where voucherno = '"+vno+"' ");
    info.next();
    ui->podate->setDate( info.value("rcvdate").toDate() );
    ui->VendorInvNo->setText( info.value("vendor_inv_no").toString() );
    erp.setComboBoxSelected(ui->purchase_type, info.value("PurchaseType").toInt() );
    ui->vendorname->setText( erp.vendorname(info.value("vendorid").toString()) );
    ui->vendorid->setText( info.value("vendorid").toString() );
    ui->notes->setText( info.value("description").toString() );
    ui->nettotal->setValue( info.value("total").toFloat() );
    QString rcvid = info.value("rcvid").toString();
    QSqlQuery det(" select * from tblinvporcvdetail where rcvid = '"+rcvid+"' ");
    while (det.next())
    {
        int row = ui->detailTable->rowCount();
        ui->detailTable->insertRow(row);
        ui->detailTable->setItem(row,ITEM,new QTableWidgetItem( erp.itemname(det.value("itemid").toString()) ));
        ui->detailTable->setItem(row,ITEMID,new QTableWidgetItem( det.value("itemid").toString() ));
        ui->detailTable->setItem(row,LOCATION,new QTableWidgetItem( erp.locationName(det.value("locationid").toString()) ));
        ui->detailTable->setItem(row,LOCATIONID,new QTableWidgetItem( det.value("locationid").toString() ));
        ui->detailTable->setItem(row,QTY,new QTableWidgetItem( erp.intString(det.value("qtyrcv").toInt()) ));
        ui->detailTable->setItem(row,UNITNAME,new QTableWidgetItem( erp.unitName(det.value("packingUnit").toString()) ));
        ui->detailTable->setItem(row,UNITID,new QTableWidgetItem( det.value("packingUnit").toString() ));
        ui->detailTable->setItem(row,PRICE,new QTableWidgetItem( erp.DecimalString(det.value("itempriceach").toFloat()) ));
        ui->detailTable->setItem(row,AMOUNT,new QTableWidgetItem( erp.DecimalString(det.value("Total").toFloat()) ));
        checktotal();
    }
    ui->searchFrame->setVisible(false);
    ui->editScrollArea->setVisible(true);
}

void EditPurchaseInvoice::on_btn_delete_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {

        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
                int crow = ui->searchTable->currentRow();
                //QString id = ui->tableWidgetSearch->item(crow,INVOICEID)->text();
                QString vno = ui->searchTable->item(crow,PINUMBER)->text();
                QRegExp separator("-");
                QStringList list = vno.split(separator);
                QString pi_code = list.value(0);//+":"+list.value(1);
                QString phid_num = list.value(1);
                QString sql1 = "DELETE FROM tblgltransaction WHERE voucherno = '"+vno+"'";
                if(!conn.execQuery(sql1))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 1");
                    return;
                }
                QString sql2 = "DELETE FROM tblgltransvoucher WHERE voucherno = '"+vno+"'";
                if(!conn.execQuery(sql2))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 2");
                    return;
                }
                QString sql3 = "DELETE FROM tblitemtransdet WHERE ITransID = '"+phid_num+"' AND ITransType = '"+pi_code+"' ";
                if(!conn.execQuery(sql3))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 3");
                    return;
                }
                QString sql4 = "DELETE FROM tblitemtrans WHERE ITransID = '"+phid_num+"' AND ITransType = '"+pi_code+"' ";
                if(!conn.execQuery(sql4))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 4");
                    return;
                }
                QString rcvid = erp.getrcvid(vno);
                QString sql5 = "DELETE FROM tblinvporcvdetail WHERE rcvid = '"+rcvid+"'";
                if(!conn.execQuery(sql5))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 5");
                    return;
                }
                QString sql6 = "DELETE FROM tblinvporcv WHERE voucherno = '"+vno+"'";
                if(!conn.execQuery(sql6))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 6");
                    return;
                }

                QString event = "Delete";
                QString transtype = "Purchase Invoice";
                QString transid = vno;
                erp.AddToEventLog(event,transtype,transid);
                QMessageBox::information(this,"Info","Purchase Invoice "+vno+" Deleted Successfully");
                on_btn_search_clicked();

          }//end if (reply == QMessageBox::Yes) {
    }//end if(ui->tableWidgetSearch->currentRow()>=0)
    else
    {
        QMessageBox::warning(this, "Warning","Please Select invoice to delete");
    }
}

void EditPurchaseInvoice::on_btn_back_clicked()
{
    ui->editScrollArea->setVisible(false);
    ui->searchFrame->setVisible(true);
}

void EditPurchaseInvoice::on_search_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->search_vendorname,ui->search_vendorid);
}
