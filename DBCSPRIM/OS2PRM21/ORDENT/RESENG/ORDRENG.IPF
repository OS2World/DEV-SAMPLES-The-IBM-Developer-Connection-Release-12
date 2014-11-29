.********************************************************************
.*   Program Name : ORDRENG.IPF ( for Order Entry Sample V2.00 )
.*
.*     English help resource definition for the ORDENT sample program
.*     Issue "ipfc ordreng.ipf" to compile this IPF at the OS/2 command
.*     prompt.
.*
.*     Exit and a short-cut to exit were removed.
.*
.********************************************************************
:userdoc.
.*
:h1 res=256.Help for Order Entry Sample 
:P.This Order Entry sample program is an example of NLS-enabling for both
SBCS languages and DBCS languages.
.*
:h1 res=257.Help for File pull-down
:P.The File pull-down contains commands to create a new order and to exit
this sample prigram.
:h2 res=258.Help for New Order
:p.Select New Order on the File pull-down to create a new order.
.*
:h1 res=260.Help for Master pull-down
:p.The Master pull-down contains commands for customer inquiry and customer
registration.
:h2 res=261.Help for Inquire customers
:p.Select Inquire customers on the Master pull-down to search customers by
a given keyword, display and change customer information.
:h2 res=262.Help for Register a customer
:p.Select Register a customer on the Master pull-down to register a new
customer information.
.*
:h1 res=263.Help for Report pull-down
:p.The Report pull-down contains a command to print an order list report by
products.
:h2 res=264.Help for Print a report
:p.Select Print a report on the Report pull-down to printan order list report
 by products during specified period.
.*
:h1 res=265. Help for Help pull-down
:p.The Help pull-down contains items you can use to get different types of
On-line help for Order Entry sample program.
:h2 res=266. Help for Help
:p.Select Help for Help on the Help pull-down for information on how to use
OS/2 on-line help facility.
:h2 res=267. Help for Extended Help
:p.Select Extended Help on the Help pull-down for information on the Order
Entry sample program.
:h2 res=268. Help for Keys Help
:p.Select Keys List on the Help pull-down for information on usages of keys.
:h2 res=269.Help for About
:p.Select About on the Help pull-down for information on the program names
and versions.
:h1 res=270. Keys Help
:p.The following short-cut keys are available from the main window :
:dl.
:dt.Keys
:dd.Descrption
:dt.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.
:dd.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.
:dt.CTL+N
:dd.Push CTL+N in the main dialog to create a new order. This is same action 
as "New Order" on the File pull-down.
:dt.CTL+Q
:dd.Push CTL+Q in the main dialog to inquiry customers by a keyword. This is
same action as "Inquire Customers" on the Master pull-down.
:dt.CTL+R
:dd.Push CTL+R in the main dialog to register a customer by a keyword. This is
same action as "Register a customer" on the Master pull-down.
:dt.CTL+P
:dd.Push CTL+P in the main dialog to print out an order list report by
products during specified period.
:dt.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.
:dd.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.&bxh.
:edl.
.*------------------------------------------------------------------------
.*  Log-on Dialog
.*------------------------------------------------------------------------
:h1 res=320.Help for Logon
:p.This dialog processes Logon to this sample program.
:h2 res=323.Help for Start button
:p.Select Start button to start this sample program
:h2 res=324.Help for Cancel button
:p.Select Cancel button to exit from this sample program.
:h2 res=325.Help for Logon ID field
:p.Please put logon id of 6 characters. Since ES_SBCS style is set to
this field, this field can accept only one byte charcters even if this
application on DBCS OS/2.
.*------------------------------------------------------------------------
.*  New Order Dialog
.*------------------------------------------------------------------------
:h1 res=400.Help for New Order
:p.This dialog processes a new order from a customer.
:h2 res=421.Help for Customer Name
:p.Display a customer name after inputiing a customer number and pushing
"confirm" button". By left/right arrow keys, you can scroll this field.
This field is not editable.
:h2 res=436.Help for Ordered Items
:p.Display a ordered item selected in the Product List. By left/right arrow
keys, you can scroll this field. This field is not editable.
:h2 res=437.Help for Ordered Items
:p.Display a ordered item selected in the Product List. By left/right arrow
keys, you can scroll this field. This field is not editable.
:h2 res=438.Help for Ordered Items
:p.Display a ordered item selected in the Product List. By left/right arrow
keys, you can scroll this field. This field is not editable.
:h2 res=470.Help for Cancel button
:p.Select Cancel buttun in the New Order Dialog to cancel this dialog
and return to the main dialog.
:h2 res=471.Help for Confirm button
:p.Select Confirm button in the New Order Dialog to check if your inputs
are valid in the New Order Dialog. 
:h2 res=472.Help for Enter button
:p.Select Enter button in the New Order Dialog to 
:h2 res=473.Help for Help button
:p.Select Help button in the dialog to display help information
:h2 res=419.Help for Customer Number
:p.Put a Customer number which are 6 digits.
:h2 res=467.Help for Product List
:p.Select 3 items to be ordered.
:h2 res=446.Help for Ordered Quantity
:p.Input quantity for ordered item 1.
:h2 res=447.Help for Ordered Quantity
:p.Input quantity for ordered item 2.
:h2 res=448.Help for Ordered Quantity
:p.Input quantity for ordered item 3.
.*------------------------------------------------------------------------
.*  Report Dialog
.*------------------------------------------------------------------------
:h1 res=700.Help for Report
:p.This dialog processes to print out an ordered list report.
:h2 res=703.Help for From date
:p.Input a date to start your report with a YYYYMMDD format.
:h2 res=705.Help for To date
:p.Input a date to end your report with a YYYYMMDD format.
:h2 res=706.Help for Print button
:p.Select Print button in the Print a Report dailog to print an order list
report by products during specified period.
:h2 res=707.Help for Cancel button
:p.Select Cancel buttun in the Print a Report dialog to cancel report 
printing and return to the main dialog.
:h2 res=708.Help for Help button
:p.Select Help button in the dialog to display help information
.*------------------------------------------------------------------------
.*  Customer Inquiry Dialog
.*------------------------------------------------------------------------
:h1 res=1000.Help for Customer Inquiry
:p.This dialog processes to search customers based on a given keyword.
:h2 res=1001.Help for Keyword field
:p.Input a keyword to search customers. If no keyword is given, all 
customers will be displayed.
:h2 res=1004.Help for Cancel button
:p.Select Cancel buttun in the Customer Inquiry dialog to cancel the inquiry
and return to the main dialog.
:h2 res=1003.Help for Search button
:p.Select Search button to search customers by using a keyword.
:h2 res=1005.Help for Help button
:p.Select Help button in the dialog to display help information
.*------------------------------------------------------------------------
.*  Customer Inquiry result Dialog
.*------------------------------------------------------------------------
:h1 res=1100.Help for Customer Inquiry Result
:p.This dialog displays the result of customer inquiry.
:h2 res=1101.Help for Customer List Box
:p.Display the result of Customer inquiry. Select a customer by mouse click.
If duble-clicked, the customer information on the mouse pointer will be 
diaplayed.
:h2 res=1102.Help for Detailed button
:p.Select Detailed button after a customer is selected in the Customer 
List Box to display the customer's information.
:h2 res=1103.Help for Cancel button
:p.Select Cancel buttun in the Customer Inquiry Result dialog to return to
the main dialog.
:h2 res=1104.Help for Gelp button
:p.Select Help button in the dialog to display help information
.*------------------------------------------------------------------------
.*  Customer Information Dialog
.*------------------------------------------------------------------------
:h1 res=1200.Help for Customer Information
:p.This dialog displays Customer Information and can change it. 
:h2 res=1206.Help for Customer name
:p.A customer name is displayed and can be changed. The maximum length is
40 bytes, however, since ES_MIXED style is set to this Entry field, the
string length in ASCII may be shorter than 40 bytes on DBCS OS/2.
:h2 res=1207.Help for Customer Address
:p.A customer address is displayed and can be changed. The maximum length
is 120 bytes.
:h2 res=1208.Help for Customer Phone Number
:p.A customer phon number is displayed and can be changed. The maximum length
is 12 bytes.
:h2 res=1209.Help for Update button
:p.Select Update button to update the customer information in the master file.
:h2 res=1210.Help for Cancel button
:p.Select Cancel buttun in the Customer Information dialog to return to
the Customer Inquiry Result dialog.
:h2 res=1211.Help for Gelp button
:p.Select Help button in the dialog to display help information
.*------------------------------------------------------------------------
.*  Customer Registration Dialog
.*------------------------------------------------------------------------
:h1 res=1300.Help for Customer Registration
:p.This dialog processes to register a new customer information.
:h2 res=1306.Help for Customer name
:p.Input a customer name. The maximum length is 40 bytes, however, since 
ES_MIXED style is set to this Entry field, the string length in ASCII may 
be shorter than 40 bytes on DBCS OS/2.
:h2 res=1307.Help for Customer Address
:p.Input a customer address. The maximum length is 120 bytes.
:h2 res=1308.Help for Customer Phone Number
:p.Input a customer phon number. The maximum length is 12 bytes.
:h2 res=1309.Help for Register button
:p.Select Register button to register a new customer information in the 
master file.
:h2 res=1310.Help for Cancel button
:p.Select Cancel buttun in the Customer Information dialog to return to
the main dialog.
:h2 res=1311.Help for Gelp button
:p.Select Help button in the dialog to display help information
.*------------------------------------------------------------------------
:euserdoc.
