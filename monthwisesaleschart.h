#ifndef MONTHWISESALESCHART_H
#define MONTHWISESALESCHART_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class MonthWiseSalesChart;
}

class MonthWiseSalesChart : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit MonthWiseSalesChart(QWidget *parent = 0);
    ~MonthWiseSalesChart();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();
    void loadParams();

    void on_customername_textEdited(const QString &arg1);

    void on_nextButton_clicked();

private:
    Ui::MonthWiseSalesChart *ui;
    QTimer *timer;
};

#endif // MONTHWISESALESCHART_H
