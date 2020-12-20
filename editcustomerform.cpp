#include "editcustomerform.h"
#include "ui_editcustomerform.h"
#include <QMessageBox>

EditCustomerForm::EditCustomerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditCustomerForm)
{
    //conn.connOpen();
    ui->setupUi(this);

    ui->lbl_pricelevel->setVisible(false);
    ui->comboBoxPriceLevel->setVisible(false);
    ui->lbl_terms->setVisible(false);
    ui->txt_termname->setVisible(false);
    ui->txt_termid->setVisible(false);
    ui->lbl_vendor->setVisible(false);
    ui->txt_vendormane->setVisible(false);
    ui->txt_vendorid->setVisible(false);

    ui->btn_delete->setVisible(false);
    ui->btn_activate->setVisible(false);

    erp.loadSalesCategoriesToComboBox(ui->comboBoxSalesCategory);
    erp.loadPriceLevelsToComboBox(ui->comboBoxPriceLevel);
}

EditCustomerForm::~EditCustomerForm()
{
    delete ui;
    //conn.connClose();
}

void EditCustomerForm::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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


    connect(completer, SIGNAL(highlighted(QModelIndex)), this, SLOT(onItemHighlighted(QModelIndex)) );
    connect(name_txt, SIGNAL(editingFinished()), this, SLOT(editingFinished()) );
}

void EditCustomerForm::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void EditCustomerForm::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();

    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}


void EditCustomerForm::on_txt_group_textEdited()
{
    QString str = sch.CustomerGroups(ui->txt_group->text());
    autocompleter(str, ui->txt_group, ui->txt_groupid);
}

void EditCustomerForm::on_txt_salesman_textEdited()
{
    QString str = sch.SalesMan(ui->txt_salesman->text());
    autocompleter(str, ui->txt_salesman, ui->txt_salesmanid);
}

void EditCustomerForm::on_txt_termname_textEdited()
{
    QString str = sch.invoiceterms(ui->txt_termname->text());
    autocompleter(str, ui->txt_termname, ui->txt_termid);
}

void EditCustomerForm::on_txt_vendormane_textEdited()
{
    QString str = sch.vendors(ui->txt_vendormane->text());
    autocompleter(str, ui->txt_vendormane, ui->txt_vendorid);
}

void EditCustomerForm::clearform()
{
    ui->txt_name->setText("");
    ui->txt_address->setPlainText("");
    ui->txt_countryname->setText("");
    ui->txt_statename->setText("");
    ui->txt_cityname->setText("");
    ui->txt_zip->setText("");
    ui->txt_firstname->setText("");
    ui->txt_middlename->setText("");
    ui->txt_lastname->setText("");
    ui->txt_phone1->setText("");
    ui->txt_phone2->setText("");
    ui->txt_phone3->setText("");
    ui->txt_phone4->setText("");
    ui->txt_phone5->setText("");
    ui->txt_email1->setText("");
    ui->txt_email2->setText("");
    ui->txt_email3->setText("");
    ui->txt_url->setText("");
    ui->txt_group->setText("");
    ui->txt_groupid->setText("");
    ui->comboBoxPriceLevel->setCurrentIndex(0);
    ui->txt_salesman->setText("");
    ui->txt_salesmanid->setText("");
    ui->txt_termname->setText("");
    ui->txt_termid->setText("");
    ui->txt_vendormane->setText("");
    ui->txt_vendorid->setText("");
    ui->comboBoxSalesCategory->setCurrentIndex(0);
    ui->txt_crlimit->setText("");
    ui->txt_discount->setText("");
    ui->txt_notes->setPlainText("");
}
bool EditCustomerForm::isValid()
{
    if(ui->txt_name->text()=="")
    {
        QMessageBox::critical(this,"Validation Error","Please Enter Company Name");
        ui->txt_name->setFocus();
        return false;
    }
    else if(ui->txt_address->toPlainText()=="")
    {
        QMessageBox::critical(this,"Validation Error","Please Enter Address");
        ui->txt_address->setFocus();
        return false;
    }
    else if(ui->txt_countryname->text()=="")
    {
        QMessageBox::critical(this,"Validation Error","Please Select Country");
        ui->txt_countryname->setFocus();
        return false;
    }
    else if(ui->txt_statename->text()=="")
    {
        QMessageBox::critical(this,"Validation Error","Please Select State");
        ui->txt_statename->setFocus();
        return false;
    }
    else if(ui->txt_cityname->text()=="")
    {
        QMessageBox::critical(this,"Validation Error","Please Select City");
        ui->txt_cityname->setFocus();
        return false;
    }
    else if(ui->txt_firstname->text()=="")
    {
        QMessageBox::critical(this,"Validation Error","Please Enter First Name");
        ui->txt_firstname->setFocus();
        return false;
    }
    else if(ui->txt_phone1->text()=="")
    {
        QMessageBox::critical(this,"Validation Error","Please Enter Office Phone 1");
        ui->txt_phone1->setFocus();
        return false;
    }
    else if(ui->comboBoxSalesCategory->currentIndex()<=0)
    {
        QMessageBox::critical(this,"Validation Error","Please Select Sales Category");
        ui->comboBoxSalesCategory->setFocus();
        return false;
    }
    else
    {
        return true;
    }
}

void EditCustomerForm::on_btn_save_clicked()
{
    if(isValid())
    {
       // QString(QString::toUtf8());
        QString name        = erp.s(ui->txt_name->text());
        QString address     = erp.s(ui->txt_address->toPlainText());
        QString countryname = erp.s(ui->txt_countryname->text());
        QString statename   = erp.s(ui->txt_statename->text());
        QString cityame     = erp.s(ui->txt_cityname->text());
        QString zip         = erp.s(ui->txt_zip->text());
        QString firstname   = erp.s(ui->txt_firstname->text());
        QString middlename  = erp.s(ui->txt_middlename->text());
        QString lastname    = erp.s(ui->txt_lastname->text());
        QString phone1      = erp.s(ui->txt_phone1->text());
        QString phone2      = erp.s(ui->txt_phone2->text());
        QString phone3      = erp.s(ui->txt_phone3->text());
        QString phone4      = erp.s(ui->txt_phone4->text());
        QString phone5      = erp.s(ui->txt_phone5->text());
        QString email1      = erp.s(ui->txt_email1->text());
        QString email2      = erp.s(ui->txt_email2->text());
        QString email3      = erp.s(ui->txt_email3->text());
        QString url         = erp.s(ui->txt_url->text());
        QString groupid     = erp.s(ui->txt_groupid->text());
        int pricelevelindex = ui->comboBoxPriceLevel->currentIndex();
        QString pricelevelid= erp.s(ui->comboBoxPriceLevel->itemData(pricelevelindex).toString());
        QString salesmanid  = erp.s(ui->txt_salesmanid->text());
        QString termid      = erp.s(ui->txt_termid->text());
        QString vendorid    = erp.s(ui->txt_vendorid->text());
        int salescatindex   = ui->comboBoxSalesCategory->currentIndex();
        QString salescatid  = erp.s(ui->comboBoxSalesCategory->itemData(salescatindex).toString());
        QString crlimit     = erp.s(ui->txt_crlimit->text());
        QString discount    = erp.s(ui->txt_discount->text());
        QString notes       = erp.s(ui->txt_notes->toPlainText());

        //QMessageBox::information(this,"Info", pricelevelid);

        QString sql = "UPDATE tblcustomers SET "
                      "name                 = '"+name+"', "
                      "entry_firstname      = '"+firstname+"',"
                      "middle_name          = '"+middlename+"', "
                      "entry_lastname       = '"+lastname+"', "
                      "address1              = '"+address+"',  "
                      "city                 = '"+cityame+"', "
                      "state                = '"+statename+"', "
                      "zip                  = '"+zip+"', "
                      "country              = '"+countryname+"', "
                      "phone1               = '"+phone1+"', "
                      "phone2               = '"+phone2+"', "
                      "phone3               = '"+phone3+"', "
                      "phone4               = '"+phone4+"', "
                      "phone5               = '"+phone5+"',"
                      "email1               = '"+email1+"', "
                      "email2               = '"+email2+"', "
                      "email3               = '"+email3+"', "
                      "groupid              = '"+groupid+"', "
                      "default_price_level  = '"+pricelevelid+"', "
                      "salesmanid           = '"+salesmanid+"', "
                      "invoicetermsid       = '"+termid+"', "
                      "notes                = '"+notes+"', "
                      "crlimit              = '"+crlimit+"',"
                      "url                  = '"+url+"',"
                      "discount             = '"+discount+"',"
                      "salescategoryid      = '"+salescatid+"',"
                      "combineVendorId      = '"+vendorid+"' "
                     " WHERE "
                      " customerid = '"+customerid+"' ";
        //conn.connOpen();
        QSqlQuery qry;
        qry.prepare(sql);
        if(qry.exec())
        {
            //qDebug() << "Last INSERT ID was:" << qry.lastInsertId();
            QMessageBox::information(this,"Info","Customer Updated Successfully");
        }
        else
        {
            //QString err = conn.db.lastError().databaseText();
            //QString err2 = conn.db.lastError().driverText();
            ui->btn_save->setEnabled(true);
            QMessageBox::critical(this, "Error", " Some problem in record Updation");
        }
        //conn.connClose();
    }
}

void EditCustomerForm::loadCustomerData()
{
    //conn.connOpen();
    QSqlQuery qry;
    QString sql = " select * from tblcustomers where customerid = '"+customerid+"' ";
    qry.prepare(sql);
    if(qry.exec())
    {
        qry.next();
        if(qry.value("dl")=="0")
        {
            ui->btn_delete->setVisible(true);
        }
        else
        {
            ui->btn_activate->setVisible(true);
        }
        QString name = qry.value("name").toString();
        ui->txt_name->setText(name);
        QString address = qry.value("address1").toString();
        ui->txt_address->setPlainText(address);
        QString countryname = qry.value("country").toString();
        ui->txt_countryname->setText(countryname);
        QString statename = qry.value("state").toString();
        ui->txt_statename->setText(statename);
        QString city = qry.value("city").toString();
        ui->txt_cityname->setText(city);
        QString zip = qry.value("zip").toString();
        ui->txt_zip->setText(zip);
        QString firstname = qry.value("entry_firstname").toString();
        ui->txt_firstname->setText(firstname);
        QString middlename = qry.value("middle_name").toString();
        ui->txt_middlename->setText(middlename);
        QString lastname = qry.value("entry_lastname").toString();
        ui->txt_lastname->setText(lastname);
        QString phone1 = qry.value("phone1").toString();
        ui->txt_phone1->setText(phone1);
        QString phone2 = qry.value("phone2").toString();
        ui->txt_phone2->setText(phone2);
        QString phone3 = qry.value("phone3").toString();
        ui->txt_phone3->setText(phone3);
        QString phone4 = qry.value("phone4").toString();
        ui->txt_phone4->setText(phone4);
        QString phone5 = qry.value("phone5").toString();
        ui->txt_phone5->setText(phone5);
        QString email1 = qry.value("email1").toString();
        ui->txt_email1->setText(email1);
        QString email2 = qry.value("email2").toString();
        ui->txt_email2->setText(email2);
        QString email3 = qry.value("email3").toString();
        ui->txt_email3->setText(email3);
        QString url = qry.value("url").toString();
        ui->txt_url->setText(url);
        QString groupid = qry.value("groupid").toString();
        ui->txt_groupid->setText(groupid);
        ui->txt_group->setText(erp.getCustomerGroupName(groupid));

        int pricelevelid = qry.value("default_price_level").toInt();
        erp.setComboBoxSelected(ui->comboBoxPriceLevel, pricelevelid);

        //ui->txt_name->setText(pricelevelid);
        QString salesmanid = qry.value("salesmanid").toString();
        ui->txt_salesmanid->setText(salesmanid);
        ui->txt_salesman->setText(erp.getSalesManName(salesmanid));
        QString invoicetermsid = qry.value("invoicetermsid").toString();
        ui->txt_termid->setText(invoicetermsid);
        ui->txt_termname->setText(erp.getInvoiceTermName(invoicetermsid));
        QString combineVendorId = qry.value("combineVendorId").toString();
        ui->txt_vendorid->setText(combineVendorId);
        ui->txt_vendormane->setText(erp.getVendorName(combineVendorId));

        int salescategoryid = qry.value("salescategoryid").toInt();
        erp.setComboBoxSelected(ui->comboBoxSalesCategory, salescategoryid);

        QString crlimit = qry.value("crlimit").toString();
        ui->txt_crlimit->setText(crlimit);
        QString discount = qry.value("discount").toString();
        ui->txt_discount->setText(discount);
        QString notes = qry.value("notes").toString();
        ui->txt_notes->setPlainText(notes);
    }

    if(customerid=="" || customerid=="0")
    {
        ui->btn_delete->setVisible(false);
        ui->btn_activate->setVisible(false);
    }
    //conn.connClose();

}

void EditCustomerForm::on_btn_close_clicked()
{
    this->close();
}

void EditCustomerForm::on_btn_delete_clicked()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          QString sql = " update tblcustomers set dl = '1' where customerid = '"+customerid+"' ";
            QSqlQuery qry;
            qry.prepare(sql);
            if(qry.exec())
            {
                ui->btn_delete->setVisible(false);
                ui->btn_activate->setVisible(true);
            }
            else
            {
                QMessageBox::critical(this,"Error","Some problem occered");
            }
      }
}

void EditCustomerForm::on_btn_activate_clicked()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Confirm", "Are you sure you want to reactivate this?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          QString sql = " update tblcustomers set dl = '0' where customerid = '"+customerid+"' ";
            QSqlQuery qry;
            qry.prepare(sql);
            if(qry.exec())
            {
                ui->btn_activate->setVisible(false);
                ui->btn_delete->setVisible(true);
            }
            else
            {
                QMessageBox::critical(this,"Error","Some problem occered");
            }

      }
}

void EditCustomerForm::on_txt_customername_textEdited()
{
    QString dl = QString::number(ui->comboStatus->currentIndex());
    QString str = sch.customers(ui->txt_customername->text(),dl);
    autocompleter(str, ui->txt_customername, ui->txt_customer_id);
}

void EditCustomerForm::on_txt_customer_id_textChanged()
{
    customerid = ui->txt_customer_id->text();
    loadCustomerData();
}
