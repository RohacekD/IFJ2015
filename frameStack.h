#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "ial.h"
#include "error.h"


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
void DLPush (tDLList *, tFrame*);
void DLFirst (tDLList *);
void DLTop (tDLList *, tFrame*);
void DLPop (tDLList *);
void DLCopy (tDLList *, tFrame *);
//void DLActualize (tDLList *, tFrame);
void DLSucc (tDLList *);
void DLPred (tDLList *);
int DLActive (tDLList *);

int pushNewFrame(tDLList*, bool);
/*
 * Volana pri navratu z fce. Maze vsechny frame i s framem funkce.
 * @param list[in]	- Ukazatel na frameStack
 */
void deleteFunctionsFrames(tDLList*);
/*
 * Mazana pri ukonceni bloku kodu. Maze jeden vrchni prvek
 * @param list[in]	- Ukazatel na frameStack
 */
void deleteTopFrame(tDLList*);

typedef enum {
	VAR_TYPE_INT,
	VAR_TYPE_BOOL,
	VAR_TYPE_DOUBLE,
	VAR_TYPE_STRING
} tVariableType;

typedef union
{
	//nejak implementovany pointer do tabulky symbolu
	int intVal;
	double doubleVal;
	string stringVal;
	bool boolVal;
}tVariableData;

typedef struct {
	tVariableType type;
	tVariableData data;
} tVariable;

/*
 * Utvori strukturu promenne, inicializuje string.
 * @param tVariable* struktura promenne
 * @param tVariableType urcuje typ
 * @void* (int*|bool*|double*|string*)
 */
void variableCreate(tVariable*, tVariableType, void*);
/*
 * Spravne smaze promennou. Preda se ukazatelem pri mazani stromu
 * @param tVariable* struktura promenne
 */
void variableDelete(tVariable*);

