#include "customerpaymentreceiptreport.h"
#include "ui_customerpaymentreceiptreport.h"

CustomerPaymentReceiptReport::CustomerPaymentReceiptReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerPaymentReceiptReport)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

CustomerPaymentReceiptReport::~CustomerPaymentReceiptReport()
{
    delete ui;
}

void CustomerPaymentReceiptReport::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void CustomerPaymentReceiptReport::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomerPaymentReceiptReport::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomerPaymentReceiptReport::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
    ui->detailTable->setColumnCount(9);
    QStringList titles;
    titles <<"Customer"<<"Voucher #"<<"Date"<<"Invoice #"<<"Method"<<"Cheque #"<<"Cheque\nDate"<<"Bank"<<"Amount";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(CUSTOMER, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(VOUCHER,70);
    ui->detailTable->setColumnWidth(DATE,70);
    ui->detailTable->setColumnWidth(INVOICE,70);
    ui->detailTable->setColumnWidth(METHOD,70);
    ui->detailTable->setColumnWidth(CHEQUE,70);
    ui->detailTable->setColumnWidth(CHEQUEDATE,70);
    ui->detailTable->setColumnWidth(BANK,170);
    ui->detailTable->setColumnWidth(AMOUNT,90);
}

void CustomerPaymentReceiptReport::on_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->customername,ui->customerid);
}

void CustomerPaymentReceiptReport::on_nextButton_clicked()
{

    this->setCursor(Qt::WaitCursor);

    ui->detailTable->setRowCount(0);
    if(ui->customerid->text()=="")
    {
        ui->lbl_customer->setText("All");
        customerid = "0";
    }
    else
    {
        ui->lbl_customer->setText( ui->customername->text() );
        customerid = ui->customerid->text();
    }
    ui->lbl_begindate->setText( ui->begindate->date().toString("dd/MM/yyyy") );
    ui->lbl_enddate->setText( ui->enddate->date().toString("dd/MM/yyyy") );

    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");

    QString customername = "All";
    if(customerid.toInt()>0)
    {
        customername = ui->customername->text();
    }
    html = "<table width='100%' style='font-size:10px;' border=1 bordercolor='#333333' cellspacing='0' cellpadding='3'>"
           "<thead>"
           "<tr>"
           "<th colspan='9'>"
               "<table width='100%'>"
                   "<tr>"
                       "<th width='50%' align='left'>Customer: "+customername+"</th>"
                       "<th width='25%' align='left'>From Date: "+erp.DDMMYYDateFromSQL(begindate)+"</th>"
                       "<th width='25%' align='left'>To Date: "+erp.DDMMYYDateFromSQL(enddate)+"</th>"
                   "</tr>"
               "</table>"
           "</th>"
           "</tr>"
            //<<"Customer"<<"Voucher #"<<"Date"<<"Invoice #"<<"Method"<<"Cheque #"<<"Cheque Date"<<"Bank"<<"Amount";
           "<tr bgcolor='#EEEEEE'>"
            "<th width='30%'>Customer</th>"
           "<th width='6%'>Voucher #</th>"
            "<th width='6%'>Date</th>"
           "<th width='6%'>Invoice #</th>"
           "<th width='6%'>Method</th>"
           "<th width='6%'>Cheque #</th>"
           "<th width='6%'>Cheque Date</th>"
           "<th width='24%'>Bank</th>"
           "<th width='10%'>Amount</th>"
           "</tr>"
           "</thead>"
           "<tbody>";


    float tot_amount = 0.00;

    QString query = " select c.name as customername, c.city, p.voucherid as voucherno,p.invoiceid , "
                    " p.datereceived,p.paymeth,p.amount , p.checkNum , p.dueDate ,  p.bank , p.ourbank "
                    " from tblinvoicepaymentdetail p left outer join tblcustomers c on p.customer_id=c.customerid "
                    " where p.cancel=0 ";
    if(customerid.toInt()>0)
    {
        query += " and p.customer_id = '"+customerid+"' ";
    }
    if(ui->begindate->text()!="" && ui->enddate->text()!="")
    {
        query += " and p.datereceived >= '"+begindate+"' and p.datereceived <='"+enddate+"' ";
    }
    query += " order by p.datereceived desc,p.voucherid desc ";
    QSqlQuery info(query);
    //int i;
    int row;
    while(info.next())
    {
//        QString order = info.value("order").toString();
        QSqlQuery info_p(" select paymeth from tblpaymeth where id = '"+ info.value("paymeth").toString() + "' ");
        info_p.next();


        QString customer_name = info.value("customername").toString();
        QString voucherno = info.value("voucherno").toString();
        QString datereceived = info.value("datereceived").toDate().toString("dd/MM/yyyy");
        QString invoiceid = info.value("invoiceid").toString();
        QString paymeth = info_p.value("paymeth").toString();
        QString checkNum = info.value("checkNum").toString();
        QString dueDate = info.value("dueDate").toDate().toString("dd/MM/yyyy");
        QString ourbank = erp.getAccount( info.value("ourbank").toString() );
        float amount = info.value("amount").toFloat();

        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;

        QTableWidgetItem *cust = new QTableWidgetItem( customer_name );
        cust->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        QTableWidgetItem *voucher = new QTableWidgetItem( voucherno );
        voucher->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *vdate = new QTableWidgetItem( datereceived );
        vdate->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *inv = new QTableWidgetItem( invoiceid );
        inv->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *pmode = new QTableWidgetItem( paymeth );
        pmode->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *chk = new QTableWidgetItem( checkNum );
        chk->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *ddate = new QTableWidgetItem( dueDate );
        ddate->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *obank = new QTableWidgetItem( ourbank );
        obank->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        QTableWidgetItem *amt = new QTableWidgetItem( erp.amountString( amount ) );
        amt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

        ui->detailTable->setItem(row, CUSTOMER,  cust );
        ui->detailTable->setItem(row, VOUCHER,     voucher );
        ui->detailTable->setItem(row, DATE, vdate );
        ui->detailTable->setItem(row, INVOICE,  inv );
        ui->detailTable->setItem(row, METHOD,   pmode );
        ui->detailTable->setItem(row, CHEQUE,      chk );
        ui->detailTable->setItem(row, CHEQUEDATE,   ddate );
        ui->detailTable->setItem(row, BANK, obank );
        ui->detailTable->setItem(row, AMOUNT,  amt );

        html += "<tr style='font-size:10px;'>"
                "<td>"+customer_name+"</td>"
                "<td align='center'>"+voucherno+"</td>"
                "<td align='center'>"+datereceived+"</td>"
                "<td align='center'>"+invoiceid+"</td>"
                "<td align='center'>"+paymeth+"</td>"
                "<td align='center'>"+checkNum+"</td>"
                "<td align='cebter'>"+dueDate+"</td>"
                "<td align='left'>"+ourbank+"</td>"
                "<td align='right'>"+erp.amountString(amount)+"</td>"
                "</tr>";

        tot_amount += amount;
        //i = ui->detailTable->rowCount();
    }//end while(info_sale.next())

    ui->detailTable->insertRow(ui->detailTable->rowCount());
    row = ui->detailTable->rowCount()-1;

    QFont font("Segoe UI", 9, QFont::Bold);
    QTableWidgetItem *tottitle = new QTableWidgetItem( "Total:" );
    tottitle->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tottitle->setFont(font);

    QTableWidgetItem *totamt = new QTableWidgetItem( erp.amountString(tot_amount) );
    totamt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totamt->setFont(font);

    ui->detailTable->setSpan(row, CUSTOMER,1,8);
    ui->detailTable->setItem(row, AMOUNT,    totamt );

    html += "<tr>"
            "<th align='right' colspan='8'>Total:</th>"
            "<th align='right'>"+erp.amountString(tot_amount)+"</th>"
            "</tr>";

    html += "</tbody></table>";
    this->setCursor(Qt::ArrowCursor);
}

void CustomerPaymentReceiptReport::on_printButton_clicked()
{
    this->setCursor(Qt::WaitCursor);
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

void CustomerPaymentReceiptReport::printPreview(QPrinter *p)
{
/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Receipt Report</th>"
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
this->setCursor(Qt::ArrowCursor);
}
