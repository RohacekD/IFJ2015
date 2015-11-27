/*
 * tabSym.c
 *  Project: IFJ2015
 *  Created on: 22. 11. 2015
 *  Author: xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 *  Description: 
 */
/**
 * @file tabSym.c
 * @author xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 * @brief 
 */

#include <stdlib.h>
#include <stdio.h>
#include "tabSym.h"
#include "insTape.h"
#include "ial.h"
#define BUFFER_SIZE 100

tTabSym* tabSymCreate(tTabSymTypes tabType) {
	tTabSym* newTable = malloc(sizeof(tTabSym));

	if (newTable == NULL) {
		// chyba alokace
		return NULL;
	}

	newTable->tabType = tabType;
	newTable->root = NULL;
	newTable->tmpCounter=0;

	return newTable;
}

tTabSymElemData* tabSymSearch(tTabSym* table, string* key) {
	tBSTNodePtr storeElement=NULL;

	BSTSearchTree(table->root, key, &storeElement);
	if (storeElement == NULL) {
		//nenasel
		return NULL;
	}

	return storeElement->data;
}

tVariableInfo* tabSymCreateVariableInfo(tTabSymVarDataType dataType) {
	tVariableInfo* varData = malloc(sizeof(tVariableInfo));
	if (varData != NULL) {
		varData->dataType = dataType;
	}
	return varData;
}

tConstantInfo* tabSymCreateConstantInfo(tTabSymVarNoAutoDataType dataType,
		unionValue value) {
	tConstantInfo* constData = malloc(sizeof(tConstantInfo));
	if (constData != NULL) {
		constData->dataType = dataType;
		constData->value = value;
	}
	return constData;
}

tFuncInfo* tabSymCreateFuncInfo(tParamListPtr* params,
		tTabSymVarNoAutoDataType retType, tTabSym* locTab,
		tTabSymList* tabBlockList, tInsTape* instTape, bool defined) {
	tFuncInfo* funcData = malloc(sizeof(tFuncInfo));
	if (funcData != NULL) {
		funcData->params = params;
		funcData->retType = retType;
		funcData->locTab = locTab;
		funcData->tabBlockList = tabBlockList;
		funcData->instTape = instTape;
		funcData->defined = defined;
	}
	return funcData;
}

/**
 * Vytvori strukturu pro data elementu
 * @param type[in]	-	typ elementu
 * @param info[in]	-	structura obsahujici informace o elementu
 * @return	Odkaz na strukturu pro data elementu, pokud chyba tak NULL.
 */
tTabSymElemData* createElemData(tTabSymDataType type, void* info) {
	//alokujeme pamet
	tTabSymElemData* elemData = malloc(sizeof(tTabSymElemData));
	if (elemData == NULL) {
		//chyba
		return NULL;
	}

	//vlozime data
	switch (type) {
	case TAB_SYM_VARIABLE:
		elemData->info.var = (tVariableInfo*) info;
		break;
	case TAB_SYM_CONSTANT:
		elemData->info.constant = (tConstantInfo*) info;
		break;
	case TAB_SYM_FUNCTION:
		elemData->info.func = (tFuncInfo*) info;
		break;
	default:
		//nedefinovany typ
		free(elemData);
		return NULL;
		break;
	}

	elemData->type = type;
	return elemData;
}

int tabSymInsertVar(tTabSym* table, string* key, tVariableInfo* varInfo) {
	//nejprve vytvorime strukturu pro data elementu
	tTabSymElemData* elemData = createElemData(TAB_SYM_VARIABLE, varInfo);
	if (elemData == NULL) {
		//chyba
		printf("Nepovedlo se vytvorit element\n");
		return 0;
	}

	//vlozime do stromu
	return BSTInsert(&(table->root), key, elemData);
}

int tabSymInsertConst(tTabSym* table, string* key, tConstantInfo* constInfo) {
	//nejprve vytvorime strukturu pro data elementu
	tTabSymElemData* elemData = createElemData(TAB_SYM_CONSTANT, constInfo);
	if (elemData == NULL) {
		//chyba
		return 0;
	}

	//vlozime do stromu
	return BSTInsert(&(table->root), key, elemData);
}

int tabSymInsertFunc(tTabSym* table, string* key, tFuncInfo* funcInfo) {
	//nejprve vytvorime strukturu pro data elementu
	tTabSymElemData* elemData = createElemData(TAB_SYM_FUNCTION, funcInfo);
	if (elemData == NULL) {
		//chyba
		return 0;
	}

	//vlozime do stromu
	return BSTInsert(&(table->root), key, elemData);
}

/**
 * Uvolneni tVariableInfo z pameti.
 * @param var[in]	-	promenna pro uvolneni
 */
void freeVariableInfo(tVariableInfo *var) {
	free(var);
}

/**
 * Uvolni tConstantInfo z pameti.
 * @param constant[in]
 */
void freeConstantInfo(tConstantInfo * constant) {
	if (constant->dataType == TAB_SYM_VAR_NO_AUTO_STRING) {
		//free stringu
		strFree(constant->value.stringVal);
		free(constant->value.stringVal);
	}
	//smazeme strukturu ke konstante
	free(constant);
}

/**
 * Uvolni tFuncInfo z pameti.
 * @param func[in]
 */
void freeFunctionInfo(tFuncInfo * func) {

	/**
	 * Informace k funkci.
	 */
	/*typedef struct {
	 tParamListPtr* params;		// parametry funkce
	 tTabSymVarNoAutoDataType retType;	// navratovy typ funkce
	 tTabSym* locTab;			// lokalni tabulka symbolu
	 tInsTape* instTape;			// instrukcni paska
	 bool defined;				// byla definovana

	 } tFuncInfo;*/

	//uvolnime list parametru
	paramListFree((*func->params)); //UPRAVENO

	//uvolnime lokalni tabulku symbolu
	tabSymFree(func->locTab);

	//TODO: tabulky symbolu

	//uvolnime pasku instrukci
	insTapeFree(func->instTape);

	free(func);
}

/**
 * Uvolni z pameti tTabSymElemData.
 * @param dataForFree[in]	-	ukazatel na uvolneni struktury slouzici pro data elementu
 */
void freeElemData(void* dataForFreePar) {
	tTabSymElemData* dataForFree = (tTabSymElemData*) dataForFreePar;
	if (dataForFree == NULL) {
		//nelze => konec
		return;
	}

	// na zaklade typu uvolnime pamet pridelenou vnitrnim prvkum
	switch (dataForFree->type) {
	case TAB_SYM_VARIABLE:
		// uvolnovani promenne
		freeVariableInfo(dataForFree->info.var);
		dataForFree->info.var = NULL;
		break;
	case TAB_SYM_CONSTANT:
		// uvolnovani konstanty
		freeConstantInfo(dataForFree->info.constant);
		dataForFree->info.constant = NULL;
		break;
	case TAB_SYM_FUNCTION:
		// uvolnovani funkce
		freeFunctionInfo(dataForFree->info.func);
		dataForFree->info.func = NULL;
		break;
	}

	free(dataForFree);
}

/**
 * Odstrani tabulku. (uvolni z pameti)
 * 	(V pripade, ze tabulka obsahuje funkce, tak maze i lokalni tabulky dane funkce.)
 * @param table[in]		-	tabulka symbolu
 */
void tabSymFree(tTabSym* table) {
	if (table == NULL)
		return;
	//uvolnime strom
	BSTFree(&(table->root), freeElemData);

	free(table);
}

string* tabSymCreateTmpSymbol(tTabSym* table) {
	char buffer[BUFFER_SIZE];
	int flag;
	//Notice that only when this returned value is non-negative and less than n, the string has been completely written.
	flag = snprintf(buffer, BUFFER_SIZE, "$tmp%u", table->tmpCounter++);//vytvoreni nazvu docasne promenne
	if (flag < 0 || flag >= BUFFER_SIZE) {
		//chyba pri vytvareni nazvu
		return NULL;
	}
	string* newString = malloc(sizeof(string));
	if (newString == NULL) {
		//chyba pri alokaci
		return NULL;
	}
	if(strInit(newString)){
		return NULL;
	}
	strCmpConstStr(newString, buffer);
	return newString;
}


/*** End of file: tabSym.c ***/
