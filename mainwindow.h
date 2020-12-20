#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbcon.h"
#include "erp.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString loginid;
    QString _yearid;
    ERP erp;
    DbCon conn;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool isNotTabOpened(QString title);

private slots:
    void on_actionAdd_Customer_triggered();

    void on_actionEdit_Customer_triggered();

    void on_actionCustomer_List_triggered();

    void on_actionAdd_Sales_Invoice_triggered();

    void on_mainTabWidget_tabCloseRequested(int index);

    void on_actionEdit_Sales_Invoice_triggered();

    void on_actionPrint_Sales_Invoice_triggered();

    void on_actionAdd_Sales_Return_triggered();

    void on_actionEdit_Sales_Return_triggered();

    void on_actionPrint_Sales_Return_triggered();

    void on_actionAdd_Customer_Receipt_triggered();

    void on_actionEdit_Customer_Receipt_triggered();

    void on_actionCustomer_Cash_Receipt_triggered();

    void on_actionEdit_Customer_Cash_Receipt_triggered();

    void on_actionCheque_Parchi_Encashed_triggered();

    void on_actionEdit_Cheque_Parchie_Encashed_triggered();

    void on_actionCustomer_Ledger_triggered();

    void on_actionAll_Customers_Sales_Recovery_triggered();

    void on_actionCustomers_Balance_triggered();

    void on_actionSummary_of_Invoices_triggered();

    void on_actionCustomer_Payment_Receipt_Report_triggered();

    void on_actionRecent_Customers_triggered();

    void on_actionOut_Standing_Cheque_Purchie_triggered();

    void on_actionSummary_of_Invoices_in_Detail_triggered();

    void on_actionCustomer_Wise_Encash_Payments_triggered();

    void on_actionCustomer_Annual_Sale_triggered();

    void on_actionMonth_Wise_Sales_Chart_triggered();

    void on_actionCustomer_s_Item_Wise_Sale_triggered();

    void on_actionItem_s_Customer_Wise_Sale_triggered();

    void on_actionCustomer_Wise_Profit_Analysis_triggered();

    void on_actionItem_s_Month_Wise_Sales_triggered();

    void on_actionItems_Month_Wise_Sales_Purchases_triggered();

    void on_actionItem_Wise_Sales_triggered();

    void on_actionDay_Wise_Sales_Summary_triggered();

    void on_actionCustomer_Sales_Analysis_triggered();

    void on_actionManage_Sales_Man_triggered();

    void on_actionManage_Carrier_triggered();

    void on_actionManage_Customer_Groups_triggered();

    void on_actionManage_Sales_Categories_triggered();

    void on_actionManage_Units_triggered();

    void on_actionAdd_Customer_Opening_Balance_triggered();
    
    void on_actionEdit_Customer_Opening_Balance_triggered();

    void on_actionAR_Standard_GL_Accounts_triggered();

    void on_actionAdd_New_Item_triggered();

    void on_actionEdit_Item_triggered();

    void on_actionItems_List_triggered();

    void on_actionStock_Adjustment_triggered();

    void on_actionEdit_Stock_Adjustment_triggered();

    void on_actionAdd_Initial_Stock_triggered();

    void on_actionEdit_Initial_Stock_triggered();

    void on_actionStock_Report_triggered();

    void on_actionLocation_Wise_Stock_triggered();

    void on_actionStock_Report_Selling_Price_triggered();

    void on_actionStock_Report_Purchase_Price_triggered();

    void on_actionItem_Ledger_triggered();

    void on_actionManage_Units_2_triggered();

    void on_actionManage_Locations_triggered();

    void on_actionManage_Item_Category_triggered();

    void on_actionManage_Item_Sub_Category_triggered();

    void on_actionManage_Item_Brands_triggered();

    void on_actionInvStandard_GL_Accounts_triggered();

    void on_actionAdd_Vendor_triggered();

    void on_actionEdit_Vendor_triggered();

    void on_actionVendors_List_triggered();

    void on_actionAdd_Purchase_Invoice_triggered();

    void on_actionEdit_Purchase_Invoice_triggered();

    void on_actionAdd_Purchase_Return_triggered();

    void on_actionEdit_Purchase_Return_triggered();

    void on_actionAdd_Vendor_Payment_triggered();

    void on_actionEdit_Vendor_Payment_triggered();

    void on_actionVendor_Ledger_triggered();

    void on_actionPurchase_Invoices_List_triggered();

    void on_actionPurchase_Detail_Report_triggered();

    void on_actionItem_Wise_Purchases_triggered();

    void on_actionPurchase_Return_List_triggered();

    void on_actionVendors_Balance_Report_triggered();

    void on_actionVendor_Payment_Report_triggered();

    void on_actionManage_Vendor_Groups_triggered();

    void on_actionAdd_Vendor_Opening_Balance_triggered();

    void on_actionEdit_Vendor_Opening_Balance_triggered();

    void on_actionAPStandard_GL_Accounts_triggered();

    void on_actionAdd_GL_Account_triggered();

    void on_actionEdit_GL_Account_triggered();

    void on_actionChart_of_Accounts_List_triggered();

    void on_actionAdd_Cash_Receipt_triggered();

    void on_actionEdit_Cash_Receipt_triggered();

    void on_actionAdd_Cash_Payment_triggered();

    void on_actionEdit_Cash_Payment_triggered();

    void on_actionAdd_Journal_Voucher_triggered();

    void on_actionEdit_Journal_Voucher_triggered();

    void on_actionAccount_Ledger_triggered();

    void on_actionDay_Book_triggered();

    void on_actionJournal_Reports_triggered();

    void on_actionAccounts_Balance_Summary_triggered();

    void on_actionTrial_Balance_triggered();

    void on_actionProfit_and_Loss_triggered();

    void on_actionBalance_Sheet_triggered();

    void on_actionAdd_GL_Opening_Balance_triggered();

    void on_actionEdit_GL_Opening_Balance_triggered();

    void on_actionStandard_GL_Accounts_triggered();

    void on_actionDatabase_Backup_triggered();

    void on_actionUser_Log_triggered();

    void on_actionFinancial_Year_triggered();

    void on_actionManage_Users_triggered();
    void checkmenues();

private:
    Ui::MainWindow *ui;
    enum collumns{
        CUSTOMERNAME,BALANCE
    };
    enum collumns2{
        DATE,VOUCHER,PMETHOD,CHEQUENUM,CUSTOMER,DESCRIPTION,AMOUNT,DUEDATE
    };
    QTimer *timer;
    void loadCustomerBalance();
    void loadOutStandingCheques();
};

#endif // MAINWINDOW_H
