# intrusive

C++ doubly linked list where being a member of the list is a
property of the object being part of it.

## Advantages to std::list:
 * No extra node allocation (good for realtime applications)
 * Objects can be moved/erased without iterator to list
 * Automatic cleanup upon destruction of members in list

## Special features:
 * Objects can be part of several different intrusive lists
   simultaneously. It will depend on the pointer-to-member
   property T::*N passed to intrusive::list.

## Missing/TODO:
 * Thread safety
