/*
 * frameStack.c
 *  Project: IFJ2015
 *  Created on: 24. 11. 2015
 *  Author: xbartu03, xdocek09, xjelin42
 */

#include "frameStack.h"

void SError() {
    //tohle se musi myslim dat pryc??
    //printf ("Chyba: doslo k nepovolene operaci.\n");
    return;
}

void SInit (tStack *Stack) {
 //prvni ani posledni neexistuji
 Stack->First = NULL; 
 Stack->Last = NULL; 
}

/*
 * @depractated musi pocitat s mazanim stromu
 */
void SDispose (tStack *Stack) {
    //ukazatel na posledni neni dale potreba
    Stack->Last=NULL;
    tSElemPtr tmp = NULL; 
    while (Stack->First!=NULL) { 
        //postupny mazani, dokud existuji nejake prvky
       tmp = Stack->First->rptr;
       free(Stack->First);
       Stack->First = tmp;       
    }
}

void Push (tStack *Stack, tFrame *val) {
tSElemPtr tmp =NULL;    
//naalokuju pamet pro novy prvek
tmp = malloc(sizeof(struct tSElem));

    if (tmp==NULL) {
        //alokace failnula
        DLError();
        return;
    }
    //naplni data
tmp->frame = *val;
tmp->lptr = NULL; 
tmp->rptr = NULL; 
    //zjistim jestli je prvek jediny
    if ((Stack->First ==NULL) && (Stack->Last == NULL)) { 
        //pokud ano tak bude jak prvni tak i posledni
        Stack->First = tmp; 
        Stack->Last = tmp; 
    } else{
        //pokud ne tak navazuju
        tmp->rptr = Stack->First; 
        Stack->First->lptr = tmp; 
        Stack->First = tmp;
        
    }
}


void First (tStack *Stack) {
    Stack->Act = Stack->First;	
}

void Top (tStack *Stack, tFrame *val) {
    //je prazdny?
    if (Stack->First == NULL) { 
        //vyhodim chybu
        DLError();
        return;
    }
    //ulozi hodnotu
    *val = Stack->First->frame; 
}

/*
* @depractated musi pocitat s mazanim stromu
*/
void Pop (tStack *Stack) {
tSElemPtr tmp = NULL;
    if (Stack->First == NULL) return;
    if (Stack->First == Stack->Last)Stack->Last = NULL; 
//to tmp pravy prvek
tmp = Stack->First->rptr; 
//odalokuju
free(Stack->First);
//prenastavim prvni prvek
Stack->First = tmp;
if(Stack->First != NULL) Stack->First->lptr = NULL; 
}	

void deleteTopFrame(tStack* Stack) {
	tFrame frame;
	Top(Stack, &frame);
	BSTFree(frame.frame, variableDelete);
}

int pushNewFrame(tStack* Stack, bool passable) {
	tFrame frame;
	frame.passable;
	tBSTNodePtr tree;
	Push(Stack, &frame);
}

void deleteFunctionsFrames(tStack* Stack) {
	while (Stack->First->frame.passable) {
		deleteTopFrame(Stack);
	}
	deleteTopFrame(Stack);
}

/*******rVariable - nutno presunout*****/
void variableCreate(tVariable* var, tVariableType type, void* data) {
	var->type = type;
	switch (type) {
	case VAR_TYPE_INT:
		var->data.intVal = *(int*)data;
		break;
	case VAR_TYPE_BOOL:
		var->data.boolVal = *(bool*)data;
		break;
	case VAR_TYPE_DOUBLE:
		var->data.doubleVal = *(double*)data;
		break;
	case VAR_TYPE_STRING:
		if (strInit(&var->data.stringVal)) {
			FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
		}
		if (strCopyString(&var->data.stringVal, data)) {
			//error
		}
		break;
	default:
		break;
	}
}
void variableDelete(tVariable* var) {
	if (var->type == VAR_TYPE_STRING) {
		strFree(&var->data.stringVal);
	}
}
