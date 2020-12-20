#include "journalreports.h"
#include "ui_journalreports.h"

JournalReports::JournalReports(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JournalReports)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

JournalReports::~JournalReports()
{
    delete ui;
}


void JournalReports::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
    QSqlQuery qry("select * from tblmodules where modulename !='Fixed Assets' order by moduleid");
    ui->wherefrom->addItem("Select",QVariant(0));
    while(qry.next())
    {
        ui->wherefrom->addItem(qry.value("modulename").toString(),qry.value("moduleid"));
    }
    on_wherefrom_currentIndexChanged(0);
}

void JournalReports::on_wherefrom_currentIndexChanged(int index)
{
    ui->transTypeList->clear();
    QString wherefrom = ui->wherefrom->itemData(index).toString();
    if(index>0)
    {
        QSqlQuery qry("select * from ( select SUBSTRING_INDEX(voucherno,'-',1) as code from tblgltransvoucher where wherefrom = '"+wherefrom+"' ) as tbl2 group by code ");
        while(qry.next())
        {
            QString code = qry.value("code").toString();
            //QString val = voucherno.split(QRegExp("-")).value(0);
            ui->transTypeList->addItem(code,QVariant(code));
        }
    }
    else
    {
        ui->transTypeList->addItem("All",QVariant("All"));
    }
}

void JournalReports::on_btn_next_clicked()
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
    pd->setWindowTitle("Journal Reports");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void JournalReports::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString wherefrom = ui->wherefrom->itemData(ui->wherefrom->currentIndex()).toString();
    QString transtype = ui->transTypeList->currentText();

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<thead>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='10%'>Voucher</th>"
               "<th width='30%'>Description</th>"
               "<th width='30%'>Account</th>"
               "<th width='10%'>Date Entered</th>"
               "<th width='10%'>Debit</th>"
               "<th width='10%'>Credit</th>"
           "</tr></thead>";

    float total_debit=0;
    float total_credit=0;
    QString query = " select * from tblgltransvoucher where entrydate >= '"+begindate+"' and entrydate <= '"+enddate+"' ";
    if(wherefrom.toInt()>0)
    {
        query += " and wherefrom = '"+wherefrom+"' ";
    }
    if(transtype!="" && transtype!="All")
    {
        query += " and voucherno LIKE '"+transtype+"%' ";
    }
    query += " order by voucherid asc ";
    //qDebug()<<query_2<<endl;

    QSqlQuery info(query);
    while(info.next())
    {
        float voucher_debit_total=0;
        float voucher_credit_total=0;
        QString voucherid = info.value("voucherid").toString();
        QString entrydate = info.value("entrydate").toDate().toString("dd/MM/yyyy");
        QString voucherno = info.value("voucherno").toString();
        QString query9 = " select * from tblgltransvoucher tv, tblgltransaction t where tv.voucherid = t.voucherid and tv.voucherid = '"+voucherid+"' order by id";
        QSqlQuery info2(query9);
        while(info2.next())
        {
            QString description = info2.value("description").toString();
            QString glaccountid = info2.value("glaccountid").toString();
            float debit = info2.value("debit").toFloat();
            float credit = info2.value("credit").toFloat();
            voucher_debit_total += debit;
            voucher_credit_total += credit;
            total_debit += debit;
            total_credit += credit;
            html +="<tr>"
                   "<td align='center'>"+voucherno+"</td>"
                   "<td>"+description+"</td>"
                   "<td>"+erp.getAccount(glaccountid)+"</td>"
                   "<td align='center'>"+entrydate+"</td>"
                   "<td align='right'>"+erp.amountString(debit)+"</td>"
                   "<td align='right'>"+erp.amountString(credit)+"</td>"
                   "</tr>";
        }
        html+="<tr>"
              "<th colspan='4' align='right'>Totals for voucher "+voucherno+":</th>"
              "<th>"+erp.amountString(voucher_debit_total)+"</th>"
              "<th>"+erp.amountString(voucher_credit_total)+"</th>"
              "</tr>";
    }

    html+="<tr>"
          "<th colspan='4' align='right'>Grand Totals:</th>"
          "<th>"+erp.amountString(total_debit)+"</th>"
          "<th>"+erp.amountString(total_credit)+"</th>"
          "</tr>";


    html +="</table>";

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Journal Reports</th>"
                          "</tr>"
                          "<tr><th colspan='3'>From: "+erp.DDMMYYDateFromSQL(begindate)+" To: "+erp.DDMMYYDateFromSQL(enddate)+"</th></tr>"                                                                                                ""                                                                                               ""
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


