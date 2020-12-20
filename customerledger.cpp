#include "customerledger.h"
#include "ui_customerledger.h"
#include <QPaintEngine>
#include <QPrintEngine>

CustomerLedger::CustomerLedger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerLedger)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadForm()) );
    timer->start(5);
}

CustomerLedger::~CustomerLedger()
{
    delete ui;
}


void CustomerLedger::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void CustomerLedger::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomerLedger::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomerLedger::on_customername_textEdited(const QString &arg1)
{
    QString sql=sch.customers(arg1);
    autocompleter(sql,ui->customername,ui->customerid);
}

void CustomerLedger::loadLedger()
{
    ui->detailTable->setRowCount(0);
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString customerid = ui->customerid->text();
    if(customerid.toInt()==0)
    {
        QMessageBox::critical(this,"Alert","Customer is required");
        return;
    }
    QString sqlob = " select sum(debit) dr, sum(credit) cr from tblgltransaction t, tblgltransvoucher v where t.voucherno=v.voucherno and t.voucherid = v.voucherid and  t.companytype='customer' and t.compid='"+customerid+"' and v.entrydate < '"+begindate+"'  ";
    QSqlQuery resob;
    resob.prepare(sqlob);
    resob.exec();
    resob.next();
    float open_dr = resob.value("dr").toFloat();
    float open_cr = resob.value("cr").toFloat();
    float a_netopen = open_dr - open_cr;
    ui->detailTable->insertRow(ui->detailTable->rowCount());
    int row = ui->detailTable->rowCount()-1;
    ui->detailTable->setSpan(row,DATE,1,5);
    QFont font("Segoe UI", 9, QFont::Bold);
    QTableWidgetItem *obtitle = new QTableWidgetItem( "Opening Balance:" );
    obtitle->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    obtitle->setFont(font);
    QTableWidgetItem *obval = new QTableWidgetItem( erp.amountString(a_netopen) );
    obval->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    obval->setFont(font);
    ui->detailTable->setItem(row,DATE, obtitle );
    ui->detailTable->setItem(row,BALANCE, obval );


    QString sqldc = " select t.description as gldesc,debit dr, credit cr, v.entrydate, t.voucherno, v.description,v.voucherno_parent,t.glaccountid,t.status "
                    " from tblgltransaction t, tblgltransvoucher v  "
                    " where t.voucherno=v.voucherno "
                    " AND t.voucherid = v.voucherid "
                    " and t.companytype='customer' "
                    " and t.compid='"+customerid+"' "
                    " and v.entrydate >= '"+begindate+"' "
                    " and v.entrydate <= '"+enddate+"' "
                    " order by v.entrydate, v.voucherid asc ";
    QSqlQuery qry_dc(sqldc);
    while(qry_dc.next())
    {
        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;

        float dr = qry_dc.value("dr").toFloat();
        float cr = qry_dc.value("cr").toFloat();
        a_netopen = a_netopen+dr-cr;


        QTableWidgetItem *entrydate = new QTableWidgetItem( qry_dc.value("entrydate").toDate().toString("dd/MM/yyyy") );
        entrydate->setTextAlignment(Qt::AlignCenter);
        QTableWidgetItem *voucherno = new QTableWidgetItem( qry_dc.value("voucherno").toString() );
        voucherno->setTextAlignment(Qt::AlignCenter);
        QTableWidgetItem *description = new QTableWidgetItem( qry_dc.value("description").toString() );
        QTableWidgetItem *debit = new QTableWidgetItem( erp.amountString(dr) );
        debit->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        QTableWidgetItem *credit = new QTableWidgetItem( erp.amountString(cr) );
        credit->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        QTableWidgetItem *balance = new QTableWidgetItem( erp.amountString(a_netopen) );
        balance->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);


        ui->detailTable->setItem(row, DATE,          entrydate );
        ui->detailTable->setItem(row, VOUCHER,       voucherno );
        ui->detailTable->setItem(row, DESCRIPTION,   description );
        ui->detailTable->setItem(row, DEBIT,         debit );
        ui->detailTable->setItem(row, CREDIT,        credit );
        ui->detailTable->setItem(row, BALANCE,       balance );

    }


    ui->detailTable->insertRow(ui->detailTable->rowCount());
    row = ui->detailTable->rowCount()-1;
    ui->detailTable->setSpan(row,DATE,1,5);
    QTableWidgetItem *closing_title = new QTableWidgetItem( "Closing Balance:" );
    closing_title->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    closing_title->setFont(font);
    QTableWidgetItem *closing_val = new QTableWidgetItem( erp.amountString(a_netopen) );
    closing_val->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    closing_val->setFont(font);
    ui->detailTable->setItem(row,DATE, closing_title );
    ui->detailTable->setItem(row,BALANCE, closing_val );



}

void CustomerLedger::loadForm()
{
    timer->stop();
    ui->begindate->setDate(erp.reportBeginDate());
    ui->enddate->setDate(QDate::currentDate());
    ui->detailTable->setColumnCount(6);
    QStringList titles;
    titles <<"Date"<<"Voucher"<<"Description"<<"Debit"<<"Credit"<<"Balance";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->setColumnWidth(DATE,70);
    ui->detailTable->setColumnWidth(VOUCHER,70);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(DESCRIPTION, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(DEBIT,80);
    ui->detailTable->setColumnWidth(CREDIT,80);
    ui->detailTable->setColumnWidth(BALANCE,80);

}

void CustomerLedger::on_customerid_textChanged(const QString &arg1)
{
    if(arg1!="" && ui->begindate->text()!="" && ui->enddate->text()!="")
    {
        loadLedger();
    }
}

void CustomerLedger::on_enddate_userDateChanged(const QDate &date)
{
    if(date.toString()!="" && ui->begindate->text()!="" && ui->customerid->text()!="")
    {
        loadLedger();
    }
}

void CustomerLedger::on_begindate_userDateChanged(const QDate &date)
{
    if(date.toString()!="" && ui->enddate->text()!="" && ui->customerid->text()!="")
    {
        loadLedger();
    }
}

void CustomerLedger::on_printButton_clicked()
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString customerid = ui->customerid->text();
    if(customerid.toInt()==0)
    {
        QMessageBox::critical(this,"Alert","Customer is required");
        return;
    }

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


void CustomerLedger::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString customerid = ui->customerid->text();
    if(customerid.toInt()==0)
    {
        QMessageBox::critical(this,"Alert","Customer is required");
        return;
    }

    QString soldToString = "<b>Addres:</b> "+erp.customerField(customerid,"address1")+"<br/><b>Phone:</b> "+erp.customerField(customerid,"phone1")+"<br/>"
                           "<b>Mobile:</b> "+erp.customerField(customerid,"phone3")+"<br/><b>Fax:</b> "+erp.customerField(customerid,"phone5");

/**************************************** Detail *******************************************************/
    QString html;
    html += "<table width='100%'>"
            "<tr>"
            "<td width='45%'>"
            "<table width='100%' border=1 cellspacing=0>"
            "<tr>"
            "<td>"
            "<table width='100%' style='font-size:12px;'>"
            "<tr>"
            "<td><b>Name:</b> "+erp.customerField(customerid,"name")+"<br/><b>City:</b> "+erp.customerField(customerid,"city")+"</td>"
                   "<td rowspan='2' width='10'><br/><br/><br/><br/><br/><br/></td>"
            "</tr>"
            "<tr>"
            "<td>"+soldToString+"</td>"
            "</tr>"
            "</table>"
            "</td>"
            "</tr>"
            "</table>"
            "</td>"
            "<td width='10%'></td>"
            "<td width='45%'>"
            "<table width='100%' border=1 cellspacing=0>"
            "<tr>"
            "<td style='border-left:0px solid #FFFFFF;'>"
            "<table width='100%' style='font-size:12px;'>"
            "<tr>"
            "<td><b>From Date:</b> "+erp.DDMMYYDateFromSQL(begindate)+"</td>"
                   "<td width='10' rowspan='2'><br/><br/><br/><br/><br/><br/></td>"
            "</tr>"
            "<tr>"
            "<td><b>To Date:</b> "+erp.DDMMYYDateFromSQL(enddate)+" "
            ""
            "</td>"
            "</tr>"
            "</table>"
            "</td>"
            "</tr>"
            "</table>"
            "</td>"
            "</tr>"
            "</table>";

    html += "<br><table width='100%' style='font-size:12px;' border=1 bordercolor='#333333' cellspacing='0' cellpadding='3'>"
            "<thead><tr bgcolor='#EEEEEE'>"
            "<th width='12%' style='font-size:12px;'>Date</th>"
            "<th width='12%' style='font-size:12px;'>Voucher</th>"
            "<th width='40%' style='font-size:12px;'>DESCRIPTION</th>"
            "<th width='12%' style='font-size:12px;'>Debit</th>"
            "<th width='12%' style='font-size:12px;'>Credit</th>"
            "<th width='12%' style='font-size:12px;'>Balance</th>"
            "</tr style='font-size:12px;'></thead><tbody>";



    QSqlQuery resob(" select sum(debit) dr, sum(credit) cr from tblgltransaction t, tblgltransvoucher v where t.voucherno=v.voucherno and t.voucherid = v.voucherid and  t.companytype='customer' and t.compid='"+customerid+"' and v.entrydate < '"+begindate+"' ");
    resob.next();
    float open_dr = resob.value("dr").toFloat();
    float open_cr = resob.value("cr").toFloat();
    float a_netopen = open_dr - open_cr;

    html += "<tr>"
            "<th colspan='5' align='right'>Opening Balance</th>"
            "<th align='right'>"+erp.amountString(a_netopen)+"</th>"
            "</tr>";


    QString sqldc = " select t.description as gldesc,debit dr, credit cr, v.entrydate, t.voucherno, v.description,v.voucherno_parent,t.glaccountid,t.status "
                    " from tblgltransaction t, tblgltransvoucher v  "
                    " where t.voucherno=v.voucherno "
                    " AND t.voucherid = v.voucherid "
                    " and t.companytype='customer' "
                    " and t.compid='"+customerid+"' "
                    " and v.entrydate >= '"+begindate+"' "
                    " and v.entrydate <= '"+enddate+"' "
                    " order by v.entrydate, v.voucherid asc ";
    QSqlQuery res_dc(sqldc);
    while(res_dc.next())
    {
        float dr = res_dc.value("dr").toFloat();
        float cr = res_dc.value("cr").toFloat();
        a_netopen = a_netopen+dr-cr;

        html += "<tr>"
                "<td align='center'>"+ res_dc.value("entrydate").toDate().toString("dd/MM/yyyy") +"</td>"
                "<td align='center'>"+ res_dc.value("voucherno").toString() +"</td>"
                "<td>"+ res_dc.value("description").toString() +"</td>"
                "<td align='right'>"+ erp.amountString(dr) +"</td>"
                "<td align='right'>"+ erp.amountString(cr) +"</td>"
                "<td align='right'>"+ erp.amountString(a_netopen) +"</td>"
                "</tr>";
    }

    html += "<tr>"
            "<th colspan='5' align='right'>Closing Balance:</th>"
            "<th align='right'>"+erp.amountString(a_netopen)+"</th>"
            "</tr>";

    html += "</tbody></table>";

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Customer Ledger Report</th>"
                          "</tr>"
                          "<tr><td colspan='3'><hr/></td></tr>"                                                                                                ""                                                                                               ""
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

    //ted->setHtml(getHtmlStr());
//    ted->pageCount();
//    QPointF pageNumberPos;
//    pageNumberPos.setX(10);
//    pageNumberPos.setY(10);
//    ted->setDocumentMargin(25);


//    ted->setHtml(html);
//    ted->print(p);

}
