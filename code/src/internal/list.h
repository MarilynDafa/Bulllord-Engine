/*
 Bulllord Game Engine
 Copyright (C) 2010-2017 Trix
 
 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.
 
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.
 */
#ifndef __list_h_
#define __list_h_
#include "thread.h"
#ifdef __cplusplus
extern "C" {
#endif
	
typedef struct _List {
	struct _ListNode{
		BLVoid** pData;
		struct _ListNode* pNext;
		struct _ListNode* pPrev;
	} *pFirst, *pLast;
	BLU32 nSize;
	blMutex* pMutex;
}BLList;

BLList* blGenList(
	IN BLBool _Withthread);

BLVoid blDeleteList(
	INOUT BLList* _Lst);

BLVoid blClearList(
	INOUT BLList* _Lst);

BLVoid* blListFrontElement(
	IN BLList* _Lst);

BLVoid* blListBackElement(
	IN BLList* _Lst);

BLVoid blListPushFront(
	INOUT BLList* _Lst, 
	IN BLVoid* _Ele);

BLVoid blListPushBack(
	INOUT BLList* _Lst, 
	IN BLVoid* _Ele);

BLVoid blListPopFront(
	INOUT BLList* _Lst);

BLVoid blListPopBack(
	INOUT BLList* _Lst);

BLVoid blListInsert(
	INOUT BLList* _Lst, 
	IN BLU32 _Idx, 
	IN BLVoid* _Ele);

BLVoid blListErase(
	INOUT BLList* _Lst, 
	IN BLVoid* _Node);
    
#ifdef __cplusplus
#   define FOREACH_LIST(type , var , container) type var = (container->pFirst)?(*(type*)(container->pFirst->pData)):NULL;\
    for (struct BLList::_ListNode* _iterator##var = container->pFirst; _iterator##var; \
         _iterator##var = _iterator##var->pNext, _iterator##var ? var = (*(type*)((_iterator##var)->pData)) : var)
#else
#   define FOREACH_LIST(type , var , container) type var = (container->pFirst)?(*(type*)(container->pFirst->pData)):NULL;\
		struct _ListNode* _iterator##var = container->pFirst;\
        for (; _iterator##var; _iterator##var = _iterator##var->pNext, _iterator##var ? var = (*(type*)((_iterator##var)->pData)) : var)
#endif
            
#ifdef __cplusplus
}
#endif

#endif/* __list_h_ */
