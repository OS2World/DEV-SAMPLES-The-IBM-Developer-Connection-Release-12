 
/*******************************************************************/
/* generals.cpp - Accompanying example program to the "Mustering   */
/* DCE Registry Objects with the Managed Object Class Library"     */
/* article.  This example assumes that the user is logged into DCE */
/* with an account that has permission to create accounts.  The    */
/* program runs against your local cell.                           */
/*                                                                 */
/* To invoke:  general ípasswordù                                  */
/*******************************************************************/
 
#include <som.xh>
#include <mocl/DCE_SecurityRegistry.xh>
#include <mocl/DCE_Domain.xh>
#include <mocl/DCE_PropertyDefs.xh>
#include <mocl/Base_PolicyRegions.xh>
#include <mocl/Base_MemUtils.xh>
#include <mocl/BaseExcept.hpp>
#include <mocl/moclbase.h>
#include <mocl/sequence.h>
#include <stream.h>
#include <dce/nbase.h>
 
//
// specify global variables
//
 
static char *pszpassword = NULL;
static char *pszcellname = "/.:";
static Base_MemUtils_Base_FreeUtils     *bf;
 
//
// specify defines
//
 
#define OK                      0
#define NOK                     1
#define NVP_LENGTH             10
#define DEFAULT_CELL_PASSWORD  "-dce-"
#define ARMY_COMMAND_ERA       "armycommand"
#define ACCT_NAME_LEE          "lee"
#define ACCT_NAME_GRANT        "grant"
 
//
// specify functions
//
 
int create_account(char                  *pszacctname,
    Environment                          *ev,
    DCE_SecurityRegistry_DCE_Account     **pacct,
    DCE_SecurityRegistry_DCE_AccountIM   *pacctim,
    DCE_Domain_DCE_Cell                  *pcellobj,
    Base_PolicyRegions_Base_PolicyRegion *ppregion);
 
int define_acct_properties(char      *pszacctname,
    Environment                      *ev,
    DCE_SecurityRegistry_DCE_Account *pacct,
    char                             *pszacctpropername,
    char                             *pszeradescription);
 
int get_acct_properties(char         *pszacctname,
    Environment                      *ev,
    DCE_SecurityRegistry_DCE_Account *pacct);
 
void print_ev(Environment *ev);
 
//
// main program
//
 
main(int argc, char *argvíù)
{
   Environment               *ev = somGetGlobalEnvironment();
   Environment               *local_ev=NULL;
   
   Base_Instances_Base_MOCL             *bm=NULL;
   Base_Instances_Base_Library          *bl=NULL;
 
   DCE_SecurityRegistry_DCE_AccountIM             *pacctim=NULL;
   DCE_Domain_DCE_Cell                            *pcell=NULL;
   DCE_SecurityRegistry_DCE_RegistrySchemaEntryIM *pschim=NULL;
   DCE_SecurityRegistry_DCE_RegistrySchemaEntry   *psch=NULL;
   Base_PolicyRegions_Base_PolicyRegion           *pdefaultpr=NULL;
 
   DCE_SecurityRegistry_DCE_Account      *pacctgrant=NULL;
   DCE_SecurityRegistry_DCE_Account      *pacctlee=NULL;
 
   SysAdminTypes_LabelType        lt;
   SysAdminTypes_ObjectLabelList  oll;
   CosLifeCycle_Key               key;
   CosLifeCycle_Criteria          my_criteria;
   NameValuePair                  my_nvpíNVP_LENGTHù;
   boolean                        brc=OK;
   char                           *psznormalizedcellname=NULL;
 
   // get password, if one was specified
 
   if (argc > 1) {
      pszpassword = (char *) malloc(strlen(argví1ù)+1);
      strcpy(pszpassword, argví1ù);
   } else {
      pszpassword = (char *) malloc(strlen(DEFAULT_CELL_PASSWORD)+1);
      strcpy(pszpassword, DEFAULT_CELL_PASSWORD);
   } /* endif */
   cout << "+++ Running generals.exe +++" << endl;
   cout << "+++ Using password " << pszpassword << endl;
 
   // MOCL_getLibrary should be the first call before your client 
   // program calls other MOCL methods.
 
   MOCL_getLibrary(ev, bm, bl, NULL);
 
   if (ev->_major ]= NO_EXCEPTION) {
      cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
      cout << "ERROR:  MOCL_getLibrary failed." << endl;
      print_ev(ev);
      return NOK;
   }
 
   // Create the global memory free utility object.
 
   bf = new Base_MemUtils_Base_FreeUtils();
 
   // Create the DCE cell object which will be used later when 
   // creating objects to specify their location.
 
   pcell = new DCE_Domain_DCE_Cell(ev, pszcellname, pszcellname);
 
   if (ev->_major ]= NO_EXCEPTION) {
      cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
      cout << "ERROR:  DCE_Cell proxy failed." << endl;
      print_ev(ev);
      return NOK;
   } else {
      psznormalizedcellname = pcell->get_cell_name(ev);
      cout << "******************************************" << endl;
      cout << "DCE_Cell proxy created for cell " << 
         psznormalizedcellname << endl;
      cout << "******************************************" << endl;
   }
 
   // MOCL_getLibrary created the default policy region when it was 
   // called.  Since this program does not need a specialized policy
   // region object, retrieve the default policy region object for use
   // when creating registry objects, such as accounts.
 
   pdefaultpr = (Base_PolicyRegions_Base_PolicyRegion *)
     ((void *)(bl->get_default_policy_region(ev)));
 
   if (ev->_major ]= NO_EXCEPTION) {
      cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
      cout << "ERROR:  Base_Library->get_default_policy_region "
           << "failed." << endl;
      print_ev(ev);
      return NOK;
   } else {
      cout << "******************************************" << endl;
      cout << "Retrieved the default policy region object." << endl;
      cout << "******************************************" << endl;
   }
 
   // Create the schema instance manager.
 
   sequenceMaximum(my_criteria) = NVP_LENGTH;
   sequenceLength(my_criteria) = 2;
   my_criteria._buffer = my_nvp;
 
   my_nvp->name = LabelidProperty;
   my_nvp->value._type = TC_string;
   my_nvp->value._value = psznormalizedcellname;
 
   (my_nvp+1)->name = ClassTypeProperty;
   (my_nvp+1)->value._type = TC_string;
   (my_nvp+1)->value._value = 
      "DCE_SecurityRegistry::DCE_RegistrySchemaEntry";
 
   sequenceLength(key) = 1;
   key._buffer = new CosNaming_NameComponent;
 
   sequenceElement(key, 0).id =
      "DCE_SecurityRegistry::DCE_RegistrySchemaEntryIM";
 
   pschim = (DCE_SecurityRegistry_DCE_RegistrySchemaEntryIM *)
        ((void *)(bl->create_object(ev, &key, &my_criteria)));
 
   delete key._buffer;
 
   if (ev->_major ]= NO_EXCEPTION) {
      cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
      cout << "ERROR:  DCE_RegistrySchemaEntryIM create failed." 
           << endl;
      print_ev(ev);
      return NOK;
   } else {
      cout << "******************************************" << endl;
      cout << "Created the DCE_RegistrySchemaEntryIM object." << endl;
      cout << "******************************************" << endl;
   }
 
   // Check if the schema entry (ERA) for army command exists.
 
   lt.id = ARMY_COMMAND_ERA;
   lt.kind = "DCE_SecurityRegistry::DCE_RegistrySchemaEntry";
 
   local_ev = SOM_CreateLocalEnvironment();
   psch = (DCE_SecurityRegistry_DCE_RegistrySchemaEntry*)(void*)
               pschim->lookup_object(local_ev, &lt, NULL);
 
   if (local_ev->_major ]= NO_EXCEPTION) {
 
      // Check to see what type of exception was returned.
 
      if (strcmp( somExceptionId(local_ev),
             ex_Base_SysAdminException_ExNotFound) ]= 0) {
 
         // The exception was not ExNotFound, so merge the local 
         // exception stack with the global one, display the
         // exception(s), and return with an error.
 
         Join_Base_SysAdminException(ev, local_ev);
         SOM_DestroyLocalEnvironment(local_ev);
         cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;        
         cout << "ERROR:  DCE_RegistrySchemaEntryIM lookup_object "
              << "failed." << endl;
         print_ev(ev);
         return NOK;
      } else {
 
         // The exception was ExObjNotFound.  Free the exception stack
         // on the local environment before later freeing the local.
 
         bf->free_BaseSysAdminException(local_ev);
 
         // The ERA does not exist, so create it.
 
         my_nvp->name = LabelidProperty;
         my_nvp->value._type = TC_string;
         my_nvp->value._value = ARMY_COMMAND_ERA;
 
         (my_nvp+1)->name = LocationProperty;
         (my_nvp+1)->value._type = TC_SysAdminLifeCycle_Location;
         (my_nvp+1)->value._value = pcell;
 
         unsigned long encoding =
            DCE_SecurityRegistry_print_string_type;
         (my_nvp+2)->name = EncodingProperty;
         (my_nvp+2)->value._type =
            TC_DCE_SecurityRegistry_ExtendedPropertyTypes;
         (my_nvp+2)->value._value = &encoding;
 
         static char *acllistíù = { "account r r r r" };
 
         DCE_SecurityRegistry_ACLMgr aclsequence;
         sequenceMaximum(aclsequence) = 1;
         sequenceLength(aclsequence) = 1;
         aclsequence._buffer = acllist;
         (my_nvp+3)->name = ACLMgrProperty;
         (my_nvp+3)->value._type = TC_DCE_SecurityRegistry_ACLMgr;
         (my_nvp+3)->value._value = &aclsequence;
 
         boolean multivalued = FALSE;
         (my_nvp+4)->name = MultiValuedProperty;
         (my_nvp+4)->value._type = TC_boolean;
         (my_nvp+4)->value._value = &multivalued;
 
         sequenceLength(my_criteria) = 5;
         psch = (DCE_SecurityRegistry_DCE_RegistrySchemaEntry*)
            ((void *)(pschim->create_object(ev, NULL, &my_criteria)));
 
         if (ev->_major ]= NO_EXCEPTION) {
            cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                 << endl;
            cout << "ERROR:  DCE_RegistrySchemaEntry create failed." 
                 << endl;
            print_ev(ev);
            return NOK;
         } else {
            cout << "******************************************" 
                 << endl;
            cout << "Created the DCE_RegistrySchemaEntry object "
                 << ARMY_COMMAND_ERA << "." << endl;
            cout << "******************************************" 
                 << endl;
         }
      }
   } else {
      cout << "******************************************" << endl;
      cout << "Found the DCE_RegistrySchemaEntry object "
           << ARMY_COMMAND_ERA << "." << endl;
      cout << "******************************************" << endl;
   }
 
   SOM_DestroyLocalEnvironment(local_ev);
 
   // Create the account instance manager.
 
   sequenceLength(my_criteria) = 2;
 
   my_nvp->name = LabelidProperty;
   my_nvp->value._type = TC_string;
   my_nvp->value._value = psznormalizedcellname;
 
   (my_nvp+1)->name = ClassTypeProperty;
   (my_nvp+1)->value._type = TC_string;
   (my_nvp+1)->value._value = "DCE_SecurityRegistry::DCE_Account";
 
   sequenceLength(key) = 1;
   key._buffer = new CosNaming_NameComponent;
 
   sequenceElement(key, 0).id =
                        "DCE_SecurityRegistry::DCE_AccountIM";
 
   pacctim = (DCE_SecurityRegistry_DCE_AccountIM *)
       ((void *)(bl->create_object(ev, &key, &my_criteria)));
 
   delete key._buffer;
 
   if (ev->_major ]= NO_EXCEPTION) {
      cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
      cout << "ERROR:  DCE_AccountIM create failed." << endl;
      print_ev(ev);
      return NOK;
   } else {
      cout << "******************************************" << endl;
      cout << "Created the DCE_AccountIM object." << endl;
      cout << "******************************************" << endl;
   }
 
   brc = create_account(ACCT_NAME_GRANT, ev, &pacctgrant,
      pacctim, pcell, pdefaultpr);
 
   if (brc ]= OK) {
      return brc;
   }
 
   brc = create_account(ACCT_NAME_LEE, ev, &pacctlee,
       pacctim, pcell, pdefaultpr);
 
   if (brc ]= OK) {
      return brc;
   }
 
   brc = define_acct_properties(ACCT_NAME_GRANT, ev,
     pacctgrant, "Ulysses S. Grant", "Army of the Potomac");
 
   if (brc ]= OK) {
      return brc;
   }
 
   brc = define_acct_properties(ACCT_NAME_LEE, ev,
     pacctlee, "Robert E. Lee", "Army of Northern Virginia");
 
   if (brc ]= OK) {
      return brc;
   }
 
   brc = get_acct_properties(ACCT_NAME_GRANT, ev, pacctgrant);
 
   if (brc ]= OK) {
      return brc;
   }
 
   brc = get_acct_properties(ACCT_NAME_LEE, ev, pacctlee);
 
   if (brc ]= OK) {
      return brc;
   }
 
   // Free the proxy objects.
 
   pcell->free_proxy(ev);
   psch->free_proxy(ev);
   pacctlee->free_proxy(ev);
   pacctgrant->free_proxy(ev);
 
   // Free memory allocated by this program.
 
   free(pszpassword);
 
   // Free the MOCL library when you're done with using MOCL objects 
   // or methods.  The MOCL_freeLibrary call will also free all the
   // instance managers too.
 
   MOCL_freeLibrary(bm, bl);
 
   return OK;
}
 
int create_account(char                     *pszacctname,
       Environment                          *ev,
       DCE_SecurityRegistry_DCE_Account     **pacct,
       DCE_SecurityRegistry_DCE_AccountIM   *pacctim,
       DCE_Domain_DCE_Cell                  *pcell,
       Base_PolicyRegions_Base_PolicyRegion *ppregion)
{
   CosLifeCycle_Criteria   my_criteria;
   NameValuePair           my_nvpíNVP_LENGTHù;
   BasePolicyRegions       rgns;
   BasePolicyRegions       *rgnsp=NULL;
   Environment             *local_ev=NULL;
   SysAdminTypes_LabelType lt;
   boolean                 brc = OK;
 
   sequenceMaximum(my_criteria) = NVP_LENGTH;
   sequenceLength(my_criteria) = 8;
   my_criteria._buffer = my_nvp;
    
   my_nvp->name = LabelidProperty;
   my_nvp->value._type = TC_string;
   my_nvp->value._value = pszacctname;
 
   (my_nvp+1)->name = LocationProperty;
   (my_nvp+1)->value._type = TC_SysAdminLifeCycle_Location;
   (my_nvp+1)->value._value = pcell;
 
   // The principal which forms this account will be a member of the 
   // to the "none" group and "none" policy group (organization)
   // objects.
 
   (my_nvp+2)->name = GroupProperty;
   (my_nvp+2)->value._type = TC_string;
   (my_nvp+2)->value._value = "none";
 
   (my_nvp+3)->name = PolicyGroupProperty;
   (my_nvp+3)->value._type = TC_string;
   (my_nvp+3)->value._value = "none";
 
   rgnsp = &rgns;
   sequence_new(&rgns, Base_PolicyRegions_Base_PolicyRegion *, 1);
   sequence_add(&rgns, Base_PolicyRegions_Base_PolicyRegion *,
     ppregion, 1);
   (my_nvp+4)->name = PolicyRegionsProperty;
   (my_nvp+4)->value._type =
      TC_Base_PolicyRegions_Base_PolicyDrivenBase_BasePolicyRegions;
   (my_nvp+4)->value._value = (void *)&rgnsp;
 
   (my_nvp+5)->name = DescriptionProperty;
   (my_nvp+5)->value._type = TC_string;
   (my_nvp+5)->value._value = "General's account";
 
   Password  mypwd;
   mypwd.version_number = 0;
   mypwd.pepper = NULL;
   mypwd.key._d = DCE_SecurityRegistry_plain_password;
   mypwd.key._u.plain = pszpassword;
   (my_nvp+6)->name = MyPwdProperty;
   (my_nvp+6)->value._value = &mypwd;
   (my_nvp+6)->value._type = TC_DCE_SecurityRegistry_Password;
 
   //  The password for the new account
 
   Password  newpwd;
   newpwd.version_number = 0;
   newpwd.pepper = NULL;
   newpwd.key._d = DCE_SecurityRegistry_plain_password;
   newpwd.key._u.plain = "-dce-";
   (my_nvp+7)->name = PasswordProperty;
   (my_nvp+7)->value._value = &newpwd;
   (my_nvp+7)->value._type = TC_DCE_SecurityRegistry_Password;
 
   lt.id = pszacctname;
   lt.kind = "DCE_SecurityRegistry::DCE_Account";
 
   local_ev = SOM_CreateLocalEnvironment();
   *pacct = (DCE_SecurityRegistry_DCE_Account *)
      ((void *)pacctim->lookup_object(local_ev, &lt, NULL));
 
   if (local_ev->_major ]= NO_EXCEPTION) {
 
      // Check to see what type of exception was returned.
 
      if (strcmp( somExceptionId(local_ev),
             ex_Base_SysAdminException_ExObjNotFound) ]= 0) {
 
         // The exception was not ExObjNotFound, so merge the local 
         // exception stack with the global one, display the
         // exception(s), and return with an error.
 
         Join_Base_SysAdminException(ev, local_ev);
         cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;        
         cout << "ERROR:  DCE_AccountM lookup_object failed." << endl;
         print_ev(ev);
         brc = NOK;
      } else {
 
         // The exception was ExObjNotFound.  Free the exception stack
         // on the local environment before later freeing the local
         // environment.
 
         bf->free_BaseSysAdminException(local_ev);
 
         // The Account does not exist, so create it.
 
         *pacct = (DCE_SecurityRegistry_DCE_Account *)
            ((void *)(pacctim->create_object(ev, NULL, 
            &my_criteria)));
 
         if (ev->_major ]= NO_EXCEPTION) {
            cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" 
                 << endl;
            cout << "ERROR:  DCE_Account Create failed for " 
                 << pszacctname << endl;
            print_ev(ev);
            return NOK;
         } else {
            cout << "******************************************" 
                 << endl;
            cout << "Created the DCE_Account object for " 
                 << pszacctname << endl;
            cout << "******************************************" 
                 << endl;
         }
      }
   } else {
      cout << "******************************************" << endl;
      cout << "Found the DCE_Account object for " << pszacctname
           << endl;
      cout << "******************************************" << endl;
   }
 
   SOM_DestroyLocalEnvironment(local_ev);
   sequence_free(&rgns);
 
   return brc;
}
 
int define_acct_properties(char      *pszacctname,
    Environment                      *ev,
    DCE_SecurityRegistry_DCE_Account *pacct,
    char                             *pszacctpropername,
    char                             *pszeradescription)
{
    Property    property;
    Properties  properties;
    boolean     brc = OK;
 
    sequence_new(&properties, Property, 2);
 
    property.property_name = DescriptionProperty;
    property.property_value._type = TC_string;
    property.property_value._value = pszacctpropername;
    sequence_add(&properties, Property, property, 1);
 
    property.property_name = ARMY_COMMAND_ERA;
    property.property_value._type = TC_string;
    property.property_value._value = pszeradescription;
    sequence_add(&properties, Property, property, 1);
 
    pacct->define_properties(ev, &properties);
    if (ev->_major ]= NO_EXCEPTION) {
       cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
       cout << "ERROR:  DCE_Account define_properties failed for "
            << pszacctname << endl;
       print_ev(ev);
       brc = NOK;
    } else {
       cout << "******************************************" << endl;
       cout << "DCE_Account define_properties succeeded for "
            << pszacctname << endl;
       cout << "******************************************" << endl;
    }
 
    sequence_free(&properties);
 
    return brc;
}
 
int get_acct_properties(char         *pszacctname,
    Environment                      *ev,
    DCE_SecurityRegistry_DCE_Account *pacct)
{
   int           i=0;
   Property      property;
   Properties    properties;
   PropertyNames names;
   boolean       bmore = FALSE;
   boolean       brc = OK;
   CosPropertyService_PropertiesIterator *propiterator=NULL;
    
   sequence_new(&names, PropertyName, 3);
   sequence_null(&properties);
    
   sequence_add(&names, PropertyName, DescriptionProperty, 1);
   sequence_add(&names, PropertyName, FullNameProperty, 1);
   sequence_add(&names, PropertyName, ARMY_COMMAND_ERA, 1);
 
   pacct->get_properties(ev, 2, &names, &properties, &propiterator);
   if (ev->_major ]= NO_EXCEPTION) {
      cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
      cout << "ERROR:  DCE_Account get_properties failed for " 
           << pszacctname << endl;
      print_ev(ev);
      brc = NOK;
   } else {
      cout << endl;
      cout << "******************************************" << endl;
      cout << "Account properties returned by get_properties for "
           << pszacctname << endl;
      cout << endl;
      for (i = 0; i < sequenceLength(properties); i++) {
         property = sequenceElement(properties, i);
         cout << "    Name:  " << property.property_name << endl;
         cout << "       Value:  " << 
            (char *)property.property_value._value << endl;
      }
      if (propiterator ]= NULL) {
         cout << endl;
         cout << "Iterator returned properties for " << pszacctname 
            << endl;
         cout << endl;
         while (TRUE) {
            bmore = propiterator->next_one(ev, &property);
            cout << "    Name:  " << property.property_name << endl;
            cout << "       Value:  "
                 << (char *)property.property_value._value << endl;
            bf->free_Property(ev, &property);
            if (]bmore) {
               break;
            }
         }
         delete propiterator;
      }
 
      bf->free_Properties(ev, &properties);
   }
   sequence_free(&names);
 
   return brc;
}
 
void print_ev(Environment *ev)
{
   error_status_t rc;
   ExCursor cursor;
 
   cout << "*** Start Printing Exceptions ***" << endl;
 
   rc = Print_First_Base_SysAdminException(ev,&cursor);
   while (rc == error_status_ok) {
      rc = Print_Next_Base_SysAdminException(ev,&cursor);
   }; 
   cout << endl << "*** End Printing Exceptions ***" << endl;
 
   return;
}
 
