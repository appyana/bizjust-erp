#include "itemsmonthwisesales.h"
#include "ui_itemsmonthwisesales.h"

ItemsMonthWiseSales::ItemsMonthWiseSales(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemsMonthWiseSales)
{
    ui->setupUi(this);
    loarForm();
}

ItemsMonthWiseSales::~ItemsMonthWiseSales()
{
    delete ui;
}


void ItemsMonthWiseSales::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
{
    sch.name_txt = name_txt;
    sch.id_txt = id_txt;
    QMap<int, QString> data = sch.data;
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        while(qry.next())
        {
            data[qry.value(0).toInt()] = qry.value(1).toString();
        }
    }
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

void ItemsMonthWiseSales::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void ItemsMonthWiseSales::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void ItemsMonthWiseSales::loarForm()
{
    ui->detailFrame->setVisible(false);
    QFont font("Segoe UI", 9, QFont::Bold);
    QStringList titles;
    titles <<"Month"<<"Qty Sold"<<"Sales";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    QStringList months;
    months <<"January"<<"February"<<"March"<<"April"<<"May"<<"June"<<"July"<<"August"<<"September"<<"October"<<"November"<<"December"<<"Total:";
    ui->detailTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    for(int i=0; i<13; i++)
    {
        QString val = months.value(i);
        ui->detailTable->setItem(i,0,new QTableWidgetItem(val));
        ui->detailTable->setItem(i,1,new QTableWidgetItem(""));
        ui->detailTable->setItem(i,2,new QTableWidgetItem(""));
        ui->detailTable->item(i,0)->setFont(font);
        ui->detailTable->item(i,0)->setBackgroundColor("#f2f2f2");
        ui->detailTable->item(i,0)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->item(i,1)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->item(i,2)->setTextAlignment(Qt::AlignCenter);
    }
    ui->detailTable->item(12,0)->setBackgroundColor("#FFFFFF");
    ui->detailTable->item(12,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    ui->detailTable->item(12,1)->setTextAlignment(Qt::AlignCenter);
    ui->detailTable->item(12,2)->setTextAlignment(Qt::AlignCenter);
}

void ItemsMonthWiseSales::viewDetail()
{
    QString itemname = erp.itemname(itemid);
    QString itemcode = erp.getItemField(itemid,"itemcode");
    ui->lbl_itemname->setText("Item: "+itemname+"-"+itemcode);
    ui->lbl_year->setText("Information For: "+foryear);
    int netunit=0;
    float price=0;
    for(int i=1; i<=12; i++)
    {
        QString month = erp.intString(i);
        int row = i-1;

        QString qtysold = erp.saleAndPurchasedUnits("si",itemid,foryear,month,true);
        float sales = erp.saleAndPurchasePrice("si",itemid,foryear,month);
        qDebug()<<qtysold;
        netunit += erp.saleAndPurchasedUnits("si",itemid,foryear,month,false).toInt();
        price += sales;
        ui->detailTable->item(row,1)->setText(qtysold);
        ui->detailTable->item(row,2)->setText( erp.amountString(sales) );
    }
    ui->detailTable->item(12,1)->setText( erp.intString(netunit) );
    ui->detailTable->item(12,2)->setText( erp.amountString(price) );
}

void ItemsMonthWiseSales::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void ItemsMonthWiseSales::on_btn_next_clicked()
{
    if(ui->itemid->text()=="")
    {
        QMessageBox::warning(this,"","Please select item");
        return;
    }
    foryear = QDate::currentDate().toString("yyyy");
    itemid = ui->itemid->text();
    ui->detailFrame->setVisible(true);
    viewDetail();
}

void ItemsMonthWiseSales::on_previous_year_btn_clicked()
{
    foryear = erp.intString(foryear.toInt()-1);
    viewDetail();
}

void ItemsMonthWiseSales::on_next_year_btn_clicked()
{
    foryear = erp.intString(foryear.toInt()+1);
    viewDetail();
}
