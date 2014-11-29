#define RED         0
#define BLUE        1
#define GREEN       2
#define ID_EXIT     264
#define ID_MENU     263
#define ID_SAVE     262
#define ID_LIST     261
#define ID_COLORS   256
#define ID_MIX      257
#define ID_GREENL   1000
#define ID_BLUEL    1001
#define ID_REDL     1002
#define ID_GREEN    ID_COLOR_BASE+GREEN
#define ID_BLUE     ID_COLOR_BASE+BLUE
#define ID_RED      ID_COLOR_BASE+RED
#define ID_COLOR_BASE   2000
typedef struct _COLORDEF { LONG SystemColor;
                           CHAR Label[40];
                           CHAR Key[40];
                           SHORT Red;
                           SHORT Green;
                           SHORT Blue;
                           } COLORDEF;
#define NUM_PMCOLORS 32
COLORDEF  PMColors[]= {
     {  SYSCLR_BUTTONLIGHT             , "ButtonLight"           ,"ButtonLight"      ,0 ,0 ,0 },
     {  SYSCLR_BUTTONMIDDLE            , "ButtonMiddle"          ,"ButtonMiddle"     ,0 ,0 ,0 },
     {  SYSCLR_BUTTONDARK              , "ButtonDark"            ,"ButtonDark"       ,0 ,0 ,0 },
     {  SYSCLR_BUTTONDEFAULT           , "ButtonDefault"         ,"ButtonDefault"    ,0 ,0 ,0 },
     {  SYSCLR_TITLEBOTTOM             , "TitleBottom"           ,"TitleBottom"      ,0 ,0 ,0 },
     {  SYSCLR_SHADOW                  , "Shadow"                ,"Shadow"           ,0 ,0 ,0 },
     {  SYSCLR_ICONTEXT                , "IconText"              ,"IconText"         ,0 ,0 ,0 },
     {  SYSCLR_DIALOGBACKGROUND        , "DialogBackground"      ,"DialogBackground" ,0 ,0 ,0 },
     {  SYSCLR_HILITEFOREGROUND        , "HiliteForeground"      ,"HiliteForeground" ,0 ,0 ,0 },
     {  SYSCLR_HILITEBACKGROUND        , "HiliteBackground"      ,"HiliteBackground" ,0 ,0 ,0 },
     {  SYSCLR_INACTIVETITLETEXTBGND   , "InactiveTitleTextBgnd" ,"FKAbackground"    ,0 ,0 ,0 },
     {  SYSCLR_ACTIVETITLETEXTBGND     , "ActiveTitleTextBgnd"   ,"FKAtext"          ,0 ,0 ,0 },
     {  SYSCLR_INACTIVETITLETEXT       , "InactiveTitleText"     ,"InactiveTitleText",0 ,0 ,0 },
     {  SYSCLR_ACTIVETITLETEXT         , "ActiveTitleText"       ,"ActiveTitleText"  ,0 ,0 ,0 },
     {  SYSCLR_OUTPUTTEXT              , "OutputText"            ,"OutputText"       ,0 ,0 ,0 },
     {  SYSCLR_WINDOWSTATICTEXT        , "WindowStaticText"      ,"WindowStaticText" ,0 ,0 ,0 },
     {  SYSCLR_SCROLLBAR               , "Scrollbar"             ,"Scrollbar"        ,0 ,0 ,0 },
     {  SYSCLR_BACKGROUND              , "Background"            ,"Background"       ,0 ,0 ,0 },
     {  SYSCLR_ACTIVETITLE             , "ActiveTitle"           ,"ActiveTitle"      ,0 ,0 ,0 },
     {  SYSCLR_INACTIVETITLE           , "InactiveTitle"         ,"InactiveTitle"    ,0 ,0 ,0 },
     {  SYSCLR_MENU                    , "Menu"                  ,"Menu"             ,0 ,0 ,0 },
     {  SYSCLR_WINDOW                  , "Window"                ,"Window"           ,0 ,0 ,0 },
     {  SYSCLR_WINDOWFRAME             , "WindowFrame"           ,"WindowFrame"      ,0 ,0 ,0 },
     {  SYSCLR_MENUTEXT                , "MenuText"              ,"MenuText"         ,0 ,0 ,0 },
     {  SYSCLR_WINDOWTEXT              , "WindowText"            ,"WindowText"       ,0 ,0 ,0 },
     {  SYSCLR_TITLETEXT               , "TitleText"             ,"TitleText"        ,0 ,0 ,0 },
     {  SYSCLR_ACTIVEBORDER            , "ActiveBorder"          ,"ActiveBorder"     ,0 ,0 ,0 },
     {  SYSCLR_INACTIVEBORDER          , "InactiveBorder"        ,"InactiveBorder"   ,0 ,0 ,0 },
     {  SYSCLR_APPWORKSPACE            , "AppWorkspace"          ,"AppWorkspace"     ,0 ,0 ,0 },
     {  SYSCLR_HELPBACKGROUND          , "HelpBackground"        ,"HelpBackground"   ,0 ,0 ,0 },
     {  SYSCLR_HELPTEXT                , "HelpText"              ,"HelpText"         ,0 ,0 ,0 },
     {  SYSCLR_HELPHILITE              , "HelpHilite"            ,"HelpHilite"       ,0 ,0 ,0 } };
