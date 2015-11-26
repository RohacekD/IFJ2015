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
#include "tabSymList.h"

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

/*** End of file: tabSymList.c ***/
