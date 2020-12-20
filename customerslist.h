#ifndef CUSTOMERSLIST_H
#define CUSTOMERSLIST_H

#include <QDialog>

namespace Ui {
class CustomersList;
}

class CustomersList : public QDialog
{
    Q_OBJECT

public:
    explicit CustomersList(QWidget *parent = 0);
    ~CustomersList();

private:
    Ui::CustomersList *ui;
};

#endif // CUSTOMERSLIST_H
