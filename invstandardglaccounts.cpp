#include "invstandardglaccounts.h"
#include "ui_invstandardglaccounts.h"

InvStandardGLAccounts::InvStandardGLAccounts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InvStandardGLAccounts)
{
    ui->setupUi(this);

    QSqlQuery info(" select * from tblinvcompany where id = '1' ");
    info.next();

    QString cash                    = info.value("cash").toString();
    QString sales                   = info.value("sales").toString();
    QString loss                    = info.value("loss").toString();
    QString cost                    = info.value("cost").toString();
    QString freight                 = info.value("freight").toString();
    QString inventory               = info.value("inventory").toString();
    QString default_inventoryglacct = info.value("default_inventoryglacct").toString();

    ui->cash->setText(cash);
    ui->sales->setText(sales);
    ui->loss->setText(loss);
    ui->cost->setText(cost);
    ui->freight->setText(freight);
    ui->inventory->setText(inventory);
    ui->default_inventoryglacct->setText(default_inventoryglacct);


    ui->cashdescription->setText(erp.getAccount(cash));
    ui->salesdescription->setText(erp.getAccount(sales));
    ui->lossdescription->setText(erp.getAccount(loss));
    ui->costdescription->setText(erp.getAccount(cost));
    ui->freightdescription->setText(erp.getAccount(freight));
    ui->inventorydescription->setText(erp.getAccount(inventory));
    ui->default_inventorydes->setText(erp.getAccount(default_inventoryglacct));
}

InvStandardGLAccounts::~InvStandardGLAccounts()
{
    delete ui;
}

void InvStandardGLAccounts::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void InvStandardGLAccounts::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void InvStandardGLAccounts::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void InvStandardGLAccounts::on_btn_save_clicked()
{
    //save code here
    QString cash                    = ui->cash->text();
    QString inventory               = ui->inventory->text();
    QString loss                    = ui->loss->text();
    QString cost                    = ui->cost->text();
    QString freight                 = ui->freight->text();
    QString default_inventoryglacct = ui->default_inventoryglacct->text();
    QString sales                   = ui->sales->text();

    QString query = "UPDATE `tblinvcompany` SET "
                    " `cash`                    = '"+cash+"', "
                    " `inventory`               = '"+inventory+"', "
                    " `loss`                    = '"+loss+"', "
                    " `cost`                    = '"+cost+"', "
                    " `freight`                 = '"+freight+"', "
                    " `default_inventoryglacct` = '"+default_inventoryglacct+"', "
                    " `sales`                   = '"+sales+"' "
                    " WHERE `id` ='1' LIMIT 1";
    if(conn.execQuery(query))
    {
        QMessageBox::information(this,"","Accounts saved successfully");
    }
    else
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
    }
}

void InvStandardGLAccounts::on_cashdescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->cashdescription,ui->cash);
}

void InvStandardGLAccounts::on_salesdescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->salesdescription,ui->sales);
}

void InvStandardGLAccounts::on_inventorydescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->inventorydescription,ui->inventory);
}

void InvStandardGLAccounts::on_lossdescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->lossdescription,ui->loss);
}

void InvStandardGLAccounts::on_costdescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->costdescription,ui->cost);
}

void InvStandardGLAccounts::on_freightdescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->freightdescription,ui->freight);
}

void InvStandardGLAccounts::on_default_inventorydes_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->default_inventorydes,ui->default_inventoryglacct);
}
