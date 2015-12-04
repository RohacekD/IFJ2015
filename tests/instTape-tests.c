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
 
 
        tVariableInfo* infIV1 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
        tVariableInfo* infIV2 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
        tVariableInfo* infIV3 = tabSymCreateVariableInfo(TAB_SYM_VAR_INTEGER);
 
        tVariableInfo* infSV1 = tabSymCreateVariableInfo(TAB_SYM_VAR_STRING);
        tVariableInfo* infSV2 = tabSymCreateVariableInfo(TAB_SYM_VAR_STRING);
        tVariableInfo* infSV3 = tabSymCreateVariableInfo(TAB_SYM_VAR_STRING);
 
        tVariableInfo* infBV1 = tabSymCreateVariableInfo(TAB_SYM_VAR_BOOLEAN);
        tVariableInfo* infBV2 = tabSymCreateVariableInfo(TAB_SYM_VAR_BOOLEAN);
        tVariableInfo* infBV3 = tabSymCreateVariableInfo(TAB_SYM_VAR_BOOLEAN);
 
        tVariableInfo* infDV1 = tabSymCreateVariableInfo(TAB_SYM_VAR_DOUBLE);
        tVariableInfo* infDV2 = tabSymCreateVariableInfo(TAB_SYM_VAR_DOUBLE);
        tVariableInfo* infDV3 = tabSymCreateVariableInfo(TAB_SYM_VAR_DOUBLE);
 
 
        string oddelovac, pivo, rum, odd2;
        strCharToString(&oddelovac, "\n-------------------\n");
        strCharToString(&odd2, "\n----Dalsi operace----\n");
        strCharToString(&pivo, "PIVICKO");
        strCharToString(&rum, "rumicek");
 
        unionValue valInt1, valInt2, valStr1, valStr2, valStr3, valStr4, valBol1, valBol2, valDub1, valDub2;
 
        valInt1.intVal = 3;
        valInt2.intVal = 2;
 
        valStr1.stringVal = &oddelovac;
        valStr2.stringVal = &odd2;
        valStr3.stringVal = &rum;
        valStr4.stringVal = &pivo;
 
        valBol1.boolVal = 1;
        valBol2.boolVal = 0;
 
        valDub1.doubleVal = 0.005;
        valDub2.doubleVal = 1.0;
 
 
 
        tConstantInfo* infIC1 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, valInt1);
        tConstantInfo* infIC2 = tabSymCreateConstantInfo(TAB_SYM_VAR_INTEGER, valInt2);
 
        tConstantInfo* infSC1 = tabSymCreateConstantInfo(TAB_SYM_VAR_STRING, valStr1);
        tConstantInfo* infSC2 = tabSymCreateConstantInfo(TAB_SYM_VAR_STRING, valStr2);
        tConstantInfo* infSC3 = tabSymCreateConstantInfo(TAB_SYM_VAR_STRING, valStr3);
        tConstantInfo* infSC4 = tabSymCreateConstantInfo(TAB_SYM_VAR_STRING, valStr4);
 
        tConstantInfo* infBC1 = tabSymCreateConstantInfo(TAB_SYM_VAR_BOOLEAN, valBol1);
        tConstantInfo* infBC2 = tabSymCreateConstantInfo(TAB_SYM_VAR_BOOLEAN, valBol2);
 
        tConstantInfo* infDC1 = tabSymCreateConstantInfo(TAB_SYM_VAR_DOUBLE, valDub1);
        tConstantInfo* infDC2 = tabSymCreateConstantInfo(TAB_SYM_VAR_DOUBLE, valDub2);
 
 
        string intC1, intC2, intV1, intV2, intV3, strC1, strC2, strC3, strC4, strV1, strV2, strV3,
                bolC1, bolC2, bolV1, bolV2, bolV3, dubC1, dubC2, dubV1, dubV2, dubV3;
 
        strCharToString(&intC1, "intC1");
        strCharToString(&intC2, "intC2");
        strCharToString(&intV1, "intV1");
        strCharToString(&intV2, "intV2");
        strCharToString(&intV3, "intV3");
 
        strCharToString(&strC1, "strC1");
        strCharToString(&strC2, "strC2");
        strCharToString(&strC3, "strC3");
        strCharToString(&strC4, "strC4");
        strCharToString(&strV1, "strV1");
        strCharToString(&strV2, "strV2");
        strCharToString(&strV3, "strV3");
 
        strCharToString(&bolC1, "bolC1");
        strCharToString(&bolC2, "bolC2");
        strCharToString(&bolV1, "bolV1");
        strCharToString(&bolV2, "bolV2");
        strCharToString(&bolV3, "bolV3");
 
        strCharToString(&dubC1, "dubC1");
        strCharToString(&dubC2, "dubC2");
        strCharToString(&dubV1, "dubV1");
        strCharToString(&dubV2, "dubV2");
        strCharToString(&dubV3, "dubV3");
 
 
        tabSymInsertConst(tab, &intC1, infIC1);
        tabSymInsertConst(tab, &intC2, infIC2);
        tabSymInsertVar(tab, &intV1, infIV1);
        tabSymInsertVar(tab, &intV2, infIV2);
        tabSymInsertVar(tab, &intV3, infIV3);
 
        tabSymInsertConst(tab, &strC1, infSC1);
        tabSymInsertConst(tab, &strC2, infSC2);
        tabSymInsertConst(tab, &strC3, infSC3);
        tabSymInsertConst(tab, &strC4, infSC4);
        tabSymInsertVar(tab, &strV1, infSV2);
        tabSymInsertVar(tab, &strV2, infSV2);
        tabSymInsertVar(tab, &strV3, infSV3);
 
        tabSymInsertConst(tab, &bolC1, infBC1);
        tabSymInsertConst(tab, &bolC2, infBC2);
        tabSymInsertVar(tab, &bolV1, infBV1);
        tabSymInsertVar(tab, &bolV2, infBV2);
        tabSymInsertVar(tab, &bolV3, infBV3);
 
        tabSymInsertConst(tab, &dubC1, infDC1);
        tabSymInsertConst(tab, &dubC2, infDC2);
        tabSymInsertVar(tab, &dubV1, infDV1);
        tabSymInsertVar(tab, &dubV2, infDV2);
        tabSymInsertVar(tab, &dubV3, infDV3);
 
 
 
 
 
        //priprava ttabsym konec
        //priprava instape
        tInsTape* mainTape = insTapeCreate();
        tInsTape* sexTape = insTapeCreate();
 
 
        //-----------------------INT-----------------------------------------
 
        insTapeInsertLast(sexTape, I_ASSIGN, &intC1, NULL, &intV1);
        insTapeInsertLast(sexTape, I_ASSIGN, &intC2, NULL, &intV2);
 
        insTapeInsertLast(sexTape, I_DEC, &intV1, NULL, &intV2);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &intV2, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
       
 
 
        insTapeInsertLast(sexTape, I_PLUS, &intV1, &intV2, &intV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &intV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_MINUS, &intV1, &intV2, &intV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &intV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_MUL, &intV1, &intV2, &intV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &intV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_DIV, &intV1, &intV2, &intV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &intV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC2, NULL, NULL);
 
        //-----------------------DOUBLE-----------------------------------------
        insTapeInsertLast(sexTape, I_ASSIGN, &dubC1, NULL, &dubV1);
        insTapeInsertLast(sexTape, I_ASSIGN, &dubC2, NULL, &dubV2);
 
 
        insTapeInsertLast(sexTape, I_PLUS, &dubV1, &dubV2, &dubV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &dubV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_MINUS, &dubV1, &dubV2, &dubV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &dubV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_MUL, &dubV1, &dubV2, &dubV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &dubV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_DIV, &dubV1, &dubV2, &dubV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &dubV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC2, NULL, NULL);
 
        //-----------------------BOOLEAN----------------------------------------
 
        insTapeInsertLast(sexTape, I_ASSIGN, &bolC1, NULL, &bolV1);
        insTapeInsertLast(sexTape, I_ASSIGN, &bolC2, NULL, &bolV2);
 
        insTapeInsertLast(sexTape, I_LOG_AND, &bolV1, &bolV2, &bolV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &bolV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_LOG_OR, &bolV1, &bolV2, &bolV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &bolV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_LOG_NOT, &bolV1, &bolV2, &bolV3);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &bolV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC2, NULL, NULL);
 
        //-----------------------STRING----------------------------------------------
 
        //nefunguje prirazeni do promenne --- nemuzu otestovat substring
        // insTapeInsertLast(sexTape, I_ASSIGN, &strC3, NULL, &strV1);
        //insTapeInsertLast(sexTape, I_ASSIGN, &strC4, NULL, &strV2);
 
        insTapeInsertLast(sexTape, I_CONCAT, &strC3, &strC4, &strV1);
        insTapeInsertLast(sexTape, I_COUT, &strV1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        
        insTapeInsertLast(sexTape, I_SUBSTR, &strV1, &intV1, &intV2);
        insTapeInsertLast(sexTape, I_SUBSTR_DEST, NULL, NULL, &strV3);
        insTapeInsertLast(sexTape, I_COUT, &strV3, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_LENGTH, &strC4, NULL, &intV1);
        insTapeInsertLast(sexTape, I_COUT, &intV1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC2, NULL, NULL);
 
 
        //-----------------------Great Less Equal---------------------------------------
       
        insTapeInsertLast(sexTape, I_GREATER, &intC1, &intC2, &bolV1);
        insTapeInsertLast(sexTape, I_COUT, &bolV1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_LESSER, &intC1, &intC2, &bolV1);
        insTapeInsertLast(sexTape, I_COUT, &bolV1, NULL, NULL);
        insTapeInsertLast(sexTape, I_COUT, &strC1, NULL, NULL);
 
        insTapeInsertLast(sexTape, I_EQUAL, &intC1, &intC2, &bolV1);
        insTapeInsertLast(sexTape, I_COUT, &bolV1, NULL, NULL);
         
        tInsTapeInsPtr point = insTapeGetLast(sexTape);
        
        insTapeInsertLast(sexTape, I_COUT, &strC2, NULL, NULL);
        insTapeInsertLast(sexTape, I_GOTO, &point, NULL, NULL);
        //insTapeInsertLast(sexTape, I_IFZERO, &bolV1,&point, NULL);
 
 
 
 
 
 
 
 
 
 
 
 
        insTapeInsertLast(sexTape, I_RETURN, NULL, NULL, NULL);
 
        insTapeInsertLast(mainTape, I_CF, tab, sexTape->first, NULL);
        insTapeInsertLast(mainTape, I_RETURN, NULL, NULL, NULL);
        ERR_CODES retErr;
        retErr = executeTape(mainTape->first);
        if (retErr != ERR_OK)
                FatalError(retErr, ERR_MESSAGES[retErr]);
 
 
 
        //priprava instape konec
 
 
 
        //      tabSymFree(tab);//musim ji uvolnit vzdyt jsem ji ja alokoval
        //      insTapeFree(sexTape);
 
        return 0;
}