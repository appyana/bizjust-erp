#include "itemwisepurchases.h"
#include "ui_itemwisepurchases.h"

ItemWisePurchases::ItemWisePurchases(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemWisePurchases)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

ItemWisePurchases::~ItemWisePurchases()
{
    delete ui;
}


void ItemWisePurchases::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void ItemWisePurchases::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void ItemWisePurchases::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void ItemWisePurchases::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
}

void ItemWisePurchases::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void ItemWisePurchases::on_btn_next_clicked()
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
    pd->setWindowTitle("Item Wise Purchases");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void ItemWisePurchases::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString itemid = ui->itemid->text();

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<thead>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='5%'>Item Code</th>"
               "<th width='25%'>Item Name</th>"
               "<th width='10%'>Qty Purchased</th>"
               "<th width='10%'>Purchase (Value)</th>"
               "<th width='10%'>Qty Returned</th>"
               "<th width='10%'>Return (Value)</th>"
               "<th width='10%'>Net Qty</th>"
               "<th width='10%'>Net Purchases</th>"
               "<th width='10%'>Avg Price</th>"
           "</tr></thead>";

    int totalqty=0;
    float totalprc=0;
    int totalretqty=0;
    float totalrettot=0;
    float Grandtot=0;
    QString query_2="select i.description, i.itemid, i.itemcode, it.ITDate as rcvdate, t.ItemPriceEach as itempriceach, "
                    " t.Qty as iqty,(t.Qty*t.ItemPriceEach) AS total_ from tblitemtransdet t, tblitems i, "
                    " tblitemtrans it, tblitemcategory s WHERE i.categoryid=s.id  and "
                    " t.ITransID=it.ITransID and t.ITransType=it.ITransType and t.ITransType in ('PI' , 'PR')  "
                    " and i.itemid = t.ItemID ";
    if(itemid!="")
    {
        query_2 += " and i.itemid='"+itemid+"' ";
    }
    query_2 += " group by i.itemid order by i.description asc ";
    //qDebug()<<query_2<<endl;

    QSqlQuery info_2(query_2);
    while(info_2.next())
    {
        QString iid = info_2.value("itemid").toString();
        QString iname = info_2.value("description").toString();
        QString icode = info_2.value("itemcode").toString();

        QString qtys_packing = erp.checkQty(iid,"pi",true,"",begindate,enddate);
        int qtys_units = erp.checkQty(iid,"pi",false,"",begindate,enddate).toInt();
        float amt = erp.checkQtyAmount(iid,"pi","",begindate,enddate);

        QString Returnqtys_packing = erp.checkQty(iid,"pr",true,"",begindate,enddate);
        int Returnqtys_units = erp.checkQty(iid,"pr",false,"",begindate,enddate).toInt();
        float Returnamt = erp.checkQtyAmount(iid,"pr","",begindate,enddate);
        int net_sale_units = qtys_units - Returnqtys_units;
        float net_sale_amount = amt - Returnamt;

        if(amt!=0)
        {
            float avg = net_sale_amount/net_sale_units;
            html+="<tr>"
                  "<td>"+ icode +"</td>"
                  "<td>"+ iname +"</td>"
                  "<td align='center'>"+qtys_packing+"</td>"
                  "<td align='right'>"+erp.amountString(amt)+"</td>"
                  "<td align='center'>"+Returnqtys_packing+"</td>"
                  "<td align='right'>"+erp.amountString(Returnamt)+"</td>"
                  "<td align='right'>"+erp.showStockInPacking(net_sale_units,iid)+"</td>"
                  "<td align='right'>"+erp.amountString(net_sale_amount)+"</td>"
                  "<td align='right'>"+erp.amountString(avg)+"</td>"
                  "</tr>";
            totalqty    = totalqty    + qtys_units;
            totalprc    = totalprc    + amt;
            totalretqty = totalretqty + Returnqtys_units;
            totalrettot = totalrettot + Returnamt;
            Grandtot    = Grandtot    + net_sale_amount;
        }

    }

    html+="<tr>"
          "<th align='right' colspan='2'>Totals:</th>"
          "<th align='center'></th>"
          "<th align='right'>"+erp.amountString(totalprc)+"</th>"
          "<th align='center'></th>"
          "<th align='right'>"+erp.amountString(totalrettot)+"</th>"
          "<th align='right'></th>"
          "<th align='right'>"+erp.amountString(Grandtot)+"</th>"
          "<th></th>"
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
                          "<th style='font-size:16px;'>Item Wise Purchases</th>"
                          "</tr>"
                          "<tr><th colspan='3'>From: "+ erp.DDMMYYDateFromSQL(begindate) +" To: "+ erp.DDMMYYDateFromSQL(enddate) +"</th></tr>"                                                                                                ""                                                                                               ""
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
