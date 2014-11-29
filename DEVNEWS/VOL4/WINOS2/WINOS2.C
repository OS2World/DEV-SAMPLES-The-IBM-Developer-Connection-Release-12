

char *VDD_NAME = "VDD.SYS";
long  far VDDPROCADR;
short OS2VERSION = 0;

main()
{

     OS2VERSION = OS2TEST();
     if(OS2VERSION)
          printf("OS/2  Version is %d",OS2VERSION);
     else
          printf("Not running on OS/2");
     GETVDDAPIENTRY();

}

