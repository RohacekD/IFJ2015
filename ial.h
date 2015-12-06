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
#include <stdlib.h>
#include <stdio.h>
#include "str.h"

typedef struct tBSTNode {
    string* key;
    void* data;
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

int find(char* string, char* searchString);

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
 * @return	0-chyba, 1-v poradku
 */
int BSTInsert(tBSTNodePtr* rootPtr, string* key, void* data);

/**
 * Maze z binarniho vyhledavaciho stromu uzel s danym klicem key.
 * @param rootPtr	-	ukazatel na koren stromu
 * @param key		-	klic uzlu, ktery ma byt odstranen
 */
void BSTDelete(tBSTNodePtr* rootPtr, string* key);

/**
 * Smaze cely strom. Pro mazani dat pouziva funkci dataFree danou ukazatelem.
 * @param rootPtr	-	ukazatel na koren stromu
 * @param dataFree	-	ukazatel na funkci, ktera uvolni polozku data z pameti
 */
void BSTFree(tBSTNodePtr* rootPtr, void (*dataFree)(void*));
/**
 * Vytvari uzel.
 * @param node	-	ukazatel pro alokaci uzlu
 * @param key	-	klic noveho uzlo
 * @param data	-	data noveho uzlo
 * @return	0-chyba, 1-v poradku
 */
int BSTCreateNode(tBSTNodePtr*, string*, void*);

/**
  * funkce prochazi stromem a nastavi priznak, pokud narazi na nedefinovanou funkci
  * @param rootPtr          -   koren stromu
  */
void BSTcheckFuncDef(tBSTNodePtr *rootPtr);
#endif /* IAL_H_ */
/*** End of file: ial.h ***/
