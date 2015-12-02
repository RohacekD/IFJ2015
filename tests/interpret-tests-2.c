#include "interpret.h"
#include "tabSym.h"
#include "frameStack.h"
#include "variable.h"

/*
* Test na prevod tTabSym na frameStack
* Test probehl ok a nenechal po sobe zadnou pamet
*/
int main() {
        //vytvorim si tabulku symbolu tab
	tTabSym* tab = tabSymCreate(TAB_SYM_LOC);
        unionValue val0 = {
		.intVal = 80
	};
        //hodnota integer 80
	unionValue val1 = {
		.intVal = 80
	};
        //hodnota integer 40
	unionValue val2 = {
		.intVal = 40
	};
        unionValue val3 = {
		.intVal = 0
	};
        unionValue val4 = {
		.boolVal = true
	};
        unionValue val5 = {
		.intVal = 9223372036854775806
	};
        unionValue val6 = {
		.doubleVal = 80.005
	};
        //hodnota integer 80
	unionValue val7 = {
		.doubleVal = 80.005
	};
       
        tVariableInfo* doublova = tabSymCreateVariableInfo(TAB_SYM_VAR_DOUBLE);
	tVariableInfo* info1 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	tVariableInfo* info2 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
	tVariableInfo* info3 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
        tConstantInfo* info0 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, val0);
	tConstantInfo* info4 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, val1);
	tConstantInfo* info5 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, val2);
        tConstantInfo* info6 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, val3);
        tConstantInfo* info7 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, val4);
        tConstantInfo* info8 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, val5);
        tConstantInfo* info9 = tabSymCreateConstantInfo(TAB_SYM_VAR_DOUBLE, val6);
        tConstantInfo* info10 = tabSymCreateConstantInfo(TAB_SYM_VAR_DOUBLE, val7);
	string doubletmp;
	strInit(&doubletmp);
	strConConstString(&doubletmp, "doubletmp");
        string temp0;
	strInit(&temp0);
	strConConstString(&temp0, "temp0");
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
        string temp6;
	strInit(&temp6);
	strConConstString(&temp6, "temp6");
        string temp7;
	strInit(&temp7);
	strConConstString(&temp7, "temp7");
        string temp8;
	strInit(&temp8);
	strConConstString(&temp8, "temp8");
        string temp9;
	strInit(&temp9);
	strConConstString(&temp9, "temp9");
        string temp10;
	strInit(&temp10);
	strConConstString(&temp10, "temp10");
        
	tabSymInsertVar(tab, &temp1, info1);
	tabSymInsertVar(tab, &temp2, info2);
	tabSymInsertVar(tab, &temp3, info3);
        tabSymInsertVar(tab, &doubletmp, doublova);
	tabSymInsertConst(tab, &temp4, info4);
	tabSymInsertConst(tab, &temp5, info5);
        tabSymInsertConst(tab, &temp6, info6);
        tabSymInsertConst(tab, &temp7, info7);
        tabSymInsertConst(tab, &temp8, info8);
        tabSymInsertConst(tab, &temp0, info0);
        tabSymInsertConst(tab, &temp9, info9);
        tabSymInsertConst(tab, &temp10, info10);
	//priprava ttabsym konec
	//priprava instape
	tInsTape* sexTape = insTapeCreate();
        //zacatek - create frame
        printf("Zacatek instrukcni pasky: \n");
	insTapeInsertLast(sexTape, I_CF, tab, NULL, NULL);
        //priradim do temp1 hodnotu z temp4
        printf("Prirazeni hodnoty 80 do temp1..\n");
	insTapeInsertLast(sexTape, I_ASSIGN, &temp4, NULL, &temp1);
        //do temp2 hodnotu z temp5
        printf("Prirazeni hodnoty 40 do temp2..\n");
	insTapeInsertLast(sexTape, I_ASSIGN, &temp5, NULL, &temp2);
        //provedu soucet temp1 a temp2 -> ulozim do temp3
	insTapeInsertLast(sexTape, I_PLUS, &temp1, &temp2, &temp3);
        //vypisu temp 3 
        printf("Soucet 80 + 40 .. \n");
	insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Odecteni 120 - 40 ..\n");
        insTapeInsertLast(sexTape, I_MINUS, &temp3, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Nasobeni 80 * 0 \n");
        insTapeInsertLast(sexTape, I_MUL, &temp3, &temp6, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Nasobeni 40*80\n");
        insTapeInsertLast(sexTape, I_MUL, &temp1, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Deleni 40/80\n");
        insTapeInsertLast(sexTape, I_DIV, &temp2, &temp1, &doubletmp);
        insTapeInsertLast(sexTape, I_COUT, &doubletmp, NULL, NULL);
        printf("Deleni 40/0 do double\n");
        insTapeInsertLast(sexTape, I_DIV, &temp2, &temp6, &doubletmp);
        insTapeInsertLast(sexTape, I_COUT, &doubletmp, NULL, NULL);
        printf("zkouska preteceni rozsahu integer..\n");
        printf("Prirazeni hodnoty 9223372036854775807 do temp2..\n");
	insTapeInsertLast(sexTape, I_ASSIGN, &temp5, NULL, &temp2);
        printf("Pricteni 80 k max integer..\n");
        insTapeInsertLast(sexTape, I_PLUS, &temp1, &temp8, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Equal pro 80 a 80 v int\n");
        insTapeInsertLast(sexTape, I_EQUAL, &temp0, &temp4, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Equal pro 80.005 a 80.005 v double - bude false\n");
        insTapeInsertLast(sexTape, I_EQUAL, &temp9, &temp10, &doubletmp);
        insTapeInsertLast(sexTape, I_COUT, &doubletmp, NULL, NULL);
        printf("Equal pro 80 a 40 - bude false\n");
        insTapeInsertLast(sexTape, I_EQUAL, &temp0, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("NotEqual pro 80 a 40 - bude true\n");
        insTapeInsertLast(sexTape, I_NOTEQUAL, &temp0, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Greater pro 80 a 40\n");
        insTapeInsertLast(sexTape, I_GREATER, &temp0, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("lesser pro 80 a 40\n");
        insTapeInsertLast(sexTape, I_LESSER, &temp0, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("GEQUAL pro 80 a 40\n");
        insTapeInsertLast(sexTape, I_GEQUAL, &temp0, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Gless pro 80 a 40\n");
        insTapeInsertLast(sexTape, I_LEQUAL, &temp0, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Gless pro 40 a 40\n");
        insTapeInsertLast(sexTape, I_LEQUAL, &temp2, &temp2, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Unarni minus 40\n");
        insTapeInsertLast(sexTape, I_UMINUS, &temp2,NULL, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("Unarni minus -40\n");
        insTapeInsertLast(sexTape, I_UMINUS, &temp3,NULL, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("LOG. not pro 0\n");
        insTapeInsertLast(sexTape, I_LOG_NOT, &temp6,NULL, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("LOG. and pro 40 a 0\n");
        insTapeInsertLast(sexTape, I_LOG_AND, &temp0,&temp6, &temp3);
        insTapeInsertLast(sexTape, I_COUT, &temp3, NULL, NULL);
        printf("LOG. or pro 40 a 0\n");
        insTapeInsertLast(sexTape, I_LOG_OR, &temp0,&temp6, &temp3);
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
        strFree(&temp0);
        strFree(&temp6);
        strFree(&temp7);
        strFree(&temp8);
        strFree(&temp9);
        strFree(&temp10);
	return 0;
}
