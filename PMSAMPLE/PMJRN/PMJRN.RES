€ € p   IC              @ А      €€€                ?€€€€€€ь?€€€€€€ь?€€€€€€ь?€€€€€€ь?€€€€€€<?€€€€€ю?€€€€€ь?€€€€€ш?€€€€€р<?€€€€€а|?€€€€€јь?€€€€€Бь?€€€€€ь?€€€€юь?€€€€ьь?€€€€шь?з€€€р?ь?з€Ааь'шь ј€ь'шp Б€ь8ш@  €ь8|   €ь8|   €ь<x   €ь?р   €ь?а   €ь?ј   €ь?А   €ь?А   €ь?    €ь?    €ь>    €ь>    €ь>    €ь<    €ь<    €ь<    €ь<    €ь<    €ь<    ?€ь<    €ь<    Oѕь<    ЅЗь>   рѕь>   рю|>   шю?   €ѕД?   €ѕД?А  €ѕƒ?ј  €зь?а €€уь?ш €€щь?€ €€ьь?€€€€€ьь?€€€зю|ь?€€€зю|ь?€€€с€ь?€€€р€Гь?€€€р€€ь?€€€ш€€ь                                                                                                                                                                                                           ?ј     €р     €ь     €€     €€А    €€€ј   €€€а   €€€а   €€€а   €€€а   ѕ€а   ѕ€а   ?Пс€а   ?Зр€а   Зр€ј   «ш€ј   €€€А   €€€€    €€€€    €€€ю    €€€ь    €€€ш    €€€р    с€ща    xщј    |ю     ?Зю     €р      €А                                                                                                    € €6p   PT    !        @ А      €€€                                           А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А              ј   €€чя€€ј€€чя€€ј€€чя€€ј   ј              А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А      А                                                   €€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€р  ј  р  ј  р  ј  р  ј  р  ј  €€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€ш?€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ € 0 Ф  
August 24, 1989 - Version 1.0
July   20, 1994 - Version 2.0 Compiled for OS/2 V2.x and C-Set compiler

This program allows you to record and playback user interactions PM passes
to windows through message queues.  The menu items allow you to
position a particular window and record or playback.

If you pointed a window it will be positionned before any recording
or playback. This ensures all interactions concerning this window to
be played back correctly, the recording storing absolute desktop coordinates.

You should always Stop the record or playback using 'StopRecordOrPlay'
before exiting the application even when playing back.

This application tries to respect the original timing between messages. But
in some cases due to PM overhead it can appear slower than the original
recording.

If you are recording interactions on windows which contain asynchronous
thread managed child windows, some of the system response could appear in
a different order from the original recording.
Eg: Positioning a frame window recorded after some interaction on a child
window was made could occur before the child window interaction response
ends.
€ € 0 _   The PMJRN Development team is:

- Marc Fiammante  (FIAMMANT @ LGEPROFS)

Version 2.0


€ € 0     R   , " "€ € 0f   f   R     ~Record    ~Playback    ~StopPlayOrRecord     '~News     "~Exit    ,F1=Help € €|0З   З   R  €€         h А  Ф    РМ |€€y          }    А   Лs |€€€€        ~  А  Й  €€€€PM Journal: Help    End Help € € 0Х  R PM Journal & test : Version 2.0 250 80 400 	PMJRNDLL xThe JOURNAL.EXE version %d requires at least the JOURNAL.DLL version %d. This does not match the loaded DLL version %d. %s (%d) 7You are still Journalling. Do you really want to stop? :Are you ready to set the hook? Is the window positionned?  PM Jrn Help: Menu and Pulldowns PM Jrn News .No more than 2 instances of PM Jrn supported!     