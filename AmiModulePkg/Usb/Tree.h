//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file Tree.h
    Tree routines header

**/

#ifndef __TREE_H__
#define __TREE_H__

typedef struct _TREENODE_T TREENODE_T;
typedef struct _TREENODE_T{
    VOID* data;
    TREENODE_T* right;
    TREENODE_T* left;
    TREENODE_T* child;
    TREENODE_T* parent;
};

typedef struct QUEUE_T;

typedef int (*TREE_PREDICATE1_T)(VOID* n, VOID* context);
typedef VOID (*TREE_CALLBACK_T)(VOID* n, VOID* context );

TREENODE_T* TreeCreate( TREENODE_T* n, VOID* d);
VOID TreeAddChild( TREENODE_T* p, TREENODE_T* c );
VOID TreeRemove( TREENODE_T* n );
TREENODE_T* TreeSearchSibling(TREENODE_T* n, TREE_PREDICATE1_T pr, VOID* data );
TREENODE_T* TreeSearchDeep(TREENODE_T* n, TREE_PREDICATE1_T pr, VOID* data );
VOID TreeForEachSibling(TREENODE_T* n, TREE_CALLBACK_T pr, VOID* data );

VOID* QueueGet( QUEUE_T* q);
int QueueSize(QUEUE_T* q);
VOID QueuePut( QUEUE_T* q, VOID * d);
VOID* QueueRemoveMsg( QUEUE_T* q, int sz);
VOID QueuePutMsg( QUEUE_T* q, VOID * d, int sz );

#endif //__TREE_H__

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
