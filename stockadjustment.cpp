#include "stockadjustment.h"
#include "ui_stockadjustment.h"

StockAdjustment::StockAdjustment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StockAdjustment)
{
    ui->setupUi(this);
    loadform();
}

StockAdjustment::~StockAdjustment()
{
    delete ui;
}


void StockAdjustment::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void StockAdjustment::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void StockAdjustment::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void StockAdjustment::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void StockAdjustment::loadform()
{
    ui->entrydate->setDate(QDate::currentDate());
    ui->detailtable->setColumnCount(9);
    QStringList titles;
    titles <<"Item"<<"Item Id"<<"Location"<<"Location Id"<<"Unit"<<"Unit Id"<<"Quantity"<<"Price"<<"Amount";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ITEMID);
    ui->detailtable->hideColumn(LOCATIONID);
    ui->detailtable->hideColumn(UNITID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(ITEMNAME,QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(LOCATIONNAME,100);
    ui->detailtable->setColumnWidth(UNITNAME,100);
    ui->detailtable->setColumnWidth(QUANTITY,100);
    ui->detailtable->setColumnWidth(PRICE,100);
    ui->detailtable->setColumnWidth(AMOUNT,100);
    erp.loadLocationsToComboBox(ui->location);
}

void StockAdjustment::checktotal()
{
    float total;
    for(int i=0; i<ui->detailtable->rowCount(); i++)
    {
        total += ui->detailtable->item(i,AMOUNT)->text().toFloat();
    }
    ui->total->setValue(total);
}



void StockAdjustment::on_btn_add_clicked()
{
    if(ui->itemid->text()=="")
    {
        QMessageBox::critical(this,"","Please select item");
        ui->itemname->setFocus();
        return;
    }
    if(ui->location->currentText()=="")
    {
        QMessageBox::critical(this,"","Please select location");
        ui->location->setFocus();
        return;
    }
    if(ui->unit->currentText()=="")
    {
        QMessageBox::critical(this,"","Please select unit");
        ui->unit->setFocus();
        return;
    }
    if(ui->qty->value()==0)
    {
        QMessageBox::critical(this,"","Quantity must be greater than 0");
        ui->qty->setFocus();
        return;
    }
    if(ui->price->value()==0)
    {
        QMessageBox::critical(this,"","Price must be greater than 0");
        ui->price->setFocus();
        return;
    }
    if(ui->amount->value()==0)
    {
        QMessageBox::critical(this,"","Amount must be greater than 0");
        return;
    }
    int row = ui->detailtable->rowCount();
    ui->detailtable->insertRow(row);
    ui->detailtable->setItem(row,ITEMNAME,new QTableWidgetItem(ui->itemname->text()));
    ui->detailtable->setItem(row,ITEMID,new QTableWidgetItem(ui->itemid->text()));
    ui->detailtable->setItem(row,LOCATIONNAME,new QTableWidgetItem(ui->location->currentText()));
    ui->detailtable->setItem(row,LOCATIONID,new QTableWidgetItem(ui->location->itemData(ui->location->currentIndex()).toString() ));
    ui->detailtable->setItem(row,UNITNAME,new QTableWidgetItem(ui->unit->currentText()));
    ui->detailtable->setItem(row,UNITID,new QTableWidgetItem(ui->unit->itemData(ui->unit->currentIndex()).toString() ));
    ui->detailtable->setItem(row,QUANTITY,new QTableWidgetItem(erp.intString(ui->qty->value()) ));
    ui->detailtable->setItem(row,PRICE,new QTableWidgetItem( erp.DecimalString(ui->price->value()) ));
    ui->detailtable->setItem(row,AMOUNT,new QTableWidgetItem( erp.DecimalString(ui->amount->value()) ));
    ui->itemname->clear();
    ui->itemid->clear();
    ui->qty->setValue(0);
    ui->price->setValue(0.00);
    ui->itemname->setFocus();
    checktotal();
}

void StockAdjustment::on_itemid_textChanged(const QString &arg1)
{
    erp.loadItemUnits(ui->unit,arg1);
    QString price = erp.getItemField(arg1,"price1");
    ui->price->setValue(price.toFloat());
}

void StockAdjustment::on_qty_valueChanged(int arg1)
{
    float amt = arg1 * ui->price->value();
    ui->amount->setValue(amt);
}

void StockAdjustment::on_price_valueChanged(double arg1)
{
    float amt = ui->qty->value() * arg1;
    ui->amount->setValue(amt);
}

void StockAdjustment::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
        checktotal();
    }
    else
    {
        QMessageBox::warning(this,"","Please select row to delete");
    }
}

void StockAdjustment::on_btn_save_clicked()
{
    QString date = ui->entrydate->date().toString("yyyy-MM-dd");
    float totalprice = ui->total->value();
    if(ui->detailtable->rowCount()>0)
    {
        QString adj_code = "sa";
        QString adj_num = erp.get_num(adj_code);
        QString adjustment_no = adj_code+"-"+adj_num;
        QString comments="Stock Adjustment ";
        int lines = ui->detailtable->rowCount();
        QString query_phys = "insert into tblinvphys "
                             " (adjustment_no, entrydate, `lines`) "
                             " values "
                             " ('"+adjustment_no+"','"+date+"', '"+erp.intString(lines)+"')" ;
        if(!conn.execQuery(query_phys))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString query_itemtrans = "insert into tblitemtrans "
                                  " ( ITransID, ITransType, ITDate, DetailedDescription) "
                                  " values "
                                  " ('"+adj_num+"', 'sa', '"+date+"', '"+comments+"')" ;
        if(!conn.execQuery(query_itemtrans))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        erp.update_ids_num(adj_code);
        QSqlQuery res(" select phid from tblinvphys where adjustment_no = '"+adjustment_no+"' ");
        res.next();
        QString phid = res.value("phid").toString();
        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString itemid = ui->detailtable->item(i,ITEMID)->text();
            int updqty = ui->detailtable->item(i,QUANTITY)->text().toInt();
            float priceach = ui->detailtable->item(i,PRICE)->text().toFloat();
            float total = ui->detailtable->item(i,AMOUNT)->text().toFloat();
            QString locationid = ui->detailtable->item(i,LOCATIONID)->text();
            QString packingUnit = ui->detailtable->item(i,UNITID)->text();
            float itemcosteach = erp.itemcost(itemid);
            int quantity=0;
            QString CalType;
            if(updqty>0)
            {
                CalType = "+";
                quantity = updqty;
            }
            else
            {
                CalType = "-";
                quantity = updqty * -1;
            }
            QString query_ins= "insert into tblinvphysdetail "
                               " (phid, itemid, itemqty,type,packingUnit ,locationid , rate , amount) "
                               " values "
                               " ('"+phid+"', '"+itemid+"', '"+erp.intString(updqty)+"','"+CalType+"', '"+packingUnit+"' , '"+locationid+"' , '"+erp.DecimalString(priceach)+"' , '"+erp.DecimalString(total)+"')";
            if(!conn.execQuery(query_ins))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
            QString query_transdet = "insert into tblitemtransdet "
                                     " ( ITransID, ITransType, ItemID, Qty, ItemPriceEach, ItemCostEach, CalType, packingUnit, locationid ) "
                                     " values "
                                     " ('"+adj_num+"', 'sa', '"+itemid+"', '"+erp.intString(quantity)+"', '"+erp.DecimalString(priceach)+"', '"+erp.DecimalString(itemcosteach)+"', '"+CalType+"', '"+packingUnit+"', '"+locationid+"')" ;
            if(!conn.execQuery(query_transdet))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
        }//end for(int i=0; i<ui->detailtable->rowCount(); i++)

        QString event = "New";
        QString transtype = "Stock Adjustment";
        QString transid = adjustment_no ;
        erp.AddToEventLog(event,transtype,transid);

        QMessageBox::information(this,"","Stock Adustment "+adjustment_no+" added Successfully!");
        clearForm();
    }//end if(totalprice!=0)
    else
    {
        QMessageBox::warning(this,"","Please add items to adjust stock ");
    }
}

void StockAdjustment::clearForm()
{
    ui->detailtable->setRowCount(0);
    checktotal();
}
