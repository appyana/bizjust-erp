#include "arstandardglaccounts.h"
#include "ui_arstandardglaccounts.h"

ARStandardGLAccounts::ARStandardGLAccounts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ARStandardGLAccounts)
{
    ui->setupUi(this);

    QSqlQuery info(" select * from tblarcompany where id = '1' ");
    info.next();

    QString cashAccountId               = info.value("cash").toString();
    QString suspectAccountId            = info.value("suspect").toString();
    QString discountGivenAccountId      = info.value("discount").toString();
    QString costOfGoodsAccountId        = info.value("cost").toString();
    QString inventoryAccountId          = info.value("inventory").toString();
    QString shippingLiabilityAccountId  = info.value("shipliability").toString();
    QString receivableAccountId         = info.value("receivables").toString();
    QString salesReturnAccountId        = info.value("return").toString();
    QString customerDepositAccountId    = info.value("depositglacctid").toString();
    QString salesIncomeAccountId        = info.value("income").toString();
    QString freightAccountId            = info.value("freight").toString();
    QString bankChargesAccountId        = info.value("bankCharges").toString();
    QString writeOffAccountId           = info.value("writeOff").toString();

    ui->cashAccountId->setText(cashAccountId);
    ui->suspectAccountId->setText(suspectAccountId);
    ui->discountGivenAccountId->setText(discountGivenAccountId);
    ui->costOfGoodsAccountId->setText(costOfGoodsAccountId);
    ui->inventoryAccountId->setText(inventoryAccountId);
    ui->shippingLiabilityAccountId->setText(shippingLiabilityAccountId);
    ui->receivableAccountId->setText(receivableAccountId);
    ui->salesReturnAccountId->setText(salesReturnAccountId);
    ui->customerDepositAccountId->setText(customerDepositAccountId);
    ui->salesIncomeAccountId->setText(salesIncomeAccountId);
    ui->freightAccountId->setText(freightAccountId);
    ui->bankChargesAccountId->setText(bankChargesAccountId);
    ui->writeOffAccountId->setText(writeOffAccountId);


    ui->cashAccountName->setText(erp.getAccount(cashAccountId));
    ui->suspectAccountName->setText(erp.getAccount(suspectAccountId));
    ui->discountGivenAccountName->setText(erp.getAccount(discountGivenAccountId));
    ui->costOfGoodsAccountName->setText(erp.getAccount(costOfGoodsAccountId));
    ui->inventoryAccountName->setText(erp.getAccount(inventoryAccountId));
    ui->shippingLiabilityAccountName->setText(erp.getAccount(shippingLiabilityAccountId));
    ui->receivableAccountName->setText(erp.getAccount(receivableAccountId));
    ui->salesReturnAccountName->setText(erp.getAccount(salesReturnAccountId));
    ui->customerDepositAccountName->setText(erp.getAccount(customerDepositAccountId));
    ui->salesIncomeAccountName->setText(erp.getAccount(salesIncomeAccountId));
    ui->freightAccountName->setText(erp.getAccount(freightAccountId));
    ui->bankChargesAccountName->setText(erp.getAccount(bankChargesAccountId));
    ui->writeOffAccountName->setText(erp.getAccount(writeOffAccountId));
}

ARStandardGLAccounts::~ARStandardGLAccounts()
{
    delete ui;
}

void ARStandardGLAccounts::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void ARStandardGLAccounts::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void ARStandardGLAccounts::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void ARStandardGLAccounts::on_cashAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->cashAccountName,ui->cashAccountId);
}

void ARStandardGLAccounts::on_suspectAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->suspectAccountName,ui->suspectAccountId);
}

void ARStandardGLAccounts::on_discountGivenAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->discountGivenAccountName,ui->discountGivenAccountId);
}

void ARStandardGLAccounts::on_costOfGoodsAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->costOfGoodsAccountName,ui->costOfGoodsAccountId);
}

void ARStandardGLAccounts::on_inventoryAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->inventoryAccountName,ui->inventoryAccountId);
}

void ARStandardGLAccounts::on_shippingLiabilityAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->shippingLiabilityAccountName,ui->shippingLiabilityAccountId);
}

void ARStandardGLAccounts::on_receivableAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->receivableAccountName,ui->receivableAccountId);
}

void ARStandardGLAccounts::on_salesReturnAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->salesReturnAccountName,ui->salesReturnAccountId);
}

void ARStandardGLAccounts::on_customerDepositAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->customerDepositAccountName,ui->customerDepositAccountId);
}

void ARStandardGLAccounts::on_salesIncomeAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->salesIncomeAccountName,ui->salesIncomeAccountId);
}

void ARStandardGLAccounts::on_freightAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->freightAccountName,ui->freightAccountId);
}

void ARStandardGLAccounts::on_bankChargesAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->bankChargesAccountName,ui->bankChargesAccountId);
}

void ARStandardGLAccounts::on_writeOffAccountName_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->writeOffAccountName,ui->writeOffAccountId);
}

void ARStandardGLAccounts::on_btn_save_clicked()
{
    //save code here
    QString cashAccountId               = ui->cashAccountId->text();
    QString suspectAccountId            = ui->suspectAccountId->text();
    QString discountGivenAccountId      = ui->discountGivenAccountId->text();
    QString costOfGoodsAccountId        = ui->costOfGoodsAccountId->text();
    QString inventoryAccountId          = ui->inventoryAccountId->text();
    QString shippingLiabilityAccountId  = ui->shippingLiabilityAccountId->text();
    QString receivableAccountId         = ui->receivableAccountId->text();
    QString salesReturnAccountId        = ui->salesReturnAccountId->text();
    QString customerDepositAccountId    = ui->customerDepositAccountId->text();
    QString salesIncomeAccountId        = ui->salesIncomeAccountId->text();
    QString freightAccountId            = ui->freightAccountId->text();
    QString bankChargesAccountId        = ui->bankChargesAccountId->text();
    QString writeOffAccountId           = ui->writeOffAccountId->text();

    QString query = "UPDATE `tblarcompany` SET "
                    " `cash`            = '"+cashAccountId+"', "
                    " `suspect`         = '"+suspectAccountId+"', "
                    " `discount`        = '"+discountGivenAccountId+"', "
                    " `cost`            = '"+costOfGoodsAccountId+"', "
                    " `inventory`       = '"+inventoryAccountId+"', "
                    " `shipliability`   = '"+shippingLiabilityAccountId+"', "
                    " `receivables`     = '"+receivableAccountId+"', "
                    " `return`          = '"+salesReturnAccountId+"', "
                    " `income`          = '"+salesIncomeAccountId+"', "
                    " `freight`         = '"+freightAccountId+"', "
                    " `depositglacctid` = '"+customerDepositAccountId+"', "
                    " `writeOff`        = '"+writeOffAccountId+"', "
                    " `bankCharges`     = '"+bankChargesAccountId+"' "
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
