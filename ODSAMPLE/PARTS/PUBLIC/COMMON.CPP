/*
 *      File:           Common.cpp
 *
 *   Contains: In-line helper functions, macros and classes for
 *             sample parts.
 *
 *   Written by:        Jason Crawford
 *
 *   Copyright: (c) 1994 by IBM Corp., all rights reserved.
 *
 *   Change History (most recent first):
 *
 *    124384      6/05/95       map  first checked in (moved function from hpp file)
 *
 *
 *   Copyright: (c) 1993-1994 by IBM Corp., all rights reserved.
 *
 */

ODTransform*
ODCopyAndRelease( Environment *ev, ODTransform *t )
{
  if (t) {
    if( t->GetRefCount(ev) == 1 )
      return t;
    ODTransform *copy = t->Copy(ev);
    t->Release(ev);
    return copy;
  }
}
#endif
