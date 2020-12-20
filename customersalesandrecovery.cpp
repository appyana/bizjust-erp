#include "customersalesandrecovery.h"
#include "ui_customersalesandrecovery.h"
#include <QPaintEngine>
#include <QPrintEngine>

CustomerSalesAndRecovery::CustomerSalesAndRecovery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerSalesAndRecovery)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

CustomerSalesAndRecovery::~CustomerSalesAndRecovery()
{
    delete ui;
}


void CustomerSalesAndRecovery::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void CustomerSalesAndRecovery::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomerSalesAndRecovery::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomerSalesAndRecovery::loadParams()
{
    timer->stop();
    ui->lbl_groupid->setVisible(false);
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
    ui->detailTable->setColumnCount(9);
    QStringList titles;
    titles <<"Customer"<<"Balance"<<"No. of\nInvoices"<<"Sales Value"<<"%"<<"Sales Return"<<"%"<<"Recovery"<<"%";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(CUSTOMER, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(BALANCE,90);
    ui->detailTable->setColumnWidth(INVOICES,50);
    ui->detailTable->setColumnWidth(SALES,90);
    ui->detailTable->setColumnWidth(SALESPERCENT,50);
    ui->detailTable->setColumnWidth(RETURN,90);
    ui->detailTable->setColumnWidth(RETURNPERCET,50);
    ui->detailTable->setColumnWidth(RECOVERY,90);
    ui->detailTable->setColumnWidth(RECOVERYPERCENT,50);
}

void CustomerSalesAndRecovery::on_groupname_textEdited(const QString &arg1)
{
    QString sql = sch.CustomerGroups(arg1);
    autocompleter(sql,ui->groupname,ui->groupid);
}

void CustomerSalesAndRecovery::on_nextButton_clicked()
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
    ui->lbl_from->setText( ui->begindate->date().toString("dd/MM/yyyy") );
    ui->lbl_to->setText( ui->enddate->date().toString("dd/MM/yyyy") );
    QString groupid = ui->groupid->text();
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");

    conn.execQuery(" DROP TABLE IF EXISTS SalesSummary ");
    QString sqltemp = " CREATE TEMPORARY TABLE SalesSummary ( "
                      " customername VARCHAR(50) NOT NULL, "
                      " customerid INT NOT NULL DEFAULT 0, "
                      " customer_bal DECIMAL(18,4) NOT NULL DEFAULT 0.0000, "
                      " No_of_salesinvoices INT NOT NULL DEFAULT 0, "
                      " sales_val DECIMAL(18,4) NOT NULL DEFAULT 0.0000, "
                      " recovery DECIMAL(18,4) NOT NULL DEFAULT 0.0000, "
                      " salesret_val DECIMAL(18,4) NOT NULL DEFAULT 0.0000) "
                      " ENGINE=MyISAM  DEFAULT CHARSET=latin1 ";
    conn.execQuery(sqltemp);

    QString query_cust = " select t.compid as customerid  from tblgltransaction t, tblgltransvoucher v, "
                         " tblcustomers c where t.voucherno=v.voucherno AND t.voucherid = v.voucherid	"
                         " and t.compid=c.customerid and  t.companytype='customer' ";
    if(groupid!="0" && groupid!="")
    {
        query_cust += " and c.groupid='"+groupid+"' ";
    }
    query_cust += " GROUP By t.compid ";
    //qDebug() << query_cust;
    QSqlQuery info_cust(query_cust);
    int i=1;
    while(info_cust.next())
    {
        //qDebug() << "ok cust";
        QString customerid = info_cust.value("customerid").toString();
        QString customername = erp.customerName(customerid);

        QString query_bal = " select (sum(debit) - sum(credit) ) as cust_bal from tblgltransaction t, "
                            " tblgltransvoucher v where t.voucherno=v.voucherno and t.voucherid = v.voucherid "
                            " and t.companytype='customer' and t.compid='"+customerid+"'";
        QSqlQuery info_bal(query_bal);
        info_bal.next();
        float customer_bal = info_bal.value("cust_bal").toFloat();

        QString query_inv = " select sum(net_total) totalsales, count(invoicenumber) as no_of_inv,sum(totalDiscount) "
                            " as totalDiscount from tblinvoice where invoicedate >='"+begindate+"' and "
                            " invoicedate <='"+enddate+"' and customerid ='"+customerid+"'";
        QSqlQuery info_inv(query_inv);
        info_inv.next();
        float sales_value = info_inv.value("totalsales").toFloat();
        int No_of_SalesInvoices = info_inv.value("no_of_inv").toInt();

        QString query_rec    = "select (sum(amount)+sum(write_off)+sum(discount)+sum(bankCharges)+sum(AdvancePayment)) "
                               " recovery from tblinvoicepaymentdetail  where customer_id = '"+customerid+"' and "
                               " cancel=0 and datereceived>='"+begindate+"' and datereceived<='"+enddate+"'";
        QSqlQuery info_rec(query_rec);
        info_rec.next();
        float recovery = info_rec.value("recovery").toFloat();

        QString query_ret   = " SELECT IFNULL(sum(total_amount),0.00) sales_ret FROM tblreturn WHERE "
                              " customer_id = '"+customerid+"' and retdate>='"+begindate+"' and retdate<='"+enddate+"'";
        QSqlQuery info_ret(query_ret);
        info_ret.next();
        float sales_ret = info_ret.value("sales_ret").toFloat();

        QString qry_temp_sales = " INSERT INTO SalesSummary "
                                 " (customerid , customername, customer_bal, sales_val, No_of_salesinvoices, recovery, `salesret_val`) "
                                 " VALUES "
                                 " ('"+customerid+"', '"+customername+"', '"+erp.DecimalString(customer_bal)+"', '"+erp.DecimalString(sales_value)+"', '"+erp.intString(No_of_SalesInvoices)+"', '"+erp.DecimalString(recovery)+"', '"+erp.DecimalString(sales_ret)+"')";
        if(!conn.execQuery(qry_temp_sales))
        {
            QMessageBox::critical(this,"Error","Error in Temporary table insertion");
            return;
        }
        i++;
    }//end while(info_cust.next())
    QString groupname = "All";
    if(groupid.toInt()>0)
    {
        groupname = erp.getCustomerGroupName(groupid);
    }
    html = "<table width='100%' style='font-size:10px;' border=1 bordercolor='#333333' cellspacing='0' cellpadding='3'>"
           "<thead>"
           "<tr>"
           "<th colspan='10'>"
               "<table width='100%'>"
                   "<tr>"
                       "<th width='50%' align='left'>Customer Group: "+groupname+"</th>"
                       "<th width='25%' align='left'>From Date: "+erp.DDMMYYDateFromSQL(begindate)+"</th>"
                       "<th width='25%' align='left'>To Date: "+erp.DDMMYYDateFromSQL(enddate)+"</th>"
                   "</tr>"
               "</table>"
           "</th>"
           "</tr>"
           "<tr bgcolor='#EEEEEE'>"
           "<th width='4%'>Sr.</th>"
           "<th width='32%'>Customer</th>"
           "<th width='10%'>Balance</th>"
           "<th width='6%'>No. of Invoices</th>"
           "<th width='10%'>Sales Value</th>"
           "<th width='6%'>%</th>"
           "<th width='10%'>Sales Return</th>"
           "<th width='6%'>%</th>"
           "<th width='10%'>Recovery</th>"
           "<th width='6%'>%</th>"
           "</tr>"
           "</thead>"
           "<tbody>";

    QString query_totals  = "select sum(sales_val) as tot_sales, sum(salesret_val) as total_salesret, sum(recovery) as total_recovery from SalesSummary ";
    QSqlQuery info_totals(query_totals);
    info_totals.next();
    float total_sales = info_totals.value("tot_sales").toFloat();
    float total_salesret = info_totals.value("total_salesret").toFloat();
    float total_recovery = info_totals.value("total_recovery").toFloat();
    float total_balance = 0.00;
    int total_of_invoices = 0;
    QSqlQuery info_sale(" SELECT * FROM SalesSummary order by customername asc ");
    int row;
    while(info_sale.next())
    {
        //qDebug() <<"ok loop";
        float customer_bal = info_sale.value("customer_bal").toFloat();
        float sales_val = info_sale.value("sales_val").toFloat();
        float salesret_val = info_sale.value("salesret_val").toFloat();
        float recovery = info_sale.value("recovery").toFloat();
        float sales_per = 0.00;
        float salesret_per = 0.00;
        float recovery_per = 0.00;
        QString customername = info_sale.value("customername").toString();
        int No_of_salesinvoices = info_sale.value("No_of_salesinvoices").toInt();
        if(customer_bal!=0 || sales_val!=0 || salesret_val!=0 || recovery!=0)
        {
            //qDebug() <<"ok if";
            if(total_sales>0)
            {
                sales_per =((sales_val*100)/total_sales);
            }
            if(total_salesret>0)
            {
                salesret_per =((salesret_val*100)/total_salesret);
            }
            if(total_recovery>0)
            {
                recovery_per =((recovery*100)/total_recovery);
            }

            ui->detailTable->insertRow(ui->detailTable->rowCount());
            row = ui->detailTable->rowCount()-1;

            QTableWidgetItem *custname = new QTableWidgetItem( customername );
            custname->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            QTableWidgetItem *cbal = new QTableWidgetItem( erp.amountString(customer_bal) );
            cbal->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            QTableWidgetItem *invoices = new QTableWidgetItem( erp.intString(No_of_salesinvoices) );
            invoices->setTextAlignment(Qt::AlignCenter);

            QTableWidgetItem *sales = new QTableWidgetItem( erp.amountString(sales_val) );
            sales->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            QTableWidgetItem *salesper = new QTableWidgetItem( erp.amountString(sales_per) );
            salesper->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            QTableWidgetItem *ret = new QTableWidgetItem( erp.amountString(salesret_val) );
            ret->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            QTableWidgetItem *retper = new QTableWidgetItem( erp.amountString(salesret_per) );
            retper->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            QTableWidgetItem *rec = new QTableWidgetItem( erp.amountString(recovery) );
            rec->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            QTableWidgetItem *recper = new QTableWidgetItem( erp.amountString(recovery_per) );
            recper->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            ui->detailTable->setItem(row, CUSTOMER,       custname );
            ui->detailTable->setItem(row, BALANCE,        cbal );
            ui->detailTable->setItem(row, INVOICES,       invoices );
            ui->detailTable->setItem(row, SALES,          sales );
            ui->detailTable->setItem(row, SALESPERCENT,   salesper );
            ui->detailTable->setItem(row, RETURN,         ret );
            ui->detailTable->setItem(row, RETURNPERCET,   retper );
            ui->detailTable->setItem(row, RECOVERY,       rec );
            ui->detailTable->setItem(row, RECOVERYPERCENT,recper );

            html += "<tr style='font-size:10px;'>"
                    "<td align='center'>"+erp.intString(ui->detailTable->rowCount())+"</td>"
                    "<td>"+customername+"</td>"
                    "<td align='right'>"+erp.amountString(customer_bal)+"</td>"
                    "<td align='center'>"+erp.intString(No_of_salesinvoices)+"</td>"
                    "<td align='right'>"+erp.amountString(sales_val)+"</td>"
                    "<td align='center'>"+erp.amountString(sales_per)+"</td>"
                    "<td align='right'>"+erp.amountString(salesret_val)+"</td>"
                    "<td align='center'>"+erp.amountString(salesret_per)+"</td>"
                    "<td align='right'>"+erp.amountString(recovery)+"</td>"
                    "<td align='center'>"+erp.amountString(recovery_per)+"</td>"
                    "</tr>";

            total_balance += customer_bal;
            total_of_invoices += No_of_salesinvoices;
        }//end if(customer_bal!=0 || sales_val!=0 || salesret_val!=0 || recovery!=0)
    }//end while(info_sale.next())


    ui->detailTable->insertRow(ui->detailTable->rowCount());
    row = ui->detailTable->rowCount()-1;

    QFont font("Segoe UI", 9, QFont::Bold);
    QTableWidgetItem *tottitle = new QTableWidgetItem( "Total:" );
    tottitle->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tottitle->setFont(font);
    QTableWidgetItem *totbal = new QTableWidgetItem( erp.amountString(total_balance) );
    totbal->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totbal->setFont(font);

    QTableWidgetItem *totinvoices = new QTableWidgetItem( erp.intString(total_of_invoices) );
    totinvoices->setTextAlignment(Qt::AlignCenter);
    totinvoices->setFont(font);

    QTableWidgetItem *totsales = new QTableWidgetItem( erp.amountString(total_sales) );
    totsales->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totsales->setFont(font);

    QTableWidgetItem *totret = new QTableWidgetItem( erp.amountString(total_salesret) );
    totret->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totret->setFont(font);

    QTableWidgetItem *totrec = new QTableWidgetItem( erp.amountString(total_recovery) );
    totrec->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totrec->setFont(font);

    ui->detailTable->setItem(row, CUSTOMER,       tottitle );
    ui->detailTable->setItem(row, BALANCE,        totbal );
    ui->detailTable->setItem(row, INVOICES,       totinvoices );
    ui->detailTable->setItem(row, SALES,          totsales );
    ui->detailTable->setItem(row, RETURN,         totret );
    ui->detailTable->setItem(row, RECOVERY,       totrec );

    html += "<tr>"
            "<th align='right' colspan='2'>Total:</th>"
            "<th align='right'>"+erp.amountString(total_balance)+"</th>"
            "<th align='center'>"+erp.intString(total_of_invoices)+"</th>"
            "<th align='right'>"+erp.amountString(total_sales)+"</th>"
            "<th align='right'></th>"
            "<th align='right'>"+erp.amountString(total_salesret)+"</th>"
            "<th align='right'></th>"
            "<th align='right'>"+erp.amountString(total_recovery)+"</th>"
            "<th align='right'></th>"
            "</tr>";

    html += "</tbody></table>";
}

void CustomerSalesAndRecovery::on_printButton_clicked()
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

void CustomerSalesAndRecovery::printPreview(QPrinter *p)
{
/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Customer Sales and Recovery</th>"
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
