#include "addcustomer.h"
#include "ui_addcustomer.h"
#include <QMessageBox>
AddCustomer::AddCustomer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddCustomer)
{
    //conn.connOpen();
    ui->setupUi(this);
    //loadcountries();
    ui->lbl_pricelevel->setVisible(false);
    ui->comboBoxPriceLevel->setVisible(false);
    ui->lbl_terms->setVisible(false);
    ui->txt_termname->setVisible(false);
    ui->txt_termid->setVisible(false);
    ui->lbl_vendor->setVisible(false);
    ui->txt_vendormane->setVisible(false);
    ui->txt_vendorid->setVisible(false);
    erp.loadSalesCategoriesToComboBox(ui->comboBoxSalesCategory);
    erp.loadPriceLevelsToComboBox(ui->comboBoxPriceLevel);

    /*QStringList CompletionList;
    CompletionList << "Raza Group" << "Tahir Group" << "Khan Group" << "Abid Group" << "Ali1 Group" << "Ali2 Group" << "Ali3 Group" << "Ali4 Group" << "Ali5 Group" << "Ali6 Group" << "Ali7 Group" << "Ali8 Group" << "Ali9 Group";
    groupCompleter = new QCompleter(CompletionList, this);
    groupCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    groupCompleter->setFilterMode(Qt::MatchContains);
    //ui->txt_group->setCompleter(groupCompleter);*/

}



AddCustomer::~AddCustomer()
{
    delete ui;
    //conn.connClose();
}

void AddCustomer::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void AddCustomer::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void AddCustomer::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

/*void AddCustomer::loadcountries()
{
    int cid = erp.getDefaultCountryID();
    erp.loadCountriesToComboBox(ui->comboBoxCountry);
    erp.setComboBoxSelected(ui->comboBoxCountry,cid);
    int i = ui->comboBoxCountry->currentIndex();
    on_comboBoxCountry_activated(i);
}


void AddCustomer::on_comboBoxCountry_activated(int index)
{
    ui->comboBoxState->clear();
    int str = ui->comboBoxCountry->itemData(index).toInt();
    erp.loadCountryStatesToComboBox(ui->comboBoxState,str);
    int sindex = ui->comboBoxState->currentIndex();
    on_comboBoxState_activated(sindex);
}

void AddCustomer::on_comboBoxState_activated(int index)
{
    ui->comboBoxCity->clear();
    int str = ui->comboBoxState->itemData(index).toInt();
    erp.loadStateCitiesToComboBox(ui->comboBoxCity,str);
}*/

void AddCustomer::on_txt_group_textEdited()
{
    QString str = sch.CustomerGroups(ui->txt_group->text());
    autocompleter(str, ui->txt_group, ui->txt_groupid);
}

void AddCustomer::on_txt_salesman_textEdited()
{
    QString str = sch.SalesMan(ui->txt_salesman->text());
    autocompleter(str, ui->txt_salesman, ui->txt_salesmanid);
}

void AddCustomer::on_txt_termname_textEdited()
{
    QString str = sch.invoiceterms(ui->txt_termname->text());
    autocompleter(str, ui->txt_termname, ui->txt_termid);
}

void AddCustomer::on_txt_vendormane_textEdited()
{
    QString str = sch.vendors(ui->txt_vendormane->text());
    autocompleter(str, ui->txt_vendormane, ui->txt_vendorid);
}

void AddCustomer::on_btn_save_clicked()
{

    if(isValid())
    {
        ui->btn_save->setEnabled(false);
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

        QString sql = "INSERT INTO tblcustomers "
                      "(name, entry_firstname, middle_name, entry_lastname, address1,  city, state, zip, country, phone1, phone2, phone3, phone4, phone5,email1, email2, email3, groupid, default_price_level, salesmanid, invoicetermsid, notes, entrydate, customerno, crlimit,dl,url,discount,salescategoryid,combineVendorId) "
                     "VALUES "
                      "('"+name+"', '"+firstname+"', '"+middlename+"', '"+lastname+"', '"+address+"', '"+cityame+"', '"+statename+"', '"+zip+"', '"+countryname+"', '"+phone1+"', '"+phone2+"', '"+phone3+"', '"+phone4+"', '"+phone5+"','"+email1+"', '"+email2+"', '"+email3+"','"+groupid+"', '"+pricelevelid+"', '"+salesmanid+"', '"+termid+"', '"+notes+"', '"+erp.CurrentDateSQL()+"', '"+erp.customerAutoCode()+"','"+crlimit+"', 0,'"+url+"','"+discount+"','"+salescatid+"','"+vendorid+"')";
        //conn.connOpen();
        QSqlQuery qry;
        qry.prepare(sql);
        if(qry.exec())
        {
            clearform();
            ui->btn_save->setEnabled(true);
            //qDebug() << "Last INSERT ID was:" << qry.lastInsertId();
            QMessageBox::information(this,"Info","Customer Added Successfully");
        }
        else
        {
            //QString err = conn.db.lastError().databaseText();
            //QString err2 = conn.db.lastError().driverText();
            ui->btn_save->setEnabled(true);
            QMessageBox::critical(this, "Error", " Some problem in record insertion");
        }
        //conn.connClose();
    }
}
void AddCustomer::clearform()
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
bool AddCustomer::isValid()
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

void AddCustomer::on_btn_close_clicked()
{
    this->close();
}
