#include "additem.h"
#include "ui_additem.h"

AddItem::AddItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddItem)
{
    ui->setupUi(this);
    loadform();
}

AddItem::~AddItem()
{
    delete ui;
}


void AddItem::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddItem::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddItem::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddItem::loadform()
{
    cb.loadItemTypesToComboBox(ui->itemtype);
    cb.loadItemCategoriesToComboBox(ui->category);
    cb.loadItemSubCategoriesToComboBox(ui->subcat,true,ui->category->itemData(ui->category->currentIndex()).toString());
    cb.loadItemBrandToComboBox(ui->brand);
    cb.loadUnitWeightToComboBox(ui->priceunit);
    cb.loadItemUnitsToComboBox(ui->pcsperpackunitid);
    cb.loadItemUnitsToComboBox(ui->packpermasterunitid);
    cb.loadItemUnitsToComboBox(ui->masterpackingunitid);
    cb.loadItemStatusToComboBox(ui->status);
}


void AddItem::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->vendorname,ui->vendorid);
}

void AddItem::on_category_currentIndexChanged(int index)
{
    cb.loadItemSubCategoriesToComboBox(ui->subcat,true,ui->category->itemData(index).toString());
}

void AddItem::on_btn_save_clicked()
{
    if(isvalidate())
    {
        QString itemcode            = ui->itemcode->text();
        QString itemtype            = ui->itemtype->itemData(ui->itemtype->currentIndex()).toString();
        QString description         = ui->description->text();
        QString categoryid          = ui->category->itemData(ui->category->currentIndex()).toString();
        QString vendorid            = ui->vendorid->text();
        QString priceunit           = ui->priceunit->currentText();
        float lbsperpriceunit       = ui->lbsperpriceunit->value();
        float itemcosteach          = ui->itemcosteach->value();
        float price1                = ui->price1->value();
        QString status              = ui->status->itemData(ui->status->currentIndex()).toString();
        QString date                = QDate::currentDate().toString("yyyy-MM-dd");
        QString subcatid            = ui->subcat->itemData(ui->subcat->currentIndex()).toString();
        QString brandid             = ui->brand->itemData(ui->brand->currentIndex()).toString();
        QString upc                 = ui->upc->text();
        QString packPerMaster       = ui->packPerMaster->text();
        QString pcsPerPack          = ui->pcsPerPack->text();
        float unitPrice             = ui->unitPrice->value();
        QString packing             = ui->pack->text();
        QString pcsperpackunitid    = ui->pcsperpackunitid->itemData(ui->pcsperpackunitid->currentIndex()).toString();
        QString packpermasterunitid = ui->packpermasterunitid->itemData(ui->packpermasterunitid->currentIndex()).toString();
        QString masterpackingunitid = ui->masterpackingunitid->itemData(ui->masterpackingunitid->currentIndex()).toString();

        QString query = "insert into tblitems "
                        " (itemcode, itemtype, subcatid, brandid, description, entrydate, categoryid, priceunit, lbsperpriceunit, status, price1, itemcosteach, vendorid, upc, packing, pcsPerPack, packPerMaster, `unitPrice`,`pcsperpackunitid`,`packpermasterunitid`,`masterpackingunitid` ) "
                        " values "
                        " ('"+itemcode+"', '"+itemtype+"', '"+subcatid+"', '"+brandid+"', '"+description+"', '"+date+"','"+categoryid+"', '"+priceunit+"', '"+erp.DecimalString(lbsperpriceunit)+"', '"+status+"', '"+erp.DecimalString(price1)+"', '"+erp.DecimalString(itemcosteach)+"', '"+vendorid+"', '"+upc+"', '"+packing+"', '"+pcsPerPack+"', '"+packPerMaster+"','"+erp.DecimalString(unitPrice)+"','"+pcsperpackunitid+"','"+packpermasterunitid+"','"+masterpackingunitid+"')";
        if(conn.execQuery(query))
        {
            QMessageBox::information(this,"","Record is inserted successfully.");
            clearform();
        }
        else
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
        }
    }
}

bool AddItem::isvalidate()
{
    if(ui->itemcode->text()=="")
    {
        QMessageBox::critical(this,"","Item Code is required");
        ui->itemcode->setFocus();
        return false;
    }
    if(ui->description->text()=="")
    {
        QMessageBox::critical(this,"","Description is required");
        ui->description->setFocus();
        return false;
    }
    if(ui->category->currentIndex()==0)
    {
        QMessageBox::critical(this,"","Category is required");
        ui->category->setFocus();
        return false;
    }
    if(ui->pcs_activeList->isChecked())
    {
        if(ui->pcsperpackunitid->currentIndex()==0)
        {
            QMessageBox::critical(this,"","Pcs Per Pack Unit is required");
            ui->pcsperpackunitid->setFocus();
            return false;
        }
        if(ui->pcsPerPack->value()==0)
        {
            QMessageBox::critical(this,"","Pcs Per Pack is required");
            ui->pcsPerPack->setFocus();
            return false;
        }
    }
    if(ui->activeList->isChecked())
    {
        if(ui->packpermasterunitid->currentIndex()==0)
        {
            QMessageBox::critical(this,"","Pack Per Master Unit is required");
            ui->packpermasterunitid->setFocus();
            return false;
        }
        if(ui->packPerMaster->value()==0)
        {
            QMessageBox::critical(this,"","Pack Per Master is required");
            ui->packPerMaster->setFocus();
            return false;
        }
    }
    if(ui->masterpackingunitid->currentIndex()==0)
    {
        QMessageBox::critical(this,"","Master Packing Unit is required");
        ui->masterpackingunitid->setFocus();
        return false;
    }
    return true;
}

void AddItem::clearform()
{
    ui->itemcode->clear();
    ui->description->clear();
    ui->upc->clear();
    ui->pack->clear();
    ui->itemtype->setCurrentText(0);
    ui->category->setCurrentIndex(0);
    ui->subcat->setCurrentIndex(0);
    ui->brand->setCurrentIndex(0);
    ui->vendorname->clear();
    ui->vendorid->clear();
    ui->lbsperpriceunit->setValue(0.00);
    ui->priceunit->setCurrentIndex(0);
    ui->pcsPerPack->setValue(0);
    ui->pcsperpackunitid->setCurrentIndex(0);
    ui->packPerMaster->setValue(0);
    ui->packpermasterunitid->setCurrentIndex(0);
    ui->masterpackingunitid->setCurrentIndex(0);
    ui->unitPrice->setValue(0.00);
    ui->price1->setValue(0.00);
    ui->itemcosteach->setValue(0.00);
    ui->status->setCurrentIndex(0);
    ui->itemcode->setFocus();
}
