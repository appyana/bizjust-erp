#ifndef ERP_H
#define ERP_H

#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QTime>
#include <QDate>
#include <QLabel>

class ERP
{

public:
    QLabel *lbl;
    QMovie *movie;
    QString loginid;
    QString _yearid;
    void setloginid(QString id)
    {
        loginid = id;
    }
    void setyearid(QString id)
    {
        _yearid = id;
    }
    void setloader(QWidget *parent)
    {
        lbl = new QLabel(parent);
        lbl->setMinimumWidth(128);
        lbl->setMaximumWidth(128);
        movie = new QMovie(":/Images/images/loading.gif");
        lbl->setMovie(movie);
    }
    void hideloader()
    {
        lbl->setVisible(false);
        movie->stop();
    }
    void showloader()
    {
        lbl->setVisible(true);
        movie->start();
    }
    void setComboBoxSelected(QComboBox *combo, int i=0)
    {
        int index = combo->findData(i);
        //if ( index != -1 ) { // -1 for not found
           combo->setCurrentIndex(index);
        //}
    }

    void loadSalesCategoriesToComboBox(QComboBox *combo)
    {
        combo->clear();
        combo->addItem("Select Sales Category", QVariant(0));
        QSqlQuery qry;
        QString sql = " select id, name from tblsalescategory order by id asc ";
        qry.prepare(sql);
        if(qry.exec())
        {
            while(qry.next())
            {
                int v = qry.value(0).toInt();
                QString s = qry.value(1).toString();
                combo->addItem(s, QVariant(v));
            }
        }
    }

    void loadSalesTypeToComboBox(QComboBox *combo)
    {
        combo->clear();
        //combo->addItem("Select", QVariant(0));
        QSqlQuery qry;
        QString sql = " select id, name from tblsalescategory order by id asc ";
        qry.prepare(sql);
        if(qry.exec())
        {
            while(qry.next())
            {
                int v = qry.value(0).toInt();
                QString s = qry.value(1).toString();
                combo->addItem(s, QVariant(v));
            }
        }
    }

    void loadCustomersGroupToComboBox(QComboBox *combo)
    {
        combo->clear();
        combo->addItem("Select", QVariant(0));
        QSqlQuery qry;
        QString sql = " select groupid, groupname from tblcustomergroups order by groupname asc ";
        qry.prepare(sql);
        if(qry.exec())
        {
            while(qry.next())
            {
                int v = qry.value(0).toInt();
                QString s = qry.value(1).toString();
                combo->addItem(s, QVariant(v));
            }
        }
    }

    void loadCarrierToComboBox(QComboBox *combo)
    {
        combo->clear();
        combo->addItem("Select", QVariant(0));
        QSqlQuery qry;
        QString sql = " select id,description from tblcarrier ORDER BY `description` asc ";
        qry.prepare(sql);
        if(qry.exec())
        {
            while(qry.next())
            {
                int v = qry.value(0).toInt();
                QString s = qry.value(1).toString();
                combo->addItem(s, QVariant(v));
            }
        }
    }

    void loadPriceLevelsToComboBox(QComboBox *combo)
    {
        combo->clear();
        QSqlQuery qry;
        QString sql = " select id, description from tblpricelevel order by id asc ";
        qry.prepare(sql);
        if(qry.exec())
        {
            while(qry.next())
            {
                int v = qry.value(0).toInt();
                QString s = qry.value(1).toString();
                combo->addItem(s, QVariant(v));
            }
        }
    }

    QString customerAutoCode()
    {
        QString v;
        QSqlQuery qry;
        QString sql = " select (max(customerid)+1) as code from `tblcustomers` ";
        qry.prepare(sql);
        if(qry.exec())
        {
            while(qry.next())
            {
                v = qry.value(0).toString();
            }
        }
        return "C0000" + v;
    }

    QString CurrentDateSQL()
    {
        QDate cd = QDate::currentDate();
        QString c= cd.toString("yyyy-MM-dd");
        return c;
    }

    QString CurrentDateDDMMYY()
    {
        QDate cd = QDate::currentDate();
        QString c= cd.toString("dd/MM/yyyy");
        return c;
    }

    QString SQLDateFromDDMMYY(QString dt)
    {
        QDate cd = QDate::fromString(dt,"dd/MM/yyyy");
        QString c= cd.toString("yyyy-MM-dd");
        return c;
    }
    QString DDMMYYDateFromSQL(QString dt)
    {
        QDate cd = QDate::fromString(dt,"yyyy-MM-dd");
        QString c= cd.toString("dd/MM/yyyy");
        return c;
    }
    QDate SQLDateToQtDate(QString dt)
    {
        QDate cd = QDate::fromString(dt,"yyyy-MM-dd");
        return cd;
    }
    QDate DDMMYYDateToQtDate(QString dt)
    {
        QDate cd = QDate::fromString(dt,"dd/MM/yyyy");
        return cd;
    }
    QString qrystring(QString str="")
    {
        return (QString)str + "";
    }
    int qrystring(int str=0)
    {
        return (int)str + 0;
    }

    QString getCustomerGroupName(QString id)
    {
        QString sql = " select groupname from tblcustomergroups where groupid = :id ";
        QSqlQuery qry;//(sql);
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        int name = qry.record().indexOf("groupname");
        qry.next();
        return qry.value(name).toString();
    }

    QString getVendorGroupName(QString id)
    {
        QString sql = " select group_name from tbl_vendor_group where id = :id ";
        QSqlQuery qry;//(sql);
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        int name = qry.record().indexOf("group_name");
        qry.next();
        return qry.value(name).toString();
    }

    QString getSalesManName(QString id)
    {
        QString sql = " select name from tblsalesman where salesmanid = :id ";
        QSqlQuery qry;//(sql);
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        int name = qry.record().indexOf("name");
        qry.next();
        return qry.value(name).toString();
    }

    QString getInvoiceTermName(QString id)
    {
        QString sql = " select verbal from tblinvoiceterms where id = :id ";
        QSqlQuery qry;//(sql);
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        int name = qry.record().indexOf("verbal");
        qry.next();
        return qry.value(name).toString();
    }

    QString getVendorName(QString id)
    {
        QString sql = " select vendorname from tblvendors where vendorid = :id ";
        QSqlQuery qry;//(sql);
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        int name = qry.record().indexOf("vendorname");
        qry.next();
        return qry.value(name).toString();
    }

    QString getItemField(QString id, QString field)
    {
        QString sql = " select `"+field+"` from tblitems where itemid = :id ";
        QSqlQuery qry;//(sql);
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        int name = qry.record().indexOf(field);
        qry.next();
        return qry.value(name).toString();
    }

    QString s(QString str)
    {
        str.replace("'","\\'");
        str.replace("\"","\\\"");
        return str;
    }

    void loadLocationsToComboBox(QComboBox *combo)
    {
        combo->clear();
        QSqlQuery qry;
        QString sql = " select id, name from tblinventorylocation order by id asc ";
        qry.prepare(sql);
        if(qry.exec())
        {
            while(qry.next())
            {
                int v = qry.value(0).toInt();
                QString s = qry.value(1).toString();
                combo->addItem(s, QVariant(v));
            }
        }
    }

    QString DecimalString(float amount)
    {
        return QString::number(amount, 'f', 2);
    }

    QString intString(qint32 amount)
    {
        return QString::number(amount);
    }



    QString convert_to_num(QString prefix, QString num)
    {
        QString number = prefix + num ;
        return number;
    }

/********************************** Get Max Number for type  ***************************************/

    QString get_spec_num( QString code, QString letters )
    {
        QString sql = " select value from tblids where code = :code ";
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":code", code);
        qry.exec();
        qry.next();
        QString num = qry.value("value").toString();
        num = this->convert_to_num(letters,num);
        return num;
    }

/**************************** Get Max Number for type  *********************************/
    QString get_num( QString code )
    {
        QString sql = " select value from tblids where code = :code ";
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":code", code);
        qry.exec();
        qry.next();
        return qry.value("value").toString();
    }
/**************************** Increment of 1 Number for type  *********************************/
    bool update_ids_num( QString code )
    {
        QString num = this->intString(this->get_num(code).toInt()+1);
        QString sql = " update `tblids` SET `value`=:num where code = :code ";
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":num", num);
        qry.bindValue(":code", code);
        return qry.exec();
    }
/***************************** Get Set Account id for  Receivable ***********************************************/
    QString getaccountAR(QString field)
    {
        QString sql = "select `"+field+"` from tblarcompany limit 1" ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.exec();
        qry.next();
        return qry.value(field).toString();
    }
/***************************** Get Set Account id for  tblglstdaccts ***********************************************/
    QString getAccountGlStd(QString field)
    {
        QString sql = "select `"+field+"` from tblglstdaccts where id='1' " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.exec();
        qry.next();
        return qry.value(field).toString();
    }
/***************************** Get Set Account id for  Payable ***********************************************/
    QString getaccountAP(QString field)
    {
        QString sql = "select "+field+" from tblapcompany limit 1" ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.exec();
        qry.next();
        return qry.value(field).toString();
    }
/*************************** GET Category name from itemid ****************************************/
    QString catname(QString itemid)
    {
        QString sql = "select c.name from tblitems i, tblitemcategory c where c.id=i.categoryid and i.itemid= :itemid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":itemid", itemid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/***************************** GET Item name from itemid **************************************/
    QString itemname(QString itemid)
    {
        QString sql = "select description from tblitems where itemid = :itemid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":itemid", itemid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/************************** Get LBS per price unit from itemid ************************************/
    QString itemlbsperpriceunit(QString itemid)
    {
        QString sql = "select lbsperpriceunit from tblitems where itemid = :itemid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":itemid", itemid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/*************************** Get Item cost from itemid ****************************************/
    float itemcost(QString itemid)
    {
        QString sql = "select itemcosteach from tblitems where itemid = :itemid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":itemid", itemid);
        qry.exec();
        qry.next();
        return qry.value(0).toFloat();
    }
/***************************** get unit name from unitid **************************************/
    QString priceunit(QString unitid)
    {
        QString sql = "select unitname from tblinventoryunitname where id = :id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", unitid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/*********************************************************/
    bool AddToEventLog(QString event,QString transtype,QString transid)
    {
        QVariant date = QDateTime::currentDateTime();
        QString sql = "insert into tbltranslog(LogDateTime,EventName,TransType,TransID, UserID)values(NOW(),'"+event+"','"+transtype+"','"+transid+"','"+loginid+"') " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":date",date);
        return qry.exec();
    }
/***************************** get Customer name from customerid **************************************/
    QString customerName(QString customerid)
    {
        QString sql = "select name from tblcustomers where customerid = :customerid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":customerid", customerid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/***************************** get Vendor Name from vendorid **************************************/
    QString vendorname(QString vendorid)
    {
        QString sql = "select vendorname from tblvendors where vendorid = :vendorid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":vendorid", vendorid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/***************************** get Customer field from customerid **************************************/
    QString customerField(QString customerid, QString field)
    {
        QString sql = "select `"+field+"` from tblcustomers where customerid = :customerid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":customerid", customerid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/******************************* get Vendor field from vendorid *********************************************************/
    QString vendorField(QString vendorid, QString field)
    {
        QString sql = "select `"+field+"` from tblvendors where vendorid = :vendorid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":vendorid", vendorid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/******************************* get Account field *********************************************************/
    QString accountField(QString id, QString field)
    {
        QString sql = "select `"+field+"` from tblglaccount where id = :id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/******************************* Get Account Type Name *********************************************************/
    QString accounttypename(QString id)
    {
        QString sql = "select description from tblaccounttype where id = :id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/******************************* Get Location Name *********************************************************/
    QString locationName(QString locationid)
    {
        QString sql = "select name from tblinventorylocation where id = :locationid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":locationid", locationid);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/****************************************************************************************/
    QString getInvoiceId(QString invoicenumber)
    {
        QString sql = "select id from tblinvoice where invoicenumber = :invoicenumber " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":invoicenumber", invoicenumber);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/****************************************************************************************/
    QString getvoucherid(QString voucherno)
    {
        QString sql = "select voucherid from tblgltransvoucher where voucherno = :voucherno " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":voucherno", voucherno);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/****************************************************************************************/
    QString getrcvid(QString voucherno)
    {
        QString sql = "select rcvid from tblinvporcv where voucherno = :voucherno " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":voucherno", voucherno);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/************************************ Company Name ****************************************************/
    QString companyname()
    {
        QString sql = "select c_name from tblcompay_mainpage_info " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.exec();
        qry.next();
        return qry.value(0).toString();//
    }
/********************************* Get Company Field *******************************************************/
    QString companyField(QString field)
    {
        QString sql = "select `"+field+"` from tblcompay_mainpage_info " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/******************************** Get Carrir from id ********************************************************/
    QString carrierName(QString id)
    {
        QString sql = "select `description` from tblcarrier where id = :id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/********************************* Load Item units in combobox by itemid *******************************************************/
    void loadItemUnits(QComboBox *combo, QString itemid)
    {
        combo->clear();
        QString sql = "select pcsperpackunitid,packpermasterunitid,masterpackingunitid from tblitems where itemid = :itemid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":itemid", itemid);
        qry.exec();
        qry.next();

        QSqlQuery units;
        units.prepare("select * from tblinventoryunitname order by unitname ASC");
        units.exec();
        while(units.next())
        {
            if( (units.value("id").toInt()==qry.value("pcsperpackunitid").toInt()) || (units.value("id").toInt()==qry.value("packpermasterunitid").toInt()) || (units.value("id").toInt()==qry.value("masterpackingunitid").toInt()) )
            {
                int v = units.value("id").toInt();
                QString s = units.value("unitname").toString();
                combo->addItem(s, QVariant(v));
                if(units.value("id").toInt()==qry.value("masterpackingunitid").toInt())
                {
                    setComboBoxSelected(combo, units.value("id").toInt());
                }
            }
        }
    }
/****************************** GET UNIT Name **********************************************************/
    QString unitName(QString id)
    {
        QString sql = "select `unitname` from tblinventoryunitname where id = :id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/******************************* GET CUSTOMER BALANCE *********************************************************/
    QString customerBalance(QString id, QString entrydate)
    {
        QString sql = "SELECT (SUM(debit)-SUM(credit)) AS balance FROM tblgltransaction WHERE companytype = 'customer' AND compid = :id AND entrydate <= :entrydate " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.bindValue(":entrydate", entrydate);
        qry.exec();
        qry.next();
        return DecimalString( qry.value(0).toFloat() );
    }
/*********************************** GET ACCOUNT DESCRIPTION  *****************************************************/
    QString getAccount(QString id)
    {
        QString sql = "select `description` from tblglaccount where id = :id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/************************************  LOAD VOUCHERS FOR CHEQUE/PURCHIE ENCASHED  ****************************************************/
    void loadChequePurchieVouchers(QComboBox *combo, QString paymeth)
    {
        combo->clear();
        combo->addItem("Select", QVariant(0));
        QString sql = " select voucherid from tblinvoicepaymentdetail where paymeth=:paymeth and status in ('0','1') " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":paymeth", paymeth);
        qry.exec();
        while(qry.next())
        {
                QString vno = qry.value("voucherid").toString();
                combo->addItem(vno, QVariant(vno));
        }
    }
/*********************************** Report Begin Date *****************************************************/
    QDate reportBeginDate()
    {
        QString sql = " select startdate from tblfinancialyear where yearid=:yearid " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":yearid", _yearid);
        qry.exec();
        qry.next();
        return qry.value(0).toDate();
    }
/****************************************************************************************/
    QString amountString(float floatNumber)
    {
        int precision = 0;
        QString stringAmount = QString::number(floatNumber, 'f', precision);
        for(int point = 0, i = (stringAmount.lastIndexOf('.') == -1 ? stringAmount.length() : stringAmount.lastIndexOf('.')); i > 0; --i, ++point)
        {
            if(point != 0 && point % 3 == 0)
            {
                stringAmount.insert(i, ',');
            }
        }
        return stringAmount;
    }
/*****************************************************************************************/
    QString paymethod(QString id)
    {
        QString sql = " select paymeth  from  tblpaymeth  where id=:id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/****************************************************************************************/
    QString categoryName(QString id)
    {
        QString sql = " select name from tblitemcategory where id=:id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        if(qry.value(0).toString()=="")
        {
            return "No Category";
        }
        else
        {
            return qry.value(0).toString();
        }
    }
/****************************************************************************************/
    QString subCategoryName(QString id)
    {
        QString sql = " select name from pro_sub_cat where id=:id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        if(qry.value(0).toString()=="")
        {
            return "No Category";
        }
        else
        {
            return qry.value(0).toString();
        }
    }
/****************************************************************************************/
    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *  This Functions can be used for getting the total stock in packings
     *
     *  INPUT : qty in smallest units,	item id
     *  OUTPUT: STOCK IN PACKIING
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    */
    QString showStockInPacking(int total_Qty_in_smallest_unit,QString itemID)
    {
        QSqlQuery Itmrow(" select * from tblitems where itemid ='"+itemID+"' ");
        Itmrow.next();
        QString stock;
        int sp=Itmrow.value("pcsperpack").toInt();
        int mp=Itmrow.value("packpermaster").toInt();
        if(sp==0){ sp=1; }
        if(mp==0){ mp=1; }
        int remQty = total_Qty_in_smallest_unit;
        int lpQty = remQty;
        if(remQty==0)
        {
            return intString(remQty);
        }
        else
        {
            if(sp != 0 && mp != 0)
            {
                lpQty = remQty/(sp * mp);
                remQty = remQty - (lpQty * (sp * mp));
            }
            if(Itmrow.value("masterpackingunitid").toInt()>0)
            {
                stock = intString(lpQty)+" "+unitName( Itmrow.value("masterpackingunitid").toString() )+"  ";
            }
            if(Itmrow.value("packpermasterunitid").toInt()>0)
            {
                int mpQty = remQty/sp;
                remQty = remQty - (mpQty * sp);
                stock = stock + intString(mpQty)+" "+unitName( Itmrow.value("packpermasterunitid").toString() )+"  ";
            }
            if(Itmrow.value("pcsperpackunitid").toInt()>0)
            {
                int spQty = remQty;
                stock = stock + intString(spQty)+" "+unitName( Itmrow.value("pcsperpackunitid").toString() );
            }
            return stock;
        }
    }
    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *  This Functions Converts customer's Item wise Sale in SMALLEST UNITS
     *
     *  INPUT : Item Id, customer id, From Date, To Date
     *  OUTPUT: Stock in Smallest Unit/Packing...
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    */
    int customerItemSale_inSmallPacket(QString itemID,QString customerId,QString begindate,QString enddate)
    {
        QSqlQuery Itmrow("select * from tblitems where itemid = '"+itemID+"' Limit 0,1");
        Itmrow.next();

        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        if(sp==0){sp=1;}
        if(mp==0){mp=1;}
        int qty_in_smallest_unit=0;
        QString masterpackingunitid = Itmrow.value("masterpackingunitid").toString();
        QString packpermasterunitid = Itmrow.value("packpermasterunitid").toString();
        QString pcsperpackunitid = Itmrow.value("pcsperpackunitid").toString();
        if(masterpackingunitid.toInt()!=0)
        {
            QString packingUnit = masterpackingunitid;
            QString ItmSaleQuery = "select IFNULL(sum(qty),0) as Qty from tblinvoice i, tblinvoicedetail d where "
                                   " i.invoicenumber = d.invoiceid and itemid='"+itemID+"' and "
                                   " packingUnit='"+packingUnit+"' and i.customerid = '"+customerId+"' ";
            if(begindate!="") { ItmSaleQuery += " and i.invoicedate>='"+begindate+"' "; }
            if(begindate!="") { ItmSaleQuery += " and i.invoicedate<='"+enddate+"' "; }
            QSqlQuery ItmSaleRow(ItmSaleQuery);
            ItmSaleRow.next();
            int Qty = ItmSaleRow.value("Qty").toInt();
            qty_in_smallest_unit = qty_in_smallest_unit + (sp * mp * Qty);
        }
        else if(packpermasterunitid.toInt()!=0)
        {
            QString packingUnit = packpermasterunitid;
            QString ItmSaleQuery = "select IFNULL(sum(qty),0) as Qty from tblinvoice i, tblinvoicedetail d where "
                                   " i.invoicenumber = d.invoiceid and itemid='"+itemID+"' and "
                                   " packingUnit='"+packingUnit+"' and i.customerid = '"+customerId+"' ";
            if(begindate!="") { ItmSaleQuery += " and i.invoicedate>='"+begindate+"' "; }
            if(begindate!="") { ItmSaleQuery += " and i.invoicedate<='"+enddate+"' "; }
            QSqlQuery ItmSaleRow(ItmSaleQuery);
            ItmSaleRow.next();
            int Qty = ItmSaleRow.value("Qty").toInt();
            qty_in_smallest_unit = qty_in_smallest_unit + (sp * Qty);
        }
        else if(pcsperpackunitid.toInt()!=0)
        {
            QString packingUnit = pcsperpackunitid;
            QString ItmSaleQuery = "select IFNULL(sum(qty),0) as Qty from tblinvoice i, tblinvoicedetail d where "
                                   " i.invoicenumber = d.invoiceid and itemid='"+itemID+"' and "
                                   " packingUnit='"+packingUnit+"' and i.customerid = '"+customerId+"' ";
            if(begindate!="") { ItmSaleQuery += " and i.invoicedate>='"+begindate+"' "; }
            if(begindate!="") { ItmSaleQuery += " and i.invoicedate<='"+enddate+"' "; }
            QSqlQuery ItmSaleRow(ItmSaleQuery);
            ItmSaleRow.next();
            int Qty = ItmSaleRow.value("Qty").toInt();
            qty_in_smallest_unit = qty_in_smallest_unit + Qty;
        }
        return qty_in_smallest_unit;
    }

/****************************************************************************************/
    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *  This Functions Converts customer's Item wise Sale in SMALLEST UNITS
     *
     *  INPUT : Item Id, customer id, From Date, To Date
     *  OUTPUT: Stock in Smallest Unit/Packing...
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    */
    QString saleAndPurchasedUnits(QString transType,QString itemID,QString year,QString month,bool packing)
    {
        QSqlQuery Itmrow("select * from tblitems where itemid = '"+itemID+"' Limit 0,1");
        Itmrow.next();

        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        if(sp==0){sp=1;}
        if(mp==0){mp=1;}
        QString masterpackingunitid = Itmrow.value("masterpackingunitid").toString();
        QString packpermasterunitid = Itmrow.value("packpermasterunitid").toString();
        QString pcsperpackunitid = Itmrow.value("pcsperpackunitid").toString();

        int addQty=0;

        QString querySum = "SELECT * FROM tblitemtransdet, tblitemtrans  WHERE tblitemtrans.ITransID "
                           " = tblitemtransdet.ITransID AND tblitemtrans.ITransType = tblitemtransdet.ITransType "
                           " AND tblitemtransdet.ITransType in('"+transType+"') AND itemid ='"+itemID+"' "
                           " AND YEAR(tblitemtrans.ITDate)='"+year+"' and MONTH(tblitemtrans.ITDate)='"+month+"' ";
        QSqlQuery infoSum(querySum);
        while(infoSum.next())
        {
            int Qty = infoSum.value("Qty").toInt();
            if(infoSum.value("packingUnit").toString()==masterpackingunitid)
            {
                addQty = addQty + (sp * mp * Qty);
            }
            else if(infoSum.value("packingUnit").toString()==packpermasterunitid)
            {
                addQty = addQty + (sp * Qty);
            }
            else if(infoSum.value("packingUnit").toString()==pcsperpackunitid)
            {
                addQty = addQty + Qty;
            }
        }
        int remQty = addQty;
        if(packing==false)
        {
            return this->intString(remQty);
        }

        if(remQty==0 || sp==0 || mp==0)
        {
            return this->intString(remQty);
        }
        int lpQty = remQty/(sp * mp);

        remQty = remQty - (lpQty * (sp * mp));

        QString stock = this->intString(lpQty) +"-"+ this->unitName(masterpackingunitid)+"  ";
        //qDebug()<<stock<<endl;
        if(packpermasterunitid.toInt()!=0)
        {
            int mpQty = remQty/sp;
            remQty = remQty - (mpQty * sp);
            stock = stock + this->intString(mpQty) +"-"+ this->unitName(packpermasterunitid)+"  ";
            //qDebug()<<stock<<endl;
        }
        if(pcsperpackunitid.toInt()!=0)
        {
            int spQty = remQty;
            stock = stock + this->intString(spQty) +"-"+ this->unitName(pcsperpackunitid);
            //qDebug()<<stock<<endl;
        }

        return stock;
    }

/****************************************************************************************/
    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *  This Functions Converts customer's Item wise Sale in SMALLEST UNITS
     *
     *  INPUT : Item Id, customer id, From Date, To Date
     *  OUTPUT: Stock in Smallest Unit/Packing...
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    */
    float saleAndPurchasePrice(QString transType,QString itemID,QString year,QString month)
    {
        QSqlQuery Itmrow("select * from tblitems where itemid = '"+itemID+"' Limit 0,1");
        Itmrow.next();

        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        if(sp==0){sp=1;}
        if(mp==0){mp=1;}
        float total=0;
        QString querySum = "SELECT * FROM tblitemtransdet, tblitemtrans  WHERE tblitemtrans.ITransID "
                           " = tblitemtransdet.ITransID AND tblitemtrans.ITransType = tblitemtransdet.ITransType "
                           " AND tblitemtransdet.ITransType in('"+transType+"') AND itemid ='"+itemID+"' "
                           " AND YEAR(tblitemtrans.ITDate)='"+year+"' and MONTH(tblitemtrans.ITDate)='"+month+"' ";
        QSqlQuery infoSum(querySum);
        while(infoSum.next())
        {
            total = total + (infoSum.value("ItemPriceEach").toFloat() * infoSum.value("Qty").toInt());
        }
        return total;
    }

/****************************************************************************************/
    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *  This Functions Converts customer's Item wise Sale in SMALLEST UNITS
     *
     *  INPUT : Item Id, customer id, From Date, To Date
     *  OUTPUT: Sum of All Qty of that particular TransType in Smallest Unit/Packing...
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    */
    QString checkQty(QString itemID,QString transType,bool packing=true,QString locationId="",QString begDate="",QString endDate="",QString process_id="")
    {
        QSqlQuery Itmrow("select * from tblitems where itemid = '"+itemID+"' Limit 0,1");
        Itmrow.next();

        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        if(sp==0){sp=1;}
        if(mp==0){mp=1;}
        QString masterpackingunitid = Itmrow.value("masterpackingunitid").toString();
        QString packpermasterunitid = Itmrow.value("packpermasterunitid").toString();
        QString pcsperpackunitid = Itmrow.value("pcsperpackunitid").toString();

        int addQty=0;
        //int subQty=0;

        QString querySum = "SELECT * FROM tblitemtransdet, tblitemtrans "
                           " WHERE tblitemtrans.ITransID = tblitemtransdet.ITransID "
                           " AND tblitemtrans.ITransType = tblitemtransdet.ITransType "
                           " AND tblitemtransdet.ITransType in('"+transType+"') "
                           " AND itemid ='"+itemID+"' ";
        if(locationId != "")
        {
            querySum = querySum + " AND tblitemtransdet.locationid = '"+locationId+"' ";
        }
        if(process_id!="")
        {
                querySum +=" and tblitemtransdet.process_id = '"+process_id+"' " ;
        }
        if(begDate != "" && endDate != ""){
            querySum = querySum + " AND tblitemtrans.ITDate BETWEEN '"+begDate+"' AND '"+endDate+"'";
        }
        QSqlQuery infoSum(querySum);
        while(infoSum.next())
        {
            int Qty = infoSum.value("Qty").toInt();
            addQty += Qty;
        }

        int remQty = addQty;
        if(packing==false)
        {
            return this->intString(remQty);
        }

        if(remQty==0 || sp==0 || mp==0)
        {
            return this->intString(remQty);
        }
        int lpQty = remQty/(sp * mp);

        remQty = remQty - (lpQty * (sp * mp));

        QString stock = this->intString(lpQty) +"-"+ this->unitName(masterpackingunitid)+"  ";
        //qDebug()<<stock<<endl;
        if(packpermasterunitid.toInt()!=0)
        {
            int mpQty = remQty/sp;
            remQty = remQty - (mpQty * sp);
            stock = stock + this->intString(mpQty) +"-"+ this->unitName(packpermasterunitid)+"  ";
            //qDebug()<<stock<<endl;
        }
        if(pcsperpackunitid.toInt()!=0)
        {
            int spQty = remQty;
            stock = stock + this->intString(spQty) +"-"+ this->unitName(pcsperpackunitid);
            //qDebug()<<stock<<endl;
        }

        return stock;
    }

/****************************************************************************************/
    float checkQtyAmount(QString itemID,QString transType,QString locationId="",QString begDate="",QString endDate="",QString process_id="")
    {
        QSqlQuery Itmrow("select * from tblitems where itemid = '"+itemID+"' Limit 0,1");
        Itmrow.next();

        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        if(sp==0){sp=1;}
        if(mp==0){mp=1;}
        float total=0;
        QString querySum = "SELECT * FROM tblitemtransdet, tblitemtrans "
                           " WHERE tblitemtrans.ITransID = tblitemtransdet.ITransID "
                           " AND tblitemtrans.ITransType = tblitemtransdet.ITransType "
                           " AND tblitemtransdet.ITransType in('"+transType+"') "
                           " AND itemid ='"+itemID+"' ";
        if(locationId != "")
        {
            querySum = querySum + " AND tblitemtransdet.locationid = '"+locationId+"' ";
        }
        if(process_id!="")
        {
                querySum +=" and tblitemtransdet.process_id = '"+process_id+"' " ;
        }
        if(begDate != "" && endDate != ""){
            querySum = querySum + " AND tblitemtrans.ITDate BETWEEN '"+begDate+"' AND '"+endDate+"'";
        }
        QSqlQuery infoSum(querySum);
        while(infoSum.next())
        {
            total = total + (infoSum.value("ItemPriceEach").toFloat() * infoSum.value("Qty").toInt());
        }
        return total;
    }

/****************************************************************************************/
    float SaleTypeSalesDated(QString date,QString saletype)
    {
        QString query=" select sum(id.qty*id.priceach) as totalAmount,sum(i.freight) as fri, sum(i.totalDiscount) "
                      " as discount, sum(i.extraDiscount) as extraDiscount from tblinvoice i, tblinvoicedetail "
                      " id where i.invoicenumber=id.invoiceid and i.invoicedate='"+date+"' and "
                      " i.sales_type='"+saletype+"' group by i.sales_type, i.invoicedate";
        QSqlQuery info(query);
        info.next();

        QString query1="select sum(i.freight) as fri, sum(i.totalDiscount) as discount, sum(i.extraDiscount) as "
                       " extraDiscount from tblinvoice i where i.invoicedate='"+date+"' "
                       " and i.sales_type='"+saletype+"' group by i.sales_type, i.invoicedate";
        QSqlQuery info1(query1);
        info1.next();
        float totalAmount = info.value("totalAmount").toFloat()+info1.value("fri").toFloat()-info1.value("discount").toFloat()-info1.value("extraDiscount").toFloat();
        return totalAmount;
    }

/****************************************************************************************/
    float ReturnSalesDated(QString date)
    {
        QString query=" select sum(rd.itemqtyorder*rd.itempriceach) AS totalReturn from  tblreturn r, tblreturndetail "
                      " rd WHERE r.id=rd.orderid and r.retdate='"+date+"' group by r.retdate ";
        QSqlQuery info(query);
        info.next();
        return info.value("totalReturn").toFloat();
    }
/****************************************************************************************/
    float RecoveryDated(QString date)
    {
        QString query=" select sum(id.amount) AS totalRecovery from tblinvoicepaymentdetail id WHERE "
                      " id.datereceived='"+date+"' and paymeth='1' group by id.datereceived ";
        QSqlQuery info(query);
        info.next();
        return info.value("totalRecovery").toFloat();
    }
/****************************************************************************************/
    float PdcRecoveryDated(QString date)
    {
        QString query=" select sum(ed.amount) AS totalPdcRecovery from tblencashpaymentdetail ed WHERE "
                      " ed.datereceived='"+date+"' group by ed.datereceived ";
        QSqlQuery info(query);
        info.next();
        return info.value("totalPdcRecovery").toFloat();
    }
/****************************************************************************************/
    float CustomerReturnSalesDated(QString customerid,QString begindate,QString enddate)
    {
        QString query=" select sum(rd.itemqtyorder*rd.itempriceach) AS totalReturn from  tblreturn r, tblreturndetail "
                      " rd WHERE r.id=rd.orderid and r.customer_id='"+customerid+"' and r.retdate >='"+begindate+"' "
                      " and r.retdate <='"+enddate+"' group by r.customer_id ";
        QSqlQuery info(query);
        info.next();
        return info.value("totalReturn").toFloat();
    }
/****************************************************************************************/
    int stockCalculationSmallPacket(QString itemID,QString locationId="",QString entDate="")
    {
        QSqlQuery Itmrow("select * from tblitems where itemid = '"+itemID+"' ");
        Itmrow.next();
        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        //if(sp==0){ sp=1; }
        //if(mp==0){ mp=1; }
        int addQty=0;
        int subQty=0;
        QString masterpackingunitid = Itmrow.value("masterpackingunitid").toString();
        QString packpermasterunitid = Itmrow.value("packpermasterunitid").toString();
        QString pcsperpackunitid = Itmrow.value("pcsperpackunitid").toString();
        QString whr = "";
        QString querySum;
        if(locationId!=""){ whr=" and locationid ='"+locationId+"' "; }
        if(entDate=="")
        {
            querySum = " select * from tblitemtransdet where caltype='+' and itemid ='"+itemID+"' "+whr;
        }
        else
        {
            querySum = "select * from tblitemtransdet "
                       " INNER JOIN tblitemtrans "
                       " ON (tblitemtrans.ITransID = tblitemtransdet.ITransID AND tblitemtrans.ITransType = "
                       " tblitemtransdet.ITransType) where caltype='+' "
                       " AND tblitemtrans.ITDate <= '"+entDate+"' and itemid ='"+itemID+"' "+whr;
        }
        QSqlQuery infoSum(querySum);
        int QtySum=0;
        while(infoSum.next())
        {
            QString sumunit = infoSum.value("packingUnit").toString();
            QtySum = infoSum.value("Qty").toInt();
            //qDebug()<<this->intString(QtySum)+" :) "<<endl;
            if(sumunit==masterpackingunitid)
            {
                addQty += (sp * mp * QtySum);
                //qDebug()<<this->intString(addQty)+" :) sum loop1 "<<endl;
            }
            if(sumunit==packpermasterunitid)
            {
                addQty += (sp * QtySum);
                //qDebug()<<this->intString(addQty)+" :) sum loop2 "<<endl;
            }
            if(sumunit==pcsperpackunitid)
            {
                addQty += QtySum;
                //qDebug()<<this->intString(addQty)+" :) sum loop3 "<<endl;
            }
        }
        //qDebug()<<this->intString(addQty)+" :) Sum "+masterpackingunitid<<endl;
        QString querySub;
        if(entDate=="")
        {
            querySub = " select * from tblitemtransdet where caltype='-' and itemid ='"+itemID+"' "+whr;
        }
        else
        {
            querySub = "select * from tblitemtransdet "
                       " INNER JOIN tblitemtrans "
                       " ON (tblitemtrans.ITransID = tblitemtransdet.ITransID AND tblitemtrans.ITransType = "
                       " tblitemtransdet.ITransType) where caltype='-' "
                       " AND tblitemtrans.ITDate <= '"+entDate+"' and itemid ='"+itemID+"' "+whr;
        }
        QSqlQuery infoSub(querySub);
        while(infoSub.next())
        {
            QString subunit = infoSub.value("packingUnit").toString();
            int QtySub = infoSub.value("Qty").toInt();
            if(subunit==masterpackingunitid)
            {
                subQty = subQty + (sp * mp * QtySub);
            }
            if(subunit==packpermasterunitid)
            {
                subQty = subQty + (sp * QtySub);
            }
            if(subunit==pcsperpackunitid)
            {
                subQty = subQty + QtySub;
            }
        }
        //qDebug()<<this->intString(subQty)+" :) Sub "<<endl;
        int remQty = addQty-subQty;
        return remQty;
    }
/****************************************************************************************/
    QString stockCalculation(QString itemID,QString locationId="",QString entDate="",bool packing=true)
    {
        QSqlQuery Itmrow("select * from tblitems where itemid = '"+itemID+"' ");
        Itmrow.next();
        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        //if(sp==0){sp=1;}
        //if(mp==0){mp=1;}
        int addQty=0;
        int subQty=0;
        QString masterpackingunitid = Itmrow.value("masterpackingunitid").toString();
        QString packpermasterunitid = Itmrow.value("packpermasterunitid").toString();
        QString pcsperpackunitid = Itmrow.value("pcsperpackunitid").toString();
        QString whr = "";
        QString querySum;
        if(locationId!=""){whr=" and locationid ='"+locationId+"' ";}
        if(entDate=="")
        {
            querySum = " select * from tblitemtransdet where caltype='+' and itemid ='"+itemID+"' "+whr;
        }
        else
        {
            querySum = "select * from tblitemtransdet "
                       " INNER JOIN tblitemtrans "
                       " ON (tblitemtrans.ITransID = tblitemtransdet.ITransID AND tblitemtrans.ITransType = "
                       " tblitemtransdet.ITransType) where caltype='+' "
                       " AND tblitemtrans.ITDate <= '"+entDate+"' and itemid ='"+itemID+"' "+whr;
        }
        QSqlQuery infoSum(querySum);
        while(infoSum.next())
        {
            QString sumunit = infoSum.value("packingUnit").toString();
            if(sumunit==masterpackingunitid)
            {
                addQty = addQty + (sp * mp * infoSum.value("Qty").toInt());
            }
            if(sumunit==packpermasterunitid)
            {
                addQty = addQty + (sp * infoSum.value("Qty").toInt());
            }
            if(sumunit==pcsperpackunitid)
            {
                addQty = addQty + infoSum.value("Qty").toInt();
            }
        }
        QString querySub;
        if(entDate=="")
        {
            querySub = " select * from tblitemtransdet where caltype='-' and itemid ='"+itemID+"' "+whr;
        }
        else
        {
            querySub = "select * from tblitemtransdet "
                       " INNER JOIN tblitemtrans "
                       " ON (tblitemtrans.ITransID = tblitemtransdet.ITransID AND tblitemtrans.ITransType = "
                       " tblitemtransdet.ITransType) where caltype='-' "
                       " AND tblitemtrans.ITDate <= '"+entDate+"' and itemid ='"+itemID+"' "+whr;
        }
        QSqlQuery infoSub(querySub);
        while(infoSub.next())
        {
            QString subunit = infoSub.value("packingUnit").toString();
            if(subunit==masterpackingunitid)
            {
                subQty = subQty + (sp * mp * infoSub.value("Qty").toInt());
            }
            if(subunit==packpermasterunitid)
            {
                subQty = subQty + (sp * infoSub.value("Qty").toInt());
            }
            if(subunit==pcsperpackunitid)
            {
                subQty = subQty + infoSub.value("Qty").toInt();
            }
        }
        int remQty = addQty-subQty;
        if(packing==false)
        {
            return this->intString(remQty);
        }
        else
        {
            if(remQty==0 || sp==0 || mp==0)
            {
                return this->intString(remQty);
            }
            else
            {
                QString stock;
                int lpQty = remQty/(sp * mp);
                remQty = remQty - (lpQty * (sp * mp));
                stock = this->intString(lpQty) +" "+this->unitName(masterpackingunitid)+"  ";
                if(packpermasterunitid!="0")
                {
                    int mpQty = remQty/sp;
                    remQty = remQty - (mpQty * sp);
                    stock = stock + this->intString(mpQty) +" "+this->unitName(packpermasterunitid)+"  ";
                }
                if(pcsperpackunitid!="0")
                {
                    int spQty = remQty;
                    stock = stock + this->intString(spQty) +" "+this->unitName(pcsperpackunitid);
                }
                return stock;
            }
        }
    }
/****************************************************************************************/
    QString showItemType(QString id)
    {
        QString sql = " select `type` from  tbl_prodcosttype  where id=:id " ;
        QSqlQuery qry;
        qry.prepare(sql);
        qry.bindValue(":id", id);
        qry.exec();
        qry.next();
        return qry.value(0).toString();
    }
/****************************************************************************************/
    float formulaCost(QString itemid,QString entryDate,QString costFormula, int currentStock)
    {
        int CuStock=currentStock;
        float calculatedPrice=0.00;
        if(costFormula=="First")
        {
            float firstCost=0;
            QString query= "select Min(tblitemtrans.ID) minID,tblitemtrans.ITransID ITID from tblitemtrans "
                           " inner join tblitemtransdet on (tblitemtrans.ITransID = tblitemtransdet.ITransID "
                           " and tblitemtrans.ITransType = tblitemtransdet.ITransType) where "
                           " tblitemtransdet.itemID ='"+itemid+"' and tblitemtrans.ITDate <= '"+entryDate+"' "
                           " and tblitemtransdet.ITransType in ('OS','IS','PI','PD') ";
            QSqlQuery firstRow(query);
            firstRow.next();
            QString minID = firstRow.value("minID").toString();
            QSqlQuery QryRow(" select ITransID from tblitemtrans where ID='"+minID+"' ");
            QryRow.next();
            QString QryItransID = QryRow.value("ITransID").toString();
            QString queryCost = "select * from tblitemtransdet where ITransID='"+QryItransID+"' and "
                                " itemID ='"+itemid+"' and ITransType in ('OS','IS','PI','PD') ";
            QSqlQuery costRow(queryCost);
            costRow.next();
            firstCost = costRow.value("ItemPriceEach").toFloat();
            calculatedPrice = firstCost;
        }
        else if(costFormula=="Last")
        {
            QString query= "select MAX(tblitemtrans.ID) maxID,tblitemtrans.ITransID ITID from tblitemtrans "
                           " inner join tblitemtransdet on (tblitemtrans.ITransID = tblitemtransdet.ITransID "
                           " and tblitemtrans.ITransType = tblitemtransdet.ITransType) where "
                           " tblitemtransdet.itemID ='"+itemid+"' and tblitemtrans.ITDate <= '"+entryDate+"' "
                           " and tblitemtransdet.ITransType in ('OS','IS','PI','PD') ";
            QSqlQuery firstRow(query);
            firstRow.next();
            QString maxID = firstRow.value("maxID").toString();
            QSqlQuery QryRow(" select ITransID from tblitemtrans where ID='"+maxID+"' ");
            QryRow.next();
            QString QryItransID = QryRow.value("ITransID").toString();
            QString queryCost = "select * from tblitemtransdet where ITransID='"+QryItransID+"' and "
                                " itemID ='"+itemid+"' and ITransType in ('OS','IS','PI','PD') ";
            QSqlQuery costRow(queryCost);
            costRow.next();
            calculatedPrice = costRow.value("ItemPriceEach").toFloat();
        }
        else if(costFormula=="Weighted")
        {
            float value=0;
            QString poQry = "select tblitemtransdet.itemID poItem,tblitemtransdet.qty poQty,"
                            " tblitemtransdet.ItemPriceEach poPrice, tblitemtransdet.packingUnit poUnit from "
                            " tblitemtrans inner join tblitemtransdet on (tblitemtrans.ITransID = "
                            " tblitemtransdet.ITransID  and tblitemtrans.ITransType = tblitemtransdet.ITransType) "
                            " where tblitemtransdet.itemID ='"+itemid+"' and tblitemtrans.ITDate <= '"+entryDate+"' "
                            " and tblitemtransdet.ITransType in ('OS','IS','PI','PD') "
                            " order by tblitemtrans.ITDate desc, tblitemtransdet.ID desc";
            QSqlQuery poRow(poQry);
            while(poRow.next())
            {
                if(currentStock <= this->stockInSmallPacket(itemid,poRow.value("poQty").toInt(),poRow.value("poUnit").toString() ) )
                {
                    if(currentStock>0)
                    {
                        value = value + ( (currentStock/this->total_pcs(itemid)) * poRow.value("poPrice").toFloat() );
                    }
                    else
                    {
                        value = value + ( poRow.value("poQty").toInt() * poRow.value("poPrice").toFloat() );
                    }
                }
                else
                {
                    currentStock = currentStock - this->stockInSmallPacket(itemid,poRow.value("poQty").toInt(),poRow.value("poUnit").toString() );
                    value = value + ( poRow.value("poQty").toInt() * poRow.value("poPrice").toFloat() );
                }
            }
            if(CuStock>0)
            {
                if(value>0)
                {
                    calculatedPrice = (value/CuStock);
                }
                else
                {
                    calculatedPrice = value;
                }
            }
            else
            {
                calculatedPrice = 0.00;
            }
        }
        return calculatedPrice;
    }
/****************************************************************************************/
    int stockInSmallPacket(QString itemID,int Qty,QString packingUnit,QString locationId="")
    {
        QString Itmquery = "select masterpackingunitid, packpermasterunitid, pcsperpackunitid, pcsperpack, "
                           " packpermaster, locationid  from tblitems where itemid = '"+itemID+"' Limit 0,1";
        QSqlQuery Itmrow(Itmquery);
        Itmrow.next();
        if(packingUnit=="" || packingUnit=="0")
        {
            return Qty;
        }
        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        int qty_in_smallest_unit=0;
        QString whr = "";
        if(locationId!=""){ whr = " and locationid ='"+locationId+"'"; };
        if(packingUnit==Itmrow.value("masterpackingunitid").toString())
        {
            qty_in_smallest_unit = qty_in_smallest_unit + (sp * mp * Qty);
        }
        else if(packingUnit==Itmrow.value("packpermasterunitid").toString())
        {
            qty_in_smallest_unit = qty_in_smallest_unit + (sp * Qty);
        }
        else if(packingUnit==Itmrow.value("pcsperpackunitid").toString())
        {
            qty_in_smallest_unit = qty_in_smallest_unit + Qty;
        }
        return qty_in_smallest_unit;
    }
/****************************************************************************************/
    int total_pcs(QString itemID)
    {
        QString Itmquery = "select * from tblitems where itemid = '"+itemID+"' Limit 0,1";
        QSqlQuery Itmrow(Itmquery);
        Itmrow.next();
        int total_pcs = Itmrow.value("pcsperpack").toInt() * Itmrow.value("packpermaster").toInt();
        return total_pcs;
    }
/****************************************************************************************/
    QString openingStockCalculation(QString itemID,QString locationId="",QString entDate="",bool packing=true)
    {
        QSqlQuery Itmrow("select * from tblitems where itemid = '"+itemID+"' Limit 0,1");
        Itmrow.next();
        int sp = Itmrow.value("pcsperpack").toInt();
        int mp = Itmrow.value("packpermaster").toInt();
        int addQty=0;
        int subQty=0;
        QString masterpackingunitid = Itmrow.value("masterpackingunitid").toString();
        QString packpermasterunitid = Itmrow.value("packpermasterunitid").toString();
        QString pcsperpackunitid = Itmrow.value("pcsperpackunitid").toString();
        QString whr="";
        QString querySum="";
        QString querySub="";
        if(locationId!=""){ whr=" and locationid ='"+locationId+"' "; }
        if(entDate=="")
        {
            querySum = "select * from tblitemtransdet where caltype='+' and itemid ='"+itemID+"' "+whr;
        }
        else
        {
            querySum = "select * from tblitemtransdet INNER JOIN tblitemtrans ON "
                       " (tblitemtrans.ITransID = tblitemtransdet.ITransID AND "
                       " tblitemtrans.ITransType = tblitemtransdet.ITransType) where caltype='+' "
                       " AND tblitemtrans.ITDate < '"+entDate+"' and itemid ='"+itemID+"'"+whr;
        }
        QSqlQuery infoSum(querySum);
        while(infoSum.next())
        {
            QString packingUnit = infoSum.value("packingUnit").toString();
            int Qty = infoSum.value("Qty").toInt();
            if(packingUnit==masterpackingunitid)
            {
                addQty += (sp * mp * Qty);
            }
            if(packingUnit==packpermasterunitid)
            {
                addQty += (sp * Qty);
            }
            if(packingUnit==packpermasterunitid)
            {
                addQty += Qty;
            }
        }
        if(entDate=="")
        {
            querySub = "select * from tblitemtransdet where caltype='-' and itemid ='"+itemID+"'"+whr;
        }
        else
        {
            querySub = "select * from tblitemtransdet INNER JOIN tblitemtrans ON (tblitemtrans.ITransID = "
                       " tblitemtransdet.ITransID AND tblitemtrans.ITransType = tblitemtransdet.ITransType) "
                       " where caltype='-' AND tblitemtrans.ITDate < '"+entDate+"' and itemid ='"+itemID+"'"+whr;
        }
        QSqlQuery infoSub(querySub);
        while(infoSub.next())
        {
            QString packingUnit = infoSub.value("packingUnit").toString();
            int Qty = infoSub.value("Qty").toInt();
            if(packingUnit==masterpackingunitid)
            {
                subQty += (sp * mp * Qty);
            }
            if(packingUnit==packpermasterunitid)
            {
                subQty += (sp * Qty);
            }
            if(packingUnit==packpermasterunitid)
            {
                subQty += Qty;
            }
        }
        QString stock="";
        int remQty = addQty-subQty;
        if(packing==false)
        {
            return this->intString(remQty);
        }
        else
        {
            if(remQty==0 || sp==0 || mp==0)
            {
                return this->intString(remQty);
            }
            else
            {
                int lpQty = remQty/(sp * mp);
                remQty = remQty - (lpQty * (sp * mp));
                stock = stock+this->intString(lpQty)+" "+this->unitName(masterpackingunitid)+" ";
                if(packpermasterunitid.toInt()!=0)
                {
                    int mpQty = remQty/sp;
                    remQty = remQty - (mpQty * sp);
                    stock = stock+this->intString(mpQty)+" "+this->unitName(packpermasterunitid);
                }
                if(pcsperpackunitid.toInt()!=0)
                {
                    int spQty = remQty;
                    stock = stock+this->intString(spQty)+" "+this->unitName(pcsperpackunitid);
                }
                return stock;
            }
        }
    }
/****************************************************************************************/
    float DayBookLedgerbalance(QString glaccountid , QString companyid , QString enddate)
    {
        QString ar = this->getaccountAR("receivables");
        QString ap = this->getaccountAP("payable");
        float balance=0;
        if(glaccountid==ar)
        {
            QString query="select (sum(debit)-sum(credit)) as balance from tblgltransaction t, tblgltransvoucher v "
                          " where t.voucherno=v.voucherno and t.voucherid = v.voucherid and  t.companytype='customer' "
                          " and t.compid='"+companyid+"' and v.entrydate <='"+enddate+"' ";
            QSqlQuery info(query);
            info.next();
            balance = info.value("balance").toFloat();
        }
        else if(glaccountid==ap)
        {
            QString query="select (sum(credit)-sum(debit)) as balance from tblgltransaction t, tblgltransvoucher v "
                          " where t.voucherno=v.voucherno and t.voucherid = v.voucherid and  t.companytype='vendor' "
                          " and t.compid='"+companyid+"' and v.entrydate <='"+enddate+"' ";
            QSqlQuery info(query);
            info.next();
            balance = info.value("balance").toFloat();
        }
        else
        {
            QString bankid = this->getAccountGlStd("bank");
            QSqlQuery row_acct(" select * from tblglaccount where summaryaccountid='"+bankid+"' and id='"+glaccountid+"' ");
            if(row_acct.size()>0)
            {
                QString query_bal="select (sum(debit)-sum(credit)) as balance from tblgltransvoucher v, "
                                  " tblgltransaction r, tblglaccount a where r.glaccountid = a.id and "
                                  " r.voucherno = v.voucherno and v.entrydate <='"+enddate+"' and "
                                  " a.id = '"+glaccountid+"'";
                QSqlQuery info_bal(query_bal);
                info_bal.next();
                balance = info_bal.value("balance").toFloat();
            }
            else
            {
                balance = 0;
            }
        }
        return balance;
    }
/****************************************************************************************/
    float calculateStockWeighted(QString endDate)
    {
        QString query = "SELECT tblitems.itemid as itemid, itemcode, description, lbsperpriceunit, tblitems.priceunit as "
                        " priceunit, categoryid, price1, itemtype, tblitems.locationid, masterpackingunitid, packpermasterunitid, "
                        " pcsperpackunitid, pcsperpack, packpermaster FROM tblitems WHERE dl <> 1 ";
        query += " Group By tblitems.itemid order by description asc ";
        //qDebug()<<query<<endl;
        float tot;
        bool zeroStock=false;
        bool negitiveStock=true;
        QSqlQuery info(query);
        while(info.next())
        {
            QString itemid = info.value("itemid").toString();
            int currentStock = this->stockCalculationSmallPacket(itemid, "", endDate);
            if(
                    (zeroStock==true && negitiveStock==true)
                    ||
                    (zeroStock==true && negitiveStock==false && currentStock>=0)
                    ||
                    (zeroStock==false && negitiveStock==true && currentStock!=0)
                    ||
                    (zeroStock==false && negitiveStock==false && currentStock>0)
              )
            {
                //QString stokInHand = erp.stockCalculation(itemid,"",endDate);
                float sotckValue=0;
                //int CuStock = currentStock;
                float calculatedPrice = this->formulaCost(itemid,endDate,"Weighted",currentStock);
                if(calculatedPrice>0 && currentStock>0)
                {
                    sotckValue = calculatedPrice * currentStock;
                    tot += sotckValue;
                }
            }//end if
        }//end while
        return tot;
    }
/***************************************************************************************/
    float calculateProfitAndLossToDate(QString entDate)
    {
        QString y = entDate.split(QRegExp("-")).value(0);
        QString m = entDate.split(QRegExp("-")).value(1);
        if(m.toInt() < 8)
        {
            y = QString::number(y.toInt()-1);
        }
        QString YTD=y+"-08-01";
        QSqlQuery info_inventory("select inventory from tblinvcompany where id='1' ");
        info_inventory.next();
        QString glacct = info_inventory.value("inventory").toString();
        QString query_ytdStock="select sum( debit - credit ) as Ytd_OS from tblgltransaction t, tblgltransvoucher tv  where t.voucherno=tv.voucherno and t.glaccountid='"+glacct+"' and tv.entrydate < '"+YTD+"' ";
        QSqlQuery info_ytdStock(query_ytdStock);
        float YtdOpeningStock = info_ytdStock.value("Ytd_OS").toFloat();
        QString query_closingStock	= "select sum(debit-credit) as closing_stock from tblgltransaction t, tblgltransvoucher tv  where t.voucherno=tv.voucherno and t.glaccountid='"+glacct+"' and tv.entrydate <= '"+entDate+"' ";
        QSqlQuery info_closingStock(query_closingStock);
        info_closingStock.next();
        float closingStock = info_closingStock.value("closing_stock").toFloat();
        QString query="select sum(t.debit) as debit, sum(t.credit) as credit,accountypeid  from tblglaccount a, tblgltransaction t , tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and tr.entrydate >= '"+YTD+"' and tr.entrydate <= '"+entDate+"' and accountypeid in (50,70,80,90) group by accountypeid";
        QSqlQuery info(query);
        float cost_debit=0;
        float cost_credit=0;
        float cost=0;
        float total_cost=0;
        float expense_debit=0;
        float expense_credit=0;
        float expense=0;
        float total_expense=0;
        float other_income_expense_debit=0;
        float other_income_expense_credit=0;
        float other_income_expense=0;
        float total_other_income_expense=0;
        float sales_debit=0;
        float sales_credit=0;
        float sales=0;
        while(info.next())
        {
            QString accountypeid = info.value("accountypeid").toString();
            float debit = info.value("debit").toFloat();
            float credit = info.value("credit").toFloat();
            if(accountypeid== "70")  //cost of goods sold
            {
                cost_debit=debit;
                cost_credit=credit;
                cost=cost_debit-cost_credit;
                total_cost +=cost ;
            }
            if(accountypeid=="80" )  //expnses
            {
                expense_debit=debit;
                expense_credit=credit;
                expense=expense_debit-expense_credit;
                total_expense +=expense;
            }
            if(accountypeid=="90") //other income and expense
             {
                other_income_expense_debit=debit;
                other_income_expense_credit=credit;

                other_income_expense = other_income_expense_credit - other_income_expense_debit;
                total_other_income_expense +=other_income_expense;
             }
          if(accountypeid== "50")  // sales
            {
                sales_debit=debit;
                sales_credit=credit;
                sales=sales_credit-sales_debit;
            }
        }
        total_cost = total_cost + YtdOpeningStock - closingStock;
        float totalYTD=sales-total_cost-total_expense+total_other_income_expense;
        return totalYTD;
    }
/***************************************************************************************/
    bool checkRights(QString action)
    {
        QSqlQuery qry(" select * from tblmenue where type = 'child' and `action` = '"+action+"' ");
        qry.next();
        QString menuid = qry.value("id").toString();
        QSqlQuery info(" select * from rights where menuid = '"+menuid+"' and userid = '"+this->loginid+"' ");
        if(info.size()>0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
/***************************************************************************************/
    bool checkSubMenuRights(QString action)
    {
        QSqlQuery qry(" select * from tblmenue where type = 'sub' and `action` = '"+action+"' ");
        qry.next();
        QString menuid = qry.value("id").toString();
        QSqlQuery info(" select * from rights where menuid in( select id from tblmenue where parent_id = '"+menuid+"' ) and userid = '"+this->loginid+"' ");
        if(info.size()>0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
/***************************************************************************************/
    bool checkMainMenuRights(QString action)
    {
        QSqlQuery qry(" select * from tblmenue where type = 'main' and `action` = '"+action+"' ");
        qry.next();
        QString menuid = qry.value("id").toString();
        QSqlQuery info(" select * from rights where menuid in( select id from tblmenue where parent_id in( select id from tblmenue where parent_id = '"+menuid+"' ) ) and userid = '"+this->loginid+"' ");
        if(info.size()>0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
/***************************************************************************************/
};



#endif // ERP_H
