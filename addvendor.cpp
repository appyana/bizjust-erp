#include "addvendor.h"
#include "ui_addvendor.h"

AddVendor::AddVendor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddVendor)
{
    ui->setupUi(this);
}

AddVendor::~AddVendor()
{
    delete ui;
}


void AddVendor::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddVendor::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddVendor::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void AddVendor::on_groupname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendorGroup(arg1),ui->groupname,ui->groupid);
}

void AddVendor::on_verbal_textEdited(const QString &arg1)
{
    autocompleter(sch.invoiceterms(arg1),ui->verbal,ui->invoicetermsid);
}

void AddVendor::on_btn_save_clicked()
{
    if(isvalidate())
    {
        QSqlQuery qry(" select (max(vendorid)+1) as code from `tblvendors` ");
        qry.next();
        QString vendor_code = qry.value("code").toString();
        QString query = "insert into tblvendors "
                        " ( "
                        " vendorname ,"
                        " firstname , "
                        " middlename , "
                        " lastname , "
                        " vendorcode , "
                        " address1 , "
                        " city, "
                        " state , "
                        " zip, "
                        " country , "
                        " phone1 , "
                        " phone2 , "
                        " phone3 , "
                        " phone4 , "
                        " entrydate, "
                        " email1 , "
                        " email2 , "
                        " website, "
                        " paytermsid , "
                        " vendor_group_id "
                        " ) "
                        " values "
                        " ( "
                        " '"+ui->cname->text()+"' , "
                        " '"+ui->first_name->text()+"' , "
                        " '"+ui->middle_name->text()+"' , "
                        " '"+ui->last_name->text()+"' , "
                        " '"+vendor_code+"' , "
                        " '"+ui->address->toPlainText()+"' , "
                        " '"+ui->city->text()+"' , "
                        " '"+ui->state->text()+"' , "
                        " '"+ui->zip->text()+"', "
                        " '"+ui->country->text()+"' , "
                        " '"+ui->phone1->text()+"' , "
                        " '"+ui->phone2->text()+"' , "
                        " '"+ui->phone3->text()+"' , "
                        " '"+ui->phone4->text()+"' , "
                        " '"+QDate::currentDate().toString("yyyy-MM-dd")+"', "
                        " '"+ui->email1->text()+"' , "
                        " '"+ui->email2->text()+"' , "
                        " '"+ui->url->text()+"' , "
                        " '"+ui->invoicetermsid->text()+"' , "
                        " '"+ui->groupid->text()+"' "
                        " ) ";
        if(conn.execQuery(query))
        {
            QMessageBox::information(this,"","Record is inserted successfully!");
            clearform();
        }
        else
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
        }
    }
}

bool AddVendor::isvalidate()
{
    if(ui->cname->text()=="")
    {
        QMessageBox::warning(this,"","Company Name is required");
        ui->cname->setFocus();
        return false;
    }
    return true;
}

void AddVendor::clearform()
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
