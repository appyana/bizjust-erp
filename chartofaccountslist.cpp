#include "chartofaccountslist.h"
#include "ui_chartofaccountslist.h"

ChartofAccountsList::ChartofAccountsList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartofAccountsList)
{
    ui->setupUi(this);
    loadReport();
}

ChartofAccountsList::~ChartofAccountsList()
{
    delete ui;
}


void ChartofAccountsList::loadReport()
{
    QStringList titles;
    titles <<"ACCOUNT"<<"DESCRIPTION"<<"ACCOUNT GROUP"<<"ACCOUNT TYPE"<<"SUMMARIZE TO";
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setColumnWidth(ACCOUNT,80);
    ui->tableWidget->setColumnWidth(ACCOUNTGROUP,120);
    ui->tableWidget->setColumnWidth(ACCOUNTTYPE,100);
    ui->tableWidget->setColumnWidth(SUMMARIZETO,150);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(DESCRIPTION, QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(titles);
    QString query = " select * from tblglaccount where cancel = '0' order by name asc ";
    QSqlQuery info(query);
    html = "<table width='100%' style='font-size:10px;' border=1 bordercolor='#333333' cellspacing='0' cellpadding='3'>"
           "<thead>"
           "<tr bgcolor='#f2f2f2'>"
               "<th width='10%'>ACCOUNT</th>"
               "<th width='35%'>DESCRIPTION</th>"
               "<th width='15%'>ACCOUNT GROUP</th>"
               "<th width='10%'>ACCOUNT TYPE</th>"
               "<th width='30%'>SUMMARIZE TO</th>"
           "</tr>"
           "</thead>"
           "<tbody>";
    while(info.next())
    {
        QString accountypeid = info.value("accountypeid").toString();
        QSqlQuery qry("select * from tblaccounttype where id = '"+accountypeid+"' ");
        qry.next();
        QSqlQuery qry2("select * from tblglaccount where id = '"+ info.value("summaryaccountid").toString() +"' ");
        qry2.next();
        QString grouptype = "";
        if(info.value("grouptype").toString()=="detail")
        {
            grouptype = "Detail";
        }
        if(info.value("grouptype").toString()=="group")
        {
            grouptype = "Group";
        }
        QString summaryaccount = qry2.value("name").toString()+" "+qry2.value("description").toString();
        html += "<tr style='font-size:10px;'>"
                    "<td align='center'>"+ info.value("name").toString() +"</td>"
                    "<td>"+ info.value("description").toString() +"</td>"
                    "<td>"+ qry.value("description").toString() +"</td>"
                    "<td align='center'>"+ grouptype +"</td>"
                    "<td>"+ summaryaccount +"</td>"
                "</tr>";
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,ACCOUNT,new QTableWidgetItem( info.value("name").toString() ));
        ui->tableWidget->item(row,ACCOUNT)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row,DESCRIPTION,new QTableWidgetItem( info.value("description").toString() ));
        ui->tableWidget->setItem(row,ACCOUNTGROUP,new QTableWidgetItem( qry.value("description").toString() ));
        ui->tableWidget->setItem(row,ACCOUNTTYPE,new QTableWidgetItem( grouptype ));
        ui->tableWidget->item(row,ACCOUNTTYPE)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row,SUMMARIZETO,new QTableWidgetItem( summaryaccount ));
    }//end while(info.next())
    html += "</tbody></table>";
}

void ChartofAccountsList::on_btn_print_clicked()
{
    printer=new QPrinter();
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QPrinter::A4);
    printer->setPageMargins(QMarginsF(10,10,10,10));
    printDlg=new QPrintDialog(printer);
    pageDlg=new QPageSetupDialog(printer);

    pd = new QPrintPreviewDialog(printer);
    connect(pd,SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));
    pd->setWindowTitle("Chart of Accounts List");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void ChartofAccountsList::printPreview(QPrinter *p)
{
/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Chart of Accounts List</th>"
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
