******************************************************************************

*                 OS/2* 2.1 Workplace Shell* .IDL files BETA                 *

*           (C) Copyright IBM Corp. 1993 All Rights Reserved                 *

*                    IBM is a registered trademark of the                    *

*                International Business Machines Corporation                 *

*  U.S. Government Users Restricted Rights - Use, duplication or disclosure  *

*           restricted by GSA ADP Schedule Contract with IBM Corp.           *

******************************************************************************


  This README document contains information that describes how the
  Beta .IDL files for the Workplace Shell are to be used.

  These files are pre-release versions and are provided on an as-is basis
  for evaluation and demonstration.  They are not intended for use with
  production code.

  The files included in this beta have been provided for evaluation by
  developers who have the SOMobjects Toolkit* and want to write their
  Workplace Shell subclasses using the Interface Definition Language
  (IDL) described in the SOMobjects Toolkit.

*************************** Disclaimer ********************************

References in this package to IBM products, programs or services do not
imply that IBM intends to make these available in all countries in which
IBM operates.  Any reference to an IBM product, program, or service is not
intended to state or imply that only IBM's product, program, or service may
be used.  Any functionally equivalent program that does not infringe any of
IBM's intellectual property rights may be used instead of the IBM product,
program or service.

Information in this package was developed in conjunction with use of the
equipment specified, and is limited in application to those specific
hardware and software products and levels.

IBM may have patents or pending patent applications covering subject matter
in this package.  The furnishing of this package does not give you any
license to these patents.  You can send license inquiries, in writing, to
the IBM Director of Commercial Relations, IBM Corporation, Purchase,
NY 10577.

The information contained in this package has not been submitted to any
formal IBM test and is distributed AS IS.

The use of this information or the implementation of any of these
techniques is a customer responsibility and depends on the customer's
ability to evaluate and integrate them into the customer's operational
environment.  While each item may have been reviewed by IBM for accuracy in
a specific situation, there is no guarantee that the same or similar
results will be obtained elsewhere.  Customers attempting to adapt these
techniques to their own environments do so at their own risk.

*********************** End of Disclaimer *****************************


General

The WPS PWFolder sample comes with the following REXX programs:

 - ADD.CMD
   This installs the example class and should only be done once.
   It will prompt you for your boot drive.

 - TEST.CMD
   This creates an object of class PWFolder on your desktop.

 - REMOVE.CMD
   This Deregisters the class. Re-boot is required after
   this step, then the DLL that ADD.CMD copied in to the
   \OS2\DLL directory may be deleted.

 - BUILD.CMD
   This runs the NMAKE utility for the samples.
   The way the make files are set up they expect the
   toolkit to be installed on the D drive, and
   the makefile will have to be edited to suit your environment.

PWFolder class example

Files:  PWFOLDER.DLL  - A WPFolder derived class that can be locked
                        with a user specified password

The PWFolder password is passed in via a Setup string. For the
example TEST.CMD sets the password to "wps" by specifying
"PASSWORD=wps;" setup string in the SysCreateObject command.

------------------------------------------------------------------------

