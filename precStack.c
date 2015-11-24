/*
 * precStack.c
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: Obsahuje implementaci zasobniku.
 */
/**
 * @file precStack
 * @author xdocek09
 * @brief Obsahuje implementaci zasobniku.
 */
#include "precStack.h"
#include <stdlib.h>
#include <stdarg.h>

void precStackInit(tPrecStack * stack){
	stack->top=NULL;
}

/**
 * Vytvori a inicializuje novy prvek
 * @param[in] data		-	Data noveho prvku.
 * @param[in] nextPtr	-	Ukazatel na dalsi prvek.
 * @return	Vraci ukazatel na novy prvek, pri chybe alikace vraci NULL>
 */
tPrecStackElemPtr stackCreateInitNewElem(tPrecStackData data, tPrecStackElemPtr nextPtr){
	tPrecStackElemPtr newElem=malloc(sizeof(struct tStackElem));
	if(newElem==NULL){
		return NULL;
	}
	newElem->data=data;
	newElem->next=nextPtr;
	return newElem;
}

int precStackPush(tPrecStack * stack, tPrecStackData data){
	//inicializujeme a vytvorime novy prvek
	tPrecStackElemPtr newElem;

	newElem=stackCreateInitNewElem(data, stack->top);

	if(newElem==NULL){
		//nepovedla se alokovat
		return 0;
	}

	//vlozime prvek na vrchol
	stack->top=newElem;
	return 1;
}

int precStackPushElementOfKind(tPrecStack * stack, tPrecStackElemType type, int key){
	tPrecStackData data;
	data.key=key;
	data.type=type;

	return precStackPush(stack, data);
}

int precStackPushElemBeforeTopTerm(tPrecStack * stack, tPrecStackElemType type, int key){
	//predpripravime data
	tPrecStackElemPtr newElem;
	tPrecStackData data;
	data.key=key;
	data.type=type;

	//ziskame nejvrchnejsi element
	tPrecStackElemPtr element=precStackTopElement(stack);
	//ztotoznime jej s elementem predchazejicim
	tPrecStackElemPtr elemBefore=NULL;

	while(element!=NULL && element->data.type!=PREC_STACK_TERMINAL){
		//preskocime vsechny neterminaly z vrcholu zasobniku
		elemBefore=element;
		element=element->next;
	}
	if(element==NULL){
		//prazdny
		return 0;
	}

	newElem=stackCreateInitNewElem(data, element);
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

void precStackPop(tPrecStack * stack){
	if(stack->top==NULL){
		//neni co rusit
		return;
	}

	tPrecStackElemPtr nextElem=stack->top->next;
	free(stack->top);
	stack->top=nextElem;
}


tPrecStackData* precStackTop(tPrecStack * stack){
	if(stack->top==NULL){
		//chyba prazdny zasobnik
		return NULL;
	}

	return &stack->top->data;
}

tPrecStackElemPtr precStackTopElement(tPrecStack * stack){
	return stack->top;
}

int precStackTopTerminal(tPrecStack* stack, int* terminal){
	//ziskame nejvrchnejsi element
	tPrecStackElemPtr element=precStackTopElement(stack);

	while(element!=NULL && element->data.type!=PREC_STACK_TERMINAL){
		//preskocime vsechny neterminaly z vrcholu zasobniku
		element=element->next;
	}
	if(element==NULL){
		return 0;
	}
	*terminal=element->data.key;
	return 1;

}

int precStackEmpty(tPrecStack * stack){
	return (stack->top==NULL)? 1:0;
}

void precStackDispose(tPrecStack * stack){
	while(!precStackEmpty(stack)){
		precStackPop(stack);
	}
	precStackInit(stack);
}
/*** End of file: precStack.c ***/
