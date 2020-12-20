#include "itemslist.h"
#include "ui_itemslist.h"

ItemsList::ItemsList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemsList)
{
    ui->setupUi(this);
    loadform();
}

ItemsList::~ItemsList()
{
    delete ui;
}

void ItemsList::loadform()
{
    ui->tableWidget->setColumnCount(7);
    QStringList titles;
    titles <<"Item Code"<<"Description"<<"Item Type"<<"Category"<<"Sub Category"<<"Brand"<<"Sales Price";
    ui->tableWidget->setHorizontalHeaderLabels(titles);
    ui->tableWidget->setColumnWidth(ITEMCODE,80);
    ui->tableWidget->setColumnWidth(ITEMTYPE,140);
    ui->tableWidget->setColumnWidth(CATEGORY,140);
    ui->tableWidget->setColumnWidth(SUBCATEGORY,140);
    ui->tableWidget->setColumnWidth(BRAND,100);
    ui->tableWidget->setColumnWidth(SALESPRICE,80);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(DESCRIPTION,QHeaderView::Stretch);
    QSqlQuery info("select * from tblitems where dl = '0' and status = '1'");
    while(info.next())
    {
        QSqlQuery type(" select type from pro_item_type where id = '"+info.value("itemtype").toString()+"' ");
        type.next();
        QSqlQuery cat(" select name from tblitemcategory where id = '"+info.value("categoryid").toString()+"' ");
        cat.next();
        QSqlQuery subcat(" select name from pro_sub_cat where id = '"+info.value("subcatid").toString()+"' ");
        subcat.next();
        QSqlQuery brand(" select name from pro_item_brand where id = '"+info.value("brandid").toString()+"' ");
        brand.next();

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,ITEMCODE, new QTableWidgetItem( info.value("itemcode").toString() ));
        ui->tableWidget->setItem(row,DESCRIPTION, new QTableWidgetItem( info.value("description").toString() ));
        ui->tableWidget->setItem(row,ITEMTYPE, new QTableWidgetItem( type.value("type").toString() ));
        ui->tableWidget->setItem(row,CATEGORY, new QTableWidgetItem( cat.value("name").toString() ));
        ui->tableWidget->setItem(row,SUBCATEGORY, new QTableWidgetItem( subcat.value("name").toString() ));
        ui->tableWidget->setItem(row,BRAND, new QTableWidgetItem( brand.value("name").toString() ));
        ui->tableWidget->setItem(row,SALESPRICE, new QTableWidgetItem( erp.amountString(info.value("price1").toFloat()) ));
    }
}
