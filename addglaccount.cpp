#include "addglaccount.h"
#include "ui_addglaccount.h"

AddGLAccount::AddGLAccount(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddGLAccount)
{
    ui->setupUi(this);
    loadForm();
}

AddGLAccount::~AddGLAccount()
{
    delete ui;
}

void AddGLAccount::loadForm()
{
    cb.loadAccountTypesToComboBox(ui->accounttypeid);
    QString accounttypeid = ui->accounttypeid->itemData(ui->accounttypeid->currentIndex()).toString();
    cb.loadSummaryAccountToComboBox(ui->summaryaccountid,accounttypeid);
    loadaccountname(ui->summaryaccountid->currentIndex());
}

void AddGLAccount::loadaccountname(int index)
{
    QString summaryaccountid = ui->summaryaccountid->itemData(index).toString();
    QString autoAccount_qry = "select name from tblglaccount where id=(select max(id) from tblglaccount where "
                              " summaryaccountid='"+summaryaccountid+"')";
    QSqlQuery autoAccount_row(autoAccount_qry);
    autoAccount_row.next();
    QString nextAccount_name= checkvalidaccount(autoAccount_row.value("name").toInt() + 1);
    if(autoAccount_row.value("name").toInt()==0)
    {
        QSqlQuery qry(" select name from tblglaccount where id = '"+summaryaccountid+"' ");
        qry.next();
        nextAccount_name = checkvalidaccount(qry.value(0).toInt()+1);
    }
    ui->name->setText(nextAccount_name);
}

QString AddGLAccount::checkvalidaccount(int name)
{
    QSqlQuery qry(" select * from tblglaccount where name = '"+QString::number(name)+"' ");
    if(qry.size()>0)
    {
        return checkvalidaccount(name+1);
    }
    else
    {
        return QString::number(name);
    }
}

void AddGLAccount::on_btn_save_clicked()
{
    if(ui->name->text()=="")
    {
        QMessageBox::critical(this,"","Name is required");
        ui->name->setFocus();
        return;
    }
    if(ui->description->text()=="")
    {
        QMessageBox::critical(this,"","Description is required");
        ui->description->setFocus();
        return;
    }
    QString name = ui->name->text();
    QString description = ui->description->text();
    QString accounttypeid = ui->accounttypeid->itemData(ui->accounttypeid->currentIndex()).toString();
    QString grouptype = ui->grouptype->currentText().toLower();
    QString summaryaccountid = ui->summaryaccountid->itemData(ui->summaryaccountid->currentIndex()).toString();
    QString costcenterid = "0";
    QString entrydate = QDate::currentDate().toString("yyyy-MM-dd");
    QString query = "insert into tblglaccount "
                    " (name, description, accountypeid, costcenterid, entrydate, grouptype, summaryaccountid) "
                    " values"
                    " ('"+name+"', '"+description+"', '"+accounttypeid+"', '"+costcenterid+"', '"+entrydate+"', '"+grouptype+"', '"+summaryaccountid+"')";
    if(!conn.execQuery(query))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QMessageBox::information(this,"","Record is inserted successfully.");
    ui->name->clear();
    ui->description->clear();
    ui->accounttypeid->setCurrentIndex(0);
    ui->grouptype->setCurrentIndex(0);
    ui->summaryaccountid->setCurrentIndex(0);
    ui->accounttypeid->setFocus();
}

void AddGLAccount::on_accounttypeid_currentIndexChanged(int index)
{
    QString accounttypeid = ui->accounttypeid->itemData(index).toString();
    cb.loadSummaryAccountToComboBox(ui->summaryaccountid,accounttypeid);
    loadaccountname(ui->summaryaccountid->currentIndex());
}

void AddGLAccount::on_summaryaccountid_currentIndexChanged(int index)
{
    loadaccountname(index);
}
