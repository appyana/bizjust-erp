#include <stdlib.h>
#include <iostream>
#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include <QCryptographicHash>


LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)//,Qt::FramelessWindowHint
{
    ui->setupUi(this);
    mSystemTrayIcon = new QSystemTrayIcon(this);
    mSystemTrayIcon->setIcon(QIcon(":/icon.ico"));
    mSystemTrayIcon->setVisible(true);
    //setStyleSheet("QWidget#LoginWindow{background:transparent;}");
    //setAttribute(Qt::WA_TranslucentBackground);
    //setWindowFlags(Qt::FramelessWindowHint);
    if(!conn.connOpen())
    {
        //Do Something
    }

    QSqlQuery qry;
    qry.prepare(" select * from tblfinancialyear order by yearid asc ");
    if(qry.exec())
    {
        while(qry.next())
        {
            ui->financialyear->addItem(qry.value("title").toString(), QVariant(qry.value("yearid")) );
            if(qry.value("yearstatus").toInt()==1)
            {
                int index = ui->financialyear->findData( qry.value("yearid").toInt() );
                ui->financialyear->setCurrentIndex(index);
            }
        }
    }


    //QPixmap pix("E:/bizjust/logo-cdr-s.png");
    //ui->label_logo->setPixmap(pix);
}

LoginWindow::~LoginWindow()
{
    delete ui;
    conn.connClose();
}

//void LoginWindow::mousePressEvent(QMouseEvent *event) {
//    m_nMouseClick_X_Coordinate = event->x();
//    m_nMouseClick_Y_Coordinate = event->y();
//}

//void LoginWindow::mouseMoveEvent(QMouseEvent *event) {
//    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
//}

void LoginWindow::on_login_btn_clicked()
{

    QString username,password,pass;
    username = ui->username_txt->text();
    pass = ui->password_txt->text();
    password = QString(QCryptographicHash::hash((pass.toUtf8()),QCryptographicHash::Md5).toHex());
    //QMessageBox::information(this,"Login",password);
    //if(!conn.connOpen())
    //{
    //    qDebug()<<"Failed to open the database!";
        //QMessageBox::critical(this,"Error","Invalid Database Connection");
    //    return;
    //}
    //conn.connOpen();
    QString loginid;
    QSqlQuery qry;
    qry.prepare("SELECT * FROM tblusers WHERE adminid = '"+ username +"' AND password = '"+ password +"' and isactive = '1'");
    if(qry.exec())
    {
        int count=0;
        while(qry.next())
        {
            loginid = qry.value(0).toString();
            count++;
        }
        if(count==1)
        {
            QString yearid = ui->financialyear->itemData(ui->financialyear->currentIndex()).toString();
            mainwin = new MainWindow();
            mainwin->loginid = loginid;
            mainwin->erp.loginid = loginid;
            mainwin->_yearid = yearid;
            mainwin->showMaximized();
            close();
        }
        else
        {
            QMessageBox::critical(this,"Error","Invalid Username or Password");
        }

        //conn.connClose();
    }
}

void LoginWindow::on_cancel_btn_clicked()
{
    this->close();
}

