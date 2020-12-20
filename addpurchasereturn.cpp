#include "addpurchasereturn.h"
#include "ui_addpurchasereturn.h"

AddPurchaseReturn::AddPurchaseReturn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddPurchaseReturn)
{
    ui->setupUi(this);

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

AddPurchaseReturn::~AddPurchaseReturn()
{
    delete ui;
}


void AddPurchaseReturn::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddPurchaseReturn::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddPurchaseReturn::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddPurchaseReturn::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->vendorname,ui->vendorid);
}

void AddPurchaseReturn::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void AddPurchaseReturn::on_itemid_textChanged(const QString &arg1)
{
    erp.loadItemUnits(ui->unit,arg1);
    QString price = erp.getItemField(arg1,"itemcosteach");
    ui->price->setValue(price.toFloat());
}

void AddPurchaseReturn::on_btn_add_clicked()
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

void AddPurchaseReturn::checktotal()
{
    float total=0.00;
    for(int i=0; i<ui->detailTable->rowCount(); i++)
    {
        total += ui->detailTable->item(i,AMOUNT)->text().toFloat();
    }
    ui->nettotal->setValue(total);
}

bool AddPurchaseReturn::isValid()
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

void AddPurchaseReturn::clearform()
{
    ui->podate->setDate(QDate::currentDate());
    ui->vendorname->clear();
    ui->vendorid->clear();
    ui->notes->clear();
    ui->detailTable->setRowCount(0);
    checktotal();
    ui->podate->setFocus();
}

void AddPurchaseReturn::on_btn_delete_row_clicked()
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

void AddPurchaseReturn::on_btn_save_clicked()
{
    QString prid_code = "pr";
    QString prid_num = erp.get_num( prid_code );
    QString prid = prid_code+"-"+prid_num;
    QString vendorid = ui->vendorid->text();
    QString ponum="0";
    QString date = ui->podate->date().toString("yyyy-MM-dd");
    QString notes = ui->notes->text();
    float totalprice = ui->nettotal->value();
    QString comments="purchased items return";
    QString wherefrom="1";
    QString query_transaction = "INSERT INTO `tblgltransvoucher` "
                                " ( `voucherno` ,`description` ,`entrydate` ,`comments` ,`companyid` ,`total_debit` ,`total_credit` ,`wherefrom`,companytype, financialyearid) "
                                " VALUES "
                                " ( '"+prid+"', '"+notes+"', '"+date+"', '"+comments+"', '"+vendorid+"', '"+erp.DecimalString(totalprice)+"', '"+erp.DecimalString(totalprice)+"','"+wherefrom+"','vendor', '"+erp._yearid+"');";
    if(!conn.execQuery(query_transaction))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QString voucher_id = erp.getvoucherid(prid);

    int lines = ui->detailTable->rowCount();
    QString query = "INSERT INTO tblpurchasereturn "
                    " (prid, `ponumber`, `description`, `entrydate`, `vendorid`, `totalprice`, `lines`) "
                    " VALUES  "
                    " ('"+prid+"', '"+ponum+"', '"+notes+"', '"+date+"', '"+vendorid+"', '"+erp.DecimalString(totalprice)+"', '"+erp.intString(lines)+"' )";
    if(!conn.execQuery(query))
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
                       " (`voucherno`, `voucherid`, `glaccountid`, `debit`, `entrydate`, `linenumber`, `companytype`, `financialyearid`, `compid`, description) "
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
                       " ( `voucherno`,`voucherid`,`glaccountid`,`credit`,`entrydate`,`linenumber`,`companytype`,`financialyearid`, description) "
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
    QString event = "New";
    QString transtype = "Purchase Return";
    QString transid = prid;
    erp.AddToEventLog(event,transtype,transid);
    QMessageBox::information(this,"","Purchase Return "+prid+" added Successfully!");
    clearform();
}

void AddPurchaseReturn::on_qty_valueChanged(int arg1)
{
    float amt=0.00;
    int qty = arg1;
    float price = ui->price->value();
    amt = qty * price;
    ui->amount->setValue(amt);
}

void AddPurchaseReturn::on_price_valueChanged(double arg1)
{
    float amt=0.00;
    int qty = ui->qty->value();
    float price = arg1;
    amt = qty * price;
    ui->amount->setValue(amt);
}
