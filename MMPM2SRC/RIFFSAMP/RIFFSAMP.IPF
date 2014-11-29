:userdoc.
:h1 res=001.Help for the RIFF Compound File Sample.
:p.
The RIFF compound file utility illustrates the concept of creating a RIFF
file, adding elements to a RIFF file, deleting elements from a RIFF file,
playing elements from a RIFF file and also compacting a RIFF file with
deleted entries.  To see the details for the RIFF compound file,
please refer to the source code of the sample program.
:p.
RIFF compound file is created when the application is started.
:p.
To add wave files to the RIFF compound file, select one wave file from
the
:hp2.Add To File:ehp2. list box and select the
:hp2.Add:ehp2. pushbutton.
The wave is added to the CTOC of the RIFF compound file.  The wave file
is deleted from the Add To File list box and added to the File TOC list box.
:p.
To delete wave element from RIFF file, select one element from
:hp2.File TOC:ehp2. list box and select the :hp2.Delete:ehp2. pushbutton.
The wave element is deleted from the CTOC of
the RIFF compound file.  The wave element is deleted from the File Toc list box
and the wave is added to the Add to File list box.
:p.
To play an element from the RIFF file, select one element from
:hp2.File TOC :ehp2. list box and select the :hp2.Play:ehp2. pushbutton.
:p.
Select :hp2.Compact:ehp2. pushbutton to clean the RIFF compound file.
When a wave is deleted from a RIFF compound file, only its entry is deleted
from the table of contents (CTOC) of RIFF file and in order to delete the
element data from CGRP a RIFF file should be compacted.
:p.
Select the :hp2.F1:ehp2. key to view this help panel.
:euserdoc.
