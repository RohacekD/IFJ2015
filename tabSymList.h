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
 * Vyhledava symbol key rekurzivne u rodicu daneho startTabSymListElem.
 * Pokud nenajde prohledava nakonec table. Vraci prvni nalezeny.
 * @param startTabSymListElem[in]	-	startovaci prvek
 * @param locTable[in]				-	Tabulka, kterou prohleda jako posledni,
 * 										tato tabulka je mimo hierarchii rodicu.
 * 										Napriklad lokalni tabulka funkce.
 * @param key[in]					-	Klic, ktery chceme vyhledat
 * @return	Vraci ukazatel na tTabSymElemData. Nebo NULL, pokud nenajde.
 */
tTabSymElemData* tabSymListSearch(tTabSymListElemPtr startTabSymListElem, tTabSym* locTable, string* key);

/**
 * Vytvori nazev docasne promenne. ($tmpI) Zohlednuje $tmp vsech rodicu.
 * Zvysuje counter tabulky symbolu v generateFor.
 * @param generateFor[in]	-	ukazatel na element listu tabulek symbolu
 * @return	Ukazatel na vygenerovany symbol. Pokud chyba NULL.
 */
string* tabSymListCreateTmpSymbol(tTabSymListElemPtr generateFor);

/**
 * Vrati nazev naposledy vytvoreneho $tmp v generateFor. (jedna se pouze o vygenerovany nazev)
 * @param generateFor[in]	-	ukazatel na element listu tabulek symbolu
 * @return	Ukazatel na nazev posledne vygenerovaneho symbolu. Pokud chyba NULL.
 */
string* tabSymListLastCreateTmpSymbol(tTabSymListElemPtr generateFor);

/**
 * Uvolni list z pameti.
 * @param tabList[in]	-	List pro uvolneni.
 */
void tabSymListFree(tTabSymList* tabList);


#endif /* TABSYMLIST_H_ */
/*** End of file: tabSymList.h ***/
