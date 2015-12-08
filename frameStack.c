#include "frameStack.h"

void SInit(tStack *Stack) {
    //prvni ani posledni neexistuji
    Stack->Top = NULL;
    Stack->Last = NULL;
}

/*
 * @depractated musi pocitat s mazanim stromu
 */
void SDispose(tStack *Stack) {
    //ukazatel na posledni neni dale potreba
    Stack->Last = NULL;
    tSElemPtr tmp = NULL;
    while (Stack->Top != NULL) {
        //postupny mazani, dokud existuji nejake prvky
        tmp = Stack->Top->rptr;
        free(Stack->Top);
        Stack->Top = tmp;
    }
}

void SPush(tStack *Stack, tFrameContainer *val) {
    tSElemPtr tmp = NULL;
    //naalokuju pamet pro novy prvek
    tmp = malloc(sizeof (struct tSElem));

    if (tmp == NULL) {
        //alokace failnula
        //DLError();
        //todo
        return;
    }
    //naplni data
    tmp->frameContainer = *val;
    tmp->lptr = NULL;
    tmp->rptr = NULL;
    //zjistim jestli je prvek jediny
    if ((Stack->Top == NULL) && (Stack->Last == NULL)) {
        //pokud ano tak bude jak prvni tak i posledni
        Stack->Top = tmp;
        Stack->Last = tmp;
    } else {
        //pokud ne tak navazuju
        tmp->rptr = Stack->Top;
        Stack->Top->lptr = tmp;
        Stack->Top = tmp;

    }
}

int STop(tStack *Stack, tFrameContainer *val) {
    //je prazdny?
    if (Stack->Top == NULL) {
		return 0;
    }
    //ulozi hodnotu
    *val = Stack->Top->frameContainer;
	return 1;
}

void SPop(tStack *Stack) {
    tSElemPtr tmp = NULL;
    if (Stack->Top == NULL) return;
    if (Stack->Top == Stack->Last)Stack->Last = NULL;
    //to tmp pravy prvek
    tmp = Stack->Top->rptr;
    //odalokuju
    free(Stack->Top);
    //prenastavim prvni prvek
    Stack->Top = tmp;
    if (Stack->Top != NULL) Stack->Top->lptr = NULL;
}

void deleteTopFrame(tStack* list) {
    tFrameContainer frame;
    STop(list, &frame);
    BSTFree(&(frame.frame), variableDelete);
	SPop(list);
}

void pushNewFrame(tStack* list, bool passable) {
    tFrameContainer frame;
    frame.passable = passable;
    frame.frame = NULL;

    SPush(list, &frame);
}

void deleteFunctionsFrames(tStack* list) {
    while (list->Top->frameContainer.passable) {
        deleteTopFrame(list);
    }
    deleteTopFrame(list);
}

void insertNewVariable(tFrameContainer* frameContainer, tVariablePtr var, string* name) {
	if (!frameContainer) return;
    if (frameContainer->frame == NULL) {
        if (!BSTCreateNode(&(frameContainer->frame), name, (void*)var)) {
            //todo
            return;
        }
    } else {
        if (!BSTInsert(&(frameContainer->frame), name, (void*)var)) {
            //todo
            return;
        }
    }
}

int findVariable(const tStack* stack, string* s, tVariablePtr* var) {
    tBSTNodePtr node = NULL;
    *var = NULL;
    if (stack == NULL || s == NULL || !stack->Top) {
        return 0;
    }

    tSElemPtr elem = stack->Top;
    if (!elem->frameContainer.passable) {
        BSTSearchTree((elem->frameContainer.frame), s, &node);
    } 
    else{
        while (elem->frameContainer.passable) {
                BSTSearchTree((elem->frameContainer.frame), s, &node);
                if (node != NULL){
                    *var=(tVariablePtr)(node->data);
                    return 1;//jiz jsme nasli promennou
                }
                if (!elem->rptr) return 0;//dosli jsme na konec stacku
                elem = elem->rptr;//iterace stackem
        }
        BSTSearchTree((elem->frameContainer.frame), s, &node);       
    }
    if (node != NULL){
        *var=(tVariablePtr)(node->data);
        return 1; //jiz jsme nasli promennou
    }
    else return 0;
} 

int findVariableInSubFrame(const tStack* stack, string* s, tVariablePtr* var) {
    tBSTNodePtr node = NULL;
    *var = NULL;
    if (stack == NULL || s == NULL || !stack->Top) {
        return 0;
    }

    tSElemPtr elem = stack->Top;
    while(elem->frameContainer.passable){
        if(stack->Top->rptr != NULL) elem = stack->Top->rptr;
        else return 0;       
    }
    if(stack->Top->rptr != NULL) elem = stack->Top->rptr;
    else return 0; 
    
    if (!elem->frameContainer.passable) {
        BSTSearchTree((elem->frameContainer.frame), s, &node);
    } 
    else{
        while (elem->frameContainer.passable) {
                BSTSearchTree((elem->frameContainer.frame), s, &node);
                if (node != NULL){
                    *var=(tVariablePtr)(node->data);
                    return 1;//jiz jsme nasli promennou
                }
                if (!elem->rptr) return 0;//dosli jsme na konec stacku
                elem = elem->rptr;//iterace stackem
        }
        BSTSearchTree((elem->frameContainer.frame), s, &node);       
    }
    if (node != NULL){
        *var=(tVariablePtr)(node->data);
        return 1; //jiz jsme nasli promennou
    }
    else return 0;
} 