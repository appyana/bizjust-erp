#ifndef DBCOB_H
#define DBCOB_H

#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QSystemTrayIcon>
class DbCon
{
public:
    QSqlDatabase db;
    void connClose()
    {
        db.close();
        db.removeDatabase(QSqlDatabase::defaultConnection);
    }
    QString dbhost()
    {
        return "localhost";
    }
    QString dbname()
    {
        return "bizjusterp";//mtg
    }
    QString dbuser()
    {
        return "root";
    }
    QString dbpass()
    {
        return "";
    }
    bool connOpen()
    {
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(this->dbhost());
        db.setDatabaseName(this->dbname());
        db.setUserName(this->dbuser());
        db.setPassword(this->dbpass());
        if(!db.open())
        {
            qDebug()<<"Foiled to connect with database";
            return false;
        }
        else
        {
            qDebug()<<"Connected....";
            return true;
        }
    }

    bool execQuery(QString sql)
    {
        QSqlQuery qry;
        qry.prepare(sql);
        if(qry.exec())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

};


#endif // DBCOB_H
