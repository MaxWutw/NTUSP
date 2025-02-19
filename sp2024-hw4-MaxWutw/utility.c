#include "utility.h"

int8_t initList(sList *pList){
	/* Return value
	 * -1: error
	 *  0: success
	 */
	if(pList == NULL){
		printf("%s(%d) %s: NULL pointer!\n", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}

	pList->pParam = malloc(1 * sizeof(sListParam));
	pList->pParam->size = 0;
	pList->pParam->myfree = free;
	pList->pParam->myprint = NULL;

	// create dummy head and tail for cursor
	sNode *dummyHead = malloc(1 * sizeof(sNode));
	dummyHead->data = NULL;
	dummyHead->prev = NULL;
	dummyHead->next = NULL;

	sNode *dummyTail = malloc(1 * sizeof(sNode));
	dummyTail->data = NULL;
	dummyTail->prev = NULL;
	dummyTail->next = NULL;
	
	pList->pHead = dummyHead;
	pList->pTail = dummyTail;
	pList->cursor = pList->pHead;

	return 0;
}

void myfree(void *ptr){
	/* Return value
	 * -1: error
	 *  0: success
	 */
	sList *pNode = (sList *)ptr;
	if(pNode){
		free(pNode);
		return;
	}
	return;
}

void myprint(const void *ptr){
	/* Return value
	 * -1: error
	 *  0: success
	 */
	sNode *pNode = (sNode *)ptr;
	if(pNode == NULL) return;
	printf("%c", *(char *)pNode->data);
	return;
}

void regFreeCallback(sList *pList, void (*myfree)(void *)){
	if(pList == NULL){
		printf("%s(%d) %s: NULL pointer!\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	pList->pParam->myfree = myfree;
	return;
}

void regPrintCallback(sList *pList, void (*myprint)(const void *)){
	if(pList == NULL){
		printf("%s(%d) %s: NULL pointer!\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	pList->pParam->myprint = myprint;
	return;
}

int8_t freeList(sList *pList){
	if(pList == NULL){
		printf("%s(%d) %s: NULL pointer!\n", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	
	sNode *pNode = pList->pHead, *pNodeTmp;
	while(pNode){
		pNodeTmp = pNode;
		pNode = pNode->next;
		pList->pParam->myfree(pNodeTmp);
	}
	free(pList->pParam);
	free(pList);
	return 0;
}

int8_t printList(sList *pList){
	if(pList == NULL){
		printf("%s(%d) %s: NULL pointer!\n", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}

	if(pList->pParam->myprint == NULL){
		printf("%s(%d) %s: Not registered myprint yet!\n", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	printf("Text: ");
	if(pList->pParam->size == 0) return 0;
	sNode *pNode = pList->pHead->next;
	if(pList->cursor == pList->pHead) printf("|");
	while(pNode != pList->pTail){
		// printf("%c", *(char *)pNode->data);
		pList->pParam->myprint(pNode);
		if(pNode == pList->cursor) printf("|");
		pNode = pNode->next;
	}

	return 0;
}

