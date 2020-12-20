#include "editstockadjustment.h"
#include "ui_editstockadjustment.h"

EditStockAdjustment::EditStockAdjustment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditStockAdjustment)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadform()) );
    timer->start(5);
    //loadform();
}

EditStockAdjustment::~EditStockAdjustment()
{
    delete ui;
}

void EditStockAdjustment::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditStockAdjustment::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditStockAdjustment::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditStockAdjustment::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void EditStockAdjustment::loadform()
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
    ui->searchTable->setHorizontalHeaderItem(0,new QTableWidgetItem("Adjustment #"));
    ui->searchTable->setHorizontalHeaderItem(1,new QTableWidgetItem("Entry Date"));
    ui->searchTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->searchTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
}

void EditStockAdjustment::checktotal()
{
    float total;
    for(int i=0; i<ui->detailtable->rowCount(); i++)
    {
        total += ui->detailtable->item(i,AMOUNT)->text().toFloat();
    }
    ui->total->setValue(total);
}



void EditStockAdjustment::on_btn_add_clicked()
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

void EditStockAdjustment::on_itemid_textChanged(const QString &arg1)
{
    erp.loadItemUnits(ui->unit,arg1);
    QString price = erp.getItemField(arg1,"price1");
    ui->price->setValue(price.toFloat());
}

void EditStockAdjustment::on_qty_valueChanged(int arg1)
{
    float amt = arg1 * ui->price->value();
    ui->amount->setValue(amt);
}

void EditStockAdjustment::on_price_valueChanged(double arg1)
{
    float amt = ui->qty->value() * arg1;
    ui->amount->setValue(amt);
}

void EditStockAdjustment::on_btn_delete_row_clicked()
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

void EditStockAdjustment::on_btn_save_clicked()
{
    QString date = ui->entrydate->date().toString("yyyy-MM-dd");
    //float totalprice = ui->total->value();
    if(ui->detailtable->rowCount()>0)
    {
        //QString adj_code = "sa";
        QString adj_num = phid.split(QRegExp("-")).value(1);//erp.get_num(adj_code);
        QString adjustment_no = phid;
        QString comments="Stock Adjustment ";
        int lines = ui->detailtable->rowCount();
        QString query_phys = "UPDATE tblinvphys "
                             " SET "
                             " entrydate = '"+date+"',"
                             " `lines` = '"+erp.intString(lines)+"' "
                             " WHERE "
                             " adjustment_no = '"+adjustment_no+"' " ;
        if(!conn.execQuery(query_phys))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        QString query_itemtrans = "UPDATE tblitemtrans SET "
                                  " ITDate = '"+date+"',"
                                  " DetailedDescription = '"+comments+"' "
                                  " WHERE "
                                  " ITransID = '"+adj_num+"' AND ITransType =  'sa' " ;
        if(!conn.execQuery(query_itemtrans))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }

        QSqlQuery res(" select phid from tblinvphys where adjustment_no = '"+adjustment_no+"' ");
        res.next();
        QString pid = res.value("phid").toString();

        if(!conn.execQuery("delete from tblinvphysdetail where phid = '"+pid+"' "))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }
        if(!conn.execQuery("delete from tblitemtransdet where ITransID = '"+adj_num+"' and ITransType = 'sa' "))
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
            return;
        }

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
            if(updqty>=0)
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
                               " ('"+pid+"', '"+itemid+"', '"+erp.intString(updqty)+"','"+CalType+"', '"+packingUnit+"' , '"+locationid+"' , '"+erp.DecimalString(priceach)+"' , '"+erp.DecimalString(total)+"')";
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

        QString event = "Update";
        QString transtype = "Stock Adjustment";
        QString transid = adjustment_no ;
        erp.AddToEventLog(event,transtype,transid);

        QMessageBox::information(this,"","Stock Adustment "+adjustment_no+" updated Successfully!");
        //clearForm();
    }//end if(totalprice!=0)
    else
    {
        QMessageBox::warning(this,"","Please add items to adjust stock ");
    }
}

void EditStockAdjustment::clearForm()
{
    ui->detailtable->setRowCount(0);
    checktotal();
    ui->lbl_vno->clear();
    phid="";
    ui->editFrame->setEnabled(false);
}

void EditStockAdjustment::on_btn_search_clicked()
{
    ui->searchTable->setRowCount(0);
    QString adj_no = ui->search_vno->text();
    QString begindate = ui->search_begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->search_enddate->date().toString("yyyy-MM-dd");
    QString query = "SELECT * FROM `tblinvphys` WHERE phid !='' ";
    if(adj_no!="")
    {
        query += " and (`adjustment_no` = '"+adj_no+"' OR `adjustment_no` = 'sa-"+adj_no+"') ";
    }
    if(begindate!="" && enddate!="")
    {
        query += " and entrydate >= '"+begindate+"' and entrydate <= '"+enddate+"' ";
    }
    query += " order by phid desc ";
    QSqlQuery info(query);
    while(info.next())
    {
        int row = ui->searchTable->rowCount();
        ui->searchTable->insertRow(row);
        ui->searchTable->setItem(row,0, new QTableWidgetItem( info.value("adjustment_no").toString() ));
        ui->searchTable->setItem(row,1, new QTableWidgetItem( info.value("entrydate").toDate().toString("dd/MM/yyyy") ));
    }
}

void EditStockAdjustment::on_btn_edit_clicked()
{
    ui->detailtable->setRowCount(0);
    if(ui->searchTable->currentRow()>=0)
    {
        ui->editFrame->setEnabled(true);
        phid = ui->searchTable->item(ui->searchTable->currentRow(),0)->text();
        QSqlQuery info("select * from tblinvphys where adjustment_no='"+phid+"' ");
        info.next();
        ui->entrydate->setDate( info.value("entrydate").toDate() );
        ui->lbl_vno->setText("Adjustment #: "+phid);
        QSqlQuery info_det(" select * from tblinvphysdetail where phid='"+info.value("phid").toString()+"' ");
        while(info_det.next())
        {
            int row = ui->detailtable->rowCount();
            ui->detailtable->insertRow(row);
            ui->detailtable->setItem(row,ITEMNAME,new QTableWidgetItem( erp.itemname(info_det.value("itemid").toString()) ));
            ui->detailtable->setItem(row,ITEMID,new QTableWidgetItem( info_det.value("itemid").toString() ));
            ui->detailtable->setItem(row,LOCATIONNAME,new QTableWidgetItem( erp.locationName(info_det.value("locationid").toString()) ));
            ui->detailtable->setItem(row,LOCATIONID,new QTableWidgetItem( info_det.value("locationid").toString() ));
            ui->detailtable->setItem(row,UNITNAME,new QTableWidgetItem( erp.unitName(info_det.value("packingUnit").toString()) ));
            ui->detailtable->setItem(row,UNITID,new QTableWidgetItem( info_det.value("packingUnit").toString() ));
            ui->detailtable->setItem(row,QUANTITY,new QTableWidgetItem( erp.intString(info_det.value("itemqty").toInt()) ));
            ui->detailtable->setItem(row,PRICE,new QTableWidgetItem( erp.DecimalString(info_det.value("rate").toFloat()) ));
            ui->detailtable->setItem(row,AMOUNT,new QTableWidgetItem( erp.DecimalString(info_det.value("amount").toFloat()) ));
            checktotal();
        }
    }
    else
    {
        QMessageBox::warning(this,""," Please select row for edit ");
    }
}

void EditStockAdjustment::on_btn_delete_clicked()
{
    if(ui->searchTable->currentRow()>=0)
    {
        QString adj_no = ui->searchTable->item(ui->searchTable->currentRow(),0)->text();
        QString si_num = adj_no.split(QRegExp("-")).value(1);
        QSqlQuery row_n("select `lines`,phid from tblinvphys  where adjustment_no='"+adj_no+"'");
        row_n.next();
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to perform this action?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
              if(!conn.execQuery("DELETE FROM tblinvphys WHERE phid = '"+row_n.value("phid").toString()+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in record insertion");
                  return;
              }
              if(!conn.execQuery("DELETE FROM tblinvphysdetail WHERE phid = '"+row_n.value("phid").toString()+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in record insertion");
                  return;
              }
              if(!conn.execQuery("DELETE FROM tblitemtransdet WHERE ITransID = '"+si_num+"' AND ITransType = 'sa' "))
              {
                  QMessageBox::critical(this,"","Some problem in record insertion");
                  return;
              }
              if(!conn.execQuery("DELETE FROM tblitemtrans WHERE ITransID = '"+si_num+"' AND ITransType = 'sa' "))
              {
                  QMessageBox::critical(this,"","Some problem in record insertion");
                  return;
              }
              QString event = "Delete";
              QString transtype = "Stock Adjustment";
              QString transid = adj_no ;
              erp.AddToEventLog(event,transtype,transid);
              QMessageBox::information(this,"","Adjustment # "+adj_no+" deleted successfully ");
              clearForm();
              on_btn_search_clicked();
          }
    }
    else
    {
        QMessageBox::warning(this,""," Please select row for delete ");
    }
}
