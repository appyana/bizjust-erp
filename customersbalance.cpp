#include "customersbalance.h"
#include "ui_customersbalance.h"
#include <QPaintEngine>
#include <QPrintEngine>

CustomersBalance::CustomersBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomersBalance)
{
    ui->setupUi(this);
    //erp.setloader(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

CustomersBalance::~CustomersBalance()
{
    delete ui;
}


void CustomersBalance::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void CustomersBalance::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomersBalance::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomersBalance::loadParams()
{
    timer->stop();
    ui->lbl_groupid->setVisible(false);
    ui->begindate->setDate( QDate::currentDate() );
    ui->detailTable->setColumnCount(5);
    QStringList titles;
    titles <<"Customer Group"<<"Customer\nID"<<"Customer Name"<<"Phone/Mobile"<<"Balance";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(CUSTOMERNAME, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(GROUP,120);
    ui->detailTable->setColumnWidth(CUSTOMERID,60);
    ui->detailTable->setColumnWidth(PHONE,120);
    ui->detailTable->setColumnWidth(BALANCE,90);
}

void CustomersBalance::on_groupname_textEdited(const QString &arg1)
{
    QString sql = sch.CustomerGroups(arg1);
    autocompleter(sql,ui->groupname,ui->groupid);
}

void CustomersBalance::on_nextButton_clicked()
{
    ui->detailTable->setRowCount(0);
    if(ui->groupid->text()=="")
    {
        ui->lbl_group->setText("All");
        ui->lbl_groupid->setText("0");
    }
    else
    {
        ui->lbl_group->setText( ui->groupname->text() );
        ui->lbl_groupid->setText( ui->groupid->text() );
    }
    ui->lbl_date->setText( ui->begindate->date().toString("dd/MM/yyyy") );
    QString groupid = ui->groupid->text();
    QString date = ui->begindate->date().toString("yyyy-MM-dd");

    QString query_cust = " select customerid , name as cname , phone1, phone2, phone3 , groupid from tblcustomers where customerid > 0 ";
    if(groupid.toInt()>0)
    {
        query_cust += " and groupid='"+groupid+"' ";
    }
    query_cust += " GROUP By name ";
    //qDebug() << query_cust;
    QSqlQuery info_cust(query_cust);

    QString groupname = "All";
    if(groupid.toInt()>0)
    {
        groupname = erp.getCustomerGroupName(groupid);
    }
    html = "<table width='100%' style='font-size:10px;' border=1 bordercolor='#333333' cellspacing='0' cellpadding='3'>"
           "<thead>"
           "<tr>"
           "<th colspan='6'>"
               "<table width='100%'>"
                   "<tr>"
                       "<th width='70%' align='left'>Customer Group: "+groupname+"</th>"
                       "<th width='30%' align='left'>To Date: "+erp.DDMMYYDateFromSQL(date)+"</th>"
                   "</tr>"
               "</table>"
           "</th>"
           "</tr>"
           "<tr bgcolor='#EEEEEE'>"
           "<th width='5%'>Sr.</th>"
            "<th width='20%'>Group Name</th>"
            "<th width='5%'>Customer Id</th>"
           "<th width='44%'>Customer Name</th>"
            "<th width='13%'>Phone</th>"
           "<th width='13%'>Balance</th>"
           "</tr>"
           "</thead>"
           "<tbody>";
    bool showzero = ui->showzero->isChecked();
    bool ok;// = false;
    float total_balance = 0.00;
    int row;
    while(info_cust.next())
    {
        //qDebug()<<"ok";
        ok=false;
        QString query_s1= " select ( sum(debit) - sum(credit) ) dc from tblgltransaction t,tblgltransvoucher v "
                          " where t.voucherno=v.voucherno AND t.voucherid = v.voucherid and  "
                          " t.compid='"+info_cust.value("customerid").toString()+"' and t.companytype='customer' "
                          " and v.entrydate <= '"+date+"' group by t.compid ";
        QSqlQuery info_s1(query_s1);
        info_s1.next();
        float a_netopen1 = info_s1.value("dc").toFloat();
        if(showzero)
        {
            ok = true;
        }
        if(!showzero)
        {
            if(a_netopen1==0)
            {
                ok = false;
            }
            else
            {
                ok = true;
            }
        }

        if(ok)
        {
            QString groupname = erp.getCustomerGroupName( info_cust.value("groupid").toString() );
            QString customerid = info_cust.value("customerid").toString();
            QString customername = erp.customerName(info_cust.value("customerid").toString());
            QString phone = "";
            if(info_cust.value("phone1").toString()!="")
            {
                phone += info_cust.value("phone1").toString();
            }
            if(info_cust.value("phone2").toString()!="")
            {
                phone += "\n"+info_cust.value("phone2").toString();
            }
            if(info_cust.value("phone3").toString()!="")
            {
                phone += "\n"+info_cust.value("phone3").toString();
            }
            float balance = a_netopen1;

            ui->detailTable->insertRow(ui->detailTable->rowCount());
            row = ui->detailTable->rowCount()-1;

            QTableWidgetItem *gname = new QTableWidgetItem( groupname );
            gname->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

            QTableWidgetItem *custid = new QTableWidgetItem( customerid );
            custid->setTextAlignment(Qt::AlignCenter);

            QTableWidgetItem *custname = new QTableWidgetItem( customername );
            custname->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

            QTableWidgetItem *ph = new QTableWidgetItem( phone );
            ph->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

            QTableWidgetItem *cbal = new QTableWidgetItem( erp.amountString(balance) );
            cbal->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            ui->detailTable->setItem(row, GROUP,        gname );
            ui->detailTable->setItem(row, CUSTOMERID,   custid );
            ui->detailTable->setItem(row, CUSTOMERNAME, custname );
            ui->detailTable->setItem(row, PHONE,        ph );
            ui->detailTable->setItem(row, BALANCE,      cbal );

            html += "<tr style='font-size:10px;'>"
                    "<td align='center'>"+erp.intString(ui->detailTable->rowCount())+"</td>"
                    "<td>"+groupname+"</td>"
                    "<td align='center'>"+customerid+"</td>"
                    "<td>"+customername+"</td>"
                    "<td>"+phone+"</td>"
                    "<td align='right'>"+erp.amountString(balance)+"</td>"
                    "</tr>";

            total_balance += balance;
        }//end if(ok)
    }//end while(info_cust.next())


    ui->detailTable->insertRow(ui->detailTable->rowCount());
    row = ui->detailTable->rowCount()-1;

    QFont font("Segoe UI", 9, QFont::Bold);
    QTableWidgetItem *tottitle = new QTableWidgetItem( "Total:" );
    tottitle->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tottitle->setFont(font);
    QTableWidgetItem *totbal = new QTableWidgetItem( erp.amountString(total_balance) );
    totbal->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totbal->setFont(font);

    ui->detailTable->setSpan(row,GROUP,1,4);
    ui->detailTable->setItem(row, GROUP,       tottitle );
    ui->detailTable->setItem(row, BALANCE,        totbal );

    html += "<tr>"
            "<th align='right' colspan='5'>Total:</th>"
            "<th align='right'>"+erp.amountString(total_balance)+"</th>"
            "</tr>";
    html += "</tbody></table>";
}

void CustomersBalance::on_printButton_clicked()
{
    printer=new QPrinter();
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QPrinter::A4);
    printer->setPageMargins(QMarginsF(10,10,10,10));
    printDlg=new QPrintDialog(printer);
    pageDlg=new QPageSetupDialog(printer);

    pd = new QPrintPreviewDialog(printer);
    connect(pd,SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));
    pd->setWindowTitle("Print Sales Invoice");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void CustomersBalance::printPreview(QPrinter *p)
{
/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Customers Balance</th>"
                          "</tr>"
                          "<!--<tr><td colspan='3'><hr/></td></tr>-->"                                                                                                ""                                                                                               ""
                          "</table>";


/******************************************* Settings ****************************************************/

    QRect printer_rect(p->pageRect());

    //Setting up the header and calculating the header size
        QTextDocument *document_header = new QTextDocument(this);
        document_header->setPageSize(printer_rect.size());
        //int pagenum = document_header->pageCount();
        document_header->setHtml(header_html);
        QSizeF header_size = document_header->size();

    //Setting up the footer and calculating the footer size
        QTextDocument *document_footer = new QTextDocument(this);
        document_footer->setPageSize(printer_rect.size());
        //document_footer->setHtml("");
        //QSizeF footer_size = document_footer->size();

    //Calculating the main document size for one page
        QSizeF center_size(printer_rect.width(), (p->pageRect().height() - header_size.toSize().height()  ));//- footer_size.toSize().height()

    //Setting up the center page
        QTextDocument *main_doc = new QTextDocument(this);
        main_doc->setHtml(html);
        main_doc->setPageSize(center_size);


    //Setting up the rectangles for each section.
        QRect pageNo = QRect(700, 40, 100, 50);
        QRect headerRect = QRect(QPoint(0,0), document_header->size().toSize());
        QRect footerRect = QRect(QPoint(0,0), document_footer->size().toSize());
        QRect contentRect = QRect(QPoint(0,0), main_doc->size().toSize());    // Main content rectangle.
        QRect currentRect = QRect(QPoint(0,0), center_size.toSize());        // Current main content rectangle.

        QPainter painter(p);
            pagenum=1;
            while (currentRect.intersects(contentRect))
            {//Loop if the current content rectangle intersects with the main content rectangle.
                //Resetting the painter matrix co ordinate system.
                painter.resetMatrix();
                //Applying negative translation of painter co-ordinate system by current main content rectangle top y coordinate.
                painter.translate(0, -currentRect.y());
                //Applying positive translation of painter co-ordinate system by header hight.
                painter.translate(0, headerRect.height());
                //Drawing the center content for current page.
                main_doc->drawContents(&painter, currentRect);
                //Resetting the painter matrix co ordinate system.
                painter.resetMatrix();
                //Drawing the header on the top of the page
                document_header->drawContents(&painter, headerRect);
                painter.drawText(pageNo,"Page. "+erp.intString(pagenum));
                //Applying positive translation of painter co-ordinate system to draw the footer
                painter.translate(0, headerRect.height());
                painter.translate(0, center_size.height());
                document_footer->drawContents(&painter, footerRect);

                //Translating the current rectangle to the area to be printed for the next page
                currentRect.translate(0, currentRect.height());
                //Inserting a new page if there is till area left to be printed
                if (currentRect.intersects(contentRect))
                {
                    p->newPage();
                    pagenum++;
                }
            }

}
