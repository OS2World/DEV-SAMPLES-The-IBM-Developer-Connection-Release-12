
#define VDDNAME   "VDD.SYS"

/*
 *the commands that get passed back and forth between the
 *vdd and the processes
 */

#define INIT_COMMAND        0
#define POST_MESSAGE        1
#define READ_QUEUE          2
#define TERMINATE_COMMAND   3

/*
 *message types that are currently supported
 */
#define TEXT_MESSAGE        0
#define BINARY              1
#define ENCRYPTED           3

#define SIZ_COMMAND_BUF  256
#define ONCE            while(0)

typedef struct _message
{
     SGID  SrcSessionIdentifier;
     SGID  DstSessionIdentifier;
     ULONG ulMessageType;
     ULONG ulMessageSubCommand;
     ULONG ulBufferSize;
     PVOID pvMessageData;
     PVOID pvNextMessage;
}MESSAGE,*PMESSAGE;

typedef struct _messagelist
{
     PMESSAGE  pHeadList;
     PMESSAGE  pHeadNext;
}MESSAGELIST,*PMESSAGELIST;


