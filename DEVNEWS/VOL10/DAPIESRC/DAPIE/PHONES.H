#define MAXFRIENDS  9

/***********************************************************************/
/*  This is a simple phone book database.                              */
/***********************************************************************/
typedef struct _Phones
{
   PSZ NickName;
   PSZ FullName;
   PSZ Phone;
}PhoneBook;

/***********************************************************************/
/*  Normal programs would read this data from a file.                  */
/***********************************************************************/
PhoneBook Friends[MAXFRIENDS] =
{
   "Giles",      "Kevin Giles",        "214-555-1212",
   "Bubba",      "Hank Smith",         "713-555-1212",
   "Fred",       "Fred Bicycle",       "817-555-1212",
   "Jack",       "Jack Anjill",        "919-555-1212",
   "John",       "John Richards",      "214-555-1212",
   "Toni",       "Toni Henderson",     "919-555-1212",
   "Babe",       "George Herman Ruth", "212-555-1212",
   "Kevin",      "Kevin Kortrel",      "817-555-1212",
   "Honest Abe", "Abraham Lincoln",    "none"
};
