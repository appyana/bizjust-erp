#include "apstandardglaccounts.h"
#include "ui_apstandardglaccounts.h"

APStandardGLAccounts::APStandardGLAccounts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::APStandardGLAccounts)
{
    ui->setupUi(this);

    QSqlQuery info(" select * from tblapcompany where id = '1' ");
    info.next();
    QString payable         = info.value("payable").toString();
    QString discount        = info.value("discount").toString();
    QString glcashid        = info.value("glcashid").toString();
    QString glccid          = info.value("glccid").toString();
    QString glcommissionid  = info.value("glcommissionid").toString();
    QString purchaseid      = info.value("purchaseid").toString();
    QString txt_labourid    = info.value("labour").toString();
    QString txt_dutyid      = info.value("duty").toString();
    QString txt_freightid   = info.value("freight").toString();
    QString txt_dfreightid  = info.value("freight_discount").toString();
    QString txt_inventoryid = info.value("inventory").toString();
    QString txt_taxid       = info.value("tax").toString();
    QString txt_bankid      = info.value("bankAcc").toString();
    QString txt_roffid      = info.value("writeOff").toString();
    QString txt_bcsid       = info.value("bankCharges").toString();
    QString txt_empcid      = info.value("emp_payable").toString();

    ui->payable->setText(payable);
    ui->discount->setText(discount);
    ui->glcashid->setText(glcashid);
    ui->glccid->setText(glccid);
    ui->glcommissionid->setText(glcommissionid);
    ui->purchaseid->setText(purchaseid);
    ui->txt_labourid->setText(txt_labourid);
    ui->txt_dutyid->setText(txt_dutyid);
    ui->txt_freightid->setText(txt_freightid);
    ui->txt_dfreightid->setText(txt_dfreightid);
    ui->txt_inventoryid->setText(txt_inventoryid);
    ui->txt_taxid->setText(txt_taxid);
    ui->txt_bankid->setText(txt_bankid);
    ui->txt_roffid->setText(txt_roffid);
    ui->txt_bcsid->setText(txt_bcsid);
    ui->txt_empcid->setText(txt_empcid);


    ui->payabledescription->setText(erp.getAccount(payable));
    ui->discountdescription->setText(erp.getAccount(discount));
    ui->glcashiddescription->setText(erp.getAccount(glcashid));
    ui->glcciddescription->setText(erp.getAccount(glccid));
    ui->glcommissioniddescription->setText(erp.getAccount(glcommissionid));
    ui->purchasedescription->setText(erp.getAccount(purchaseid));
    ui->txt_labour->setText(erp.getAccount(txt_labourid));
    ui->txt_duty->setText(erp.getAccount(txt_dutyid));
    ui->txt_freight->setText(erp.getAccount(txt_freightid));
    ui->txt_dfreight->setText(erp.getAccount(txt_dfreightid));
    ui->txt_inventory->setText(erp.getAccount(txt_inventoryid));
    ui->txt_tax->setText(erp.getAccount(txt_taxid));
    ui->txt_bank->setText(erp.getAccount(txt_bankid));
    ui->txt_roff->setText(erp.getAccount(txt_roffid));
    ui->txt_bcs->setText(erp.getAccount(txt_bcsid));
    ui->txt_empc->setText(erp.getAccount(txt_empcid));
}

APStandardGLAccounts::~APStandardGLAccounts()
{
    delete ui;
}

void APStandardGLAccounts::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void APStandardGLAccounts::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void APStandardGLAccounts::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void APStandardGLAccounts::on_btn_save_clicked()
{
    //save code here
    QString payable         = ui->payable->text();
    QString discount        = ui->discount->text();
    QString glcashid        = ui->glcashid->text();
    QString glccid          = ui->glccid->text();
    QString glcommissionid  = ui->glcommissionid->text();
    QString purchaseid      = ui->purchaseid->text();
    QString txt_labourid    = ui->txt_labourid->text();
    QString txt_dutyid      = ui->txt_dutyid->text();
    QString txt_freightid   = ui->txt_freightid->text();
    QString txt_dfreightid  = ui->txt_dfreightid->text();
    QString txt_inventoryid = ui->txt_inventoryid->text();
    QString txt_taxid       = ui->txt_taxid->text();
    QString txt_bankid      = ui->txt_bankid->text();
    QString txt_roffid      = ui->txt_roffid->text();
    QString txt_bcsid       = ui->txt_bcsid->text();
    QString txt_empcid      = ui->txt_empcid->text();

    QString query = "UPDATE tblapcompany "
                    " SET "
                    " `payable`          = '"+payable+"', "
                    " `discount`         = '"+discount+"', "
                    " `glcashid`         = '"+glcashid+"', "
                    " `glccid`           = '"+glccid+"', "
                    " `glcommissionid`   = '"+glcommissionid+"', "
                    " `purchaseid`       = '"+purchaseid+"', "
                    " `freight`          = '"+txt_freightid+"', "
                    " `freight_discount` = '"+txt_dfreightid+"', "
                    " `inventory`        = '"+txt_inventoryid+"', "
                    " `tax`              = '"+txt_taxid+"', "
                    " `labour`           = '"+txt_labourid+"', "
                    " `duty`             = '"+txt_dutyid+"', "
                    " `bankAcc`          = '"+txt_bankid+"', "
                    " `bankCharges`      = '"+txt_bcsid+"', "
                    " `writeOff`         = '"+txt_roffid+"', "
                    " `emp_payable` 	 = '"+txt_empcid+"' "
                    " WHERE "
                    " `id` = '1' ";
    if(conn.execQuery(query))
    {
        QMessageBox::information(this,"","Accounts saved successfully");
    }
    else
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
    }
}

void APStandardGLAccounts::on_payabledescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->payabledescription,ui->payable);
}

void APStandardGLAccounts::on_discountdescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->discountdescription,ui->discount);
}

void APStandardGLAccounts::on_glcashiddescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->glcashiddescription,ui->glcashid);
}

void APStandardGLAccounts::on_glcciddescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->glcciddescription,ui->glccid);
}

void APStandardGLAccounts::on_glcommissioniddescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->glcommissioniddescription,ui->glcommissionid);
}

void APStandardGLAccounts::on_txt_freight_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_freight,ui->txt_freightid);
}

void APStandardGLAccounts::on_txt_dfreight_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_dfreight,ui->txt_dfreightid);
}

void APStandardGLAccounts::on_purchasedescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->purchasedescription,ui->purchaseid);
}

void APStandardGLAccounts::on_txt_inventory_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_inventory,ui->txt_inventoryid);
}

void APStandardGLAccounts::on_txt_tax_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_tax,ui->txt_taxid);
}

void APStandardGLAccounts::on_txt_labour_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_labour,ui->txt_labourid);
}

void APStandardGLAccounts::on_txt_duty_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_duty,ui->txt_dutyid);
}

void APStandardGLAccounts::on_txt_bank_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_bank,ui->txt_bankid);
}

void APStandardGLAccounts::on_txt_roff_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_roff,ui->txt_roffid);
}

void APStandardGLAccounts::on_txt_bcs_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_bcs,ui->txt_bcsid);
}

void APStandardGLAccounts::on_txt_empc_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->txt_empc,ui->txt_empcid);
}
