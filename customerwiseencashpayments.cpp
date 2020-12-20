#include "customerwiseencashpayments.h"
#include "ui_customerwiseencashpayments.h"

CustomerWiseEncashPayments::CustomerWiseEncashPayments(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerWiseEncashPayments)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

CustomerWiseEncashPayments::~CustomerWiseEncashPayments()
{
    delete ui;
}


void CustomerWiseEncashPayments::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void CustomerWiseEncashPayments::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomerWiseEncashPayments::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomerWiseEncashPayments::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
    ui->detailTable->setColumnCount(5);
    QStringList titles;
    titles <<"Voucher Id"<<"Description"<<"Payment #"<<"Payment Method"<<"Amount";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(DESCRIPTION, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(VOUCHER,100);
    ui->detailTable->setColumnWidth(PAYMENTNUM,100);
    ui->detailTable->setColumnWidth(METHOD,100);
    ui->detailTable->setColumnWidth(AMOUNT,100);
}

void CustomerWiseEncashPayments::on_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->customername,ui->customerid);
}

void CustomerWiseEncashPayments::on_nextButton_clicked()
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
           "<th colspan='5'>"
               "<table width='100%'>"
                   "<tr>"
                       "<th width='50%' align='left'>Customer: "+customername+"</th>"
                       "<th width='25%' align='left'>From Date: "+erp.DDMMYYDateFromSQL(begindate)+"</th>"
                       "<th width='25%' align='left'>To Date: "+erp.DDMMYYDateFromSQL(enddate)+"</th>"
                   "</tr>"
               "</table>"
           "</th>"
           "</tr>"
            //<<"Voucher Id"<<"Description"<<"Payment #"<<"Payment Method"<<"Amount";
           "<tr bgcolor='#EEEEEE'>"
           "<th width='15%'>Voucher Id</th>"
           "<th width='40%'>Description</th>"
           "<th width='15%'>Payment #</th>"
           "<th width='15%'>Payment Method</th>"
           "<th width='15%'>Amount</th>"
           "</tr>"
           "</thead>"
           "<tbody>";


    float grand_total = 0.00;

    QString query = " SELECT DISTINCT (enc.customer_id) AS id, c.name nam FROM  tblencashpaymentdetail enc, "
                    " tblcustomers c WHERE enc.customer_id = c.customerid ";
    if(customerid.toInt()>0)
    {
        query += " and enc.customer_id = '"+customerid+"' ";
    }
    if(ui->begindate->text()!="" && ui->enddate->text()!="")
    {
        query += " and enc.datereceived >= '"+begindate+"' and enc.datereceived <='"+enddate+"' ";
    }
    query += " ORDER BY c.name desc ";
    QSqlQuery info(query);
    //int i;
    int row;
    while(info.next())
    {
        QString customer_name = info.value("nam").toString();
        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;

        QTableWidgetItem *cust = new QTableWidgetItem( customer_name );
        cust->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->setItem(row, VOUCHER,  cust );

        html += "<tr style='font-size:10px;'>"
                "<th colspan='5'>"+customer_name+"</th>"
                "</tr>";
        float total=0.00;
        QSqlQuery info2(" select * from  tblencashpaymentdetail where customer_id = '"+info.value("id").toString()+"'");
        while(info2.next())
        {



        QString voucherid = info2.value("voucherid").toString();
        QString description = info2.value("description").toString();
        QString pmtno = info2.value("pmtno").toString();
        QString paymeth = erp.paymethod(info2.value("paymeth").toString());
        float amount = info2.value("amount").toFloat();

        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;

        QTableWidgetItem *voucher = new QTableWidgetItem( voucherid );
        voucher->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *desc = new QTableWidgetItem( description );
        desc->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        QTableWidgetItem *pmt = new QTableWidgetItem( pmtno );
        pmt->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *pmode = new QTableWidgetItem( paymeth );
        pmode->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *amt = new QTableWidgetItem( erp.amountString( amount ) );
        amt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

        ui->detailTable->setItem(row, VOUCHER,     voucher );
        ui->detailTable->setItem(row, DESCRIPTION, desc );
        ui->detailTable->setItem(row, PAYMENTNUM,  pmt );
        ui->detailTable->setItem(row, METHOD,   pmode );
        ui->detailTable->setItem(row, AMOUNT,  amt );

        html += "<tr style='font-size:10px;'>"
                "<td align='center'>"+voucherid+"</td>"
                "<td align='left'>"+description+"</td>"
                "<td align='center'>"+pmtno+"</td>"
                "<td align='center'>"+paymeth+"</td>"
                "<td align='right'>"+erp.amountString(amount)+"</td>"
                "</tr>";

        total += amount;
        //i = ui->detailTable->rowCount();


        }//end while(info2.next())
        QFont font1("Segoe UI", 9, QFont::Bold);
        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;

        QTableWidgetItem *tot_title = new QTableWidgetItem( "Total:" );
        tot_title->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        tot_title->setFont(font1);

        QTableWidgetItem *tot_amt = new QTableWidgetItem( erp.amountString( total ) );
        tot_amt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        tot_amt->setFont(font1);

        ui->detailTable->setItem(row, VOUCHER,     tot_title );
        ui->detailTable->setSpan(row,VOUCHER,1,4);
        ui->detailTable->setItem(row, AMOUNT,  tot_amt );

        html += "<tr style='font-size:10px;'>"
                "<th align='right' colspan='4'>Total:</th>"
                "<th align='right'>"+erp.amountString(total)+"</th>"
                "</tr>";




        grand_total += total;

    }//end while(info.next())

    ui->detailTable->insertRow(ui->detailTable->rowCount());
    row = ui->detailTable->rowCount()-1;

    QFont font("Segoe UI", 9, QFont::Bold);
    QTableWidgetItem *grandtot_title = new QTableWidgetItem( "Grand Total:" );
    grandtot_title->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    grandtot_title->setFont(font);

    QTableWidgetItem *totamt = new QTableWidgetItem( erp.amountString(grand_total) );
    totamt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totamt->setFont(font);

    ui->detailTable->setItem(row, VOUCHER,    grandtot_title );
    ui->detailTable->setSpan(row, VOUCHER,1,4);
    ui->detailTable->setItem(row, AMOUNT,    totamt );

    html += "<tr>"
            "<th align='right' colspan='4'>Grand Total:</th>"
            "<th align='right'>"+erp.amountString(grand_total)+"</th>"
            "</tr>";

    html += "</tbody></table>";
    this->setCursor(Qt::ArrowCursor);
}

void CustomerWiseEncashPayments::on_printButton_clicked()
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

void CustomerWiseEncashPayments::printPreview(QPrinter *p)
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
