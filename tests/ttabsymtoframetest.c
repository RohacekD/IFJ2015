#include "interpret.h"
#include "tabSym.h"
#include "frameStack.h"
#include "variable.h"

int main() {
	tTabSym* tab = tabSymCreate(TAB_SYM_LOC);
	tVariableInfo* info = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	string s;
	strInit(&s);
	strCharToString(&s, "ahoj");
	tabSymInsertVar(tab, &s, info);


	tStack stack;
	SInit(&stack);
	pushNewFrame(&stack, false);


	tTabSymToFrame(tab->root, &stack.Top->frameContainer);

	getchar();
	return 0;
}