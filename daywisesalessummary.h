#ifndef DAYWISESALESSUMMARY_H
#define DAYWISESALESSUMMARY_H

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

namespace Ui {
class DayWiseSalesSummary;
}

class DayWiseSalesSummary : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit DayWiseSalesSummary(QWidget *parent = 0);
    ~DayWiseSalesSummary();

private slots:
    void loadParams();
    void printPreview(QPrinter *p);

    void on_btn_next_clicked();

private:
    Ui::DayWiseSalesSummary *ui;
    QString html;
    QTimer *timer;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // DAYWISESALESSUMMARY_H
