#ifndef FRAMESTACK_H_
#define FRAMESTACK_H_
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "ial.h"
#include "error.h"
#include "variable.h"

typedef struct {
	//jestli je ramec pruchozi
	bool passable;
	//ukazatel na prvni prvek stromu
	tBSTNodePtr frame;
} tFrameContainer;

//prvek dvousmerne vazaneho seznamu
typedef struct tSElem {
	//uzitecna data
	tFrameContainer frameContainer;
	//ukazatel na predchozi prvek
	struct tSElem *lptr;
	//ukazatel na nasledujici prvek
	struct tSElem *rptr;
} *tSElemPtr;

typedef struct {
	//ukazatel na prvni prvek
	tSElemPtr Top;
	//ukazatel na posledni prvek
	tSElemPtr Last;
} tStack;

void SInit(tStack *);
void SDispose(tStack *);
void Push(tStack *, tFrameContainer*);
/*
 * Vrati prvek na vrcholu zasobniku
 * @return 0 pri chybe 1 pri success
 */
int STop(tStack *, tFrameContainer*);
void SPop(tStack *);

void pushNewFrame(tStack*, bool);
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

/*
 * Vlozi promennou do ramce.
 * Musi se uvolnit string?
 */
void insertNewVariable(tFrameContainer*, tVariablePtr, string*);

/*
 * Hleda varaible podle stringu skrz passable framy
 * @param stack[in] frameStack
 * @param s[in] jmeno hledane promenne
 * @param var[out] zde vrati ukazatel na promennou pokud ji najde jinak NULL
 */
int findVariable(const tStack* , string *, tVariablePtr*); 

#endif //FRAMESTACK_H_