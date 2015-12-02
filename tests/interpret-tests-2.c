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
	unionValue val1 = {
		.intVal = 42
	};
	unionValue val2 = {
		.intVal = 35
	};
	tVariableInfo* info1 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	tVariableInfo* info2 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	tVariableInfo* info3 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	tConstantInfo* info4 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, val1);
	tConstantInfo* info5 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, val2);
	string temp1;
	strInit(&temp1);
	strConConstString(&temp1, "temp1");
	string temp2;
	strInit(&temp2);
	strConConstString(&temp2, "temp2");
	string temp3;
	strInit(&temp3);
	strConConstString(&temp3, "temp3");
	string temp4;
	strInit(&temp4);
	strConConstString(&temp4, "temp4");
	string temp5;
	strInit(&temp5);
	strConConstString(&temp5, "temp5");
	tabSymInsertVar(tab, &temp1, info1);
	tabSymInsertVar(tab, &temp2, info2);
	tabSymInsertVar(tab, &temp3, info3);
	tabSymInsertConst(tab, &temp4, info4);
	tabSymInsertConst(tab, &temp5, info5);
	//priprava ttabsym konec
	//priprava instape
	tInsTape* sexTape = insTapeCreate();
	insTapeInsertLast(sexTape, I_CF, tab, NULL, NULL);
	insTapeInsertLast(sexTape, I_ASSIGN, &temp4, NULL, &temp1);
	insTapeInsertLast(sexTape, I_ASSIGN, &temp5, NULL, &temp2);
	insTapeInsertLast(sexTape, I_PLUS, &temp1, &temp2, &temp3);
	insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
	insTapeInsertLast(sexTape, I_RETURN, NULL, NULL, NULL);
	//priprava instape konec
	executeTape(sexTape->first);


	tabSymFree(tab);//musim ji uvolnit vzdyt jsem ji ja alokoval
	insTapeFree(sexTape);
	strFree(&temp1);
	strFree(&temp2);
	strFree(&temp3);
	strFree(&temp4);
	strFree(&temp5);
	return 0;
}