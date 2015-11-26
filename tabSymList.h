/*
 * tabSymList.h
 *  Project: IFJ2015
 *  Created on: 26. 11. 2015
 *  Author: xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 *  Description: Hlavickovy soubor pro tabSymList.c
 */
/**
 * @file tabSymList.h
 * @author xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 * @brief Hlavickovy soubor pro tabSymList.c
 */
#ifndef TABSYMLIST_H_
#define TABSYMLIST_H_

#include "tabSym.h"

typedef struct tTabSymList;

typedef struct tTabSymListElem{
	tTabSym* table;
	struct tTabSymListElemPtr* next;
	tTabSymList* childList;
	struct tTabSymListElem* parentElement;
}*tTabSymListElemPtr;

typedef struct{
	tTabSymListElemPtr first;
}tTabSymList;

/**
 * Alokuje pamet pro tTabSymList.
 * @return Ukazatel na nove alokovanou strukturu. Pokud chyba NULL.
 */
tTabSymList* tabSymListCreate();

/**
 * Vlozi do listu tabulek novou tabulku.
 * @param tabList[in]		-	List, do ktereho bude vlozena tabulka.
 * @param table[in]			-	Ukazatel na tabulku, kterou vlozi.
 * @param parentElement[in]	-	Ukazatel na rodicovsky prvek.
 * @return	Vraci ukazatel na nove vnoreny prvek. Pokud chyba NULL.
 */
tTabSymListElemPtr* tabSymListInsertTable(tTabSymList* tabList, tTabSym* table, tTabSymListElemPtr parentElement);

/**
 * Uvolni list z pameti.
 * @param tabList[in]	-	List pro uvolneni.
 */
void tabSymListFree(tTabSymList* tabList);


#endif /* TABSYMLIST_H_ */
/*** End of file: tabSymList.h ***/
