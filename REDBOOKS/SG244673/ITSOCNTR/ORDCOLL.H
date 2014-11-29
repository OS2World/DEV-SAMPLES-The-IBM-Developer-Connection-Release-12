/*
  File:    OrdColl.h

  Contains:  Definition of class ODxOrderedCollection

  Written by:  Richard Rodseth

  Copyright:  © 1993-94 by Apple Computer, Inc., all rights reserved.

  Change History (most recent first):

     <6>   6/20/94  RR    ODMemoryHeap* -> ODMemoryHeapID
     <5>   6/14/94  RR    Added forward declaration ODMemoryHeap
     <4>    6/9/94  RR    Remove ASLM stuff
     <2>   5/10/94  RR    Removed ASLM_COMPATIBLE/CDECL
     <1>    5/5/94  CG    first checked in
     <9>   3/15/94  MB    Changes to support SCpp/ASLM builds,
                  #1150864.
     <8>    2/9/94  T‚    add a couple of explicit "virtual" to the
                  class declaration to pacify ASLM
     <7>    2/7/94  JA    Tiger Team Makeover!
     <6>    2/2/94  NP    Added support for allocating internal
                  structures in given heap.
     <5>   12/7/93  NP    Added helpful comments, because PEOPLE
                  AREN'T COMMENTING HEADER FILES!!!!!
     <4>  11/19/93  PH    ASLM fix - add class id for ODxOCValueLink
     <3>  11/19/93  NP    Add definition of ODxOCValueLink to here (to
                  allow subclassing.)
     <1>   8/13/93  RCR    first checked in

  To Do:
*/

#ifndef _ORDCOLL_
#define _ORDCOLL_

#ifndef _ODTYPES_
   // pick up definitions like ODBoolean, kODNULL, etc
   #include "ODTypes.h"
#endif


//==============================================================================
// Theory of Operation
//==============================================================================

//==============================================================================
// Constants
//==============================================================================

//==============================================================================
// Scalar Types
//==============================================================================

typedef void* ElementType;

//=====================================================================================
// Classes defined in this interface
//=====================================================================================

class ODxOrderedCollection;  // An ordered (not sorted) collection of ElementTypes
class ODxOrderedCollectionIterator;

//=====================================================================================
// Classes used by this interface
//=====================================================================================

class ODxOCValueLink;       // A link plus a value of type ElementType.

//=====================================================================================
// Global Variables
//=====================================================================================

//=====================================================================================
// Class ODxOCValueLink - Definition
//=====================================================================================


//=====================================================================================
// Class ODxOrderedCollection
//=====================================================================================

class ODxOrderedCollection
{
  
public:

  ODxOrderedCollection();
  //ODxOrderedCollection(ODMemoryHeapID where);
  virtual ~ODxOrderedCollection();

           long Count() const;
  
  virtual void  AddFirst(ElementType element);
  virtual void  AddLast(ElementType element);
  virtual void  AddBefore(ElementType existing, ElementType tobeadded);
  virtual void  AddAfter(ElementType existing, ElementType tobeadded);

  ElementType  After(ElementType existing);
  ElementType  Before(ElementType existing);

  ElementType  First();
    // Returns kODNULL if there is no first element.
  ElementType  Last();

  virtual ElementType  RemoveFirst();
    // Don't call if there are no elements. Crash will result.
  virtual ElementType  RemoveLast();
  virtual void  RemoveAll();
  
    // Called from the destructor. Removes all elements, deleting the links
    // Does not delete the elements themselves
    
  //virtual void  DeleteAll();
  
    // Removes and deletes all elements
    
  virtual void  Remove(ElementType existing);
  virtual ODBoolean  Contains(ElementType existing);
  
  virtual ODxOrderedCollectionIterator* CreateIterator();

  ODxOCValueLink * After(ODxOCValueLink *existing);
  ODxOCValueLink * Before(ODxOCValueLink *existing);
  ODxOCValueLink * FirstVL();
  ODxOCValueLink * LastVL();

protected:
   virtual ODxOCValueLink*   CreateNewLink(ElementType value) const;
   virtual ODBoolean  ElementsMatch(ElementType v1,ElementType v2) const;
     // Does a pointer comparison by default 

private:   
  ODxOCValueLink * fImplementation;

  friend class ODxOrderedCollectionIterator;
  friend class ListIterator;
};

//=====================================================================================
// Class ODxOrderedCollectionIterator
//=====================================================================================

class ODxOrderedCollectionIterator {
public:
  ODxOrderedCollectionIterator(ODxOrderedCollection* collection);
  ~ODxOrderedCollectionIterator();
  ElementType  First();
  ElementType  Next();
  ElementType  Last();
  // ElementType  Previous();
  ODBoolean  IsNotComplete();
  
private:
    ODxOrderedCollection*  fCollection;
    ODxOCValueLink * fImplementation;
};

#endif // _ORDCOLL_
