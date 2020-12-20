#-------------------------------------------------
#
# Project created by QtCreator 2019-10-21T14:54:20
#
#-------------------------------------------------

QT       += core gui sql sensors printsupport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BizjustERP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    loginwindow.cpp \
    addcustomer.cpp \
    editcustomerform.cpp \
    customerlist.cpp \
    addsalesinvoice.cpp \
    editsalesinvoice.cpp \
    printsalesinvoice.cpp \
    addsalesreturn.cpp \
    editsalesreturn.cpp \
    printsalesreturn.cpp \
    customerreceipt.cpp \
    editcustomerreceipt.cpp \
    addcustomercashreceipt.cpp \
    editcustomercashreceipt.cpp \
    chequepurchieencashed.cpp \
    editchequeparchieencashed.cpp \
    customerledger.cpp \
    customersalesandrecovery.cpp \
    customersbalance.cpp \
    summaryofinvoices.cpp \
    customerpaymentreceiptreport.cpp \
    recentcustomers.cpp \
    outstandingchequespurchie.cpp \
    summaryofinvoicesindetail.cpp \
    customerwiseencashpayments.cpp \
    customerannualsale.cpp \
    monthwisesaleschart.cpp \
    customersitemwisesale.cpp \
    itemscustomerwisesale.cpp \
    customerwiseprofitanalysis.cpp \
    itemsmonthwisesales.cpp \
    itemsmonthwisesalespurchases.cpp \
    itemwisesales.cpp \
    daywisesalessummary.cpp \
    customersalesanalysis.cpp \
    managesalesman.cpp \
    managecarrier.cpp \
    managecustomergroups.cpp \
    managesalescategories.cpp \
    manageunits.cpp \
    addcustomeropeningbalance.cpp \
    editcustomeropeningbalance.cpp \
    arstandardglaccounts.cpp \
    additem.cpp \
    edititem.cpp \
    itemslist.cpp \
    stockadjustment.cpp \
    editstockadjustment.cpp \
    addinitialstock.cpp \
    editinitialstock.cpp \
    stockreport.cpp \
    locationwisestock.cpp \
    stockreportsellingprice.cpp \
    stockreportpurchaseprice.cpp \
    itemledger.cpp \
    managelocations.cpp \
    manageitemcategory.cpp \
    manageitemsubcategory.cpp \
    manageitembrands.cpp \
    invstandardglaccounts.cpp \
    addvendor.cpp \
    editvendor.cpp \
    vendorslist.cpp \
    addpurchaseinvoice.cpp \
    editpurchaseinvoice.cpp \
    addpurchasereturn.cpp \
    editpurchasereturn.cpp \
    addvendorpayment.cpp \
    editvendorpayment.cpp \
    vendorledger.cpp \
    purchaseinvoiceslist.cpp \
    purchasedetailreport.cpp \
    itemwisepurchases.cpp \
    purchasereturnlist.cpp \
    vendorsbalancereport.cpp \
    vendorpaymentreport.cpp \
    managevendorgroups.cpp \
    addvendoropeningbalance.cpp \
    editvendoropeningbalance.cpp \
    apstandardglaccounts.cpp \
    addglaccount.cpp \
    editglaccount.cpp \
    chartofaccountslist.cpp \
    addcashreceipt.cpp \
    editcashreceipt.cpp \
    addcashpayment.cpp \
    editcashpayment.cpp \
    addjournalvoucher.cpp \
    editjournalvoucher.cpp \
    accountledger.cpp \
    daybook.cpp \
    journalreports.cpp \
    accountsbalancesummary.cpp \
    trialbalance.cpp \
    profitandloss.cpp \
    balancesheet.cpp \
    addglopeningbalance.cpp \
    editglopeningbalance.cpp \
    standardglaccounts.cpp \
    databasebackup.cpp \
    userlog.cpp \
    financialyear.cpp \
    manageusers.cpp

HEADERS += \
        mainwindow.h \
    loginwindow.h \
    dbcon.h \
    addcustomer.h \
    erp.h \
    sch.h \
    editcustomerform.h \
    customerlist.h \
    addsalesinvoice.h \
    editsalesinvoice.h \
    printsalesinvoice.h \
    addsalesreturn.h \
    editsalesreturn.h \
    printsalesreturn.h \
    customerreceipt.h \
    editcustomerreceipt.h \
    addcustomercashreceipt.h \
    editcustomercashreceipt.h \
    chequepurchieencashed.h \
    editchequeparchieencashed.h \
    customerledger.h \
    customersalesandrecovery.h \
    customersbalance.h \
    summaryofinvoices.h \
    customerpaymentreceiptreport.h \
    recentcustomers.h \
    outstandingchequespurchie.h \
    summaryofinvoicesindetail.h \
    customerwiseencashpayments.h \
    customerannualsale.h \
    monthwisesaleschart.h \
    customersitemwisesale.h \
    itemscustomerwisesale.h \
    customerwiseprofitanalysis.h \
    itemsmonthwisesales.h \
    itemsmonthwisesalespurchases.h \
    itemwisesales.h \
    daywisesalessummary.h \
    customersalesanalysis.h \
    managesalesman.h \
    managecarrier.h \
    managecustomergroups.h \
    managesalescategories.h \
    manageunits.h \
    addcustomeropeningbalance.h \
    editcustomeropeningbalance.h \
    arstandardglaccounts.h \
    additem.h \
    comboboxes.h \
    edititem.h \
    itemslist.h \
    stockadjustment.h \
    editstockadjustment.h \
    addinitialstock.h \
    editinitialstock.h \
    stockreport.h \
    locationwisestock.h \
    stockreportsellingprice.h \
    stockreportpurchaseprice.h \
    itemledger.h \
    managelocations.h \
    manageitemcategory.h \
    manageitemsubcategory.h \
    manageitembrands.h \
    invstandardglaccounts.h \
    addvendor.h \
    editvendor.h \
    vendorslist.h \
    addpurchaseinvoice.h \
    editpurchaseinvoice.h \
    addpurchasereturn.h \
    editpurchasereturn.h \
    addvendorpayment.h \
    editvendorpayment.h \
    vendorledger.h \
    purchaseinvoiceslist.h \
    purchasedetailreport.h \
    itemwisepurchases.h \
    purchasereturnlist.h \
    vendorsbalancereport.h \
    vendorpaymentreport.h \
    managevendorgroups.h \
    addvendoropeningbalance.h \
    editvendoropeningbalance.h \
    apstandardglaccounts.h \
    addglaccount.h \
    editglaccount.h \
    chartofaccountslist.h \
    addcashreceipt.h \
    editcashreceipt.h \
    addcashpayment.h \
    editcashpayment.h \
    addjournalvoucher.h \
    editjournalvoucher.h \
    accountledger.h \
    daybook.h \
    journalreports.h \
    accountsbalancesummary.h \
    trialbalance.h \
    profitandloss.h \
    balancesheet.h \
    addglopeningbalance.h \
    editglopeningbalance.h \
    standardglaccounts.h \
    databasebackup.h \
    userlog.h \
    financialyear.h \
    manageusers.h

FORMS += \
        mainwindow.ui \
    loginwindow.ui \
    addcustomer.ui \
    editcustomerform.ui \
    customerlist.ui \
    addsalesinvoice.ui \
    editsalesinvoice.ui \
    printsalesinvoice.ui \
    addsalesreturn.ui \
    editsalesreturn.ui \
    printsalesreturn.ui \
    customerreceipt.ui \
    editcustomerreceipt.ui \
    addcustomercashreceipt.ui \
    editcustomercashreceipt.ui \
    chequepurchieencashed.ui \
    editchequeparchieencashed.ui \
    customerledger.ui \
    customersalesandrecovery.ui \
    customersbalance.ui \
    summaryofinvoices.ui \
    customerpaymentreceiptreport.ui \
    recentcustomers.ui \
    outstandingchequespurchie.ui \
    summaryofinvoicesindetail.ui \
    customerwiseencashpayments.ui \
    customerannualsale.ui \
    monthwisesaleschart.ui \
    customersitemwisesale.ui \
    itemscustomerwisesale.ui \
    customerwiseprofitanalysis.ui \
    itemsmonthwisesales.ui \
    itemsmonthwisesalespurchases.ui \
    itemwisesales.ui \
    daywisesalessummary.ui \
    customersalesanalysis.ui \
    managesalesman.ui \
    managecarrier.ui \
    managecustomergroups.ui \
    managesalescategories.ui \
    manageunits.ui \
    addcustomeropeningbalance.ui \
    editcustomeropeningbalance.ui \
    arstandardglaccounts.ui \
    additem.ui \
    edititem.ui \
    itemslist.ui \
    stockadjustment.ui \
    editstockadjustment.ui \
    addinitialstock.ui \
    editinitialstock.ui \
    stockreport.ui \
    locationwisestock.ui \
    stockreportsellingprice.ui \
    stockreportpurchaseprice.ui \
    itemledger.ui \
    managelocations.ui \
    manageitemcategory.ui \
    manageitemsubcategory.ui \
    manageitembrands.ui \
    invstandardglaccounts.ui \
    addvendor.ui \
    editvendor.ui \
    vendorslist.ui \
    addpurchaseinvoice.ui \
    editpurchaseinvoice.ui \
    addpurchasereturn.ui \
    editpurchasereturn.ui \
    addvendorpayment.ui \
    editvendorpayment.ui \
    vendorledger.ui \
    purchaseinvoiceslist.ui \
    purchasedetailreport.ui \
    itemwisepurchases.ui \
    purchasereturnlist.ui \
    vendorsbalancereport.ui \
    vendorpaymentreport.ui \
    managevendorgroups.ui \
    addvendoropeningbalance.ui \
    editvendoropeningbalance.ui \
    apstandardglaccounts.ui \
    addglaccount.ui \
    editglaccount.ui \
    chartofaccountslist.ui \
    addcashreceipt.ui \
    editcashreceipt.ui \
    addcashpayment.ui \
    editcashpayment.ui \
    addjournalvoucher.ui \
    editjournalvoucher.ui \
    accountledger.ui \
    daybook.ui \
    journalreports.ui \
    accountsbalancesummary.ui \
    trialbalance.ui \
    profitandloss.ui \
    balancesheet.ui \
    addglopeningbalance.ui \
    editglopeningbalance.ui \
    standardglaccounts.ui \
    databasebackup.ui \
    userlog.ui \
    financialyear.ui \
    manageusers.ui

RESOURCES += \
    erpres.qrc \
    qss.qrc

DISTFILES += \
    style.qss

RC_ICONS = icon.ico
