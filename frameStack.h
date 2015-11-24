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
/*
 * Vrati prvek na vrcholu zasobniku
 */
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

/*
 * Vlozi promennou do ramce.
 * Musi se uvolnit string?
 */
void insertNewVariable(tFrame*, tVariable*, string*);

/*
 * Hleda varaible podle stringu skrz passable framy
 */
int findVariable(const tFrame*, string s);