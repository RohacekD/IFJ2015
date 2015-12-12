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
#include "ial.h"
#include "tabSym.h"

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
		charJump[(unsigned)string[k]] = lengthString - k - 1;
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
			j = j + charJump[(unsigned)string[j]];
			k = searchStringLength - 1;
		}
	}
	if (k == -1) {
		return j + 1;
	} else {
		return stringLength;
	}
}

int find(char* string, char* searchString) {
	int jumps[256];
	computeJumps(searchString, jumps);
	return BMA(string, searchString, jumps);
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

int BSTSearch(tBSTNodePtr rootPtr, string* key) {
	if (rootPtr == NULL) {
		return 0;
	}

	if (strCmpString(rootPtr->key,key) == 0 ) {
		//nalezeno
		return 1;
	}
	if (strCmpString(rootPtr->key,key) < 0) {
		//hledani pokracuje v pravem podstromu
		return BSTSearch(rootPtr->r, key);
	} else {
		//hledani pokracuje v levem podstromu
		return BSTSearch(rootPtr->l, key);
	}
}

void BSTSearchTree(tBSTNodePtr rootPtr, string* key, tBSTNodePtr* store) {
	if(store==NULL){
		//chyba
		return;
	}
	if (rootPtr == NULL) {
		*store = NULL;
		return;
	}

	if (strCmpString(rootPtr->key,key) == 0) {
		//nalezeno
		*store = rootPtr;
		return;
	}
	if (strCmpString(rootPtr->key,key) < 0) {
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
 * @return	0-chyba, 1-v poradku
 */
int BSTCreateNode(tBSTNodePtr* node, string* key, void* data) {
	tBSTNodePtr newNode = (tBSTNodePtr)malloc(sizeof(struct tBSTNode));
	string* newString = (string*)malloc(sizeof(string));
	if (newNode == NULL || newString==NULL) {
		//pokus o alokaci se nazdaril
		return 0;
	}

	//naplneni stringu
	if (strInit(newString) || strCopyString(newString, key)) {
		//pokus o alokaci se nazdaril
		return 0;
	}

	newNode->key = newString;
	newNode->data = data;
	newNode->l = NULL;
	newNode->r = NULL;
	*node=newNode;
	return 1;
}

int BSTInsert(tBSTNodePtr* rootPtr, string* key, void* data) {
	if(rootPtr == NULL){
		//chyba nelze
		return 0;
	}
	if ((*rootPtr) == NULL) {
		//vytvorime novy uzel

		if(BSTCreateNode(rootPtr, key, data)==0){
			return 0;
		}
		return 1;
	}

	tBSTNodePtr workWithRootPtr=*rootPtr;//pro zprehledneni kodu

	if (strCmpString(key,workWithRootPtr->key)<0) {
		//pokracujeme levym podstrom
		return BSTInsert(&(workWithRootPtr->l), key, data);
	}
	if (strCmpString(key,workWithRootPtr->key)>0) {
		//pokracujeme pravym podstrom
		BSTInsert(&(workWithRootPtr->r), key, data);
		return BSTInsert(&(workWithRootPtr->r), key, data);
	}
	//aktualizace
	workWithRootPtr->data = data;
	return 1;
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
	//prepiseme navzajem uzly
	void* forDelData=(*assistPtr)->data;
	string* forDelKey=(*assistPtr)->key;

	(*assistPtr)->data = workWithRootPtr->data;
	(*assistPtr)->key = workWithRootPtr->key;

	workWithRootPtr->data=forDelData;
	workWithRootPtr->key=forDelKey;
	(*assistPtr) = workWithRootPtr;

	//uvolnime uzel
	*rootPtr=workWithRootPtr->l;

}

/**
 * Uvolni z pameti uzel
 * @param node[in]	-	Uzel pro uvolneni z pameti.
 */
void BSTFreeNode(tBSTNodePtr node) {
	strFree(node->key);//musime uvolnit retezec z pameti
	free(node->key);
	free(node);
}

void BSTDelete(tBSTNodePtr* rootPtr, string* key) {
	if (rootPtr==NULL && *rootPtr == NULL) {
		//chyba / konec vyhledavani
		return;
	}

	tBSTNodePtr workWithRootPtr=*rootPtr;//pro zprehledneni kodu
	if (strCmpString(key,workWithRootPtr->key)<0) {
		//pokracujeme levym podstromem
		BSTDelete(&(workWithRootPtr->l), key);
		return;
	}

	if (strCmpString(key,workWithRootPtr->key)>0) {
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

		BSTFreeNode(assistPtr);
	}
}

void BSTFree(tBSTNodePtr* rootPtr, void (*dataFree)(void*) ){

	if(rootPtr==NULL){
		//chybny ukazatel
		return;
	}
	if(*rootPtr==NULL){
		//hotovo
		return;
	}

	//mazeme levou vetev
	BSTFree(&((*rootPtr)->l), dataFree);
	//mazeme pravou vetev
	BSTFree(&((*rootPtr)->r), dataFree);

	//smazeme data
	dataFree((*rootPtr)->data);

	//smazeme uzel
	BSTFreeNode(*rootPtr);
	*rootPtr=NULL;
}

//promenna, kterou nastavim, pokud najdu nedefinovanou funkci
 int notDefined = 0;

 
void BSTcheckFuncDef(tBSTNodePtr *rootPtr) {
    
    if(rootPtr == NULL) {
        //chubny ukazatel
        return;
    }
    
    if(*rootPtr == NULL) {
        //hotovo
        return;
    }
    
    tTabSymElemData *elem = (tTabSymElemData *) (*rootPtr)->data;
    
    //pokud je nektera z funkci nedefinovana, nastavime promennou notDefined
    if(elem->info.func->defined == false)
        notDefined = 1;
    
    //prochazime levou vetvi
    BSTcheckFuncDef(&((*rootPtr)->l));
    //prochazime pravou vetvi
    BSTcheckFuncDef(&((*rootPtr)->r));
    
}
/*** End of file: ial.c ***/
