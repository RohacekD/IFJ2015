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

void SPush(tStack *Stack, tFrame *val) {
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
    tmp->frame = *val;
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

void STop(tStack *Stack, tFrame *val) {
    //je prazdny?
    if (Stack->Top == NULL) {
        //vyhodim chybu
        //DLError();
        //todo
        return;
    }
    //ulozi hodnotu
    *val = Stack->Top->frame;
}

/*
 * @depractated musi pocitat s mazanim stromu
 */
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
    tFrame frame;
    STop(list, &frame);
    BSTFree(&(frame.frame), variableDelete);
}

int pushNewFrame(tStack* list, bool passable) {
    tFrame frame;
    frame.passable;
    frame.frame = NULL;

    SPush(list, &frame);
}

void deleteFunctionsFrames(tStack* list) {
    while (list->Top->frame.passable) {
        deleteTopFrame(list);
    }
    deleteTopFrame(list);
}

void insertNewVariable(tFrame* frame, tVariable* var, string* name) {
    if (frame->frame == NULL) {
        if (!BSTCreateNode(&(frame->frame), name, var)) {
            //todo
            return;
        }
    } else {
        if (!BSTInsert(&(frame->frame), name, var)) {
            //todo
            return;
        }
    }
}

void findVariable(const tStack* stack, string* s, tVariable* var) {
    tBSTNodePtr node = NULL;
    tSElemPtr elem = stack->Top;
    if (elem->frame.passable) {
        while (elem->frame.passable && node == NULL) {
            BSTSearchTree(&(elem->frame.frame), s, node);
            elem = elem->rptr;
        }
    } else BSTSearchTree(&(elem->frame.frame), s, node);
    var = node->data;
} 