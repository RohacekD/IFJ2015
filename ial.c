/*
 * ial.c
 *  Project: interpreter
 *  Created on: 18. 10. 2015
 *  Author: xdocek09
 *  Description: Vestavene funkce pro IAL.
 */
/**
 * @file ial.c
 * @author xdocek09
 * @brief Vestavene funkce pro IAL.
 */
#include <stdlib.h>
#include <stdio.h>
#include "ial.h"

int len(char* string) {
	int num = 0;
	while (string[num] != 0)
		num++;
	return num;
}

void computeJumps(char* string, int charJump[]) {
	int lengthString = len(string);

	for (int c = 0; c < 256; c++) {

		//plnime maximalnim skokem
		charJump[c] = lengthString;

	}

	for (int k = 0; k < lengthString; k++) {
		charJump[string[k]] = lengthString - k - 1;
	}
}

int BMA(char* string, char* searchString, int charJump[]) {

	int stringLength = len(string);
	int searchStringLength = len(searchString);
	int j = searchStringLength - 1;	//index v textu
	int k = searchStringLength - 1;	//index ve vzorku

	while (j < stringLength && k >= 0) {

		if (string[j] == searchString[k]) {
			j--;
			k--;
		} else {
			j = j + charJump[string[j]];
			k = searchStringLength - 1;
		}
	}
	if (k == -1) {
		return j + 1;
	} else {
		return stringLength;
	}
}
/**
 * Pouziva funkce heapsort.
 * @param string
 * @param l
 * @param r
 */
void siftDown(char * string, int l, int r) {
	int i = l;
	int j = 2 * i;
	int con = (j <= r);
	int tmp = string[i];

	while (con) {
		if (j < r) {
			if (string[j] < string[j + 1]) {
				j++;
			}
		}
		if (tmp >= string[j]) {
			con = 0;
		} else {
			string[i] = string[j];
			i = j;
			j = 2 * i;
			con = (j <= r);
		}

	}
	string[i] = tmp;
}

void heapSort(char* string) {
	int stringLength = len(string);
	int l;
	int r;
	char tmp;
	l = stringLength / 2;
	r = stringLength - 1;

	for (int i = l; i >= 0; i--) {
		siftDown(string, i, r);
	}

	for (r = stringLength - 1; r > 0; r--) {

		tmp = string[0];
		string[0] = string[r];
		string[r] = tmp;

		siftDown(string, 0, r - 1);
	}
}

int BSTSearch(tBSTNodePtr rootPtr, int key) {
	if (rootPtr == NULL) {
		return 0;
	}

	if (rootPtr->key == key) {
		//nalezeno
		return 1;
	}
	if ((rootPtr->key) < key) {
		//hledani pokracuje v pravem podstromu
		return BSTSearch(rootPtr->r, key);
	} else {
		//hledani pokracuje v levem podstromu
		return BSTSearch(rootPtr->l, key);
	}
}

void BSTSearchTree(tBSTNodePtr rootPtr, int key, tBSTNodePtr* store) {
	if(store==NULL){
		//chyba
		return;
	}
	if (rootPtr == NULL) {
		*store = NULL;
		return;
	}

	if (rootPtr->key == key) {
		//nalezeno
		*store = rootPtr;
		return;
	}
	if ((rootPtr->key) < key) {
		//hledani pokracuje v pravem podstromu
		BSTSearchTree(rootPtr->r, key, store);
	} else {
		//hledani pokracuje v levem podstromu
		BSTSearchTree(rootPtr->l, key, store);
	}

}

/**
 * Vytvari uzel.
 * @param node	-	ukazatel pro alokaci uzlu
 * @param key	-	klic noveho uzlo
 * @param data	-	data noveho uzlo
 */
void BSTCreateNode(tBSTNodePtr* node, int key, tData* data) {
	tBSTNodePtr newNode = malloc(sizeof(struct tBSTNode));

	if (newNode == NULL) {
		//pokus o alokaci se nazdaril
		return;
	}
	newNode->key = key;
	newNode->data = data;
	newNode->l = NULL;
	newNode->r = NULL;
	*node=newNode;
}
void BSTInsert(tBSTNodePtr* rootPtr, int key, tData* data) {
	if(rootPtr == NULL){
		//chyba nelze
		return;
	}
	if ((*rootPtr) == NULL) {
		//vytvorime novy uzel
		BSTCreateNode(rootPtr, key, data);
		return;
	}

	tBSTNodePtr workWithRootPtr=*rootPtr;//pro zprehledneni kodu

	if (key < workWithRootPtr->key) {
		//pokracujeme levym podstrom
		BSTInsert(&(workWithRootPtr->l), key, data);
		return;
	}
	if (key > workWithRootPtr->key) {
		//pokracujeme pravym podstrom
		BSTInsert(&(workWithRootPtr->r), key, data);
		return;
	}
	//aktualizace
	workWithRootPtr->data = data;
}

/**
 * Vyhledava uzel pro odstraneni (jedna se o nejpravejsi uzel). Pokud najde prepise
 * s nim assistPtr.
 * @param rootPtr	-	ukazatel na korenovy uzel leveho podstromu ruseneho uzlu.
 * @param assistPtr	-	Po provedeni je zde ulozen odkaz na uzel pro odstraneni.
 */
void BSTDeleteVictimSearch(tBSTNodePtr* rootPtr, tBSTNodePtr* assistPtr) {
	if (rootPtr==NULL){
		//chyba
		return;
	}
	tBSTNodePtr workWithRootPtr=*rootPtr;//pro zprehledneni kodu
	if (workWithRootPtr->r != NULL) {
		//pokracujeme v pravem podstromu
		BSTDeleteVictimSearch(&(workWithRootPtr->r), assistPtr);
		return;
	}
	//nejpravejsi ukazatel je nalezen
	//prepiseme uzel
	(*assistPtr)->data = workWithRootPtr->data;
	(*assistPtr)->key = workWithRootPtr->key;
	(*assistPtr) = workWithRootPtr;

	//uvolnime uzel
	*rootPtr=workWithRootPtr->l;	/**todo Proc to tu je?*/

}

void BSTDelete(tBSTNodePtr* rootPtr, int key) {
	if (rootPtr==NULL && *rootPtr == NULL) {
		//chyba / konec vyhledavani
		return;
	}

	tBSTNodePtr workWithRootPtr=*rootPtr;//pro zprehledneni kodu
	if (key < workWithRootPtr->key) {
		//pokracujeme levym podstromem
		BSTDelete(&(workWithRootPtr->l), key);
		return;
	}

	if (key > workWithRootPtr->key) {
		//pokracujeme pravym podstromem
		BSTDelete(&(workWithRootPtr->r), key);
		return;
	}

	//jsme na klici=>rusime ukazatel rootPtr
	tBSTNodePtr assistPtr = workWithRootPtr;
	if (assistPtr->r == NULL) {
		//uzel nema pravy podstrom, pripojime levy podstrom
		*rootPtr = assistPtr->l;
	} else {
		if (assistPtr->l == NULL) {
			//uzel nema levy podstrom, pripojime pravy podstrom
			*rootPtr = assistPtr->r;
			return;
		} else {
			//prohledame levy podstrom pro nejpravejsi uzel
			BSTDeleteVictimSearch(&(assistPtr->l), &assistPtr);
		}
		free(assistPtr);
	}
}

/*** End of file: ial.c ***/
