/*---------------------------------------------------------------------*/
/* Recording Started on Thursday 4 July 1991 at 14:38:30 */
signal on ERROR
signal on FAILURE
trace off
Funct1='GETOBJECTTEXT'
if RxFuncQuery( Funct1 ) Then Call RxFuncAdd Funct1, 'GETTEXT', Funct1
say GetObjectText()
say '>'text'<'
say 'Text Length' length(text)
say '>'substr(viotext,1,80)'<'
say '>'length(viotext)'<'
Call RxFuncDrop Funct1
exit rc
FAILURE:
ERROR:
Call RxFuncDrop Funct1
say "Error "rc  line sigl":"sourceline(sigl)
say errortext(rc)
Exit rc
/* Recording Ended on Thursday 4 July 1991 at 14:39:00 */
