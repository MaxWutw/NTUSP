#ifndef _UTILITY_H_
#define _UTILITY_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define reset "\e[0m"
#define CRESET "\e[0m"
#define COLOR_RESET "\e[0m"
typedef struct _sList{
	struct _sListParam *pParam;
	struct _sNode *pHead;
	struct _sNode *pTail;
	struct _sNode *cursor;
}sList;

typedef struct _sNode{
	void *data;
	struct _sNode *prev;
	struct _sNode *next;
}sNode;

typedef struct _sListParam{
	int32_t size;
	void (*myfree)(void *); // free the whole list
	void (*myprint)(const void *); // print the whole list
}sListParam;

int8_t initList(sList *);
void myfree(void *);
void myprint(const void *);
int8_t freeList(sList *);
int8_t printList(sList *);
void regFreeCallback(sList *, void (*myfree)(void *));
void regPrintCallback(sList *, void (*myprint)(const void *));

#endif
