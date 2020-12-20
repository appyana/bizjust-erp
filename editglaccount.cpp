#include "editglaccount.h"
#include "ui_editglaccount.h"

EditGLAccount::EditGLAccount(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditGLAccount)
{
    ui->setupUi(this);
    loadForm();
}

EditGLAccount::~EditGLAccount()
{
    delete ui;
}


void EditGLAccount::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditGLAccount::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditGLAccount::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}


void EditGLAccount::loadForm()
{
    cb.loadAccountTypesToComboBox(ui->accounttypeid);
    QString accounttypeid = ui->accounttypeid->itemData(ui->accounttypeid->currentIndex()).toString();
    cb.loadSummaryAccountToComboBox(ui->summaryaccountid,accounttypeid);
    loadaccountname(ui->summaryaccountid->currentIndex());
}

void EditGLAccount::loadaccountname(int index)
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

QString EditGLAccount::checkvalidaccount(int name)
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

void EditGLAccount::on_btn_save_clicked()
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
    QString id = ui->accountid->text();
    QString name = ui->name->text();
    QString description = ui->description->text();
    QString accounttypeid = ui->accounttypeid->itemData(ui->accounttypeid->currentIndex()).toString();
    QString grouptype = ui->grouptype->currentText().toLower();
    QString summaryaccountid = ui->summaryaccountid->itemData(ui->summaryaccountid->currentIndex()).toString();
    QString costcenterid = "0";
    QString query = "UPDATE tblglaccount "
                    " SET "
                    " name = '"+name+"',"
                    " description = '"+description+"',"
                    " accountypeid = '"+accounttypeid+"',"
                    " costcenterid = '"+costcenterid+"',"
                    " grouptype = '"+grouptype+"',"
                    " summaryaccountid = '"+summaryaccountid+"' "
                    " WHERE "
                    " id = '"+id+"' ";
    if(!conn.execQuery(query))
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
        return;
    }
    QMessageBox::information(this,"","Record is updated successfully.");
}

void EditGLAccount::on_accounttypeid_currentIndexChanged(int index)
{
    QString accounttypeid = ui->accounttypeid->itemData(index).toString();
    cb.loadSummaryAccountToComboBox(ui->summaryaccountid,accounttypeid);
    loadaccountname(ui->summaryaccountid->currentIndex());
}

void EditGLAccount::on_summaryaccountid_currentIndexChanged(int index)
{
    loadaccountname(index);
}

void EditGLAccount::on_accountname_textEdited(const QString &arg1)
{
    autocompleter(sch.glaccount_generalize(arg1),ui->accountname,ui->accountid);
}

void EditGLAccount::on_accountid_textChanged(const QString &arg1)
{

    QSqlQuery qry(" select * from tblglaccount where id = '"+arg1+"' ");
    qry.next();
    erp.setComboBoxSelected(ui->accounttypeid,qry.value("accountypeid").toInt());
    int grp = 0;
    if(qry.value("grouptype").toString()=="group")
    {
        grp = 1;
    }
    ui->grouptype->setCurrentIndex(grp);
    erp.setComboBoxSelected(ui->summaryaccountid,qry.value("summaryaccountid").toInt());
    ui->description->setText(qry.value("description").toString());
    ui->name->setText(qry.value("name").toString());
    if(arg1!="")
    {
        ui->editFrame->setEnabled(true);
    }
    else
    {
        ui->editFrame->setEnabled(false);
    }
}

void EditGLAccount::on_btn_delete_clicked()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Confirm", "Are you sure you want to perform this action?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          QString id = ui->accountid->text();
          QSqlQuery qry(" select glaccountid from tblgltransaction where glaccountid = '"+id+"' ");
          if(qry.size()<=0)
          {
              if(!conn.execQuery("delete from tblglaccount where id = '"+id+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in record deletion");
                  return;
              }
          }
          else
          {
              if(!conn.execQuery("update tblglaccount set cancel = '1' where id = '"+id+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in record deletion");
                  return;
              }
          }
          QMessageBox::information(this,"","Account is deleted successfully.");
          ui->accountid->clear();
          ui->accountname->clear();
          ui->accountname->setFocus();
          ui->editFrame->setEnabled(false);
      }
}
