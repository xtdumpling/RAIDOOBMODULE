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

/** @file Tree.c
    USB tree manipulation routines

**/


#include <Efi.h>
#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"

#include "Tree.h"

/**
    initializes TREENODE_T structure

    @param 
        n - pointer to TREENODE_T structure
        d - pointer to data structure specific to
        the type of tree node

    @retval Pointer to TREENODE_T structure that was passed in as parrameter

**/

TREENODE_T* TreeCreate( TREENODE_T* n, VOID* d )
{
    n->data = d;
    n->child = 0;
    n->right = 0;
    n->left = 0;
    n->parent = 0;
    return n;
}


/**
    add a child node to the TREENODE_T structure

    @param 
        p - pointer to parent TREENODE_T structure
        c - pointer to child TREENODE_T structure

**/

VOID TreeAddChild( TREENODE_T* p, TREENODE_T* c )
{
    TREENODE_T* n = p->child;
    //for( n = p->child; n != NULL; n = n->right );
    p->child = c;
    c->right = n;
    if(n!=0)
        n->left = c;
    c->left = 0;
    c->parent = p;
}


/**
    removes a node from the tree

    @param 
        n - pointer to TREENODE_T structure
**/

VOID TreeRemove( TREENODE_T* n )
{
    if(  n->right != 0){
        n->right->left = n->left;
    }
    if(  n->left != 0){
        n->left->right = n->right;
    }
    if( n->parent && n->parent->child == n )
        n->parent->child = n->right;
    n->left = 0;
    n->right = 0;
    n->parent = 0;
}


/**
    Enumerates nodes of the tree which are direct children of
    the same parent

    @param 
        n   - pointer to TREENODE_T structure
        pr  - predicate function that is called for each node
        and controll whether enumeration should continue
        once predicate returns TRUE the enumeration will
        data - pointer that is passed to predicate to maintain
        the context of the enumeration

    @retval the node that cause enumeration to stop
**/

TREENODE_T* TreeSearchSibling( TREENODE_T* n, TREE_PREDICATE1_T pr, VOID* data )
{
    TREENODE_T *r;
    for(;n;n=r){
        r = n->right;
        if(pr(n->data,data))return n;
    }
    return NULL;
}


/**
    Enumerates nodes of the tree which are direct and indirect
    children of the same parent

    @param 
        n   - pointer to TREENODE_T structure
        pr  - predicate function that is called for each node;
        controlls whether enumeration should continue
        once predicate returns TRUE the enumeration will
        data - pointer that is passed to predicate to maintain
        the context of the enumeration

    @retval the node that cause enumeration to stop
**/

TREENODE_T* TreeSearchDeep( TREENODE_T* n, TREE_PREDICATE1_T pr, VOID* data )
{
    TREENODE_T *r;
    TREENODE_T *c;
    for(;n;n=r){
        r = n->right;
        c = n->child;
        if(pr(n->data,data))return n;
        if(c){
            TREENODE_T* c1 = TreeSearchDeep(n->child,pr,data);
            if(c1)
                return c1;
        }
    }
    return 0;
}


/**
    Enumerates nodes of the tree which are direct children of
    the same parent; In contrust to TreeSearchSibling this
    function ignores the result returned from call-back routine
    and always enumerates all sibling nodes

    @param 
        n - pointer to TREENODE_T structure
        pr  - call-back function that is called for each node
        data - pointer that is passed to call-back to maintain
        the context of the enumeration
**/

VOID TreeForEachSibling( TREENODE_T* n, TREE_CALLBACK_T pr, VOID* data )
{
    for(;n;n=n->right)
        pr(n->data,data);
}


/**
    retrieves data stored at the tail of the queue and
    removes the tail item

    @param 
        q - pointer to QUEUE_T structure
**/

VOID* QueueGet( QUEUE_T* q)
{
    VOID* d;
    if( q->tail == q->head ) return NULL;
    d = q->data[q->tail++];
    if( q->tail == q->maxsize ) q->tail -= q->maxsize;
    return d;
}


/**
    retrieves number of items stored in the queue

    @param 
        q - pointer to QUEUE_T structure
**/

int QueueSize(QUEUE_T* q)
{
    return (q->head >= q->tail)? q->head - q->tail:
        q->head  + q->maxsize - q->tail;
}


/**
    add a new item in front of the head of the queue

    @param 
        q - pointer to QUEUE_T structure
**/

VOID QueuePut( QUEUE_T* q, VOID * d)
{
    ASSERT(QueueSize(q) < q->maxsize );
    q->data[q->head++] = d;
    if(q->head==q->maxsize) q->head -= q->maxsize;
    if(q->head==q->tail){
        //Drop data from queue
        q->tail++;
        if( q->tail == q->maxsize ) q->tail -= q->maxsize;
    }
}


/**
    add a variable size item to the queue

    @param 
        q - pointer to QUEUE_T structure
        sz - number of dwords to add to the queue
**/

VOID QueuePutMsg( QUEUE_T* q, VOID * d, int sz )
{
    ASSERT(QueueSize(q) < q->maxsize );
    ASSERT(sz<q->maxsize);
    if(q->head + sz > q->maxsize )
        q->head = 0;
    EfiCopyMem( (char*)q->data + q->head, d, sz );
    q->head += sz;
    if(q->head==q->maxsize) q->head = 0;
    if(q->head==q->tail){
        //Drop data from queue
        q->tail+=sz;
        if( q->tail >= q->maxsize ) q->tail = 0;
    }
}


/**
    retrieves a variable size item from the queue

    @param 
        q - pointer to QUEUE_T structure
        sz - number of dwords to remove from the queue
**/

VOID* QueueRemoveMsg( QUEUE_T* q, int sz)
{
    VOID* d;
    if( q->tail == q->head ) return NULL;
    d = (char*)q->data + q->tail;
    q->tail += sz;
    if( q->tail > q->maxsize ){
        d = q->data;
        q->tail = sz;
    } else if(q->tail == q->maxsize ){
        q->tail = 0;
    }
    return d;
}

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
