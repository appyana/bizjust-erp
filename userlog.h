#ifndef USERLOG_H
#define USERLOG_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include <QtCore>
#include <QtGui>

namespace Ui {
class UserLog;
}

class UserLog : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit UserLog(QWidget *parent = 0);
    ~UserLog();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();


    void on_username_textEdited(const QString &arg1);

    void loadForm();

    void on_userid_textChanged(const QString &arg1);

    void on_enddate_userDateChanged(const QDate &date);

    void on_begindate_userDateChanged(const QDate &date);

private:
    Ui::UserLog *ui;
    enum collumns{
        DATE,TIME,EVENT,TRANSACTION,TRANSNO
    };
    QTimer *timer;
    void loadReport();
};

#endif // USERLOG_H
