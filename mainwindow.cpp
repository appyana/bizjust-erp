#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addcustomer.h"
#include "editcustomerform.h"
#include "customerlist.h"
#include "addsalesinvoice.h"
#include "editsalesinvoice.h"
#include "printsalesinvoice.h"
#include "addsalesreturn.h"
#include "editsalesreturn.h"
#include "printsalesreturn.h"
#include "customerreceipt.h"
#include "editcustomerreceipt.h"
#include "addcustomercashreceipt.h"
#include "editcustomercashreceipt.h"
#include "chequepurchieencashed.h"
#include "editchequeparchieencashed.h"
#include "customerledger.h"
#include "customersalesandrecovery.h"
#include "customersbalance.h"
#include "summaryofinvoices.h"
#include "customerpaymentreceiptreport.h"
#include "recentcustomers.h"
#include "outstandingchequespurchie.h"
#include "summaryofinvoicesindetail.h"
#include "customerwiseencashpayments.h"
#include "customerannualsale.h"
#include "monthwisesaleschart.h"
#include "customersitemwisesale.h"
#include "itemscustomerwisesale.h"
#include "customerwiseprofitanalysis.h"
#include "itemsmonthwisesales.h"
#include "itemsmonthwisesalespurchases.h"
#include "itemwisesales.h"
#include "daywisesalessummary.h"
#include "customersalesanalysis.h"
#include "managesalesman.h"
#include "managecarrier.h"
#include "managecustomergroups.h"
#include "managesalescategories.h"
#include "manageunits.h"
#include "addcustomeropeningbalance.h"
#include "editcustomeropeningbalance.h"
#include "arstandardglaccounts.h"
#include "additem.h"
#include "edititem.h"
#include "itemslist.h"
#include "stockadjustment.h"
#include "editstockadjustment.h"
#include "addinitialstock.h"
#include "editinitialstock.h"
#include "stockreport.h"
#include "locationwisestock.h"
#include "stockreportsellingprice.h"
#include "stockreportpurchaseprice.h"
#include "itemledger.h"
#include "managelocations.h"
#include "manageitemcategory.h"
#include "manageitemsubcategory.h"
#include "manageitembrands.h"
#include "invstandardglaccounts.h"
#include "addvendor.h"
#include "editvendor.h"
#include "vendorslist.h"
#include "addpurchaseinvoice.h"
#include "editpurchaseinvoice.h"
#include "addpurchasereturn.h"
#include "editpurchasereturn.h"
#include "addvendorpayment.h"
#include "editvendorpayment.h"
#include "vendorledger.h"
#include "purchaseinvoiceslist.h"
#include "purchasedetailreport.h"
#include "itemwisepurchases.h"
#include "purchasereturnlist.h"
#include "vendorsbalancereport.h"
#include "vendorpaymentreport.h"
#include "managevendorgroups.h"
#include "addvendoropeningbalance.h"
#include "editvendoropeningbalance.h"
#include "apstandardglaccounts.h"
#include "addglaccount.h"
#include "editglaccount.h"
#include "chartofaccountslist.h"
#include "addcashreceipt.h"
#include "editcashreceipt.h"
#include "addcashpayment.h"
#include "editcashpayment.h"
#include "addjournalvoucher.h"
#include "editjournalvoucher.h"
#include "accountledger.h"
#include "daybook.h"
#include "journalreports.h"
#include "accountsbalancesummary.h"
#include "trialbalance.h"
#include "profitandloss.h"
#include "balancesheet.h"
#include "addglopeningbalance.h"
#include "editglopeningbalance.h"
#include "standardglaccounts.h"
#include "databasebackup.h"
#include "userlog.h"
#include "financialyear.h"
#include "manageusers.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //conn.connOpen();
    ui->setupUi(this);
    ui->mainTabWidget->tabBar()->setTabButton(0,QTabBar::RightSide,0);
    ui->statusBar->setVisible(false);
    loadCustomerBalance();
    loadOutStandingCheques();
    //checkmenues();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkmenues()) );
    timer->start(5);
}

MainWindow::~MainWindow()
{
    conn.connClose();
    delete ui;
}

void MainWindow::checkmenues()
{
    timer->stop();
    if(erp.loginid=="1")
    {
        return;
    }
    if(erp.checkMainMenuRights("menuSales")){ ui->menuSales->setEnabled(false); }
    if(erp.checkMainMenuRights("menuInventory")){ ui->menuInventory->setEnabled(false); }
    if(erp.checkMainMenuRights("menuPurchases")){ ui->menuPurchases->setEnabled(false); }
    if(erp.checkMainMenuRights("menuLedger")){ ui->menuLedger->setEnabled(false); }
    if(erp.checkMainMenuRights("menuAdmin")){ ui->menuAdmin->setEnabled(false); }

    if(erp.checkSubMenuRights("menuCustomers")){ ui->menuCustomers->setEnabled(false); }
    if(erp.checkSubMenuRights("menuInvoices")){ ui->menuInvoices->setEnabled(false); }
    if(erp.checkSubMenuRights("menuSales_Return")){ ui->menuSales_Return->setEnabled(false); }
    if(erp.checkSubMenuRights("menuReceipts")){ ui->menuReceipts->setEnabled(false); }
    if(erp.checkSubMenuRights("menuReports")){ ui->menuReports->setEnabled(false); }
    if(erp.checkSubMenuRights("menuSales_Analysis")){ ui->menuSales_Analysis->setEnabled(false); }
    if(erp.checkSubMenuRights("menuSetup")){ ui->menuSetup->setEnabled(false); }
    if(erp.checkSubMenuRights("menuItems")){ ui->menuItems->setEnabled(false); }
    if(erp.checkSubMenuRights("menuProcess")){ ui->menuProcess->setEnabled(false); }
    if(erp.checkSubMenuRights("menuInvReports")){ ui->menuInvReports->setEnabled(false); }
    if(erp.checkSubMenuRights("menuSetupInv")){ ui->menuSetupInv->setEnabled(false); }
    if(erp.checkSubMenuRights("menuVendors")){ ui->menuVendors->setEnabled(false); }
    if(erp.checkSubMenuRights("menuPurchasing")){ ui->menuPurchasing->setEnabled(false); }
    if(erp.checkSubMenuRights("menuPayments")){ ui->menuPayments->setEnabled(false); }
    if(erp.checkSubMenuRights("menuReportsAP")){ ui->menuReportsAP->setEnabled(false); }
    if(erp.checkSubMenuRights("menuSetupAP")){ ui->menuSetupAP->setEnabled(false); }
    if(erp.checkSubMenuRights("menuChart_of_Accounts")){ ui->menuChart_of_Accounts->setEnabled(false); }
    if(erp.checkSubMenuRights("menuJournal_Entries")){ ui->menuJournal_Entries->setEnabled(false); }
    if(erp.checkSubMenuRights("menuReportsJV")){ ui->menuReportsJV->setEnabled(false); }
    if(erp.checkSubMenuRights("menuSetupJV")){ ui->menuSetupJV->setEnabled(false); }
    if(erp.checkSubMenuRights("menuSetupAdmin")){ ui->menuSetupAdmin->setEnabled(false); }

    if(erp.checkRights("actionAdd_Customer")){ ui->actionAdd_Customer->setEnabled(false); }
    if(erp.checkRights("actionEdit_Customer")){ ui->actionEdit_Customer->setEnabled(false); }
    if(erp.checkRights("actionCustomer_List")){ ui->actionCustomer_List->setEnabled(false); }
    if(erp.checkRights("actionAdd_Sales_Invoice")){ ui->actionAdd_Sales_Invoice->setEnabled(false); }
    if(erp.checkRights("actionEdit_Sales_Invoice")){ ui->actionEdit_Sales_Invoice->setEnabled(false); }
    if(erp.checkRights("actionPrint_Sales_Invoice")){ ui->actionPrint_Sales_Invoice->setEnabled(false); }
    if(erp.checkRights("actionAdd_Sales_Return")){ ui->actionAdd_Sales_Return->setEnabled(false); }
    if(erp.checkRights("actionEdit_Sales_Return")){ ui->actionEdit_Sales_Return->setEnabled(false); }
    if(erp.checkRights("actionPrint_Sales_Return")){ ui->actionPrint_Sales_Return->setEnabled(false); }
    if(erp.checkRights("actionAdd_Customer_Receipt")){ ui->actionAdd_Customer_Receipt->setEnabled(false); }
    if(erp.checkRights("actionEdit_Customer_Receipt")){ ui->actionEdit_Customer_Receipt->setEnabled(false); }
    if(erp.checkRights("actionCustomer_Cash_Receipt")){ ui->actionCustomer_Cash_Receipt->setEnabled(false); }
    if(erp.checkRights("actionEdit_Customer_Cash_Receipt")){ ui->actionEdit_Customer_Cash_Receipt->setEnabled(false); }
    if(erp.checkRights("actionCheque_Parchi_Encashed")){ ui->actionCheque_Parchi_Encashed->setEnabled(false); }
    if(erp.checkRights("actionEdit_Cheque_Parchie_Encashed")){ ui->actionEdit_Cheque_Parchie_Encashed->setEnabled(false); }
    if(erp.checkRights("actionCustomer_Ledger")){ ui->actionCustomer_Ledger->setEnabled(false); }
    if(erp.checkRights("actionAll_Customers_Sales_Recovery")){ ui->actionAll_Customers_Sales_Recovery->setEnabled(false); }
    if(erp.checkRights("actionCustomers_Balance")){ ui->actionCustomers_Balance->setEnabled(false); }
    if(erp.checkRights("actionSummary_of_Invoices")){ ui->actionSummary_of_Invoices->setEnabled(false); }
    if(erp.checkRights("actionCustomer_Payment_Receipt_Report")){ ui->actionCustomer_Payment_Receipt_Report->setEnabled(false); }
    if(erp.checkRights("actionRecent_Customers")){ ui->actionRecent_Customers->setEnabled(false); }
    if(erp.checkRights("actionOut_Standing_Cheque_Purchie")){ ui->actionOut_Standing_Cheque_Purchie->setEnabled(false); }
    if(erp.checkRights("actionSummary_of_Invoices_in_Detail")){ ui->actionSummary_of_Invoices_in_Detail->setEnabled(false); }
    if(erp.checkRights("actionCustomer_Wise_Encash_Payments")){ ui->actionCustomer_Wise_Encash_Payments->setEnabled(false); }
    if(erp.checkRights("actionCustomer_Annual_Sale")){ ui->actionCustomer_Annual_Sale->setEnabled(false); }
    if(erp.checkRights("actionMonth_Wise_Sales_Chart")){ ui->actionMonth_Wise_Sales_Chart->setEnabled(false); }
    if(erp.checkRights("actionCustomer_s_Item_Wise_Sale")){ ui->actionCustomer_s_Item_Wise_Sale->setEnabled(false); }
    if(erp.checkRights("actionItem_s_Customer_Wise_Sale")){ ui->actionItem_s_Customer_Wise_Sale->setEnabled(false); }
    if(erp.checkRights("actionCustomer_Wise_Profit_Analysis")){ ui->actionCustomer_Wise_Profit_Analysis->setEnabled(false); }
    if(erp.checkRights("actionItem_s_Month_Wise_Sales")){ ui->actionItem_s_Month_Wise_Sales->setEnabled(false); }
    if(erp.checkRights("actionItems_Month_Wise_Sales_Purchases")){ ui->actionItems_Month_Wise_Sales_Purchases->setEnabled(false); }
    if(erp.checkRights("actionItem_Wise_Sales")){ ui->actionItem_Wise_Sales->setEnabled(false); }
    if(erp.checkRights("actionDay_Wise_Sales_Summary")){ ui->actionDay_Wise_Sales_Summary->setEnabled(false); }
    if(erp.checkRights("actionCustomer_Sales_Analysis")){ ui->actionCustomer_Sales_Analysis->setEnabled(false); }
    if(erp.checkRights("actionManage_Sales_Man")){ ui->actionManage_Sales_Man->setEnabled(false); }
    if(erp.checkRights("actionManage_Carrier")){ ui->actionManage_Carrier->setEnabled(false); }
    if(erp.checkRights("actionManage_Customer_Groups")){ ui->actionManage_Customer_Groups->setEnabled(false); }
    if(erp.checkRights("actionManage_Sales_Categories")){ ui->actionManage_Sales_Categories->setEnabled(false); }
    if(erp.checkRights("actionManage_Units")){ ui->actionManage_Units->setEnabled(false); }
    if(erp.checkRights("actionAdd_Customer_Opening_Balance")){ ui->actionAdd_Customer_Opening_Balance->setEnabled(false); }
    if(erp.checkRights("actionEdit_Customer_Opening_Balance")){ ui->actionEdit_Customer_Opening_Balance->setEnabled(false); }
    if(erp.checkRights("actionAR_Standard_GL_Accounts")){ ui->actionAR_Standard_GL_Accounts->setEnabled(false); }
    if(erp.checkRights("actionAdd_New_Item")){ ui->actionAdd_New_Item->setEnabled(false); }
    if(erp.checkRights("actionEdit_Item")){ ui->actionEdit_Item->setEnabled(false); }
    if(erp.checkRights("actionItems_List")){ ui->actionItems_List->setEnabled(false); }
    if(erp.checkRights("actionStock_Adjustment")){ ui->actionStock_Adjustment->setEnabled(false); }
    if(erp.checkRights("actionEdit_Stock_Adjustment")){ ui->actionEdit_Stock_Adjustment->setEnabled(false); }
    if(erp.checkRights("actionAdd_Initial_Stock")){ ui->actionAdd_Initial_Stock->setEnabled(false); }
    if(erp.checkRights("actionEdit_Initial_Stock")){ ui->actionEdit_Initial_Stock->setEnabled(false); }
    if(erp.checkRights("actionStock_Report")){ ui->actionStock_Report->setEnabled(false); }
    if(erp.checkRights("actionLocation_Wise_Stock")){ ui->actionLocation_Wise_Stock->setEnabled(false); }
    if(erp.checkRights("actionStock_Report_Selling_Price")){ ui->actionStock_Report_Selling_Price->setEnabled(false); }
    if(erp.checkRights("actionStock_Report_Purchase_Price")){ ui->actionStock_Report_Purchase_Price->setEnabled(false); }
    if(erp.checkRights("actionItem_Ledger")){ ui->actionItem_Ledger->setEnabled(false); }
    if(erp.checkRights("actionManage_Units_2")){ ui->actionManage_Units_2->setEnabled(false); }
    if(erp.checkRights("actionManage_Locations")){ ui->actionManage_Locations->setEnabled(false); }
    if(erp.checkRights("actionManage_Item_Category")){ ui->actionManage_Item_Category->setEnabled(false); }
    if(erp.checkRights("actionManage_Item_Sub_Category")){ ui->actionManage_Item_Sub_Category->setEnabled(false); }
    if(erp.checkRights("actionManage_Item_Brands")){ ui->actionManage_Item_Brands->setEnabled(false); }
    if(erp.checkRights("actionInvStandard_GL_Accounts")){ ui->actionInvStandard_GL_Accounts->setEnabled(false); }
    if(erp.checkRights("actionAdd_Vendor")){ ui->actionAdd_Vendor->setEnabled(false); }
    if(erp.checkRights("actionEdit_Vendor")){ ui->actionEdit_Vendor->setEnabled(false); }
    if(erp.checkRights("actionVendors_List")){ ui->actionVendors_List->setEnabled(false); }
    if(erp.checkRights("actionAdd_Purchase_Invoice")){ ui->actionAdd_Purchase_Invoice->setEnabled(false); }
    if(erp.checkRights("actionEdit_Purchase_Invoice")){ ui->actionEdit_Purchase_Invoice->setEnabled(false); }
    if(erp.checkRights("actionAdd_Purchase_Return")){ ui->actionAdd_Purchase_Return->setEnabled(false); }
    if(erp.checkRights("actionEdit_Purchase_Return")){ ui->actionEdit_Purchase_Return->setEnabled(false); }
    if(erp.checkRights("actionAdd_Vendor_Payment")){ ui->actionAdd_Vendor_Payment->setEnabled(false); }
    if(erp.checkRights("actionEdit_Vendor_Payment")){ ui->actionEdit_Vendor_Payment->setEnabled(false); }
    if(erp.checkRights("actionVendor_Ledger")){ ui->actionVendor_Ledger->setEnabled(false); }
    if(erp.checkRights("actionPurchase_Invoices_List")){ ui->actionPurchase_Invoices_List->setEnabled(false); }
    if(erp.checkRights("actionPurchase_Detail_Report")){ ui->actionPurchase_Detail_Report->setEnabled(false); }
    if(erp.checkRights("actionItem_Wise_Purchases")){ ui->actionItem_Wise_Purchases->setEnabled(false); }
    if(erp.checkRights("actionPurchase_Return_List")){ ui->actionPurchase_Return_List->setEnabled(false); }
    if(erp.checkRights("actionVendors_Balance_Report")){ ui->actionVendors_Balance_Report->setEnabled(false); }
    if(erp.checkRights("actionVendor_Payment_Report")){ ui->actionVendor_Payment_Report->setEnabled(false); }
    if(erp.checkRights("actionManage_Vendor_Groups")){ ui->actionManage_Vendor_Groups->setEnabled(false); }
    if(erp.checkRights("actionAdd_Vendor_Opening_Balance")){ ui->actionAdd_Vendor_Opening_Balance->setEnabled(false); }
    if(erp.checkRights("actionEdit_Vendor_Opening_Balance")){ ui->actionEdit_Vendor_Opening_Balance->setEnabled(false); }
    if(erp.checkRights("actionAPStandard_GL_Accounts")){ ui->actionAPStandard_GL_Accounts->setEnabled(false); }
    if(erp.checkRights("actionAdd_GL_Account")){ ui->actionAdd_GL_Account->setEnabled(false); }
    if(erp.checkRights("actionEdit_GL_Account")){ ui->actionEdit_GL_Account->setEnabled(false); }
    if(erp.checkRights("actionChart_of_Accounts_List")){ ui->actionChart_of_Accounts_List->setEnabled(false); }
    if(erp.checkRights("actionAdd_Cash_Receipt")){ ui->actionAdd_Cash_Receipt->setEnabled(false); }
    if(erp.checkRights("actionEdit_Cash_Receipt")){ ui->actionEdit_Cash_Receipt->setEnabled(false); }
    if(erp.checkRights("actionAdd_Cash_Payment")){ ui->actionAdd_Cash_Payment->setEnabled(false); }
    if(erp.checkRights("actionEdit_Cash_Payment")){ ui->actionEdit_Cash_Payment->setEnabled(false); }
    if(erp.checkRights("actionAdd_Journal_Voucher")){ ui->actionAdd_Journal_Voucher->setEnabled(false); }
    if(erp.checkRights("actionEdit_Journal_Voucher")){ ui->actionEdit_Journal_Voucher->setEnabled(false); }
    if(erp.checkRights("actionAccount_Ledger")){ ui->actionAccount_Ledger->setEnabled(false); }
    if(erp.checkRights("actionDay_Book")){ ui->actionDay_Book->setEnabled(false); }
    if(erp.checkRights("actionJournal_Reports")){ ui->actionJournal_Reports->setEnabled(false); }
    if(erp.checkRights("actionAccounts_Balance_Summary")){ ui->actionAccounts_Balance_Summary->setEnabled(false); }
    if(erp.checkRights("actionTrial_Balance")){ ui->actionTrial_Balance->setEnabled(false); }
    if(erp.checkRights("actionProfit_and_Loss")){ ui->actionProfit_and_Loss->setEnabled(false); }
    if(erp.checkRights("actionBalance_Sheet")){ ui->actionBalance_Sheet->setEnabled(false); }
    if(erp.checkRights("actionAdd_GL_Opening_Balance")){ ui->actionAdd_GL_Opening_Balance->setEnabled(false); }
    if(erp.checkRights("actionEdit_GL_Opening_Balance")){ ui->actionEdit_GL_Opening_Balance->setEnabled(false); }
    if(erp.checkRights("actionStandard_GL_Accounts")){ ui->actionStandard_GL_Accounts->setEnabled(false); }
    if(erp.checkRights("actionDatabase_Backup")){ ui->actionDatabase_Backup->setEnabled(false); }
    if(erp.checkRights("actionUser_Log")){ ui->actionUser_Log->setEnabled(false); }
    if(erp.checkRights("actionFinancial_Year")){ ui->actionFinancial_Year->setEnabled(false); }
    if(erp.checkRights("actionManage_Users")){ ui->actionManage_Users->setEnabled(false); }
}

void MainWindow::on_actionAdd_Customer_triggered()
{
    QString title = "Add Customer";
    if(isNotTabOpened(title))
    {
        AddCustomer *mw = new AddCustomer(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }

}

bool MainWindow::isNotTabOpened(QString title)
{
    int tot = ui->mainTabWidget->count();
    for(int i=0; i<tot; i++)
    {
        QString tname = ui->mainTabWidget->tabText(i);
        if(tname==title)
        {
            ui->mainTabWidget->setCurrentIndex(i);
            return false;
        }
    }
    return true;
}

void MainWindow::on_actionEdit_Customer_triggered()
{
//    EditCustomer w;
//    w.loginid = loginid;
//    w.setModal(true);
//    w.exec();
    QString title = "Edit Customer";
    if(isNotTabOpened(title))
    {
        EditCustomerForm *mw = new EditCustomerForm(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_List_triggered()
{
//    CustomerList w;
//    w.loginid = loginid;
//    w.setModal(true);
//    w.exec();
    QString title = "Customers List";
    if(isNotTabOpened(title))
    {
        CustomerList *mw = new CustomerList(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Sales_Invoice_triggered()
{
    //AddSalesInvoice w;
    //w.loginid = loginid;
    //w.setModal(true);
    //w.exec();
    QString title = "Add Sales Invoice";
    if(isNotTabOpened(title))
    {
        AddSalesInvoice *mw = new AddSalesInvoice(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_mainTabWidget_tabCloseRequested(int index)
{
    ui->mainTabWidget->removeTab(index);
}

void MainWindow::on_actionEdit_Sales_Invoice_triggered()
{
    QString title = "Edit Sales Invoice";
    if(isNotTabOpened(title))
    {
        EditSalesInvoice *mw = new EditSalesInvoice(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionPrint_Sales_Invoice_triggered()
{
    QString title = "Print Sales Invoice";
    if(isNotTabOpened(title))
    {
        PrintSalesInvoice *mw = new PrintSalesInvoice(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Sales_Return_triggered()
{
    QString title = "Add Sales Return";
    if(isNotTabOpened(title))
    {
        AddSalesReturn *mw = new AddSalesReturn(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Sales_Return_triggered()
{
    QString title = "Edit Sales Return";
    if(isNotTabOpened(title))
    {
        EditSalesReturn *mw = new EditSalesReturn(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionPrint_Sales_Return_triggered()
{
    QString title = "Print Sales Return";
    if(isNotTabOpened(title))
    {
        PrintSalesReturn *mw = new PrintSalesReturn(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Customer_Receipt_triggered()
{
    QString title = "Add Customer Receipt";
    if(isNotTabOpened(title))
    {
        CustomerReceipt *mw = new CustomerReceipt(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Customer_Receipt_triggered()
{
    QString title = "Edit Customer Receipt";
    if(isNotTabOpened(title))
    {
        EditCustomerReceipt *mw = new EditCustomerReceipt(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_Cash_Receipt_triggered()
{
    QString title = "Customer Cash Receipt";
    if(isNotTabOpened(title))
    {
        AddCustomerCashReceipt *mw = new AddCustomerCashReceipt(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Customer_Cash_Receipt_triggered()
{
    QString title = "Edit Customer Cash Receipt";
    if(isNotTabOpened(title))
    {
        EditCustomerCashReceipt *mw = new EditCustomerCashReceipt(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCheque_Parchi_Encashed_triggered()
{
    QString title = "Cheque/Purchie Encashed";
    if(isNotTabOpened(title))
    {
        ChequePurchieEncashed *mw = new ChequePurchieEncashed(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Cheque_Parchie_Encashed_triggered()
{
    QString title = "Edit Cheque/Purchie Encashed";
    if(isNotTabOpened(title))
    {
        EditChequeParchieEncashed *mw = new EditChequeParchieEncashed(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_Ledger_triggered()
{
    QString title = "Customer Ledger";
    if(isNotTabOpened(title))
    {
        CustomerLedger *mw = new CustomerLedger(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAll_Customers_Sales_Recovery_triggered()
{
    QString title = "Customer Sales and Recovery";
    if(isNotTabOpened(title))
    {
        CustomerSalesAndRecovery *mw = new CustomerSalesAndRecovery(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomers_Balance_triggered()
{
    QString title = "Customers Balance";
    if(isNotTabOpened(title))
    {
        CustomersBalance *mw = new CustomersBalance(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionSummary_of_Invoices_triggered()
{
    QString title = "Summary of Invoices";
    if(isNotTabOpened(title))
    {
        SummaryOfInvoices *mw = new SummaryOfInvoices(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_Payment_Receipt_Report_triggered()
{
    QString title = "Customer Payment Receipt Report";
    if(isNotTabOpened(title))
    {
        CustomerPaymentReceiptReport *mw = new CustomerPaymentReceiptReport(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionRecent_Customers_triggered()
{
    QString title = "Recent Customers";
    if(isNotTabOpened(title))
    {
        RecentCustomers *mw = new RecentCustomers(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionOut_Standing_Cheque_Purchie_triggered()
{
    QString title = "Out Standing Cheques/Purchie";
    if(isNotTabOpened(title))
    {
        OutStandingChequesPurchie *mw = new OutStandingChequesPurchie(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionSummary_of_Invoices_in_Detail_triggered()
{
    QString title = "Summary of Invoices in Detail";
    if(isNotTabOpened(title))
    {
        SummaryOfInvoicesInDetail *mw = new SummaryOfInvoicesInDetail(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_Wise_Encash_Payments_triggered()
{
    QString title = "Customer Wise Encashed Payments";
    if(isNotTabOpened(title))
    {
        CustomerWiseEncashPayments *mw = new CustomerWiseEncashPayments(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_Annual_Sale_triggered()
{
    QString title = "Customer Annual Sale";
    if(isNotTabOpened(title))
    {
        CustomerAnnualSale *mw = new CustomerAnnualSale(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionMonth_Wise_Sales_Chart_triggered()
{
    QString title = "Month Wise Sales/Recovery Chart";
    if(isNotTabOpened(title))
    {
        MonthWiseSalesChart *mw = new MonthWiseSalesChart(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_s_Item_Wise_Sale_triggered()
{
    QString title = "Customer's Item Wise Sale";
    if(isNotTabOpened(title))
    {
        CustomersItemWiseSale *mw = new CustomersItemWiseSale(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionItem_s_Customer_Wise_Sale_triggered()
{
    QString title = "Item's Customer Wise Sale";
    if(isNotTabOpened(title))
    {
        ItemsCustomerWiseSale *mw = new ItemsCustomerWiseSale(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_Wise_Profit_Analysis_triggered()
{
    QString title = "Customer Wise Profit Analysis";
    if(isNotTabOpened(title))
    {
        CustomerWiseProfitAnalysis *mw = new CustomerWiseProfitAnalysis(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionItem_s_Month_Wise_Sales_triggered()
{
    QString title = "Item's Month Wise Sales";
    if(isNotTabOpened(title))
    {
        ItemsMonthWiseSales *mw = new ItemsMonthWiseSales(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionItems_Month_Wise_Sales_Purchases_triggered()
{
    QString title = "Item's Month Wise Sales/Purchases";
    if(isNotTabOpened(title))
    {
        ItemsMonthWiseSalesPurchases *mw = new ItemsMonthWiseSalesPurchases(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionItem_Wise_Sales_triggered()
{
    QString title = "Item Wise Sales";
    if(isNotTabOpened(title))
    {
        ItemWiseSales *mw = new ItemWiseSales(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionDay_Wise_Sales_Summary_triggered()
{
    QString title = "Day Wise Sales Summary";
    if(isNotTabOpened(title))
    {
        DayWiseSalesSummary *mw = new DayWiseSalesSummary(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionCustomer_Sales_Analysis_triggered()
{
    QString title = "Customer Sales Analysis";
    if(isNotTabOpened(title))
    {
        CustomerSalesAnalysis *mw = new CustomerSalesAnalysis(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Sales_Man_triggered()
{
    //Manage Sales Man
    QString title = "Manage Sales Man";
    if(isNotTabOpened(title))
    {
        ManageSalesMan *mw = new ManageSalesMan(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Carrier_triggered()
{
    //Manage Carrier
    QString title = "Manage Carrier";
    if(isNotTabOpened(title))
    {
        ManageCarrier *mw = new ManageCarrier(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Customer_Groups_triggered()
{
    //Manage Customer Groups
    QString title = "Manage Customer Groups";
    if(isNotTabOpened(title))
    {
        ManageCustomerGroups *mw = new ManageCustomerGroups(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Sales_Categories_triggered()
{
    //Manage Sales Categories
    QString title = "Manage Sales Categories";
    if(isNotTabOpened(title))
    {
        ManageSalesCategories *mw = new ManageSalesCategories(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Units_triggered()
{
    //Manage Units
    QString title = "Manage Units";
    if(isNotTabOpened(title))
    {
        ManageUnits *mw = new ManageUnits(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Customer_Opening_Balance_triggered()
{
    //Add Customer Opening Balance
    QString title = "Add Customer Opening Balance";
    if(isNotTabOpened(title))
    {
        AddCustomerOpeningBalance *mw = new AddCustomerOpeningBalance(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Customer_Opening_Balance_triggered()
{
    //Edit Customer Opening Balance
    QString title = "Edit Customer Opening Balance";
    if(isNotTabOpened(title))
    {
        EditCustomerOpeningBalance *mw = new EditCustomerOpeningBalance(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAR_Standard_GL_Accounts_triggered()
{
    //AR Standard GL Accounts
    QString title = "AR Standard GL Accounts";
    if(isNotTabOpened(title))
    {
        ARStandardGLAccounts *mw = new ARStandardGLAccounts(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_New_Item_triggered()
{
    //Add New Item
    QString title = "Add New Item";
    if(isNotTabOpened(title))
    {
        AddItem *mw = new AddItem(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Item_triggered()
{
    //Edit Item
    QString title = "Edit Item";
    if(isNotTabOpened(title))
    {
        EditItem *mw = new EditItem(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionItems_List_triggered()
{
    //Items List
    QString title = "Items List";
    if(isNotTabOpened(title))
    {
        ItemsList *mw = new ItemsList(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionStock_Adjustment_triggered()
{
    //Stock Adjustment
    QString title = "Stock Adjustment";
    if(isNotTabOpened(title))
    {
        StockAdjustment *mw = new StockAdjustment(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Stock_Adjustment_triggered()
{
    //Edit Stock Adjustment
    QString title = "Edit Stock Adjustment";
    if(isNotTabOpened(title))
    {
        EditStockAdjustment *mw = new EditStockAdjustment(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Initial_Stock_triggered()
{
    //Add Initial Stock
    QString title = "Add Initial Stock";
    if(isNotTabOpened(title))
    {
        AddInitialStock *mw = new AddInitialStock(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Initial_Stock_triggered()
{
    //Edit Initial Stock
    QString title = "Edit Initial Stock";
    if(isNotTabOpened(title))
    {
        EditInitialStock *mw = new EditInitialStock(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionStock_Report_triggered()
{
    //Stock Report
    QString title = "Stock Report";
    if(isNotTabOpened(title))
    {
        StockReport *mw = new StockReport(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionLocation_Wise_Stock_triggered()
{
    //Location Wise Stock
    QString title = "Location Wise Stock";
    if(isNotTabOpened(title))
    {
        LocationWiseStock *mw = new LocationWiseStock(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionStock_Report_Selling_Price_triggered()
{
    //Stock Report Selling Price
    QString title = "Stock Report Selling Price";
    if(isNotTabOpened(title))
    {
        StockReportSellingPrice *mw = new StockReportSellingPrice(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionStock_Report_Purchase_Price_triggered()
{
    //Stock Report Purchase Price
    QString title = "Stock Report Purchase Price";
    if(isNotTabOpened(title))
    {
        StockReportPurchasePrice *mw = new StockReportPurchasePrice(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionItem_Ledger_triggered()
{
    //Item Ledger
    QString title = "Item Ledger";
    if(isNotTabOpened(title))
    {
        ItemLedger *mw = new ItemLedger(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Units_2_triggered()
{
    //Manage Units
    QString title = "Manage Units";
    if(isNotTabOpened(title))
    {
        ManageUnits *mw = new ManageUnits(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Locations_triggered()
{
    //Manage Locations
    QString title = "Manage Locations";
    if(isNotTabOpened(title))
    {
        ManageLocations *mw = new ManageLocations(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Item_Category_triggered()
{
    //Manage Item Category
    QString title = "Manage Item Category";
    if(isNotTabOpened(title))
    {
        ManageItemCategory *mw = new ManageItemCategory(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Item_Sub_Category_triggered()
{
    //Manage Item Sub Category
    QString title = "Manage Item Sub Category";
    if(isNotTabOpened(title))
    {
        ManageItemSubCategory *mw = new ManageItemSubCategory(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Item_Brands_triggered()
{
    //Manage Item Brands
    QString title = "Manage Item Brands";
    if(isNotTabOpened(title))
    {
        ManageItemBrands *mw = new ManageItemBrands(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionInvStandard_GL_Accounts_triggered()
{
    //Inv Standard GL Accounts
    QString title = "Inv Standard GL Accounts";
    if(isNotTabOpened(title))
    {
        InvStandardGLAccounts *mw = new InvStandardGLAccounts(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Vendor_triggered()
{
    //Add Vendor
    QString title = "Add Vendor";
    if(isNotTabOpened(title))
    {
        AddVendor *mw = new AddVendor(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Vendor_triggered()
{
    //Edit Vendor
    QString title = "Edit Vendor";
    if(isNotTabOpened(title))
    {
        EditVendor *mw = new EditVendor(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionVendors_List_triggered()
{
    //Vendors List
    QString title = "Vendors List";
    if(isNotTabOpened(title))
    {
        VendorsList *mw = new VendorsList(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Purchase_Invoice_triggered()
{
    //Add Purchase Invoice
    QString title = "Add Purchase Invoice";
    if(isNotTabOpened(title))
    {
        AddPurchaseInvoice *mw = new AddPurchaseInvoice(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Purchase_Invoice_triggered()
{
    //Edit Purchase Invoice
    QString title = "Edit Purchase Invoice";
    if(isNotTabOpened(title))
    {
        EditPurchaseInvoice *mw = new EditPurchaseInvoice(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Purchase_Return_triggered()
{
    //Add Purchase Return
    QString title = "Add Purchase Return";
    if(isNotTabOpened(title))
    {
        AddPurchaseReturn *mw = new AddPurchaseReturn(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Purchase_Return_triggered()
{
    //Edit Purchase Return
    QString title = "Edit Purchase Return";
    if(isNotTabOpened(title))
    {
        EditPurchaseReturn *mw = new EditPurchaseReturn(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Vendor_Payment_triggered()
{
    //Add Vendor Payment
    QString title = "Add Vendor Payment";
    if(isNotTabOpened(title))
    {
        AddVendorPayment *mw = new AddVendorPayment(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Vendor_Payment_triggered()
{
    //Edit Vendor Payment
    QString title = "Edit Vendor Payment";
    if(isNotTabOpened(title))
    {
        EditVendorPayment *mw = new EditVendorPayment(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionVendor_Ledger_triggered()
{
    //Vendor Ledger
    QString title = "Vendor Ledger";
    if(isNotTabOpened(title))
    {
        VendorLedger *mw = new VendorLedger(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionPurchase_Invoices_List_triggered()
{
    //Purchase Invoices List
    QString title = "Purchase Invoices List";
    if(isNotTabOpened(title))
    {
        PurchaseInvoicesList *mw = new PurchaseInvoicesList(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionPurchase_Detail_Report_triggered()
{
    //Purchase Detail Report
    QString title = "Purchase Detail Report";
    if(isNotTabOpened(title))
    {
        PurchaseDetailReport *mw = new PurchaseDetailReport(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionItem_Wise_Purchases_triggered()
{
    //Item Wise Purchases
    QString title = "Item Wise Purchases";
    if(isNotTabOpened(title))
    {
        ItemWisePurchases *mw = new ItemWisePurchases(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionPurchase_Return_List_triggered()
{
    //Purchase Return List
    QString title = "Purchase Return List";
    if(isNotTabOpened(title))
    {
        PurchaseReturnList *mw = new PurchaseReturnList(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionVendors_Balance_Report_triggered()
{
    //Vendors Balance Report
    QString title = "Vendors Balance Report";
    if(isNotTabOpened(title))
    {
        VendorsBalanceReport *mw = new VendorsBalanceReport(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionVendor_Payment_Report_triggered()
{
    //Vendor Payment Report
    QString title = "Vendor Payment Report";
    if(isNotTabOpened(title))
    {
        VendorPaymentReport *mw = new VendorPaymentReport(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Vendor_Groups_triggered()
{
    //Manage Vendor Groups
    QString title = "Manage Vendor Groups";
    if(isNotTabOpened(title))
    {
        ManageVendorGroups *mw = new ManageVendorGroups(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Vendor_Opening_Balance_triggered()
{
    //Add Vendor Opening Balance
    QString title = "Add Vendor Opening Balance";
    if(isNotTabOpened(title))
    {
        AddVendorOpeningBalance *mw = new AddVendorOpeningBalance(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Vendor_Opening_Balance_triggered()
{
    //Edit Vendor Opening Balance
    QString title = "Edit Vendor Opening Balance";
    if(isNotTabOpened(title))
    {
        EditVendorOpeningBalance *mw = new EditVendorOpeningBalance(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAPStandard_GL_Accounts_triggered()
{
    //AP Standard GL Accounts
    QString title = "AP Standard GL Accounts";
    if(isNotTabOpened(title))
    {
        APStandardGLAccounts *mw = new APStandardGLAccounts(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_GL_Account_triggered()
{
    //Add GL Account
    QString title = "Add GL Account";
    if(isNotTabOpened(title))
    {
        AddGLAccount *mw = new AddGLAccount(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_GL_Account_triggered()
{
    //Edit GL Account
    QString title = "Edit GL Account";
    if(isNotTabOpened(title))
    {
        EditGLAccount *mw = new EditGLAccount(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionChart_of_Accounts_List_triggered()
{
    //Chart of Accounts List
    QString title = "Chart of Accounts List";
    if(isNotTabOpened(title))
    {
        ChartofAccountsList *mw = new ChartofAccountsList(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Cash_Receipt_triggered()
{
    //Add Cash Receipt
    QString title = "Add Cash Receipt";
    if(isNotTabOpened(title))
    {
        AddCashReceipt *mw = new AddCashReceipt(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Cash_Receipt_triggered()
{
    //Edit Cash Receipt
    QString title = "Edit Cash Receipt";
    if(isNotTabOpened(title))
    {
        EditCashReceipt *mw = new EditCashReceipt(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Cash_Payment_triggered()
{
    //Add Cash Payment
    QString title = "Add Cash Payment";
    if(isNotTabOpened(title))
    {
        AddCashPayment *mw = new AddCashPayment(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Cash_Payment_triggered()
{
    //Edit Cash Payment
    QString title = "Edit Cash Payment";
    if(isNotTabOpened(title))
    {
        EditCashPayment *mw = new EditCashPayment(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_Journal_Voucher_triggered()
{
    //Add Journal Voucher
    QString title = "Add Journal Voucher";
    if(isNotTabOpened(title))
    {
        AddJournalVoucher *mw = new AddJournalVoucher(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_Journal_Voucher_triggered()
{
    //Edit Journal Voucher
    QString title = "Edit Journal Voucher";
    if(isNotTabOpened(title))
    {
        EditJournalVoucher *mw = new EditJournalVoucher(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAccount_Ledger_triggered()
{
    //Account Ledger
    QString title = "Account Ledger";
    if(isNotTabOpened(title))
    {
        AccountLedger *mw = new AccountLedger(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionDay_Book_triggered()
{
    //Day Book
    QString title = "Day Book";
    if(isNotTabOpened(title))
    {
        DayBook *mw = new DayBook(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionJournal_Reports_triggered()
{
    //Journal Reports
    QString title = "Journal Reports";
    if(isNotTabOpened(title))
    {
        JournalReports *mw = new JournalReports(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAccounts_Balance_Summary_triggered()
{
    //Accounts Balance Summary
    QString title = "Accounts Balance Summary";
    if(isNotTabOpened(title))
    {
        AccountsBalanceSummary *mw = new AccountsBalanceSummary(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionTrial_Balance_triggered()
{
    //Trial Balance
    QString title = "Trial Balance";
    if(isNotTabOpened(title))
    {
        TrialBalance *mw = new TrialBalance(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionProfit_and_Loss_triggered()
{
    //Profit and Loss
    QString title = "Profit and Loss";
    if(isNotTabOpened(title))
    {
        ProfitAndLoss *mw = new ProfitAndLoss(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionBalance_Sheet_triggered()
{
    //Balance Sheet
    QString title = "Balance Sheet";
    if(isNotTabOpened(title))
    {
        BalanceSheet *mw = new BalanceSheet(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionAdd_GL_Opening_Balance_triggered()
{
    //Add GL Opening Balance
    QString title = "Add GL Opening Balance";
    if(isNotTabOpened(title))
    {
        AddGLOpeningBalance *mw = new AddGLOpeningBalance(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionEdit_GL_Opening_Balance_triggered()
{
    //Edit GL Opening Balance
    QString title = "Edit GL Opening Balance";
    if(isNotTabOpened(title))
    {
        EditGLOpeningBalance *mw = new EditGLOpeningBalance(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionStandard_GL_Accounts_triggered()
{
    //Standard GL Accounts
    QString title = "Standard GL Accounts";
    if(isNotTabOpened(title))
    {
        StandardGLAccounts *mw = new StandardGLAccounts(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionDatabase_Backup_triggered()
{
    //Database Backup
    QString title = "Database Backup";
    if(isNotTabOpened(title))
    {
        DatabaseBackup *mw = new DatabaseBackup(this);
        mw->savedb();
//        ui->mainTabWidget->addTab(mw, title);
//        mw->erp.setloginid(loginid);
//        mw->erp.setyearid(_yearid);
//        int i = ui->mainTabWidget->count()-1;
//        ui->mainTabWidget->setTabToolTip(i,title);
//        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionUser_Log_triggered()
{
    //User Log
    QString title = "User Log";
    if(isNotTabOpened(title))
    {
        UserLog *mw = new UserLog(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionFinancial_Year_triggered()
{
    //Financial Year
    QString title = "Financial Year";
    if(isNotTabOpened(title))
    {
        FinancialYear *mw = new FinancialYear(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::on_actionManage_Users_triggered()
{
    //Manage Users
    QString title = "Manage Users";
    if(isNotTabOpened(title))
    {
        ManageUsers *mw = new ManageUsers(this);
        ui->mainTabWidget->addTab(mw, title);
        mw->erp.setloginid(loginid);
        mw->erp.setyearid(_yearid);
        int i = ui->mainTabWidget->count()-1;
        ui->mainTabWidget->setTabToolTip(i,title);
        ui->mainTabWidget->setCurrentIndex(i);
    }
}

void MainWindow::loadCustomerBalance()
{
    ui->balanceTable->setColumnCount(2);
    QStringList titles;
    titles <<"Customer Name"<<"Balance";
    ui->balanceTable->setHorizontalHeaderLabels(titles);
    ui->balanceTable->horizontalHeader()->setSectionResizeMode(CUSTOMERNAME, QHeaderView::Stretch);
    ui->balanceTable->setColumnWidth(BALANCE,90);
    ui->balanceTable->setRowCount(0);
    QString date = QDate::currentDate().toString("yyyy-MM-dd");

    QString query_cust = " select customerid , name as cname , phone1, phone2, phone3 , groupid from tblcustomers where customerid > 0 ";
    query_cust += " GROUP By name ";
    //qDebug() << query_cust;
    QSqlQuery info_cust(query_cust);
    int row;
    while(info_cust.next())
    {
        //qDebug()<<"ok";
        QString query_s1= " select ( sum(debit) - sum(credit) ) dc from tblgltransaction t,tblgltransvoucher v "
                          " where t.voucherno=v.voucherno AND t.voucherid = v.voucherid and  "
                          " t.compid='"+info_cust.value("customerid").toString()+"' and t.companytype='customer' "
                          " and v.entrydate <= '"+date+"' group by t.compid ";
        QSqlQuery info_s1(query_s1);
        info_s1.next();
        float a_netopen1 = info_s1.value("dc").toFloat();
        if(a_netopen1!=0)
        {
            QString customername = erp.customerName(info_cust.value("customerid").toString());
            float balance = a_netopen1;
            row = ui->balanceTable->rowCount();
            ui->balanceTable->insertRow(row);

            QTableWidgetItem *custname = new QTableWidgetItem( customername );
            custname->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

            QTableWidgetItem *cbal = new QTableWidgetItem( erp.amountString(balance) );
            cbal->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            ui->balanceTable->setItem(row, CUSTOMERNAME, custname );
            ui->balanceTable->setItem(row, BALANCE,      cbal );

        }//end if(ok)
    }//end while(info_cust.next())
}

void MainWindow::loadOutStandingCheques()
{
    ui->chequesTable->setColumnCount(8);
    QStringList titles;
    titles <<"Date"<<"Voucher #"<<"Pay Method"<<"Cheque/\nPurchie #"<<"Customer"<<"Description"<<"Amount"<<"Due Date";
    ui->chequesTable->setHorizontalHeaderLabels(titles);
    ui->chequesTable->horizontalHeader()->setSectionResizeMode(CUSTOMER, QHeaderView::Stretch);
    ui->chequesTable->setColumnWidth(DATE,70);
    ui->chequesTable->setColumnWidth(VOUCHER,70);
    ui->chequesTable->setColumnWidth(PMETHOD,70);
    ui->chequesTable->setColumnWidth(CHEQUENUM,70);
    ui->chequesTable->setColumnWidth(DESCRIPTION,150);
    ui->chequesTable->setColumnWidth(AMOUNT,90);
    ui->chequesTable->setColumnWidth(DUEDATE,70);

    ui->chequesTable->setRowCount(0);
    QString query = "select * from tblgltransvoucher"
                    " inner join tblinvoicepaymentdetail on (tblgltransvoucher.voucherno = "
                    " tblinvoicepaymentdetail.voucherid) where voucherno like 'pmt%' "
                    " and tblinvoicepaymentdetail.cancel =0 and tblinvoicepaymentdetail.paymeth!='1' "
                    " and tblinvoicepaymentdetail.status in ('0','1')";
    query += " order by dueDate asc ";
    QSqlQuery info(query);
    //int i;
    int row;
    while(info.next())
    {
        QString voucherid = info.value("voucherid").toString();
        QSqlQuery row2("select sum(amount) as amount from tblencashpaymentdetail where pmtno = '"+voucherid+"'");
        row2.next();

        float bal_amount = info.value("amount").toFloat() - row2.value("amount").toFloat();
        QString datereceived = info.value("datereceived").toDate().toString("dd/MM/yyyy");
        QString method = erp.paymethod( info.value("paymeth").toString() );
        QString checkNum = info.value("checkNum").toString();
        QString cust_name = erp.customerName( info.value("customer_id").toString() );
        QString bank = info.value("bank").toString();
        QString dueDate = info.value("dueDate").toDate().toString("dd/MM/yyyy");

        row = ui->chequesTable->rowCount();
        ui->chequesTable->insertRow(row);

        QTableWidgetItem *dt = new QTableWidgetItem( datereceived );
        dt->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *vid = new QTableWidgetItem( voucherid );
        vid->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *pmode = new QTableWidgetItem( method );
        pmode->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        QTableWidgetItem *chkno = new QTableWidgetItem( checkNum );
        chkno->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        QTableWidgetItem *cust = new QTableWidgetItem( cust_name );
        //cust->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *desc = new QTableWidgetItem( bank );
        desc->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *amt = new QTableWidgetItem( erp.amountString(bal_amount) );
        amt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

        QTableWidgetItem *ddate = new QTableWidgetItem( dueDate );
        ddate->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

        ui->chequesTable->setItem(row, DATE,  dt );
        ui->chequesTable->setItem(row, VOUCHER,     vid );
        ui->chequesTable->setItem(row, PMETHOD, pmode );
        ui->chequesTable->setItem(row, CHEQUENUM,  chkno );
        ui->chequesTable->setItem(row, CUSTOMER,   cust );
        ui->chequesTable->setItem(row, DESCRIPTION,      desc );
        ui->chequesTable->setItem(row, AMOUNT,   amt );
        ui->chequesTable->setItem(row, DUEDATE, ddate );
    }//end while(info_sale.next())
}
