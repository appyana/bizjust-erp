#ifndef PROFITANDLOSS_H
#define PROFITANDLOSS_H

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
class ProfitAndLoss;
}

class ProfitAndLoss : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    ComboBoxes cb;
    explicit ProfitAndLoss(QWidget *parent = 0);
    ~ProfitAndLoss();

private slots:
    void loadParams();
    void printPreview(QPrinter *p);
    void on_btn_next_clicked();

private:
    Ui::ProfitAndLoss *ui;
    QString html;
    QTimer *timer;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // PROFITANDLOSS_H
