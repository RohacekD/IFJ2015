/*
 * stack.c
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: Obsahuje implementaci zasobniku.
 */
/**
 * @file stack.c
 * @author xdocek09
 * @brief Obsahuje implementaci zasobniku.
 */
#include "stack.h"
#include <stdlib.h>
#include <stdarg.h>

void stackInit(tStack * stack){
	stack->top=NULL;
}

/**
 * Vytvori a inicializuje novy prvek
 * @param[out] newElem	-	Zde bude ulozen ukazatel na novy prvek.
 * 							Pri chybe alokace ulozi zde NULL.
 * @param[in] data		-	Data noveho prvku.
 * @param[in] nextPtr	-	Ukazatel na dalsi prvek.
 */
void stackCreateInitNewElem(tStackElemPtr newElem, tStackData data, tStackElemPtr nextPtr){
	newElem=malloc(sizeof(struct tStackElem));
	if(newElem==NULL){
		return;
	}
	newElem->data=data;
	newElem->next=nextPtr;
}

int stackPush(tStack * stack, tStackData data){
	//inicializujeme a vytvorime novy prvek
	tStackElemPtr newElem;
	stackCreateInitNewElem(newElem, data, stack->top);
	if(newElem==NULL){
		//nepovedla se alokovat
		return 0;
	}

	//vlozime prvek na vrchol
	stack->top=newElem;
	return 1;
}

int stackPushElementOfKind(tStack * stack, tStackElemType type, int key){
	tStackData data;
	data->key=key;
	data->type=type;

	return stackPush(stack, data);
}

int stackPushElemBeforeTopTerm(tStack * stack, tStackElemType type, int key){
	//predpripravime data
	tStackElemPtr newElem;
	tStackData data;
	data->key=key;
	data->type=type;

	//ziskame nejvrchnejsi element
	tStackElemPtr element=stackTopElement(stack);
	//ztotoznime jej s elementem predchazejicim
	tStackElemPtr elemBefore=NULL;

	while(element!=NULL && element->data->type!=TERMINAL){
		//preskocime vsechny neterminaly z vrcholu zasobniku
		elemBefore=element;
		element=element->next;
	}
	if(element==NULL){
		//prazdny
		return 0;
	}

	stackCreateInitNewElem(newElem, data, element);
	if(newElem==NULL){
		//nepovedla se alokovat
		return 0;
	}

	if(elemBefore==NULL){
		//obsahuje pouze jeden prvek
		//vlozime prvek na vrchol
		stack->top=newElem;
	}else{
		//vlozime novy element pred terminal
		elemBefore->next=newElem;
	}

	return 1;
}

void stackPop(tStack * stack){
	if(stack->top==NULL){
		//neni co rusit
		return;
	}

	tStackElemPtr nextElem=stack->top->next;
	free(stack->top);
	stack->top=nextElem;
}


int stackTop(tStack * stack, tStackData* data){
	if(stack->top==NULL){
		//chyba prazdny zasobnik
		return 0;
	}

	*data=stack->top->data;
	return 1;
}

int stackTopElement(tStack * stack){
	return stack->top;
}

int stackTopTerminal(tStack* stack, int* terminal){
	//ziskame nejvrchnejsi element
	tStackElemPtr element=stackTopElement(stack);

	while(element!=NULL && element->data->type!=TERMINAL){
		//preskocime vsechny neterminaly z vrcholu zasobniku
		element=element->next;
	}
	if(element==NULL){
		return 0;
	}

	*terminal=element->data->key;
	return 1;

}

int stackEmpty(tStack * stack){
	return (stack->top==NULL)? 0:1;
}
/*** End of file: stack.c ***/
