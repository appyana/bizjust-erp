#include "databasebackup.h"
#include "ui_databasebackup.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

DatabaseBackup::DatabaseBackup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseBackup)
{
    ui->setupUi(this);
}

DatabaseBackup::~DatabaseBackup()
{
    delete ui;
}

void DatabaseBackup::savedb()
{
    on_save_btn_clicked();
}

void DatabaseBackup::on_save_btn_clicked()
{
    //QString backup="";
    //QString fname = ui->lineEdit->text()+"-"+QDate::currentDate().toString("yyyy-MM-dd")+".sql";
    QString filter = "SQL File (*.sql)";
    QString filename = QFileDialog::getSaveFileName(this,"Save Database file",QDir::homePath() , filter );
    QFile file(filename);

    QString sqlBackup="";
    QSqlQuery qrytbl(" SHOW TABLES ");
    while(qrytbl.next())
    {
        QString tblname = qrytbl.value(0).toString();
        //stream << "DROP TABLE IF EXISTS `"+tblname+"`;" << endl;
        sqlBackup += "DROP TABLE IF EXISTS `"+tblname+"`;\n\n";
        QSqlQuery tblcreate("SHOW CREATE TABLE `"+tblname+"`");
        tblcreate.next();
        //stream << tblcreate.value(1).toString()+";" << endl;
        sqlBackup += tblcreate.value(1).toString()+";\n\n";
        QSqlQuery qry("SELECT * FROM `"+tblname+"`");
        while(qry.next())
        {
            QString sqlinsert="INSERT INTO `"+tblname+"` VALUES (";
            int cols = qry.record().count();
            for(int i=0; i<cols; i++)
            {
                sqlinsert += "'"+erp.s(qry.value(i).toString())+"'";
                if(i<(cols-1)){ sqlinsert += ","; }
            }
            sqlinsert += "); ";
            //stream << sqlinsert << endl;
            sqlBackup += sqlinsert+"\n";
        }
        sqlBackup += "\n\n";
    }
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << sqlBackup << endl;
        file.close();
    }
        //QMessageBox::information(this,"","Backup file "+filename+" Save successfully to software location.");
}
