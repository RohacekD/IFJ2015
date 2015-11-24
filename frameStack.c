#include "frameStack.h"


void SInit(tStack *Stack) {
	//prvni ani posledni neexistuji
	Stack->First = NULL;
	Stack->Last = NULL;
}

/*
* @depractated musi pocitat s mazanim stromu
*/
void SDispose(tStack *Stack) {
	//ukazatel na posledni neni dale potreba
	Stack->Last = NULL;
	tSElemPtr tmp = NULL;
	while (Stack->First != NULL) {
		//postupny mazani, dokud existuji nejake prvky
		tmp = Stack->First->rptr;
		free(Stack->First);
		Stack->First = tmp;
	}
}

void Push(tStack *Stack, tFrame *val) {
	tSElemPtr tmp = NULL;
	//naalokuju pamet pro novy prvek
	tmp = malloc(sizeof(struct tSElem));

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
	if ((Stack->First == NULL) && (Stack->Last == NULL)) {
		//pokud ano tak bude jak prvni tak i posledni
		Stack->First = tmp;
		Stack->Last = tmp;
	}
	else {
		//pokud ne tak navazuju
		tmp->rptr = Stack->First;
		Stack->First->lptr = tmp;
		Stack->First = tmp;

	}
}

void Top(tStack *Stack, tFrame *val) {
	//je prazdny?
	if (Stack->First == NULL) {
		//vyhodim chybu
		//DLError();
		//todo
		return;
	}
	//ulozi hodnotu
	*val = Stack->First->frame;
}

/*
* @depractated musi pocitat s mazanim stromu
*/
void Pop(tStack *Stack) {
	tSElemPtr tmp = NULL;
	if (Stack->First == NULL) return;
	if (Stack->First == Stack->Last)Stack->Last = NULL;
	//to tmp pravy prvek
	tmp = Stack->First->rptr;
	//odalokuju
	free(Stack->First);
	//prenastavim prvni prvek
	Stack->First = tmp;
	if (Stack->First != NULL) Stack->First->lptr = NULL;
}


void deleteTopFrame(tStack* list) {
	tFrame frame;
	DLTop(list, &frame);
	BSTFree(frame.frame, variableDelete);
}

int pushNewFrame(tStack* list, bool passable) {
	tFrame frame;
	frame.passable;
	frame.frame = NULL;

	DLPush(list, &frame);
}

void deleteFunctionsFrames(tStack* list) {
	while (list->First->frame.passable) {
		deleteTopFrame(list);
	}
	deleteTopFrame(list);
}


void insertNewVariable(tFrame* frame, tVariable* var, string* name) {
	if(frame->frame==NULL){
		if (!BSTCreateNode(frame->frame, name, var)) {
			//todo
			return;
		}
	}
	else {
		if (!BSTInsert(frame->frame, name, var)) {
			//todo
			return;
		}
	}
}

int findVariable(const tStack* stack, string* s, tVariable* var) {
	
}