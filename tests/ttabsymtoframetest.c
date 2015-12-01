#include "interpret.h"
#include "tabSym.h"
#include "frameStack.h"
#include "variable.h"

/*
 * Test na prevod tTabSym na frameStack
 * Test probehl ok a nenechal po sobe zadnou pamet
 */
int main() {
	tTabSym* tab = tabSymCreate(TAB_SYM_LOC);
	tVariableInfo* info1 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	tVariableInfo* info2 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	tVariableInfo* info3 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	string temp1;
	strInit(&temp1);
	strConConstString(&temp1, "temp1");
	string temp2;
	strInit(&temp2);
	strConConstString(&temp2, "temp2");
	string temp3;
	strInit(&temp3);
	strConConstString(&temp3, "temp3");
	tabSymInsertVar(tab, &temp1, info1);
	tabSymInsertVar(tab, &temp2, info2);
	tabSymInsertVar(tab, &temp3, info3);


	tStack stack;
	SInit(&stack);
	pushNewFrame(&stack, false);


	tTabSymToFrame(tab->root, &stack.Top->frameContainer);



	tabSymFree(tab);
	deleteFunctionsFrames(&stack);
	strFree(&temp1);
	strFree(&temp2);
	strFree(&temp3);
	return 0;
}