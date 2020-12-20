#include "userlog.h"
#include "ui_userlog.h"

UserLog::UserLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserLog)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadForm()) );
    timer->start(5);
}

UserLog::~UserLog()
{
    delete ui;
}


void UserLog::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
{
    sch.name_txt = name_txt;
    sch.id_txt = id_txt;
    QMap<int, QString> data = sch.data;
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        while(qry.next())
        {
            data[qry.value(0).toInt()] = qry.value(1).toString();
        }
    }
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

void UserLog::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void UserLog::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void UserLog::on_username_textEdited(const QString &arg1)
{
    QString sql=sch.users(arg1);
    autocompleter(sql,ui->username,ui->userid);
}

void UserLog::loadReport()
{
    ui->detailTable->setRowCount(0);
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString userid = ui->userid->text();
    if(userid=="")
    {
        QMessageBox::critical(this,"","User is required");
        return;
    }



    QString query = "select * from tbltranslog where UserID = '"+userid+"' and LogDateTime >= INTERVAL 1 DAY +'"+begindate+"' and LogDateTime <= INTERVAL 1 DAY +'"+enddate+"' order by ID desc";
    QSqlQuery info(query);
    while(info.next())
    {
        int row = ui->detailTable->rowCount();
        ui->detailTable->insertRow(row);
        ui->detailTable->setItem(row, DATE,         new QTableWidgetItem( info.value("LogDateTime").toDate().toString("dd/MM/yyyy") ) );
        ui->detailTable->item(row,DATE)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->setItem(row, TIME,         new QTableWidgetItem( info.value("LogDateTime").toTime().toString("hh:mm:ss") ) );
        ui->detailTable->item(row,TIME)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->setItem(row, EVENT,        new QTableWidgetItem( info.value("EventName").toString() ));
        ui->detailTable->item(row,EVENT)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->setItem(row, TRANSACTION,  new QTableWidgetItem( info.value("TransType").toString() ));
        ui->detailTable->setItem(row, TRANSNO,      new QTableWidgetItem( info.value("TransID").toString() ));
        ui->detailTable->item(row,TRANSNO)->setTextAlignment(Qt::AlignCenter);
    }
}

void UserLog::loadForm()
{
    timer->stop();
    ui->begindate->setDate(erp.reportBeginDate());
    ui->enddate->setDate(QDate::currentDate());
    ui->detailTable->setColumnCount(5);
    QStringList titles;
    titles <<"Event Date"<<"Event Time"<<"Event"<<"Transaction"<<"Trans No.";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->setColumnWidth(DATE,80);
    ui->detailTable->setColumnWidth(TIME,80);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(TRANSACTION, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(EVENT,80);
    ui->detailTable->setColumnWidth(TRANSNO,80);
}

void UserLog::on_userid_textChanged(const QString &arg1)
{
    if(arg1!="" && ui->begindate->text()!="" && ui->enddate->text()!="")
    {
        loadReport();
    }
}

void UserLog::on_enddate_userDateChanged(const QDate &date)
{
    if(date.toString("yyyy-MM-dd")!="" && ui->begindate->text()!="" && ui->userid->text()!="")
    {
        loadReport();
    }
}

void UserLog::on_begindate_userDateChanged(const QDate &date)
{
    if(date.toString("yyyy-MM-dd")!="" && ui->enddate->text()!="" && ui->userid->text()!="")
    {
        loadReport();
    }
}
