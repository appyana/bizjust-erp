#include "editvendor.h"
#include "ui_editvendor.h"

EditVendor::EditVendor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditVendor)
{
    ui->setupUi(this);
}

EditVendor::~EditVendor()
{
    delete ui;
}

void EditVendor::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void EditVendor::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditVendor::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void EditVendor::on_groupname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendorGroup(arg1),ui->groupname,ui->groupid);
}

void EditVendor::on_verbal_textEdited(const QString &arg1)
{
    autocompleter(sch.invoiceterms(arg1),ui->verbal,ui->invoicetermsid);
}

void EditVendor::on_btn_save_clicked()
{
    if(isvalidate())
    {
        QString query = "UPDATE tblvendors "
                        " SET "
                        " vendorname        = '"+ui->cname->text()+"',"
                        " firstname         = '"+ui->first_name->text()+"', "
                        " middlename        = '"+ui->middle_name->text()+"', "
                        " lastname          = '"+ui->last_name->text()+"', "
                        " address1          = '"+ui->address->toPlainText()+"', "
                        " city              = '"+ui->city->text()+"', "
                        " state             = '"+ui->state->text()+"', "
                        " zip               = '"+ui->zip->text()+"', "
                        " country           = '"+ui->country->text()+"', "
                        " phone1            = '"+ui->phone1->text()+"', "
                        " phone2            = '"+ui->phone2->text()+"', "
                        " phone3            = '"+ui->phone3->text()+"', "
                        " phone4            = '"+ui->phone4->text()+"', "
                        " email1            = '"+ui->email1->text()+"', "
                        " email2            = '"+ui->email2->text()+"', "
                        " website           = '"+ui->url->text()+"', "
                        " paytermsid        = '"+ui->invoicetermsid->text()+"', "
                        " vendor_group_id   = '"+ui->groupid->text()+"' "
                        " WHERE "
                        " vendorid = '"+ui->vendorid->text()+"' ";
        if(conn.execQuery(query))
        {
            QMessageBox::information(this,"","Record is updated successfully!");
            //clearform();
        }
        else
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
        }
    }
}

bool EditVendor::isvalidate()
{
    if(ui->vendorid->text()=="")
    {
        QMessageBox::warning(this,"","Please select vendor to edit");
        ui->vendorname->setFocus();
        return false;
    }
    if(ui->cname->text()=="")
    {
        QMessageBox::warning(this,"","Company Name is required");
        ui->cname->setFocus();
        return false;
    }
    return true;
}

void EditVendor::clearform()
{
    ui->cname->clear();
    ui->first_name->clear();
    ui->middle_name->clear();
    ui->last_name->clear();
    ui->address->clear();
    ui->city->clear();
    ui->state->clear();
    ui->zip->clear();
    ui->country->clear();
    ui->phone1->clear();
    ui->phone2->clear();
    ui->phone3->clear();
    ui->phone4->clear();
    ui->email1->clear();
    ui->email2->clear();
    ui->url->clear();
    ui->invoicetermsid->clear();
    ui->verbal->clear();
    ui->groupid->clear();
    ui->groupname->clear();
    ui->cname->setFocus();
}

void EditVendor::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendorsEdit(arg1),ui->vendorname,ui->vendorid);
}


void EditVendor::on_vendorid_textChanged(const QString &arg1)
{
    QSqlQuery info(" select * from tblvendors where vendorid = '"+arg1+"' ");
    info.next();
    ui->cname->setText( info.value("vendorname").toString() );
    ui->first_name->setText( info.value("firstname").toString() );
    ui->middle_name->setText( info.value("middlename").toString() );
    ui->last_name->setText( info.value("lastname").toString() );
    ui->address->setPlainText( info.value("address1").toString() );
    ui->city->setText( info.value("city").toString() );
    ui->state->setText( info.value("state").toString() );
    ui->zip->setText( info.value("zip").toString() );
    ui->country->setText( info.value("country").toString() );
    ui->phone1->setText( info.value("phone1").toString() );
    ui->phone2->setText( info.value("phone2").toString() );
    ui->phone3->setText( info.value("phone3").toString() );
    ui->phone4->setText( info.value("phone4").toString() );
    ui->email1->setText( info.value("email1").toString() );
    ui->email2->setText( info.value("email2").toString() );
    ui->url->setText( info.value("website").toString() );
    ui->invoicetermsid->setText( info.value("paytermsid").toString() );
    ui->verbal->setText( erp.getInvoiceTermName(info.value("paytermsid").toString()) );
    ui->groupid->setText( info.value("vendor_group_id").toString() );
    ui->groupname->setText( erp.getVendorGroupName(info.value("vendor_group_id").toString()) );
    dl=info.value("dl").toInt();
    if(dl==0)
    {
        ui->btn_delete->setText("Delete");
    }
    else
    {
        ui->btn_delete->setText("Active Deleted");
    }
}

void EditVendor::on_btn_delete_clicked()
{
    if(ui->vendorid->text()=="")
    {
        QMessageBox::warning(this,"","Please select vendor to delete");
        ui->vendorname->setFocus();
        return;
    }
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Confirm", "Are you sure you want to perform this action?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          if(dl==0)
          {
              if(!conn.execQuery("update tblvendors set dl='1' where vendorid= '"+ui->vendorid->text()+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in record insertion");
                  return;
              }
              QMessageBox::information(this,"","Vendor deleted successfully ");
              dl=1;
              ui->btn_delete->setText("Active Deleted");
          }
          else
          {
              if(!conn.execQuery("update tblvendors set dl='0' where vendorid= '"+ui->vendorid->text()+"' "))
              {
                  QMessageBox::critical(this,"","Some problem in record insertion");
                  return;
              }
              QMessageBox::information(this,"","vendor activated successfully");
              dl=0;
              ui->btn_delete->setText("Delete");
          }
      }
}
