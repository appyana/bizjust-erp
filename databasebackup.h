#ifndef DATABASEBACKUP_H
#define DATABASEBACKUP_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class DatabaseBackup;
}

class DatabaseBackup : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    void savedb();
    explicit DatabaseBackup(QWidget *parent = 0);
    ~DatabaseBackup();

private slots:
    void on_save_btn_clicked();

private:
    Ui::DatabaseBackup *ui;
};

#endif // DATABASEBACKUP_H
