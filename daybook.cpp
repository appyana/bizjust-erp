#include "daybook.h"
#include "ui_daybook.h"

DayBook::DayBook(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DayBook)
{
    ui->setupUi(this);
    ui->detailtable->setColumnCount(7);
    QStringList titles;
    titles <<"Voucher #"<<"Account"<<"Description"<<"Entry Date"<<"Receipts"<<"Payments"<<"Balance";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->setColumnWidth(VNO,80);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(ACCOUNT, QHeaderView::Stretch);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(DESCRIPTION, QHeaderView::Stretch);
    ui->detailtable->setColumnWidth(ENTRYDATE,90);
    ui->detailtable->setColumnWidth(RECEIPTS,90);
    ui->detailtable->setColumnWidth(PAYMENTS,90);
    ui->detailtable->setColumnWidth(BALANCE,90);
    ui->date->setDate(QDate::currentDate());
}

DayBook::~DayBook()
{
    delete ui;
}


void DayBook::on_date_userDateChanged(const QDate &date)
{
    ui->detailtable->setRowCount(0);
    QFont font("Segoe UI", 9, QFont::Bold);
    QString todate = date.toString("yyyy-MM-dd");
    QString r_acct72 = erp.getaccountAR("cash");
    QString FinalQuery="SELECT "
                       " SUM(tblgltransaction.debit) AS TotDebit, "
                       " SUM(tblgltransaction.credit) AS TotCredit "
                       " FROM "
                       " tblgltransvoucher "
                       " INNER JOIN tblgltransaction ON (tblgltransvoucher.voucherno = tblgltransaction.voucherno) "
                       " WHERE "
                       " tblgltransvoucher.entrydate<'"+todate+"' AND tblgltransaction.glaccountid='"+r_acct72+"'";
    QSqlQuery data(FinalQuery);
    data.next();
    float netP = data.value("TotDebit").toFloat() - data.value("TotCredit").toFloat();
    int row = ui->detailtable->rowCount();
    ui->detailtable->insertRow(row);
    ui->detailtable->setSpan(row,VNO,1,6);
    ui->detailtable->setItem(row,VNO,new QTableWidgetItem("Opening Balance:"));
    ui->detailtable->item(row,VNO)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    ui->detailtable->item(row,VNO)->setFont(font);
    ui->detailtable->setItem(row,BALANCE,new QTableWidgetItem( erp.amountString(netP) ));
    ui->detailtable->item(row,BALANCE)->setFont(font);


    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<thead>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='10%'>Voucher #</th>"
               "<th width='25%'>Account</th>"
               "<th width='25%'>Description</th>"
               "<th width='10%'>Entry Date</th>"
               "<th width='10%'>Receipts</th>"
               "<th width='10%'>Payments</th>"
               "<th width='10%'>Balance</th>"
           "</tr></thead>"
           "<tr>"
           "<th align='right' colspan='6'>Openong Balance:</th>"
           "<th>"+erp.amountString(netP)+"</th>"
           "</tr>";



    QString ar = erp.getaccountAR("receivables");
    QString ap = erp.getaccountAR("payable");
    QString ep = erp.getaccountAR("labour");
    QString invact = erp.getaccountAR("inventory");
    QString costact = erp.getaccountAR("cost");
    float total_debit=0;
    float total_credit=0;
    QString query9;
    query9 = "(SELECT "
             " t.voucherno, t.glaccountid, "
             " IF(glaccountid='"+ar+"', "
             " (SELECT CONCAT_WS(' - ',tblcustomers.name,tblcustomers.city) "
             " FROM tblcustomers "
             " WHERE tblcustomers.customerid = t.compid), "
             " IF(glaccountid='"+ap+"', "
             " (SELECT CONCAT_WS(' - ',tblvendors.vendorname) "
             " FROM tblvendors "
             " WHERE tblvendors.vendorid = t.compid), "
             " IF(glaccountid='"+ep+"', "
             " (SELECT CONCAT_WS(' - ',tblemp.firstname) "
             " FROM tblemp "
             " WHERE tblemp.empid = t.compid), "
             " (SELECT description "
             " FROM tblglaccount "
             " WHERE id = t.glaccountid LIMIT 0,1) "
             " ) "
             " ) "
             " ) AS glaccountdesc, "
             " t.debit, t.credit, tv.status AS 'status', t.description description, t.entrydate AS trdate, t.compid "
             " FROM tblgltransvoucher tv "
             " INNER JOIN tblgltransaction t "
             " ON (tv.voucherno = t.voucherno) "
             " WHERE tv.entrydate = '"+todate+"' AND substr(tv.voucherno,1,3) != 'pmt' AND substr(tv.voucherno,1,2) != 'vp' "
             " AND glaccountid NOT IN ('"+r_acct72+"', '"+costact+"', '"+invact+"')) "
             " UNION ALL "
             "(SELECT "
             " t.voucherno, t.glaccountid, "
             " IF(glaccountid='"+ar+"', "
             " (SELECT CONCAT_WS(' - ',tblcustomers.name,tblcustomers.city) "
             " FROM tblcustomers "
             " WHERE tblcustomers.customerid = t.compid), "
             " IF(glaccountid='"+ap+"', "
             " (SELECT CONCAT_WS(' - ',tblvendors.vendorname) "
             " FROM tblvendors "
             " WHERE tblvendors.vendorid = t.compid), "
             " IF(glaccountid='"+ep+"', "
             " (SELECT CONCAT_WS(' - ',tblemp.firstname) "
             " FROM tblemp "
             " WHERE tblemp.empid = t.compid), "
             " (SELECT description "
             " FROM tblglaccount "
             " WHERE id = t.glaccountid LIMIT 0,1) "
             " ) "
             " ) "
             " ) AS glaccountdesc, SUM(t.debit) debit, sum(t.credit) credit, tv.status AS 'status', "
             " tv.description   description, t.entrydate AS trdate, t.compid "
             " FROM tblgltransvoucher tv "
             " INNER JOIN tblgltransaction t "
             " ON (tv.voucherno = t.voucherno) "
             " WHERE tv.entrydate = '"+todate+"' AND (substr(tv.voucherno,1,3) = 'pmt' OR substr(tv.voucherno,1,2) = 'vp') "
             " AND glaccountid NOT IN ('"+r_acct72+"', '"+costact+"', '"+invact+"') "
             " GROUP BY t.voucherno, t.glaccountid, t.compid) "
             " order by voucherno ASC, credit DESC, debit DESC ";
    conn.execQuery("SET SQL_BIG_SELECTS=1");
    QSqlQuery info(query9);
    while(info.next())
    {
        QString voucherno = info.value("voucherno").toString();
        QString account = info.value("glaccountdesc").toString();
        QString description = info.value("description").toString();
        QString trdate = info.value("trdate").toDate().toString("dd/MM/yyyy");
        float credit = info.value("credit").toFloat();
        float debit = info.value("debit").toFloat();
        float ledgerBalance=erp.DayBookLedgerbalance( info.value("glaccountid").toString(),info.value("compid").toString(), todate ) ;
        total_credit += credit;
        total_debit += debit;
        row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,VNO,new QTableWidgetItem( voucherno ));
        ui->detailtable->setItem(row,ACCOUNT,new QTableWidgetItem( account ));
        ui->detailtable->setItem(row,DESCRIPTION,new QTableWidgetItem( description ));
        ui->detailtable->setItem(row,ENTRYDATE,new QTableWidgetItem( trdate ));
        ui->detailtable->setItem(row,RECEIPTS,new QTableWidgetItem( erp.amountString(credit) ));
        ui->detailtable->setItem(row,PAYMENTS,new QTableWidgetItem( erp.amountString(debit) ));
        ui->detailtable->setItem(row,BALANCE,new QTableWidgetItem( erp.amountString(ledgerBalance) ));

        html +="<tr>"
                   "<td align='center'>"+voucherno+"</td>"
                   "<td>"+account+"</td>"
                   "<td>"+description+"</td>"
                   "<td align='center'>"+trdate+"</td>"
                   "<td align='right'>"+erp.amountString(credit)+"</td>"
                   "<td align='right'>"+erp.amountString(debit)+"</td>"
                   "<td align='right'>"+erp.amountString(ledgerBalance)+"</td>"
               "</tr>";
    }
    row = ui->detailtable->rowCount();
    ui->detailtable->insertRow(row);
    ui->detailtable->setSpan(row,VNO,1,4);
    ui->detailtable->setItem(row,VNO,new QTableWidgetItem("Closing Balance:"));
    ui->detailtable->item(row,VNO)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    ui->detailtable->item(row,VNO)->setFont(font);
    ui->detailtable->setItem(row,RECEIPTS,new QTableWidgetItem( erp.amountString(total_credit) ));
    ui->detailtable->item(row,RECEIPTS)->setFont(font);
    ui->detailtable->setItem(row,PAYMENTS,new QTableWidgetItem( erp.amountString(total_debit) ));
    ui->detailtable->item(row,PAYMENTS)->setFont(font);
    ui->detailtable->setItem(row,BALANCE,new QTableWidgetItem( erp.amountString(netP+(total_credit-total_debit)) ));
    ui->detailtable->item(row,BALANCE)->setFont(font);

    html += "<tr>"
            "<th align='right' colspan='4'>Closing Balance:</th>"
            "<th>"+erp.amountString(total_credit)+"</th>"
            "<th>"+erp.amountString(total_debit)+"</th>"
            "<th>"+erp.amountString(netP+(total_credit-total_debit))+"</th>"
            "</tr>";
    html +="</table>";
}

void DayBook::on_btn_print_clicked()
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
    pd->setWindowTitle("Day Book");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void DayBook::printPreview(QPrinter *p)
{
    QString date = ui->date->date().toString("yyyy-MM-dd");

/******************************************report detail***********************************************/

    html = html;

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Day Book</th>"
                          "</tr>"
                          "<tr><th colspan='3'>For Date:"+erp.DDMMYYDateFromSQL(date)+"</th></tr>"                                                                                                ""                                                                                               ""
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
