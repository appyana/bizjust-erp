#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include "mainwindow.h"
#include "dbcon.h"
#include <QMouseEvent>
#include <QPoint>
namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    DbCon conn;
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

private slots:
    void on_login_btn_clicked();

    void on_cancel_btn_clicked();

private:
    Ui::LoginWindow *ui;
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    int m_nMouseClick_X_Coordinate;
//    int m_nMouseClick_Y_Coordinate;
    MainWindow *mainwin;
    QSystemTrayIcon *mSystemTrayIcon;
};

#endif // LOGINWINDOW_H
