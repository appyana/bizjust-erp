#include "editpurchasereturn.h"
#include "ui_editpurchasereturn.h"

EditPurchaseReturn::EditPurchaseReturn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditPurchaseReturn)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadForm()) );
    timer->start(5);

    ui->podate->setDate(QDate::currentDate());
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

EditPurchaseReturn::~EditPurchaseReturn()
{
    delete ui;
}


void EditPurchaseReturn::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditPurchaseReturn::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditPurchaseReturn::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditPurchaseReturn::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->vendorname,ui->vendorid);
}

void EditPurchaseReturn::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void EditPurchaseReturn::on_itemid_textChanged(const QString &arg1)
{
    erp.loadItemUnits(ui->unit,arg1);
    QString price = erp.getItemField(arg1,"itemcosteach");
    ui->price->setValue(price.toFloat());
}

void EditPurchaseReturn::on_btn_add_clicked()
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

void EditPurchaseReturn::checktotal()
{
    float total=0.00;
    for(int i=0; i<ui->detailTable->rowCount(); i++)
    {
        total += ui->detailTable->item(i,AMOUNT)->text().toFloat();
    }
    ui->nettotal->setValue(total);
}

bool EditPurchaseReturn::isValid()
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

void EditPurchaseReturn::clearform()
{
    ui->podate->setDate(QDate::currentDate());
    ui->vendorname->clear();
    ui->vendorid->clear();
    ui->notes->clear();
    ui->detailTable->setRowCount(0);
    checktotal();
    ui->podate->setFocus();
}

void EditPurchaseReturn::on_btn_delete_row_clicked()
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

void EditPurchaseReturn::on_btn_save_clicked()
{
    QString prid_code = "pr";
    QString prid_num = editvno.split(QRegExp("-")).value(1);//erp.get_num( phid_code );
    QString prid = prid_code+"-"+prid_num;
    QString vendorid = ui->vendorid->text();
    QString ponum="0";
    QString date = ui->podate->date().toString("yyyy-MM-dd");
    QString notes = ui->notes->text();
    float totalprice = ui->nettotal->value();
    QString total_amount = erp.DecimalString(totalprice);
    QString comments="purchased items return";
    QString query_transaction = "update tblgltransvoucher "
                                " SET "
                                " companyid='"+vendorid+"',"
                                " description = '"+notes+"',"
                                " entrydate = '"+date+"',"
                                " total_debit='"+total_amount+"',"
                                " total_credit='"+total_amount+"'"
                                " WHERE "
                                " voucherno = '"+prid+"' ";
    if(!conn.execQuery(query_transaction))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString voucher_id = erp.getvoucherid(prid);

    QString query = "UPDATE tblpurchasereturn "
                    " SET "
                    " description = '"+notes+"',"
                    " entrydate = '"+date+"',"
                    " vendorid = '"+vendorid+"',"
                    " totalprice = '"+total_amount+"'"
                    " WHERE prid = '"+prid+"'";
    if(!conn.execQuery(query))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }


    if(!conn.execQuery(" delete from tblgltransaction where voucherno = '"+prid+"' "))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    if(!conn.execQuery(" delete from tblitemtrans where ITransID = '"+prid_num+"' and ITransType = 'pr' "))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    if(!conn.execQuery(" delete from tblitemtransdet where ITransID = '"+prid_num+"' and ITransType = 'pr' "))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    if(!conn.execQuery(" delete from tblpreturndetail where prid = '"+prid+"' "))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }

    QString query_itemtrans = "insert into tblitemtrans "
                              " ( ITransID, ITransType, ITDate, DetailedDescription, OrderID, RecieveDate, Recieved) "
                              " values "
                              " ('"+prid_num+"', 'PR', '"+date+"', '"+comments+"', 0, '"+date+"', '1')" ;
    if(!conn.execQuery(query_itemtrans))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }


    QString p_acct = erp.getaccountAP("payable");
    int ln=1;
    QString query_cr = "INSERT INTO `tblgltransaction` "
                       " (`voucherno`, `voucherid`, `glaccountid`, `debit`, `entrydate`, `linenumber`, `companytype`, `financialyearid`, `compid`,description) "
                       " VALUES "
                       " ('"+prid+"', '"+voucher_id+"', '"+p_acct+"', '"+erp.DecimalString(totalprice)+"', '"+date+"', '"+erp.intString(ln)+"', 'vendor', '"+erp._yearid+"', '"+vendorid+"','"+comments+" "+notes+"')";
    if(!conn.execQuery(query_cr))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString glaccountid = erp.getaccountAP("purchaseReturn");
    ln++;
    QString query_dr = "INSERT INTO `tblgltransaction` "
                       " ( `voucherno`,`voucherid`,`glaccountid`,`credit`,`entrydate`,`linenumber`,`companytype`,`financialyearid`,description) "
                       " VALUES "
                       " ( '"+prid+"', '"+voucher_id+"', '"+glaccountid+"', '"+erp.DecimalString(totalprice)+"', '"+date+"', '"+erp.intString(ln)+"', '', '"+erp._yearid+"','"+comments+" "+notes+"')";
    if(!conn.execQuery(query_dr))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    erp.update_ids_num(prid_code);
    for(int i=0; i<ui->detailTable->rowCount(); i++)
    {
        QString itemid = ui->detailTable->item(i,ITEMID)->text();
        //QString item_desc = ui->detailTable->item(i,ITEM)->text();
        int qty = ui->detailTable->item(i,QTY)->text().toInt();
        float priceach = ui->detailTable->item(i,PRICE)->text().toFloat();
        float total = ui->detailTable->item(i,AMOUNT)->text().toFloat();
        QString locationid = ui->detailTable->item(i,LOCATIONID)->text();
        QString packingUnit = ui->detailTable->item(i,UNITID)->text();
        query = "INSERT INTO `tblpreturndetail` "
                " ( `prid`, `ponumber` ,`itemid` ,`itempriceach` ,`priceunitsperstockunit` ,`itemqty` ,`packingUnit` ,`locationid` ,`itemvendorid` ,`detailnote`,total) "
                " VALUES "
                " ( '"+prid+"', '"+ponum+"', '"+itemid+"', '"+erp.DecimalString(priceach)+"' , '1', '"+erp.intString(qty)+"','"+packingUnit+"','"+locationid+"', '"+vendorid+"', '"+notes+"','"+erp.DecimalString(total)+"');";
        if(!conn.execQuery(query))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString query_transdet;
        query_transdet = "insert into tblitemtransdet "
                         " ( ITransID, ITransType, ItemID, Qty, ItemPriceEach, CalType, packingUnit, locationid) "
                         " values "
                         " ('"+prid_num+"', 'PR', '"+itemid+"', '"+erp.intString(qty)+"', '"+erp.DecimalString(priceach)+"', '-', '"+packingUnit+"','"+locationid+"')" ;
        if(!conn.execQuery(query_transdet))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
    }
    QString event = "Update";
    QString transtype = "Purchase Return";
    QString transid = prid;
    erp.AddToEventLog(event,transtype,transid);
    QMessageBox::information(this,"","Purchase Return "+prid+" updated Successfully!");
    //clearform();
}

void EditPurchaseReturn::on_qty_valueChanged(int arg1)
{
    float amt=0.00;
    int qty = arg1;
    float price = ui->price->value();
    amt = qty * price;
    ui->amount->setValue(amt);
}

void EditPurchaseReturn::on_price_valueChanged(double arg1)
{
    float amt=0.00;
    int qty = ui->qty->value();
    float price = arg1;
    amt = qty * price;
    ui->amount->setValue(amt);
}

void EditPurchaseReturn::loadForm()
{
    timer->stop();
    ui->editScrollArea->setVisible(false);
    ui->search_begindate->setDate(erp.reportBeginDate());
    ui->search_enddate->setDate(QDate::currentDate());

    QStringList titles;
    ui->searchTable->setColumnCount(4);
    titles <<"Return\n#"<<"Vendor Name"<<"Entry Date"<<"Amount";
    ui->searchTable->setHorizontalHeaderLabels(titles);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(VENDORNAME, QHeaderView::Stretch);
    ui->searchTable->setColumnWidth(PINUMBER,80);
    ui->searchTable->setColumnWidth(ENTRYDATE,80);
    ui->searchTable->setColumnWidth(SEARCHAMOUNT,80);
}

void EditPurchaseReturn::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString begindate = ui->search_begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->search_enddate->date().toString("yyyy-MM-dd");
    QString vendorid = ui->search_vendorid->text();
    QString vno = ui->search_vno->text();
    QString query = "SELECT * FROM `tblpurchasereturn` WHERE prid !='' AND ponumber='0' and `entrydate` BETWEEN "
                    " '"+begindate+"' AND '"+enddate+"' ";
    if(vendorid!="")
    {
        query += " and vendorid = '"+vendorid+"' ";
    }
    if(vno!="")
    {
        query += " and (`prid` = '"+vno+"' OR `prid` = 'pr-"+vno+"') ";
    }
    query += " order by poid desc ";
    QSqlQuery info(query);
    while(info.next())
    {
        int row = ui->searchTable->rowCount();
        ui->searchTable->insertRow(row);
        ui->searchTable->setItem(row,PINUMBER,new QTableWidgetItem( info.value("prid").toString() ));
        ui->searchTable->setItem(row,VENDORNAME,new QTableWidgetItem( erp.vendorname( info.value("vendorid").toString() ) ));
        ui->searchTable->setItem(row,ENTRYDATE,new QTableWidgetItem( info.value("entrydate").toDate().toString("dd/MM/yyyy") ));
        ui->searchTable->setItem(row,SEARCHAMOUNT,new QTableWidgetItem( erp.amountString( info.value("totalprice").toFloat() ) ));
    }
}

void EditPurchaseReturn::on_btn_edit_clicked()
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
    QSqlQuery info(" select * from tblpurchasereturn where prid = '"+vno+"' ");
    info.next();
    ui->podate->setDate( info.value("entrydate").toDate() );
    ui->vendorname->setText( erp.vendorname(info.value("vendorid").toString()) );
    ui->vendorid->setText( info.value("vendorid").toString() );
    ui->notes->setText( info.value("description").toString() );
    ui->nettotal->setValue( info.value("totalprice").toFloat() );
    QSqlQuery det(" select * from tblpreturndetail where prid = '"+vno+"' ");
    while (det.next())
    {
        int row = ui->detailTable->rowCount();
        ui->detailTable->insertRow(row);
        ui->detailTable->setItem(row,ITEM,new QTableWidgetItem( erp.itemname(det.value("itemid").toString()) ));
        ui->detailTable->setItem(row,ITEMID,new QTableWidgetItem( det.value("itemid").toString() ));
        ui->detailTable->setItem(row,LOCATION,new QTableWidgetItem( erp.locationName(det.value("locationid").toString()) ));
        ui->detailTable->setItem(row,LOCATIONID,new QTableWidgetItem( det.value("locationid").toString() ));
        ui->detailTable->setItem(row,QTY,new QTableWidgetItem( erp.intString(det.value("itemqty").toInt()) ));
        ui->detailTable->setItem(row,UNITNAME,new QTableWidgetItem( erp.unitName(det.value("packingUnit").toString()) ));
        ui->detailTable->setItem(row,UNITID,new QTableWidgetItem( det.value("packingUnit").toString() ));
        ui->detailTable->setItem(row,PRICE,new QTableWidgetItem( erp.DecimalString(det.value("itempriceach").toFloat()) ));
        ui->detailTable->setItem(row,AMOUNT,new QTableWidgetItem( erp.DecimalString(det.value("total").toFloat()) ));
        checktotal();
    }
    ui->searchFrame->setVisible(false);
    ui->editScrollArea->setVisible(true);
}

void EditPurchaseReturn::on_btn_delete_clicked()
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
                QString sql5 = "DELETE FROM tblpurchasereturn WHERE prid = '"+vno+"'";
                if(!conn.execQuery(sql5))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 5");
                    return;
                }
                QString sql6 = "DELETE FROM tblpreturndetail WHERE prid = '"+vno+"'";
                if(!conn.execQuery(sql6))
                {
                    QMessageBox::critical(this,"Error","Some problem deletion 6");
                    return;
                }

                QString event = "Delete";
                QString transtype = "Purchase Return";
                QString transid = vno;
                erp.AddToEventLog(event,transtype,transid);
                QMessageBox::information(this,"Info","Purchase Return "+vno+" Deleted Successfully");
                on_btn_search_clicked();

          }//end if (reply == QMessageBox::Yes) {
    }//end if(ui->tableWidgetSearch->currentRow()>=0)
    else
    {
        QMessageBox::warning(this, "Warning","Please Select invoice to delete");
    }
}

void EditPurchaseReturn::on_btn_back_clicked()
{
    ui->editScrollArea->setVisible(false);
    ui->searchFrame->setVisible(true);
}

void EditPurchaseReturn::on_search_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->search_vendorname,ui->search_vendorid);
}
