/********************************************************************/
/*  Licensed Materials - Property of IBM                            */
/*                                                                  */
/*                                                                  */
/* Copyright (C) International Business Machines Corp., 1994.       */
/* Copyright (C) Apple Computer, Inc., 1994                         */
/*                                                                  */
/*  US Government Users Restricted Rights -                         */
/*  Use, duplication, or disclosure restricted                      */
/*  by GSA ADP Schedule Contract with IBM Corp.                     */
/*                                                                  */
/*  IBM Change History (most recent first):                         */
/*  <OS2>   8/18/94 JLC First implementation.  Buggy, but works.    */
/*                                                                  */
/********************************************************************/

#ifndef _ORDCOLL_
   #include "OrdColl.h"
#endif

//#ifndef _ODNEW_
//#include "ODNew.h"
//#endif


class ODxOCValueLink /*: public Link*/ {
   friend ODxOrderedCollection;  

   public:
                ODxOCValueLink(ElementType value);    
     virtual  ~ODxOCValueLink();
               ElementType  GetValue()            { return fValue;}
               void             SetValue(ElementType v)      { fValue = v;}
   
   private:
     ElementType     fValue;
     ODxOCValueLink *     fNext;
};


//======================================================================================
// Class ODxOCValueLink - Implementation
//======================================================================================

ODxOCValueLink::ODxOCValueLink(ElementType value)
{
  fValue = value;
  fNext  = 0;
}

ODxOCValueLink::~ODxOCValueLink()
{
}

//======================================================================================
// Class ODxOrderedCollection
//======================================================================================

//------------------------------------------------------------------------------
// ODxOrderedCollection::ODxOrderedCollection
//------------------------------------------------------------------------------

ODxOrderedCollection::ODxOrderedCollection()
{
  // fHeap = kODNULL;
  fImplementation = 0;
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::ODxOrderedCollection
//------------------------------------------------------------------------------

//ODxOrderedCollection::ODxOrderedCollection(ODMemoryHeapID where)
//{
//  //fHeap = where;
//}

// ODxOrderedCollection::~ODxOrderedCollection
//------------------------------------------------------------------------------

ODxOrderedCollection::~ODxOrderedCollection()
{
  this->RemoveAll();
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::AddFirst
//------------------------------------------------------------------------------

void ODxOrderedCollection::AddFirst(ElementType element)
{
  ODxOCValueLink* newLink = this->CreateNewLink(element);
  newLink->fNext = fImplementation;
  fImplementation = newLink;
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::AddLast
//------------------------------------------------------------------------------

void ODxOrderedCollection::AddLast(ElementType element)
{
  ODxOCValueLink* newLink = CreateNewLink(element);
  ODxOCValueLink* * nlr = &fImplementation;
  while (*nlr) nlr = &((*nlr)->fNext);
  *nlr = newLink;
}


//------------------------------------------------------------------------------
// ODxOrderedCollection::AddBefore
//------------------------------------------------------------------------------

void ODxOrderedCollection::AddBefore(ElementType existing, ElementType tobeadded)
{
  ODxOCValueLink* * aLink = &fImplementation;
  while (*aLink) {
     ElementType v = (*aLink)->GetValue();
 
     if (this->ElementsMatch(v,existing)) {
       ODxOCValueLink* newLink = CreateNewLink(tobeadded);
       newLink->fNext = *aLink;
       *aLink = newLink;
       return;
     } else
       aLink = &((*aLink)->fNext);
  } /* endwhile */
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::AddAfter
//------------------------------------------------------------------------------

void ODxOrderedCollection::AddAfter(ElementType existing, ElementType tobeadded)
{
  ODxOCValueLink* aLink = fImplementation;
  while (aLink) {
    ElementType v = aLink->GetValue();

    if (this->ElementsMatch(v,existing)) {
      ODxOCValueLink* newLink = CreateNewLink(tobeadded);
      newLink->fNext = aLink->fNext;
      aLink->fNext = newLink;
      return;
    }
    aLink = aLink->fNext;
  }
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::After
//------------------------------------------------------------------------------

ElementType ODxOrderedCollection::After(ElementType existing)
{
  for (ODxOCValueLink* link = fImplementation; link; link = link->fNext) {
    ElementType v = link->GetValue();

    if (this->ElementsMatch(v,existing)) {
       return (link->fNext) ? link->fNext->GetValue() : kODNULL;
    } /* endif */
  }

  return (ElementType)kODNULL;
}

ODxOCValueLink * ODxOrderedCollection::After(ODxOCValueLink *existing)
{
  for (ODxOCValueLink* link = fImplementation; link; link = link->fNext) {

    if (link == existing) return link->fNext;
  }
  return kODNULL;
}

#if 1
   //------------------------------------------------------------------------------
   // ODxOrderedCollection::Before
   //------------------------------------------------------------------------------
   
   ElementType ODxOrderedCollection::Before(ElementType existing)
   {
     ODxOCValueLink* linkBefore = kODNULL;
   
     for (ODxOCValueLink* link = fImplementation; link; linkBefore = link, link = link->fNext) {
       ElementType v = link->GetValue();
   
       if (this->ElementsMatch(v,existing)) {
          if (linkBefore) {
             return linkBefore->GetValue();
          } else {
             return kODNULL;  // there is none before
          } /* endif */
       }
     }
   
     return kODNULL;  // element not found
   }
   ODxOCValueLink * ODxOrderedCollection::Before(ODxOCValueLink *existing)
   {
     ODxOCValueLink* linkBefore = kODNULL;
   
     for (ODxOCValueLink* link = fImplementation; link; linkBefore = link, link = link->fNext) {
       if (link == existing) return linkBefore;
     }
   
     return kODNULL;  // element not found
   }
#endif

//------------------------------------------------------------------------------
// ODxOrderedCollection::First
//------------------------------------------------------------------------------

ElementType ODxOrderedCollection::First()
{
  ODxOCValueLink* firstLink = fImplementation;
  return firstLink ? firstLink->GetValue() : (ElementType)kODNULL;
}

ODxOCValueLink * ODxOrderedCollection::FirstVL()
{
   return fImplementation;
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::Last
//------------------------------------------------------------------------------

ElementType ODxOrderedCollection::Last()
{
  ODxOCValueLink *lastLink = LastVL();
  return (lastLink ? lastLink->GetValue() : (ElementType) kODNULL);
}

ODxOCValueLink * ODxOrderedCollection::LastVL()
{
  ODxOCValueLink *lastLink = fImplementation;
  if (lastLink) {
     while (lastLink->fNext) lastLink = lastLink->fNext;
     return lastLink;
  } else {
     return kODNULL;
  } /* endif */
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::RemoveFirst
//------------------------------------------------------------------------------

ElementType  ODxOrderedCollection::RemoveFirst()
{
  ODxOCValueLink* aLink = fImplementation;
  ElementType value = aLink->GetValue();
  fImplementation = aLink->fNext;
  delete aLink;
  return value;
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::RemoveLast
//------------------------------------------------------------------------------

ElementType  ODxOrderedCollection::RemoveLast()
{
  if (fImplementation) {
     ODxOCValueLink* *aLink = &fImplementation;
     while ((*aLink)->fNext) aLink = &((*aLink)->fNext);
     ElementType value = (*aLink)->GetValue();
     delete (*aLink);
     *aLink = 0;
     return value;
  } else {
     return (ElementType)kODNULL;
  } /* endif */
  
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::Remove
//------------------------------------------------------------------------------

void ODxOrderedCollection::Remove(ElementType existing)
{
  ODxOCValueLink* *aLink = &fImplementation;
  while (*aLink) {
    ElementType v = (*aLink)->GetValue();

    if (this->ElementsMatch(v,existing)) {
      ODxOCValueLink *vl = *aLink;
      *aLink = vl->fNext;
      delete vl;
      return;
    }
    aLink = &((*aLink)->fNext);
  }
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::RemoveAll
//------------------------------------------------------------------------------

void ODxOrderedCollection::RemoveAll()
{
  while (fImplementation) {
     ODxOCValueLink* link = fImplementation->fNext;
     delete fImplementation;
     fImplementation = link;
  }
}

#if 0
   //------------------------------------------------------------------------------
   // ODxOrderedCollection::DeleteAll
   //------------------------------------------------------------------------------
   
   void ODxOrderedCollection::DeleteAll()
   {
     Link* link = fImplementation.RemoveFirst();
     while (link != kODNULL)
     {
       ElementType value = ((ODxOCValueLink*) link)->GetValue();
       delete value;
       delete link;
       link = fImplementation.RemoveFirst();
     }
   }
#endif

//------------------------------------------------------------------------------
// ODxOrderedCollection::Contains
//------------------------------------------------------------------------------

ODBoolean  ODxOrderedCollection::Contains(ElementType existing)
{
  ODxOCValueLink* aLink = fImplementation;
  while (aLink) {
    ElementType v = aLink->GetValue();

    if (this->ElementsMatch(v,existing)) return kODTrue;
    aLink = aLink->fNext;
  }
  return kODFalse;
}

long ODxOrderedCollection::Count() const
{
  ODxOCValueLink* aLink = fImplementation;
  int ct = 0;
  while (aLink) {
    ct++;
    aLink = aLink->fNext;
  }
  return ct;
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::CreateIterator
//------------------------------------------------------------------------------

ODxOrderedCollectionIterator* ODxOrderedCollection::CreateIterator()
{
  return new ODxOrderedCollectionIterator(this);
}


//------------------------------------------------------------------------------
// ODxOrderedCollection::CreateNewLink
//------------------------------------------------------------------------------

ODxOCValueLink*  ODxOrderedCollection::CreateNewLink(ElementType value) const
{
  return new ODxOCValueLink(value);
}

//------------------------------------------------------------------------------
// ODxOrderedCollection::ElementsMatch
//------------------------------------------------------------------------------

ODBoolean  ODxOrderedCollection::ElementsMatch(ElementType v1,ElementType v2) const
{
  return (v1 == v2);
}

//======================================================================================
// ODxOrderedCollectionIterator
//======================================================================================

//------------------------------------------------------------------------------
// ODxOrderedCollectionIterator::ODxOrderedCollectionIterator
//------------------------------------------------------------------------------

ODxOrderedCollectionIterator::ODxOrderedCollectionIterator(ODxOrderedCollection* collection)
  : fImplementation(collection ? (collection->fImplementation) : kODNULL)
{
  fCollection =  collection;
}

//------------------------------------------------------------------------------
// ODxOrderedCollectionIterator::~ODxOrderedCollectionIterator
//------------------------------------------------------------------------------

ODxOrderedCollectionIterator::~ODxOrderedCollectionIterator()
{
}

//------------------------------------------------------------------------------
// ODxOrderedCollectionIterator::First
//------------------------------------------------------------------------------

ElementType  ODxOrderedCollectionIterator::First()
{
  fImplementation = fCollection->FirstVL();

  return fImplementation ? fImplementation->GetValue() : (ElementType)kODNULL;
}

//------------------------------------------------------------------------------
// ODxOrderedCollectionIterator::Next
//------------------------------------------------------------------------------

ElementType  ODxOrderedCollectionIterator::Next()
{
  ODxOCValueLink* link = fCollection->After(fImplementation);
  fImplementation = link;
  return link ? link->GetValue() : (ElementType)kODNULL;
}

//------------------------------------------------------------------------------
// ODxOrderedCollectionIterator::Last
//------------------------------------------------------------------------------

ElementType  ODxOrderedCollectionIterator::Last()
{
  ODxOCValueLink* link = fCollection->LastVL();
  fImplementation = link;
  return link ? link->GetValue() : (ElementType)kODNULL;
}

//------------------------------------------------------------------------------
// ODxOrderedCollectionIterator::Previous
//------------------------------------------------------------------------------

//ElementType  ODxOrderedCollectionIterator::Previous()
//{
//  ODxOCValueLink* link = (ODxOCValueLink*) fImplementation.Previous();
//
//  return link ? link->GetValue() : (ElementType)kODNULL;
//}

//------------------------------------------------------------------------------
// ODxOrderedCollectionIterator::IsNotComplete
//------------------------------------------------------------------------------

ODBoolean  ODxOrderedCollectionIterator::IsNotComplete()
{
   #if 1
      return (fImplementation && fCollection);
   #else
      if (fImplementation && fCollection) {
         ODxOCValueLink* nextlink = fCollection->After(fImplementation);
         if (nextlink) {
            return kODTrue;
         } else {
            return kODFalse;
         } /* endif */
      } else {
         return kODFalse;
      } /* endif */
   #endif
}
