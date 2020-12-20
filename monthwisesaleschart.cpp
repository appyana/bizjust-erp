#include "monthwisesaleschart.h"
#include "ui_monthwisesaleschart.h"
#include <QtCharts>

MonthWiseSalesChart::MonthWiseSalesChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonthWiseSalesChart)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

MonthWiseSalesChart::~MonthWiseSalesChart()
{
    delete ui;
}


void MonthWiseSalesChart::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void MonthWiseSalesChart::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void MonthWiseSalesChart::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void MonthWiseSalesChart::loadParams()
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
    ui->tableWidgetChart->verticalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
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

    ui->tableWidgetChart->setItem(3,0,new QTableWidgetItem("Recovery"));
    ui->tableWidgetChart->item(3,0)->setFont(font);

    ui->tableWidgetChart->setSpan(0,0,1,13);

}

void MonthWiseSalesChart::on_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->customername,ui->customerid);
}

void MonthWiseSalesChart::on_nextButton_clicked()
{

    QString year = ui->year->currentText();
    QString customerid = ui->customerid->text();
    QString cust = "All Customers";
    if(customerid!="")
    {
        cust = "Customer: "+erp.customerName(customerid);
    }

    QFont font("Segoe UI", 9, QFont::Bold);

    ui->tableWidgetChart->setItem(0,0,new QTableWidgetItem( cust ));
    ui->tableWidgetChart->item(0,0)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetChart->item(0,0)->setFont(font);

      QBarSet *set0 = new QBarSet("Sale");
      QBarSet *set1 = new QBarSet("Recovery");
      QMap<int,float> sale;
      QMap<int,float> recovery;
      int k=0;
      for(int i=1;i<=12; i++)
      {
          QString month = erp.intString(i);
          QString query_sh= " SELECT sum(net_total) as totalAmount,sum(totalDiscount) as totalDiscount "
                            " FROM tblinvoice  "
                            " WHERE MONTH(invoicedate) = '"+month+"' AND YEAR(invoicedate)='"+year+"' ";

          if(customerid!="")
          {
              query_sh += " AND customerid= '"+customerid+"' ";
          }

          QSqlQuery info_sh(query_sh);
          info_sh.next();
          float montlySale = info_sh.value("totalAmount").toFloat();
          //float discount = info_sh.value("totalDiscount").toFloat();
          sale[k]=montlySale;
          ui->tableWidgetChart->setItem(2,i,new QTableWidgetItem( erp.amountString(montlySale) ));
          ui->tableWidgetChart->item(2,i)->setTextAlignment(Qt::AlignCenter);


          QString query2=" select sum(id.amount) as total, sum(id.AdvancePayment) AS totalRecovery from "
                         " tblinvoicepaymentdetail id WHERE MONTH(id.datereceived) ='"+month+"' "
                         " AND YEAR(id.datereceived) ='"+year+"' ";
          if(customerid!="")
          {
              query2 += " AND id.customer_id='"+customerid+"' group by id.customer_id ";
          }

          QSqlQuery info2(query2);
          info2.next();
          float totalRecovery = info2.value("totalRecovery").toFloat() + info2.value("total").toFloat();

          recovery[k] = totalRecovery;
          ui->tableWidgetChart->setItem(3,i,new QTableWidgetItem( erp.amountString(totalRecovery) ));
          ui->tableWidgetChart->item(3,i)->setTextAlignment(Qt::AlignCenter);
            set0->append(montlySale);
            set1->append(totalRecovery);

          k++;
      }
      set0->setLabelColor("#000");
      set1->setLabelColor("#000");
      set0->setColor("#006699");
      set1->setColor("#990000");

      //*set0 << sale[0] << sale[1] << sale[2] << sale[3] << sale[4] << sale[5] << sale[6] << sale[7] << sale[8] << sale[9] << sale[10] << sale[11];
      //*set1 << recovery[0] << recovery[1] << recovery[2] << recovery[3] << recovery[4] << recovery[5] << recovery[6] << recovery[7] << recovery[8] << recovery[9] << recovery[10] << recovery[11];

      QBarSeries *series = new QBarSeries();
      series->append(set0);
      series->append(set1);
      //series->setLabelsVisible(true);
      //series->setLabelsAngle(270);
      //series->setBarWidth(0.8);
      //series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);


      QChart *chart = new QChart();
      chart->addSeries(series);
      chart->setTitle("Month Wise Sales/Recovery Chart for "+year);
      chart->setAnimationOptions(QChart::SeriesAnimations);

      QStringList categories;
      categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec";
      QBarCategoryAxis *axis = new QBarCategoryAxis();
      axis->append(categories);
      chart->createDefaultAxes();
      chart->setAxisX(axis, series);

      chart->legend()->setVisible(true);
      chart->legend()->setAlignment(Qt::AlignBottom);

      QChartView *chartView = new QChartView(chart);
      chartView->setRenderHint(QPainter::Antialiasing);
      chartView->resize(900,450);
      chartView->setParent(ui->widget);
      chartView->show();
}
