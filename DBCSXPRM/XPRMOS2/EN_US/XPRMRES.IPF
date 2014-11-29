.***************************************************************************
.* XPRMRES.IPF   XPG4 Primer for OS/2 WARP - V1.0       11/15/95           *
.*               Help source for English.                                  *
.***************************************************************************
:userdoc.
.**************************************************************************
.* help for MID_HELP
:h1 res=17110.Help
:p.The following service is available via :hp2.Help:ehp2. menu&colon.
:ul.
:li.Display help topics provided by this program.
:li.General information on Help Facility.
:li.Help for keys
:li.Product information
:eul.
.* help for MID_HELPINDEX
:h2 res=17111.Help Index
:p.Shows a list of help topics.
.* help for MID_HELPKESY
:h2 res=17112.Help for Keys
:p.Show the usage of keys.
.* help for MID_HELPFORHELP
:h2 res=17113.Help Usage
:p.Shows the usage of the help facility.
:h2 res=17120.Help for Keys
:p.To quit any dialog window, press ESC key.
.**************************************************************************
.* help for WID_MAIN
:h1 res=17001.General Help
:i1 id=overview.Overview
:p.This program is a NLS sample for OS/2 and PM programmers.  It introduces
Hints and Tips to support both SBCS and DBCS in a single source module.
:p.The program has the following order entry scenario&colon.
:ul.
:li.Order entry of office equipments
:li.Query the exsiting order record(s)
:li.Register a new customer
:li.Find customer record(s)
:eul.
:i2 refid=overview.Environment Varialbes
:p.:hp2.Before starting the program, ensure the following environment
variables are set to your system.:ehp2.
:sl.
:li.set lang=ja_jp
:li.set locpath=:hp1.x:ehp1.&colon.xprmos2\locale
:esl.
.* help for MID_ORDER
:h2 res=17101.Order
:i1 id=order.Order
:p.The order menu contains commands you use to create and inquire order
records.
.* help for MID_NEWORDER
:h2 res=17102.New
:i2 refid=order.New
:p.You can create a new order.
create a new order.
.* help for MID_FIND
:h2 res=17103.Inquire
:i2 refid=order.Inquire
:p.You can inquire existing orders.
.* help for MID_CUST
:h2 res=17104.Cust
:i1 id=customer.Cust
:p.The cust menu contains commands you use to register and find customer
records.
.* help for MID_NEWCUST
:h2 res=17105.Register
:i2 refid=customer.Register
:p.You can register a new customer.
.* help for MID_FIND
:h2 res=17106.Find
:i2 refid=customer.Find
:p.You can find customer record(s).
.**************************************************************************
.* help for DID_LOGIN
:h1 res=17201.Help for Login Dialog
:i1 id=login.Login
:p.This dialog processes login to the system.
.* help for LI_EF_EMPNUM
:h2 res=17202.Help for Employee Number
:i2 refid=login.Enployee Number
:p.Enter your employee number which should be a digit number.  The maximum
length is 6.  Only SBCS characters are allowed.
:p.The program will not show the input.
.**************************************************************************
.* help for DID_ORDERENTRY
:h1 res=17301.Order Entry
:i1 id=ordent.Order Entry
:p.A new number is assigned for your new order entry.  Write down the number
for the future inquiry.
:p.Select the customer name from the customer name combo-box.  The customer
number is set automatically.  If the customer name is not found, cancel the
order and register the new customer at first.
:nt.
Even if the customer
has been registered, you may not be able to find the name when the customer
registration had been done under another locale.  In such case, you need
to edit the customer record file, :hp5.CUSTREC.DAT:ehp5., directly.  The
file is plane ASCII text.
:ent.
:p.Select the products from the list below and enter the quantities.  You
can select three products at maximum.
:p.Press OK to save and quit.  Press Cancel to quit without saving
the data.
.* help for OE_CB_CUSTNAME
:h2 res=17302.Customer Name
:i2 refid=ordent.Customer Name
:p.Select the customer name.  The customer
number is set automatically.  If the customer name is not found, cancel the
order and register the new customer at first.
:nt.
Even if the customer has been registered, you may not be able to find the
name when the customer registration had been done under another locale.
In such case, you need to edit the customer record file,
:hp5.CUSTREC.DAT:ehp5., directly.  The file is plane ASCII text.
:ent.
.* help for OE_EF_QTY
:h2 res=17303.Product Quantity
:i2 refid=ordent.Product Quantity
:p.Enter quantity of the product.
The number should be between 1 to 999.  Only SBCS characters are
allowed.
.* help for OE_LI_PRODUCTS
:h2 res=17304.Product List
:i2 refid=ordent.Product List
:p.Select the products from the list below and enter the quantities.  You
can select three products at maximum.  The prices are in yen.
.**************************************************************************
.* help for DID_QRYORDR
:h1 res=17401.Inquire Order Records
:i1 id=qorder.Inquire Order Records
:p.You can inquire order records by specifying&colon.
:ul.
:li.Order number
:li.Date
:eul.
.* help for QO_EF_KEY
:h2 res=17402.Key word
:i2 refid=qorder.Key word
:p.Enter either order number or date.  You have to select search category
prior.
Date must be specified as a period
with the format of :hp2.yymmdd-yymmdd:ehp2..  The order number must be a
digit number.  The maximum length is 6.  Padding 0 characters are not
required.
:p.An asterisk (*) can be used to show all records.
.* help for QO_PB_DATE
:h2 res=17403.Date
:i2 refid=qorder.Date
:p.Selecting this option causes searching by date.
.* help for QO_PB_NUMBER
:h2 res=17404.Order Number
:i2 refid=qorder.Order Number
:p.Selecting this option causes searching by order number.
.**************************************************************************
.* help for WID_VIEWORDER
:h1 res=17501.View Order Records
:i1 id=vieworder.View Order Records
:p.This panel shows you the result of your inquiry.  The order number of
each product is shown in the title bar.  To show the next/previous record,
press Next/Previous button, respectively.  To exit this window, press
Cancel button.
.* help for MID_OPTION
:h2 res=17502.Option
:i2 refid=vieworder.Option
:p.The option menu contains a command to change font of this display.
.* help for MID_FONT
:h2 res=17503.Option
:i2 refid=vieworder.Font
:p.Change font of the area where the order record is shown.  If you change
the font, the next/previous records will be show by using the new font as
well as the current record.  However,
after quiting this window by Cancel button, the change will be lost.
.**************************************************************************
.* help for DID_CUSTINFO
:h1 res=17601.Customer Information
:i1 id=custinfo.Customer Information
:p.A new customer number is assigned for the new customer. Write down the
number for the future inquiry.  The customer number is not modifiable.
:p.Enter the necessary information to each entry field.  You must fill the
all entry field.
:p.Press OK to save and quit.  Press Cancel to quit without saving
the data.
.* help for CI_EF_CUSTNAME
:h2 res=17602.Customer name
:i2 refid=custinfo.Customer Name
:p.Customer name should be within 47 character regardless to SBCS or DBCS.
.* help for QO_EF_CUSTADDR
:h2 res=17603.Customer Address
:i2 refid=custinfo.Customer Address
:p.Customer address should be within 127 characters regardless to SBCS or DBCS.
.* help for QO_EF_PHONE
:h2 res=17604.Telephone Number
:i2 refid=custinfo.Telephone Number
:p.Customer telephone number should be within 15 SBCS characters.
.**************************************************************************
.* help for DID_QRYCUST
:h1 res=17701.Find Customer Records
:i1 id=findcust.Find Customer Records
:p.You can find customer records by specifying&colon.
:ul.
:li.Customer number
:li.Customer name
:li.Telephone number
:eul.
.* help for QC_EF_KEY
:h2 res=17702.Key word
:i2 refid=findcust.Key Word
:p.Enter customer number, customer name or phone number.
You have to select category prior.  Customer number must be a digit number.
Input should be SBCS characters for customer number and phone number, and
only exact match is supported.  The padding 0 is not required.
:p.For customer name, you can specify part of customer name.
:p.Fot telephone number, you can specify telephone number in SBCS.
only exact match is supported.
:p.An asterisk (*) can be used to show all records.
.* help for QC_PB_CUSTNUM
:h2 res=17703.Customer Number
:i2 refid=findcust.Customer Number
:p.Selectiong this option causes seraching by customer number.
.* help for QC_PB_CUSTNAME
:h2 res=17704.Customer Name
:i2 refid=findcust.Customer Name
:p.Selectiong this option causes seraching by customer name.
.* help for QC_PB_TELNUM
:h2 res=17705.Telephone Number
:i2 refid=findcust.Telephone Number
:p.Selectiong this option causes seraching by telephone number.
.**************************************************************************
.* help for DID_CUSTLIST
:h1 res=17801.Customer List
:i1.Customer List
:p.This is the result of your customer inquiry.  Select one you desired, then
press OK.
.**************************************************************************
:euserdoc.
