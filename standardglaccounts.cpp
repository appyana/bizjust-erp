#include "standardglaccounts.h"
#include "ui_standardglaccounts.h"

StandardGLAccounts::StandardGLAccounts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandardGLAccounts)
{
    ui->setupUi(this);

    QSqlQuery info(" select * from tblglstdaccts where id = '1' ");
    info.next();
    QString earning         = info.value("RetainedEarnings").toString();
    QString purchie        = info.value("purchie").toString();
    QString bank        = info.value("bank").toString();

    ui->earningid->setText(earning);
    ui->purchieid->setText(purchie);
    ui->bankacctid->setText(bank);


    ui->earningdescription->setText(erp.getAccount(earning));
    ui->purchiedescription->setText(erp.getAccount(purchie));
    ui->bankacctdesc->setText(erp.getAccount(bank));
}

StandardGLAccounts::~StandardGLAccounts()
{
    delete ui;
}


void StandardGLAccounts::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void StandardGLAccounts::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void StandardGLAccounts::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void StandardGLAccounts::on_btn_save_clicked()
{
    //save code here
    QString RetainedEarnings= ui->earningid->text();
    QString purchie         = ui->purchieid->text();
    QString bank            = ui->bankacctid->text();
    QString query = "UPDATE tblglstdaccts "
                    " SET "
                    " `RetainedEarnings`= '"+RetainedEarnings+"', "
                    " `purchie`         = '"+purchie+"', "
                    " `bank`            = '"+bank+"' "
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

void StandardGLAccounts::on_earningdescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->earningdescription,ui->earningid);
}

void StandardGLAccounts::on_bankacctdesc_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->bankacctdesc,ui->bankacctid);
}

void StandardGLAccounts::on_purchiedescription_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->purchiedescription,ui->purchieid);
}
