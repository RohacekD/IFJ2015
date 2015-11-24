#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "ial.h"


#define FALSE 0
#define TRUE 1

typedef struct{
    bool passable; // zda je ramec pruchozi
    tBSTNodePtr frame; //ukazatel na první prvek stromu ramce
} tFrame;
 
typedef struct tDLElem {                 /* prvek dvousměrně vázaného seznamu */ 
        tFrame frame;            /* užitečná data */
        struct tDLElem *lptr;          /* ukazatel na předchozí prvek seznamu */
        struct tDLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

                                             /* prototypy jednotlivých funkcí */
void DLInitList (tDLList *);
void DLDisposeList (tDLList *);
void DLPush (tDLList *, int);
void DLFirst (tDLList *);
void DLTop (tDLList *, int *);
void DLPop (tDLList *);
void DLCopy (tDLList *, int *);
void DLActualize (tDLList *, int);
void DLSucc (tDLList *);
void DLPred (tDLList *);
int DLActive (tDLList *);

