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

int precStackPushElementOfKind(tPrecStack * stack, tPrecStackElemType type, int key, string* id){
	tPrecStackData data;
	data.key=key;
	data.type=type;
	data.id=id;
	return precStackPush(stack, data);
}

int precStackPushElemBeforeTopTerm(tPrecStack * stack, tPrecStackElemType type, int key, string* id){
	//predpripravime data
	tPrecStackElemPtr newElem;
	tPrecStackData data;
	data.key=key;
	data.type=type;
	data.id=id;

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
	if(stack->top.data.id!=NULL){
		//mazeme id
		strFree(stack->top.data.id);
		free(stack->top.data.id);
	}
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

int precStackTopTerminal(tPrecStack* stack, tParExpTerminals* terminal){
	//ziskame nejvrchnejsi element
	tPrecStackElemPtr element=precStackTopElement(stack);

	while(element!=NULL &&
			(element->data.type!=PREC_STACK_TERMINAL || element->data.type!=PREC_STACK_ENDMARK)){
		//preskocime vsechny neterminaly z vrcholu zasobniku
		element=element->next;
	}
	if(element==NULL){
		return 0;
	}
	*terminal=element->data.key;
	return 1;

}

tPrecStack * precStackCreateRevertedTopReduc(tPrecStack * stack){
	tPrecStack * newStack=malloc(sizeof(tPrecStack));
	if(newStack!=NULL){

		precStackInit(newStack);
		tPrecStackData actData = *precStackTop(stack);

		//naplnime zasobnikem <y
		while(!precStackEmpty(stack) &&  actData.type!=PREC_STACK_ENDMARK){
			precStackPush(newStack,actData);
			precStackPop(stack);
			if(actData.type!=PREC_STACK_SIGN){
				precStackPop(newStack);	//sign nechceme
				//koncime
				break;
			}
			actData = precStackTop(stack);
		}

	}
	return newStack;
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
void precStackFree(tPrecStack ** stack){
	if(stack==NULL){
		return;
	}
	if(* stack==NULL){
		return;
	}
	precStackDispose(*stack);
	free(*stack);
	*stack=NULL;
}
/*** End of file: precStack.c ***/
