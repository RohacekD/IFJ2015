/*
 * insTape.c
 *  Project: IFJ2015
 *  Created on: 21. 11. 2015
 *  Author: xdocek09
 *  Description: Implementace instrukcni pasky, pomoci obousmerne vazaneho seznamu.
 */
/**
 * @file insTape.c
 * @author xdocek09
 * @brief  Implementace instrukcni pasky, pomoci obousmerne vazaneho seznamu.
 */

#include <stdlib.h>
#include "insTape.h"

tInsTape* insTapeCreate() {
	tInsTape* newTape = malloc(sizeof(tInsTape));

	if (newTape != NULL) {
		//inicializace
		newTape->act = NULL;
		newTape->first = NULL;
		newTape->last = NULL;
	}

	return newTape;
}

/**
 * Uvolni instrukcni pasku z pameti.
 * @param tape[in]	-	instrukcni paska pro uvolneni.
 */
void insTapeFree(tInsTape* tape) {
	//uvolnime prvky z pameti
	tInsTapeInsPtr act;
	tInsTapeInsPtr next = tape->first;

	while (next != NULL) {
		act = next;
		next = act->rptr;
		free(act);
	}
	//uvolnime z pameti pasku samotnou
	free(tape);
}

/**
 * Vytvari novou instrukci.
 * @param tape[in]	-	Paska kam vlozi instrukci.
 * @param type[in]	-	Druh instrukce.
 * @param adr1[in]	-	Prvni adresa.
 * @param adr2[in]	-	Druha adresa.
 * @param adr3[in]	-	Treti adresa.
 * @param lptr[in]	-	ukazatel na predchozi instrukci
 * @param rptr[in]	-	ukazatel na nasledujici instrukci
 * @return	Vraci ukazatel na novou instrukci. Pri chybe vraci NULL.
 */
tInsTapeInsPtr createInstruction(tInstructTypes type, void* adr1, void* adr2,
		void* adr3, tInsTapeInsPtr lptr, tInsTapeInsPtr rptr) {

	tInsTapeInsPtr newIns = malloc(sizeof(struct tInsTapeIns));
	if (newIns != NULL) {
		//nastavime hodnoty
		newIns->type = type;
		newIns->adr1 = adr1;
		newIns->adr2 = adr2;
		newIns->adr3 = adr3;
		newIns->lptr = lptr;
		newIns->rptr = rptr;
		return newIns;
	}
	return NULL;
}

int insTapeInsertFirst(tInsTape* tape, tInstructTypes type, void* adr1,
		void* adr2, void* adr3) {
	//pokusime se alokovat pamet pro novou instrukci
	tInsTapeInsPtr newIns = createInstruction(type, adr1, adr2, adr3, NULL,
			tape->first);

	if (newIns == NULL) {
		//chyba
		return 0;
	}

	//zacleneni nove instrukce

	if (tape->first == NULL) {
		//prvek je jedinny
		tape->last = newIns;
	} else {
		tape->first->lptr = newIns;
	}
	tape->first = newIns;
	return 1;
}

int insTapeInsertLast(tInsTape* tape, tInstructTypes type, void* adr1,
		void* adr2, void* adr3) {

	//pokusime se alokovat pamet pro novou instrukci
	tInsTapeInsPtr newIns = createInstruction(type, adr1, adr2, adr3,
			tape->last, NULL);

	if (newIns == NULL) {
		//chyba
		return 0;
	}

	//zacleneni nove instrukce

	if (tape->last == NULL) {
		//prvek je jedinny
		tape->first = newIns;
	} else {
		tape->last->rptr = newIns;
	}
	tape->last = newIns;
	return 1;
}

void insTapeFirst(tInsTape* tape) {
	tape->act = tape->first;
}

void insTapeLast(tInsTape* tape) {
	tape->act = tape->last;
}

tInsTapeInsPtr insTapeGetFirst(tInsTape* tape) {
	return tape->first;
}

tInsTapeInsPtr insTapeGetLast(tInsTape* tape) {
	return tape->last;
}

void insTapeDeleteFirst(tInsTape* tape) {
	if (tape->first == NULL) {
		return;
	}

	//zkontrolujeme, jestli nahodou neni prvni aktivni
	if (tape->first == tape->act) {
		// rusime aktivitu
		tape->act = NULL;
	}

	//smazeme prvni prvek a nastavim druhy prvek jako prvni
	tInsTapeInsPtr newFirst = tape->first->rptr;
	if (newFirst == NULL) {
		//prvni byl i posledni
		tape->last = NULL;
	} else {
		//nove prvni
		newFirst->lptr = NULL;
	}
	free(tape->first);
	tape->first = newFirst;
}

void insTapeDeleteLast(tInsTape* tape) {
	if (tape->last == NULL) {
		return;
	}

	//zkontrolujeme, jestli nahodou neni posledni aktivni
	if (tape->last == tape->act) {
		// rusime aktivitu
		tape->act = NULL;
	}

	//smazeme posledni prvek a nastavime predposledni jako posledni
	tInsTapeInsPtr newLast = tape->last->lptr;
	if (newLast == NULL) {
		//posledni byl i prvni
		tape->first = NULL;
	} else {
		//nove posledni
		newLast->rptr = NULL;
	}
	free(tape->last);
	tape->last = newLast;
}

void insTapePostDelete(tInsTape* tape) {
	if (tape->act == NULL || tape->last == tape->act) {
		//paska neni aktivni, nebo je aktivni posledni instrukce
		return;
	}
	//ulozime si odkaz na prvek za prvekem, ktery mazeme
	tInsTapeInsPtr newPost = tape->act->rptr->rptr;
	free(tape->act->rptr);
	tape->act->rptr = newPost;

	if (newPost != NULL) {
		//v pripade, ze smazana instrukce nebyla posledni
		newPost->lptr = tape->act;
	} else {
		//v pripade, ze smazana instrukce byla posledni
		tape->last = tape->act;
	}
}

void insTapePreDelete(tInsTape* tape){
	if(tape->act==NULL || tape->first==tape->act){
		//paska neni aktivni, nebo je aktivni prvni instrukce
		return;
	}
	//ulozime si odkaz na instrukci pred prvekem, ktery mazeme
	tInsTapeInsPtr newPre=tape->act->lptr->lptr;
	free(tape->act->lptr);

	tape->act->lptr=newPre;

	if(newPre!=NULL){
		//v pripade, ze smazana instrukce nebyla prvni
		newPre->rptr=tape->act;
	}else{
		//v pripade, ze smazana instrukce byla prvni
		tape->first=tape->act;
	}
}

int insTapePostInsert(tInsTape* tape, tInstructTypes type, void* adr1,
		void* adr2, void* adr3){
	if(tape->act==NULL){
		//paska neni aktivni
		return 0;
	}
	//pokusime se alokovat pamet pro novou instrukci
	tInsTapeInsPtr newInstruct=createInstruction(type, adr1, adr2, adr3, tape->act, tape->act->rptr);

	if(newInstruct==NULL){
		//chyba
		return 0;
	}

	//provazeme
	if(tape->act==tape->last){
		//aktivni byla posledni
		tape->last=newInstruct;
	}else{
		tape->act->rptr->lptr=newInstruct;
	}
	tape->act->rptr=newInstruct;
	return 1;
}

int insTapePreInsert(tInsTape* tape, tInstructTypes type, void* adr1,
		void* adr2, void* adr3){
	if(tape->act==NULL){
		//paska neni aktivni
		return 0;
	}

	//pokusime se alokovat pamet novou instrukci
	tInsTapeInsPtr newInstruct=createInstruction(type, adr1, adr2, adr3, tape->act->lptr, tape->act);
	if(newInstruct==NULL){
		//chyba
		return 0;
	}

	//provazeme
	if(tape->act==tape->first){
		//aktivni byla prvni
		tape->first=newInstruct;
	}else{
		tape->act->lptr->rptr=newInstruct;
	}
	tape->act->lptr=newInstruct;
	return 1;
}

tInsTapeInsPtr insTapeGetActive(tInsTape* tape){
	return tape->act;
}

void insTapeActualize(tInsTape* tape, tInstructTypes type, void* adr1,
		void* adr2, void* adr3){

	if(tape->act==NULL) return;

	tape->act->type=type;
	tape->act->adr1=adr1;
	tape->act->adr2=adr2;
	tape->act->adr3=adr3;
}

void insTapeSucc(tInsTape* tape){
	if(tape->act==NULL) return;

	tape->act=tape->act->rptr;
}

void insTapePred(tInsTape* tape){
	if(tape->act==NULL) return;

	tape->act=tape->act->lptr;
}

int insTapeActive(tInsTape* tape){
	return (tape->act==NULL) ? 0:1;
}
/*** End of file: insTape.c ***/
