#include "recentcustomers.h"
#include "ui_recentcustomers.h"

RecentCustomers::RecentCustomers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecentCustomers)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate().addDays(-30));
}

RecentCustomers::~RecentCustomers()
{
    delete ui;
}

void RecentCustomers::on_dateEdit_userDateChanged(const QDate &date)
{
    //QSqlQueryModel *model = new QSqlQueryModel();
    QStandardItemModel *model=new QStandardItemModel();
    QSqlQuery qry;
    QString sql = "select name,address1, city, CONCAT(entry_firstname,' ', middle_name,' ',entry_lastname) as fullname, phone1, email1, groupid, discount  from tblcustomers where dl =0 and entrydate >= '"+date.toString("yyyy-MM-dd")+"' ";
    qry.prepare(sql);
    qry.exec();
    int row=0;

    while(qry.next())
    {
        QStandardItem *item0= new QStandardItem();
        item0->setText(qry.value("name").toString());
        item0->setTextAlignment(Qt::AlignLeft);
        item0->setTextAlignment(Qt::AlignVCenter);
        model->setItem(row,0,item0);

        QStandardItem *item1= new QStandardItem();
        item1->setText(qry.value("address1").toString());
        item1->setTextAlignment(Qt::AlignLeft);
        item1->setTextAlignment(Qt::AlignVCenter);
        model->setItem(row,1,item1);

        QStandardItem *item2= new QStandardItem();
        item2->setText(qry.value("city").toString());
        item2->setTextAlignment(Qt::AlignLeft);
        item2->setTextAlignment(Qt::AlignVCenter);
        model->setItem(row,2,item2);

        QStandardItem *item3= new QStandardItem();
        item3->setText(qry.value("fullname").toString());
        item3->setTextAlignment(Qt::AlignLeft);
        item3->setTextAlignment(Qt::AlignVCenter);
        model->setItem(row,3,item3);

        QStandardItem *item4= new QStandardItem();
        item4->setText(qry.value("phone1").toString());
        item4->setTextAlignment(Qt::AlignCenter);
        model->setItem(row,4,item4);

        QStandardItem *item5= new QStandardItem();
        item5->setText(qry.value("email1").toString());
        item5->setTextAlignment(Qt::AlignCenter);
        model->setItem(row,5,item5);

        QStandardItem *item6= new QStandardItem();
        item6->setText(erp.getCustomerGroupName(qry.value("groupid").toString()));
        item6->setTextAlignment(Qt::AlignLeft);
        item6->setTextAlignment(Qt::AlignVCenter);
        model->setItem(row,6,item6);

        QStandardItem *item7= new QStandardItem();
        item7->setText(qry.value("discount").toString());
        item7->setTextAlignment(Qt::AlignCenter);
        model->setItem(row,7,item7);
        row++;
    }
    //model->setQuery(qry);
    model->setHeaderData(0, Qt::Horizontal, tr("Company Name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Address"));
    model->setHeaderData(2, Qt::Horizontal, tr("City"));
    model->setHeaderData(3, Qt::Horizontal, tr("Contact Name"));
    model->setHeaderData(4, Qt::Horizontal, tr("Phone"));
    model->setHeaderData(5, Qt::Horizontal, tr("Email"));
    model->setHeaderData(6, Qt::Horizontal, tr("Group Name"));
    model->setHeaderData(7, Qt::Horizontal, tr("Discount"));

   //ui->tableViewCustomerList->setSpan(2,0,1,5);
    ui->tableViewCustomerList->setModel(model);
    //ui->tableViewCustomerList->setColumnWidth(0,220);
    ui->tableViewCustomerList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableViewCustomerList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    //ui->tableViewCustomerList->setColumnWidth(1,200);
    ui->tableViewCustomerList->setColumnWidth(2,100);
    ui->tableViewCustomerList->setColumnWidth(3,200);
    ui->tableViewCustomerList->setColumnWidth(4,80);
    ui->tableViewCustomerList->setColumnWidth(5,80);
    ui->tableViewCustomerList->setColumnWidth(6,120);
    ui->tableViewCustomerList->setColumnWidth(7,80);

}
