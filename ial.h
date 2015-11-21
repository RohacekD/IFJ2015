/*
 * ial.h
 *  Project: interpreter
 *  Created on: 18. 10. 2015
 *  Author: xdocek09
 *  Description: Hlavickovy soubor (pro ial.c) vestavenych funkci pro IAL.
 */
/**
 * @file ial.h
 * @author xdocek09
 * @brief Hlavickovy soubor (pro ial.c) vestavenych funkci pro IAL.
 */

#ifndef IAL_H_
#define IAL_H_

#include "str.h"
#include <stdbool.h>

typedef struct tBSTNode *tBSTNodePtr;

typedef enum
{
	TAB_SYM_VARIABLE,
	TAB_SYM_FUNCTION
}tTabSymDataType;

typedef enum
{
	TAB_SYM_VAR_INTEGER,
	TAB_SYM_VAR_DOUBLE,
	TAB_SYM_VAR_STRING,
	TAB_SYM_VAR_BOOLEAN,
	TAB_SYM_VAR_AUTO
}tTabSymVarDataType;

typedef enum
{
	TAB_SYM_RET_INTEGER,
	TAB_SYM_RET_DOUBLE,
	TAB_SYM_RET_STRING,
	TAB_SYM_RET_BOOLEAN
}tTabSymRetDataType;

typedef struct{
	tTabSymVarDataType dataType;
}tVariableInfo;


typedef struct tParamListElem {
    struct tParamListElem *next;
    string idName;
    tTabSymVarDataType dataType;
} *tParamListElemPtr;

typedef struct{
	tParamListElemPtr first;
}*tParamListPtr;


typedef struct{
	tParamListPtr params;		// parametry funkce
	tTabSymRetDataType retType;	// navratovy typ funkce
	tBSTNodePtr	locTab;			// lokalni tabulka symbolu
	bool defined;				// byla definovana

}tFuncInfo;


typedef struct{
	tTabSymDataType type;	//druh identifikatoru
	union{
		tVariableInfo *var;
		tFuncInfo * func;
	}info;
}tData;

typedef struct tBSTNode{
	string* key;
	tData * data;
	struct tBSTNode *l;
	struct tBSTNode *r;

} *tBSTNodePtr;


int len(char* string);
/**
 * Vypocita u retezce, hodnoty skoku pro dané znaky.
 * @param string	-	retezec
 * @param charJump	-	zde se ulozi hodnoty skoku (indexujeme pomoci znaku)
 */
void computeJumps(char* string, int charJump[]);


/**
 * Stanovi index pocatecni polohy vzorku v hledanem textu.
 * @param string		-	text pro vyhledavani
 * @param searchString	-	retezec, ktery se vyhledava
 * @param charJump		-	predvypocitane hodnoty skoku pro searchString pomoci computeJumps
 * @return Vraci index pocatecni polohy vzorku v danem textu. Pri neuspechu
 * 	vraci maximalni index stringu +1.
 */
int BMA(char* string, char* searchString, int charJump[]);

/**
 * Razeni pomoci heap sort
 * @param string	-	razeny retezec
 */
void heapSort(char* string);

/**
 * Vyhledava v binarnim vyhledavacim stromu
 * @param rootPtr	-	ukazatel na koren stromu
 * @param key		-	vyhledavany klic
 * @return	0-nenalezeno, 1-nalezeno
 */
int BSTSearch(tBSTNodePtr rootPtr, string* key);

/**
 * Vyhledava v binarnim vyhledavacim stromu. Vraci ukazatel na nalezeny uzel jinak NULL.
 * @param rootPtr	-	ukazatel na koren stromu
 * @param key		-	vyhledavany klic
 * @param store		-	 Vraci zde ukazatel na nalezeny uzel nebo NULL.
 */
void BSTSearchTree(tBSTNodePtr rootPtr, string* key, tBSTNodePtr* store);

/**
 * Vklada do binarniho vyhledavaciho stromu novy uzel nebo
 * aktualizuje stary v pripade, ze klic jiz ve stromu existuje.
 * @param rootPtr	-	ukazatel na koren stromu
 * @param key		-	klic vkladaneho/aktualizovaneho uzlu
 * @param data		-	data vkladaneho/aktualizovaneho uzlu
 */
void BSTInsert(tBSTNodePtr* rootPtr, string* key, tData* data);

/**
 * Maze z binarniho vyhledavaciho stromu uzel s danym klicem key.
 * @param rootPtr	-	ukazatel na koren stromu
 * @param key		-	klic uzlu, ktery ma byt odstranen
 */
void BSTDelete(tBSTNodePtr* rootPtr, string* key);

#endif /* IAL_H_ */
/*** End of file: ial.h ***/
