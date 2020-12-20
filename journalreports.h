#ifndef JOURNALREPORTS_H
#define JOURNALREPORTS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include <QtCore>
#include <QtGui>
#include <QtPrintSupport/qprinter.h>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextDocument>
#include <QPrintPreviewWidget>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPainter>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "comboboxes.h"

namespace Ui {
class JournalReports;
}

class JournalReports : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    ComboBoxes cb;
    explicit JournalReports(QWidget *parent = 0);
    ~JournalReports();

private slots:
    void loadParams();
    void printPreview(QPrinter *p);
    void on_btn_next_clicked();

    void on_wherefrom_currentIndexChanged(int index);

private:
    Ui::JournalReports *ui;
    QString html;
    QTimer *timer;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // JOURNALREPORTS_H
