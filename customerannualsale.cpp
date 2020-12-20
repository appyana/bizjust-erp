#include "customerannualsale.h"
#include "ui_customerannualsale.h"
#include <QtCharts>


QT_CHARTS_USE_NAMESPACE

CustomerAnnualSale::CustomerAnnualSale(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerAnnualSale)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

CustomerAnnualSale::~CustomerAnnualSale()
{
    delete ui;
}

void CustomerAnnualSale::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
{
    sch.name_txt = name_txt;
    sch.id_txt = id_txt;
    QMap<int, QString> data = sch.data;
    //conn.connOpen();
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        while(qry.next())
        {
            data[qry.value(0).toInt()] = qry.value(1).toString();
        }
    }
    //conn.connClose();
    /*data[2] = "Moscow";
    data[4] = "London";
    data[6] = "Paris";*/
    QCompleter *completer = new QCompleter(this);
    QStandardItemModel *model = new QStandardItemModel(completer);
    QMapIterator<int, QString> it(data);
    while (it.hasNext())
    {
        it.next();
        int code = it.key();
        QString name = it.value();
        QStandardItem *item = new QStandardItem;
        item->setText(name);
        item->setData(code, Qt::UserRole);
        model->appendRow(item);
    }
    completer->setModel(model);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCurrentRow(0);
    completer->setFilterMode(Qt::MatchContains);
    name_txt->setCompleter(completer);


    connect(completer, SIGNAL(highlighted(QModelIndex)),this,SLOT(onItemHighlighted(QModelIndex)),Qt::QueuedConnection);
    connect(name_txt,SIGNAL(editingFinished()),this,SLOT(editingFinished() ));
}

void CustomerAnnualSale::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomerAnnualSale::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomerAnnualSale::loadParams()
{
    timer->stop();
    QString year = QDate::currentDate().toString("yyyy");
    ui->year->setCurrentText(year);
    for(int i=0; i<=12; i++)
    {
        ui->tableWidgetChart->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    ui->tableWidgetChart->verticalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidgetChart->verticalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidgetChart->verticalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    QFont font("Segoe UI", 9, QFont::Bold);
    int col = 0;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Month"));
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Jan"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Feb"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Mar"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Apr"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("May"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Jun"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Jul"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Aug"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Sep"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Oct"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Nov"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    col++;
    ui->tableWidgetChart->setItem(1,col,new QTableWidgetItem("Dec"));
    ui->tableWidgetChart->item(1,col)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(1,col)->setFont(font);
    ui->tableWidgetChart->setItem(2,0,new QTableWidgetItem("Sale"));
    ui->tableWidgetChart->item(2,0)->setFont(font);

    ui->tableWidgetChart->setSpan(0,0,1,13);



}

void CustomerAnnualSale::on_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->customername,ui->customerid);
}

void CustomerAnnualSale::on_nextButton_clicked()
{
    if(ui->customerid->text().toInt()==0)
    {
        QMessageBox::warning(this,"","Please select customer");
        return;
    }
    QString year = ui->year->currentText();
    QFont font("Segoe UI", 9, QFont::Bold);

    ui->tableWidgetChart->setItem(0,0,new QTableWidgetItem( erp.customerName(ui->customerid->text()) ));
    ui->tableWidgetChart->item(0,0)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(0,0)->setFont(font);



      QBarSet *set0 = new QBarSet("Sale");
      QMap<int,float> data;
      int k=0;
      for(int i=1;i<=12; i++)
      {
          QString query_sh= " SELECT sum(net_total) as subtotal FROM tblinvoice "
                            " WHERE MONTH(invoicedate) = '"+erp.intString(i)+"' AND YEAR(invoicedate)='"+ui->year->currentText()+"' "
                            " AND customerid= '"+ui->customerid->text()+"' ";

          QString query=" select sum(rd.itemqtyorder*rd.itempriceach) AS totalReturn from  tblreturn r, "
                        " tblreturndetail rd WHERE r.id=rd.orderid and r.customer_id='"+ui->customerid->text()+"' and "
                        " MONTH(r.retdate) ='"+erp.intString(i)+"' and YEAR(r.retdate) ='"+ui->year->currentText()+"' group by r.customer_id "
                        " and  MONTH(r.retdate) and YEAR(r.retdate)";
          QSqlQuery info(query);
          info.next();
          float totalReturn = info.value("totalReturn").toFloat();
          float montlySale = 0;
          QSqlQuery info_sh(query_sh);
          while(info_sh.next())
          {
              float netSale = info_sh.value("subtotal").toFloat() - totalReturn;
              montlySale += netSale;
          }
          //*set0->insert(k,qreal(montlySale));
          //*set0->append(montlySale);
          //*set0->count();


          ui->tableWidgetChart->setItem(2,i,new QTableWidgetItem( erp.amountString(montlySale) ));
          ui->tableWidgetChart->item(2,i)->setTextAlignment(Qt::AlignCenter);
          data[k]=montlySale;
          //QMessageBox::information(this,"",erp.DecimalString(montlySale));
          k++;
      }

      *set0 << data[0] << data[1] << data[2] << data[3] << data[4] << data[5] << data[6] << data[7] << data[8] << data[9] << data[10] << data[11];

      QBarSeries *series = new QBarSeries();
      series->append(set0);


      QChart *chart = new QChart();
      chart->addSeries(series);
      chart->setTitle("Customer Annual Sale "+year);
      chart->setAnimationOptions(QChart::SeriesAnimations);

      QStringList categories;
      categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec";
      QBarCategoryAxis *axis = new QBarCategoryAxis();
      axis->append(categories);
      chart->createDefaultAxes();
      chart->setAxisX(axis, series);

      chart->legend()->setVisible(false);
      //chart->legend()->setAlignment(Qt::AlignBottom);

      QChartView *chartView = new QChartView(chart);
      chartView->setRenderHint(QPainter::Antialiasing);
      chartView->resize(900,450);
      chartView->setParent(ui->widget);
      chartView->show();
}
