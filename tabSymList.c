/*
 * tabSymList.c
 *  Project: IFJ2015
 *  Created on: 26. 11. 2015
 *  Author: xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 *  Description: Implementace listu pro ulozeni tabulek symbolu. Umoznuje vytvaret hirearchii pro vnorene bloky.
 */
/**
 * @file tabSymList.c
 * @author xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 * @brief Implementace listu pro ulozeni tabulek symbolu. Umoznuje vytvaret hirearchii pro vnorene bloky.
 */
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "tabSymList.h"
#define BUFFER_SIZE 100

tTabSymList* tabSymListCreate(){
	tTabSymList* newTabSymList=malloc(sizeof(tTabSymList));
	if(newTabSymList==NULL){
		return NULL;
	}
	newTabSymList->first=NULL;
	return newTabSymList;
}

/**
 * Vlozi do listu tabulek novou tabulku.
 * @param tabList[in]		-	List, do ktereho bude vlozena tabulka.
 * @param table[in]			-	Ukazatel na tabulku, kterou vlozi.
 * @param parentElement[in]	-	Ukazatel na rodicovsky prvek.
 * @return	Vraci ukazatel na nove vnoreny prvek. Pokud chyba NULL.
 */
tTabSymListElemPtr* tabSymListInsertTable(tTabSymList* tabList, tTabSym* table, tTabSymListElemPtr parentElement){
	typedef struct tTabSymListElem{
		tTabSym* table;
		struct tTabSymListElemPtr* next;
		tTabSymList* childList;
		struct tTabSymListElem* parentElement;
	}*tTabSymListElemPtr;

	tTabSymListElemPtr* newElement=malloc(sizeof(struct tTabSymListElem));
	if(newElement==NULL){
		//chyba
		return NULL;
	}
	newElement->table=table;
	newElement->next=NULL;
	newElement->parentElement=parentElement;

	//vytvorime novy list
	newElement->childList=tabSymListCreate();
	if(newElement->childList==NULL){
		//chyba
		free(newElement);
		return NULL;
	}

	//zaradime do seznamu
	if(tabList->first==NULL){
		//vkladame prvni
		tabList->first=newElement;
	}else{
		//vkladame dalsi
		tabList->first->next=newElement;
	}

	return newElement;
}

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
tTabSymElemData* tabSymListSearch(tTabSymListElemPtr startTabSymListElem, tTabSym* locTable, string* key){
	if(startTabSymListElem==NULL){
		//koncovy, porhledame nakonec locTable
		return tabSymSearch(locTable, key);
	}
	//prohledame aktualni element
	tTabSymElemData* searched=NULL;
	searched=tabSymSearch(locTable, key);
	if(searched==NULL){
		//prohledame rekurzivne
		return tabSymListSearch(startTabSymListElem->parentElement, locTable, key);
	}
	//nalezeno na aktualni urovni
	return searched;

}

void tabSymListFree(tTabSymList* tabList){
	tTabSymListElemPtr act=tabList->first;
	tTabSymListElemPtr next;

	/*
	 * Projdeme list uvolnime jeho data.
	 * Rekurzivne volame tuto funkci pro vnorene listy.
	 */
	while(act!=NULL){
		next=act->next;
		tabSymFree(act->table);
		tabSymListFree(act->childList);
		free(act);
		act=next;
	}
	//nakonec uvolnime samotny list
	free(tabList);
}

/**
 * Secta countery od pocinajicico prvku az po posledniho rodice.
 * @param forStartElement[in]	-	zacinajici prvek
 * @param sum					-	Soucet vsech counteru rodicu + zacinajiciho prveku
 * @return	1 v poradku. 0 chyba(overflow).
 */
int sumCounters(tTabSymListElemPtr forStartElement, unsigned int* sum){
	if(forStartElement==NULL) return 0; //konecny

	unsigned int counterSum=0;
	counterSum=sumCounters(forStartElement->parentElement);
	//zkontrolujeme overflow
	if (forStartElement->table->tmpCounter > UINT_MAX - counterSum){
		//chyba overflow
		return 0;
	}

	*sum=counterSum+forStartElement->table->tmpCounter;
	return 1;
}

string* tabSymListCreateTmpSymbol(tTabSymListElemPtr generateFor) {
	char buffer[BUFFER_SIZE];
	int flag;
	unsigned int index;

	if((1>(UINT_MAX-generateFor->table->tmpCounter)) || sumCounters(generateFor,&index)==0){
		//chyba overflow
		return NULL;
	}

	//Notice that only when this returned value is non-negative and less than n, the string has been completely written.
	flag = snprintf(buffer, BUFFER_SIZE, "$tmp%u", index);//vytvoreni nazvu docasne promenne
	if (flag < 0 || flag >= BUFFER_SIZE) {
		//chyba pri vytvareni nazvu
		return NULL;
	}
	string* newString = malloc(sizeof(string));
	if (newString == NULL) {
		//chyba pri alokaci
		return NULL;
	}
	if(strInit(newString)){
		free(newString);
		return NULL;
	}
	strCmpConstStr(newString, buffer);

	//zvysime counter u tabulky symbolu
	generateFor->table->tmpCounter++;
	return newString;
}
/**
 * Vrati nazev naposledy vytvoreneho $tmp v generateFor. (jedna se pouze o vygenerovany nazev)
 * @param generateFor[in]	-	ukazatel na element listu tabulek symbolu
 * @return	Ukazatel na nazev posledne vygenerovaneho symbolu. Pokud chyba NULL.
 */
string* tabSymListLastCreateTmpSymbol(tTabSymListElemPtr generateFor){
	if(generateFor->table->tmpCounter==0){
		//nebyl zatim zadny vygenerovany.
		return NULL;
	}
	//snizime counter a vygenerujeme $tmp nazev
	generateFor->table->tmpCounter--;
	return tabSymListCreateTmpSymbol(generateFor);
}
/*** End of file: tabSymList.c ***/
