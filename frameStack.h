/*
 * frameStack.h
 *  Project: IFJ2015
 *  Created on: 24. 11. 2015
 *  Author: xbartu03, xdocek09, xjelin42
 *  Description: Hlavickovy soubor (pro frameStack.c).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ial.h"
#include "error.h"

 #define FALSE 0
 #define TRUE 1

 typedef struct {
 	//jestli je ramec pruchozi
 	bool passable;
 	//ukazatel na prvni prvek stromu
 	tBSTNodePtr frame;
 } tFrame;

 //prvek dvousmerne vazaneho seznamu
 typedef struct tSElem { 
 	//uzitecna data
 	tFrame frame;
 	//ukazatel na predchozi prvek
 	struct tSElem *lptr;
 	//ukazatel na nasledujici prvek
 	struct tSElem *rptr;
 } *tSElemPtr;

 typedef struct {
 	//ukazatel na prvni prvek
 	tSElemPtr First;
 	//ukazatel na posledni prvek
 	tSElemPtr Last;
 } tStack;

 void SInit(tStack *);
 void SDispose(tStack *);
 void Push(tStack *, tFrame*);
 void First(tStack *);
 void Top(tStack *, tFrame*);
 void Pop(tStack *);

 int pushNewFrame(tStack*, bool);
/*
 * Volana pri navratu z fce. Maze vsechny frame i s framem funkce.
 * @param list[in]	- Ukazatel na frameStack
 */
void deleteFunctionsFrames(tStack*);
/*
 * Mazana pri ukonceni bloku kodu. Maze jeden vrchni prvek
 * @param list[in]	- Ukazatel na frameStack
 */
void deleteTopFrame(tStack*);

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
