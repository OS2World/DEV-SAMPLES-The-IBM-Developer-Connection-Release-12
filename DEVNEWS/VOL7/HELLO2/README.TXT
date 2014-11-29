Building Hello World II

Building the Hello World part is easy.  Install the OpenDoc Alpha toolkit from DevCon7 on your 
system, and ensure that you can rebuild one of the sample parts.  Then, create a new 
subdirectory called "...\Samples\Parts\Hello2."

Set your default directory to ...\HELLO2

Copy the Hello World files from the DevCon7 CD-ROM to the HELLO2 directory and then type
"nmake."

The Hello World part will be compiled and linked.  You must copy the resulting .DLL into your
OpenDoc "bin" directory.

Running Hello World II

Start the docshell.  An untitled document will appear.  Select the EMBED menu pull-down from
the action bar.  Select "Hello World" from the menu; the part will appear in the lower left-hand
corner of the document.

To move or resize it, hold down the Ctrl key and press hte left-mouse button somewhere in the
Hello World part.  A selection border and resize handles will appear.  Dragging the selection border
moves the part; the resize handles change its shape.

A single click in the part activates it.  Clicking with the right-mouse button increments the 
iteration count. 

With the Hello World part selected, go to the SELECTED menu pull-down.  The "Show As"
menu contains Frame, Small, Icon, and Thumbnail views.   Choose one of these standard view 
types and Hello World will change accordingly.


