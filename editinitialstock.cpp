#include "editinitialstock.h"
#include "ui_editinitialstock.h"

EditInitialStock::EditInitialStock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditInitialStock)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadform()) );
    timer->start(5);
}

EditInitialStock::~EditInitialStock()
{
    delete ui;
}


void EditInitialStock::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditInitialStock::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditInitialStock::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditInitialStock::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void EditInitialStock::loadform()
{
    timer->stop();
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
    ui->search_begindate->setDate(erp.reportBeginDate());
    ui->search_enddate->setDate(QDate::currentDate());
    ui->searchTable->setColumnCount(2);
    ui->searchTable->setHorizontalHeaderItem(0,new QTableWidgetItem("Stock #"));
    ui->searchTable->setHorizontalHeaderItem(1,new QTableWidgetItem("Date"));
    ui->searchTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
}




void EditInitialStock::on_btn_add_clicked()
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

void EditInitialStock::on_itemid_textChanged(const QString &arg1)
{
    erp.loadItemUnits(ui->unit,arg1);
    QString price = erp.getItemField(arg1,"price1");
    ui->price->setValue(price.toFloat());
}

void EditInitialStock::on_qty_valueChanged(int arg1)
{
    float amt = arg1 * ui->price->value();
    ui->amount->setValue(amt);
}

void EditInitialStock::on_price_valueChanged(double arg1)
{
    float amt = ui->qty->value() * arg1;
    ui->amount->setValue(amt);
}

void EditInitialStock::on_btn_delete_row_clicked()
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

void EditInitialStock::on_btn_save_clicked()
{
    QString date = ui->entrydate->date().toString("yyyy-MM-dd");
    if(ui->detailtable->rowCount()>0)
    {
        //QString isid_code = "IS";
        QString ITransID = phid;
        QString TransType="Initial stock of items";
        int lines = ui->detailtable->rowCount();

        QString query_itemtrans = "UPDATE tblitemtrans SET "
                                  " ITDate = '"+date+"',"
                                  " DetailedDescription = '"+TransType+"', "
                                  " `lines` = '"+erp.intString(lines)+"' "
                                  " WHERE "
                                  " ITransID = '"+ITransID+"' AND ITransType =  'IS' " ;
        if(!conn.execQuery(query_itemtrans))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }

        if(!conn.execQuery("delete from item_locations where TransID='"+ITransID+"' "))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }

        if(!conn.execQuery("delete from tblitemtransdet where ITransID = '"+ITransID+"' and ITransType = 'IS' "))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }

        for(int i=0; i<ui->detailtable->rowCount(); i++)
        {
            QString itemid = ui->detailtable->item(i,ITEMID)->text();
            int qty = ui->detailtable->item(i,QUANTITY)->text().toInt();
            float priceach = ui->detailtable->item(i,PRICE)->text().toFloat();
            //float total = ui->detailtable->item(i,AMOUNT)->text().toFloat();
            QString locationid = ui->detailtable->item(i,LOCATIONID)->text();
            QString packingUnit = ui->detailtable->item(i,UNITID)->text();
            float itemcosteach = erp.itemcost(itemid);

            QString query_ins= "INSERT INTO item_locations "
                               " (TransID,item_id, location_id, beginning_stock_level, beginning_stock_packingUnit,  min_stock_packingUnit,itemcost) "
                               " VALUES "
                               " ('"+ITransID+"' , '"+itemid+"', '"+locationid+"', '"+erp.intString(qty)+"', '"+packingUnit+"', '"+packingUnit+"','"+erp.DecimalString(priceach)+"') ";
            if(!conn.execQuery(query_ins))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
            QString query_transdet = "insert into tblitemtransdet "
                                     " ( ITransID, ITransType, ItemID, Qty, ItemPriceEach, CalType, packingUnit, ItemCostEach, locationid ) "
                                     " values "
                                     " ('"+ITransID+"', 'IS', '"+itemid+"', '"+erp.intString(qty)+"', '"+erp.DecimalString(priceach)+"', '+', '"+packingUnit+"','"+erp.DecimalString(itemcosteach)+"','"+locationid+"' )" ;
            if(!conn.execQuery(query_transdet))
            {
                QMessageBox::critical(this,"","Some problem in record insertion");
                return;
            }
        }//end for(int i=0; i<ui->detailtable->rowCount(); i++)

        QString event = "Update";
        QString transtype = "Opening Stock";
        QString transid = ITransID ;
        erp.AddToEventLog(event,transtype,transid);

        QMessageBox::information(this,"","Stock No: "+ITransID+" updated Successfully!");
        //clearForm();
    }//end if(totalprice!=0)
    else
    {
        QMessageBox::warning(this,"","Please add items for initial stock ");
    }
}

void EditInitialStock::clearForm()
{
    ui->detailtable->setRowCount(0);
    ui->lbl_vno->clear();
    phid="";
    ui->editFrame->setEnabled(false);
}

void EditInitialStock::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString adj_no = ui->search_vno->text();
    QString begindate = ui->search_begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->search_enddate->date().toString("yyyy-MM-dd");
    QString query = "select * from tblitemtrans where ITransType = 'IS' ";
    if(adj_no!="")
    {
        QString si_num = adj_no.split(QRegExp("-")).value(1);
        query += " and (`ITransID` = '"+adj_no+"' OR `ITransID` = '"+si_num+"') ";
    }
    if(begindate!="" && enddate!="")
    {
        query += " and ITDate >= '"+begindate+"' and ITDate <= '"+enddate+"' ";
    }
    query += " order by ITransID desc ";
    QSqlQuery info(query);
    while(info.next())
    {
        int row = ui->searchTable->rowCount();
        ui->searchTable->insertRow(row);
        ui->searchTable->setItem(row,0, new QTableWidgetItem( info.value("ITransID").toString() ));
        ui->searchTable->setItem(row,1, new QTableWidgetItem( info.value("ITDate").toDate().toString("dd/MM/yyyy") ));
    }
}

void EditInitialStock::on_btn_edit_clicked()
{
    ui->detailtable->setRowCount(0);
    if(ui->searchTable->currentRow()>=0)
    {
        ui->editFrame->setEnabled(true);
        phid = ui->searchTable->item(ui->searchTable->currentRow(),0)->text();
        QSqlQuery info("select * from tblitemtrans where ITransID='"+phid+"' and ITransType='IS' ");
        info.next();
        ui->entrydate->setDate( info.value("ITDate").toDate() );
        ui->lbl_vno->setText("Stock #: "+info.value("ITransType").toString()+"-"+info.value("ITransID").toString());
        QSqlQuery info_det(" select * from tblitemtransdet where ITransID='"+info.value("ITransID").toString()+"' and ITransType='IS' ");
        while(info_det.next())
        {
            int row = ui->detailtable->rowCount();
            ui->detailtable->insertRow(row);
            ui->detailtable->setItem(row,ITEMNAME,new QTableWidgetItem( erp.itemname(info_det.value("ItemID").toString()) ));
            ui->detailtable->setItem(row,ITEMID,new QTableWidgetItem( info_det.value("ItemID").toString() ));
            ui->detailtable->setItem(row,LOCATIONNAME,new QTableWidgetItem( erp.locationName(info_det.value("locationid").toString()) ));
            ui->detailtable->setItem(row,LOCATIONID,new QTableWidgetItem( info_det.value("locationid").toString() ));
            ui->detailtable->setItem(row,UNITNAME,new QTableWidgetItem( erp.unitName(info_det.value("packingUnit").toString()) ));
            ui->detailtable->setItem(row,UNITID,new QTableWidgetItem( info_det.value("packingUnit").toString() ));
            ui->detailtable->setItem(row,QUANTITY,new QTableWidgetItem( erp.intString(info_det.value("Qty").toInt()) ));
            ui->detailtable->setItem(row,PRICE,new QTableWidgetItem( erp.DecimalString(info_det.value("ItemPriceEach").toFloat()) ));
            ui->detailtable->setItem(row,AMOUNT,new QTableWidgetItem( erp.DecimalString( info_det.value("Qty").toInt() * info_det.value("ItemPriceEach").toFloat()) ));
        }
    }
    else
    {
        QMessageBox::warning(this,""," Please select row for edit ");
    }
}

void EditInitialStock::on_btn_delete_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        QString adj_no = ui->searchTable->item(ui->searchTable->currentRow(),0)->text();
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to perform this action?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
              if(!conn.execQuery("DELETE FROM tblitemtransdet WHERE ITransID = '"+adj_no+"' AND ITransType = 'IS' "))
              {
                  QMessageBox::critical(this,"","Some problem in record deletion");
                  return;
              }
              if(!conn.execQuery("DELETE FROM tblitemtrans WHERE ITransID = '"+adj_no+"' AND ITransType = 'IS' "))
              {
                  QMessageBox::critical(this,"","Some problem in record deletion");
                  return;
              }
              QString event = "Delete";
              QString transtype = "Opening Stock";
              QString transid = adj_no ;
              erp.AddToEventLog(event,transtype,transid);
              QMessageBox::information(this,"","Stock # "+adj_no+" deleted successfully ");
              clearForm();
              on_btn_search_clicked();
          }
    }
    else
    {
        QMessageBox::warning(this,""," Please select row for delete ");
    }
}
