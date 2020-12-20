#include "trialbalance.h"
#include "ui_trialbalance.h"

TrialBalance::TrialBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrialBalance)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

TrialBalance::~TrialBalance()
{
    delete ui;
}



void TrialBalance::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
    QSqlQuery qry("select * from tblaccounttype order by id asc");
    ui->accgroup->addItem("All",QVariant(-1));
    while(qry.next())
    {
        ui->accgroup->addItem(qry.value("description").toString(),qry.value("id"));
    }
    on_accgroup_currentIndexChanged(0);
}


void TrialBalance::on_accgroup_currentIndexChanged(int index)
{
    ui->accgroup_sub->clear();
    QString accgroup = ui->accgroup->itemData(index).toString();
    ui->accgroup_sub->addItem("All",QVariant(-1));
    QSqlQuery qry("select * from tblglaccount where accountypeid='"+accgroup+"' and grouptype='group' ");
    while(qry.next())
    {
        ui->accgroup_sub->addItem(qry.value("description").toString(),qry.value("id"));
    }
}


void TrialBalance::on_btn_next_clicked()
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
    pd->setWindowTitle("Trial Balance");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void TrialBalance::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString accgroup = ui->accgroup->itemData(ui->accgroup->currentIndex()).toString();
    QString accgroup_sub = ui->accgroup_sub->itemData(ui->accgroup_sub->currentIndex()).toString();
    //bool zero_bal=true;
    //QString financialyearid = erp._yearid;

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>";

    float grand_netbal_debit = 0 ;
    float grand_netbal_credit = 0 ;
    float grand_beg_debit=0;
    float grand_beg_credit=0;
    float grand_current_debit=0;
    float grand_current_credit=0;

    QString query = "select distinct(t.id), t.description from tblaccounttype t, tblglaccount a, tblgltransaction s where t.id=a.accountypeid and s.glaccountid=a.id order by id";
    QSqlQuery info(query);
    while(info.next())
    {
        QString accountypeid = info.value("id").toString();
        query = "select a.id, a.description, tr.status, tr.entrydate, a.name, sum(t.debit) as debit, sum(t.credit) as credit from tblglaccount a, tblgltransaction t , tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and t.voucherid = tr.voucherid and accountypeid ='"+accountypeid+"' and tr.entrydate <='"+enddate+"' ";
        if(accgroup.toInt() != -1)
        {
            query +=" and t.glaccountid IN (select ID from tblglaccount where accountypeid='"+accgroup+"') " ;
        }
        if(accgroup_sub.toInt() != -1)
        {
            query ="select a.id, a.description, tr.status, tr.entrydate, a.name, sum(t.debit) as debit, sum(t.credit) as credit from tblglaccount a, tblgltransaction t , tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and t.voucherid = tr.voucherid and accountypeid ='"+accountypeid+"' and tr.entrydate <='"+enddate+"' and t.glaccountid IN (select ID from tblglaccount where accountypeid='"+accgroup+"') and a.summaryaccountid='"+accgroup_sub+"' and a.grouptype='detail'" ;
        }
        query +=" group by t.glaccountid " ;

        html += "<tr>"
                "<th colspan='8'>"+info.value("description").toString()+"</th>"
                "</tr>";
        html += "<tr>"
                "<th rowspan='2'>Description</th>"
                "<th rowspan='2'>Acct. No.</th>"
                "<th colspan='2'>Begining</th>"
                "<th colspan='2'>Current</th>"
                "<th colspan='2'>Closing</th>"
                "</tr>"
                "<tr>"
                "<th>Debit</th>"
                "<th>Credit</th>"
                "<th>Debit</th>"
                "<th>Credit</th>"
                "<th>Debit</th>"
                "<th>Credit</th>"
                "</tr>";
        float tot_netbalance_debit=0;
        float tot_netbalance_credit=0;
        float tot_beg_debit=0;
        float tot_beg_credit=0;
        float tot_current_debit=0;
        float tot_current_credit=0;
        QSqlQuery row(query);
        while(row.next())
        {
            float netbalance_debit=0;
            float netbalance_credit=0;
            float current_debit=0;
            float current_credit=0;
            float t_beg_debit=0;
            float t_beg_credit=0;
            float t_netbalance_debit=0;
            float t_netbalance_credit=0;
            QString id = row.value("id").toString();
            netbalance_debit = row.value("debit").toFloat();
            netbalance_credit = row.value("credit").toFloat();
            if(netbalance_debit > netbalance_credit)
            {
                t_netbalance_debit = netbalance_debit-netbalance_credit;
            }
            else if(netbalance_credit > netbalance_debit)
            {
                t_netbalance_credit = netbalance_credit-netbalance_debit;
            }
            float beg_debit = 0 ;
            float beg_credit = 0 ;
            QString query5 = " select sum(r.debit) as debit, sum(r.credit) as credit from tblgltransvoucher v, tblgltransaction r, tblglaccount a where r.glaccountid = a.id and r.voucherno = v.voucherno and r.voucherid = v.voucherid and v.entrydate < '"+begindate+"' and a.id = '"+id+"' group by a.id  ";
            QSqlQuery row5(query5);
            row5.next();
            beg_debit = row5.value("debit").toFloat();
            beg_credit = row5.value("credit").toFloat();
            if(beg_debit > beg_credit)
            {
                t_beg_debit=beg_debit-beg_credit;
            }
            else if(beg_credit > beg_debit)
            {
                t_beg_credit=beg_credit-beg_debit;
            }

            current_debit=netbalance_debit-beg_debit;
            current_credit=netbalance_credit-beg_credit;

            tot_netbalance_debit += t_netbalance_debit;
            tot_netbalance_credit += t_netbalance_credit;

            tot_beg_debit +=t_beg_debit;
            tot_beg_credit +=t_beg_credit;

            tot_current_debit +=current_debit;
            tot_current_credit +=current_credit;

            html += "<tr>"
                    "<td>"+row.value("description").toString()+"</td>"
                    "<td>"+row.value("name").toString()+"</td>"
                    "<td>"+erp.amountString(t_beg_debit)+"</td>"
                    "<td>"+erp.amountString(t_beg_credit)+"</td>"
                    "<td>"+erp.amountString(current_debit)+"</td>"
                    "<td>"+erp.amountString(current_credit)+"</td>"
                    "<td>"+erp.amountString(t_netbalance_debit)+"</td>"
                    "<td>"+erp.amountString(t_netbalance_credit)+"</td>"
                    "</tr>";
        }// end inner while loop

        html += "<tr>"
                "<th colspan='2' align='right'>Total "+info.value("description").toString()+":</th>"
                "<th>"+erp.amountString(tot_beg_debit)+"</th>"
                "<th>"+erp.amountString(tot_beg_credit)+"</th>"
                "<th>"+erp.amountString(tot_current_debit)+"</th>"
                "<th>"+erp.amountString(tot_current_credit)+"</th>"
                "<th>"+erp.amountString(tot_netbalance_debit)+"</th>"
                "<th>"+erp.amountString(tot_netbalance_credit)+"</th>"
                "</tr>";

        grand_netbal_debit  += tot_netbalance_debit;
        grand_netbal_credit += tot_netbalance_credit;

        grand_beg_debit += tot_beg_debit;
        grand_beg_credit += tot_beg_credit;

        grand_current_debit +=tot_current_debit;
        grand_current_credit +=tot_current_credit;

    }

    html += "<tr>"
            "<th colspan='2' align='right'>Grand Total:</th>"
            "<th>"+erp.amountString(grand_beg_debit)+"</th>"
            "<th>"+erp.amountString(grand_beg_credit)+"</th>"
            "<th>"+erp.amountString(grand_current_debit)+"</th>"
            "<th>"+erp.amountString(grand_current_credit)+"</th>"
            "<th>"+erp.amountString(grand_netbal_debit)+"</th>"
            "<th>"+erp.amountString(grand_netbal_credit)+"</th>"
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
                          "<th style='font-size:16px;'>Trial Balance</th>"
                          "</tr>"
                          "<tr><th colspan='3'>From: "+erp.DDMMYYDateFromSQL(begindate)+" To: "+erp.DDMMYYDateFromSQL(enddate)+"</th></tr>"
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
