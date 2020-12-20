#ifndef COMBOBOXES_H
#define COMBOBOXES_H

#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QTime>
#include <QDate>
#include <QLabel>

class ComboBoxes
{

public:
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

    void loadItemTypesToComboBox(QComboBox *combo,bool select=true)
    {
        combo->clear();
        if(select==true)
        {
            combo->addItem("Select", QVariant(0));
        }
        QSqlQuery qry;
        QString sql = " select id, type from tbl_prodcosttype order by id asc ";
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

    void loadItemCategoriesToComboBox(QComboBox *combo,bool select=true)
    {
        combo->clear();
        if(select==true)
        {
            combo->addItem("Select", QVariant(0));
        }
        QSqlQuery qry;
        QString sql = " select id, name from tblitemcategory order by name asc ";
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

    void loadItemSubCategoriesToComboBox(QComboBox *combo,bool select=true,QString catid="0")
    {
        combo->clear();
        if(select==true)
        {
            combo->addItem("Select", QVariant(0));
        }
        QSqlQuery qry;
        QString sql = " select id, name from pro_sub_cat WHERE catid = '"+catid+"' order by name asc ";
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

    void loadItemBrandToComboBox(QComboBox *combo,bool select=true)
    {
        combo->clear();
        if(select==true)
        {
            combo->addItem("Select", QVariant(0));
        }
        QSqlQuery qry;
        QString sql = " select id, name from pro_item_brand order by name asc ";
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

    void loadItemUnitsToComboBox(QComboBox *combo,bool select=true)
    {
        combo->clear();
        if(select==true)
        {
            combo->addItem("Select", QVariant(0));
        }
        QSqlQuery qry;
        QString sql = " select id, unitname from tblinventoryunitname order by unitname asc ";
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
    void loadUnitWeightToComboBox(QComboBox *combo)
    {
        combo->clear();
        combo->addItem("gm", QVariant("gm"));
        combo->addItem("kg", QVariant("kg"));
    }

    void loadItemStatusToComboBox(QComboBox *combo)
    {
        combo->clear();
        combo->addItem("Active", QVariant(1));
        combo->addItem("In Active", QVariant(0));
    }
    void loadLocationsToComboBox(QComboBox *combo,int top=0)
    {
        combo->clear();
        if(top==1)
        {
            combo->addItem("Select", QVariant(0));
        }
        if(top==2)
        {
            combo->addItem("All", QVariant(0));
        }
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
    void loadAccountTypesToComboBox(QComboBox *combo,int top=0)
    {
        combo->clear();
        if(top==1)
        {
            combo->addItem("Select", QVariant(0));
        }
        if(top==2)
        {
            combo->addItem("All", QVariant(0));
        }
        QSqlQuery qry;
        QString sql = " select id,description from tblaccounttype order by description ASC ";
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
    void loadSummaryAccountToComboBox(QComboBox *combo, QString accounttypeid)
    {
        combo->clear();
        combo->addItem("0 - not summarized", QVariant(0));
        QSqlQuery qry;
        QString sql = " select id,CONCAT(name,'-',description) as dsc from tblglaccount where accountypeid = '"+accounttypeid+"' and cancel='0' and grouptype = 'group' order by name, description ";
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


};

#endif // COMBOBOXES_H
