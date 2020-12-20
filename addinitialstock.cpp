#include "addinitialstock.h"
#include "ui_addinitialstock.h"

AddInitialStock::AddInitialStock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddInitialStock)
{
    ui->setupUi(this);
    loadform();
}

AddInitialStock::~AddInitialStock()
{
    delete ui;
}


void AddInitialStock::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddInitialStock::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddInitialStock::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddInitialStock::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void AddInitialStock::loadform()
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



void AddInitialStock::on_btn_add_clicked()
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
}

void AddInitialStock::on_itemid_textChanged(const QString &arg1)
{
    erp.loadItemUnits(ui->unit,arg1);
    QString price = erp.getItemField(arg1,"price1");
    ui->price->setValue(price.toFloat());
}

void AddInitialStock::on_qty_valueChanged(int arg1)
{
    float amt = arg1 * ui->price->value();
    ui->amount->setValue(amt);
}

void AddInitialStock::on_price_valueChanged(double arg1)
{
    float amt = ui->qty->value() * arg1;
    ui->amount->setValue(amt);
}

void AddInitialStock::on_btn_delete_row_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        ui->detailtable->removeRow(ui->detailtable->currentRow());
    }
    else
    {
        QMessageBox::warning(this,"","Please select row to delete");
    }
}

void AddInitialStock::on_btn_save_clicked()
{
    QString date = ui->entrydate->date().toString("yyyy-MM-dd");
    if(ui->detailtable->rowCount()>0)
    {
        QString isid_code = "IS";
        QString isid_num = erp.get_num(isid_code);
        QString isid = isid_code+"-"+isid_num;
        QString TransType="Initial stock of items";
        int lines = ui->detailtable->rowCount();
        QString query = "insert into tblitemtrans "
                             " ( ITransID, ITransType, ITDate, DetailedDescription,`lines`) "
                             " values "
                             " ('"+isid_num+"', 'IS', '"+date+"', '"+TransType+"','"+erp.intString(lines)+"' )" ;
        if(!conn.execQuery(query))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        erp.update_ids_num(isid_code);
        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString itemid = ui->detailtable->item(i,ITEMID)->text();
            int qty = ui->detailtable->item(i,QUANTITY)->text().toInt();
            float priceach = ui->detailtable->item(i,PRICE)->text().toFloat();
            //float total = ui->detailtable->item(i,AMOUNT)->text().toFloat();
            QString locationid = ui->detailtable->item(i,LOCATIONID)->text();
            QString packingUnit = ui->detailtable->item(i,UNITID)->text();
            float itemcosteach = erp.itemcost(itemid);
            QString sql= " INSERT INTO item_locations "
                               " (TransID,item_id, location_id, beginning_stock_level, beginning_stock_packingUnit,  min_stock_packingUnit,itemcost) "
                               " VALUES "
                               " ('"+isid_num+"' , '"+itemid+"', '"+locationid+"', '"+erp.intString(qty)+"', '"+packingUnit+"', '"+packingUnit+"','"+erp.DecimalString(priceach)+"') ";
            if(!conn.execQuery(sql))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
            QString query_transdet = "insert into tblitemtransdet "
                                     " ( ITransID, ITransType, ItemID, Qty, ItemPriceEach, CalType, packingUnit, ItemCostEach, locationid ) "
                                     " values "
                                     " ('"+isid_num+"', 'IS', '"+itemid+"', '"+erp.intString(qty)+"', '"+erp.DecimalString(priceach)+"', '+', '"+packingUnit+"','"+erp.DecimalString(itemcosteach)+"','"+locationid+"' )" ;
            if(!conn.execQuery(query_transdet))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
        }//end for(int i=0; i<ui->detailtable->rowCount(); i++)

        QString event = "New";
        QString transtype = "Opening Stock";
        QString transid = isid ;
        erp.AddToEventLog(event,transtype,transid);

        QMessageBox::information(this,"","Initial Stock No : "+isid+" added Successfully!");
        clearForm();
    }//end if(totalprice!=0)
    else
    {
        QMessageBox::warning(this,"","Please add items for initial stock ");
    }
}

void AddInitialStock::clearForm()
{
    ui->detailtable->setRowCount(0);
}
