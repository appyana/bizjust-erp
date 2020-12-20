#ifndef SCH_H
#define SCH_H

#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QObject>
#include <QCompleter>

class SCH
{

public:
    QString searchname;
    QString searchid;
    QLineEdit *name_txt;
    QLineEdit *id_txt;
    QMap<int, QString> data;

    QString SalesMan(QString str="")
    {
        return " SELECT salesmanid, name from tblsalesman where name like '%"+ str +"%' order by name ASC LIMIT 0,20 ";
    }
    QString CustomerGroups(QString str="")
    {
        return " SELECT groupid, groupname from tblcustomergroups where groupname like '%"+ str +"%' order by groupname ASC LIMIT 0,20 ";
    }
    QString invoiceterms(QString str="")
    {
        return " SELECT id, verbal from tblinvoiceterms where verbal like '%"+ str +"%' order by verbal ASC LIMIT 0,20 ";
    }
    QString vendors(QString str="")
    {
        return " SELECT vendorid, CONCAT(vendorname,' ',firstname,' ',middlename,' ',lastname,' ',city,' ',vendorid) "
               " from tblvendors WHERE (vendorname like '%"+ str +"%' or firstname like '%"+ str +"%' "
               " or vendorid like '%"+str+"%') and dl='0'   LIMIT 0,20 ";
    }
    QString vendorsEdit(QString str="")
    {
        return " SELECT vendorid, CONCAT(vendorname,' ',firstname,' ',middlename,' ',lastname,' ',city,' ',vendorid) "
               " from tblvendors WHERE (vendorname like '%"+ str +"%' or firstname like '%"+ str +"%' "
               " or vendorid like '%"+str+"%')   LIMIT 0,20 ";
    }

    QString customers(QString str="", QString dl="0")
    {
        return  " select customerid, CONCAT(name,'-',customerid) as dsc "
                " from tblcustomers where (( name like '%" + str +"%' and dl = '" + dl +"' ) or "
                " ( customerid like '%" + str +"%' and dl = '" + dl +"' )) LIMIT 0,20 ";

    }

    QString items(QString str="")
    {
        return  " select itemid, CONCAT(description,'-',itemcode) as dsc "
                " from tblitems where (( description like '%" + str +"%' and dl = '0' and status='1' ) or "
                " ( itemcode like '%" + str +"%' and dl = '0' and status='1' )) LIMIT 0,20 ";

    }

    QString edititems(QString str="")
    {
        return  " select itemid, CONCAT(description,'-',itemcode) as dsc "
                " from tblitems where (( description like '%" + str +"%' ) or "
                " ( itemcode like '%" + str +"%' )) LIMIT 0,20 ";

    }

    QString glaccount_generalize(QString str="")
    {
        return "SELECT id, CONCAT(name,' ',description,'-',id) "
               " FROM tblglaccount WHERE cancel = '0' AND grouptype = 'detail' AND ("
               " description like '%"+str+"%' OR name like '%"+str+"%' "
               ") order by name  LIMIT 0,20 ";
    }

    QString vendorGroup(QString str="")
    {
        return  " SELECT id, CONCAT(group_name,'-',id) as dsc from tbl_vendor_group where group_name like "
                " '%"+ str +"%' OR id like '%"+ str +"%' order by group_name ASC LIMIT 0,20 ";
    }

    QString users(QString str="")
    {
        return  " select * from tblusers where adminid like '%"+str+"%'  or  id like '%"+str+"%' limit 0,20  ";
    }

};

#endif // SCH_H
