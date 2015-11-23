/*
 * tabSym.h
 *  Project: IFJ2015
 *  Created on: 22. 11. 2015
 *  Author: xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 *  Description: 
 */
/**
 * @file tabSym.h
 * @author xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 * @brief 
 */

#ifndef TABSYM_H_
#define TABSYM_H_
#include <stdbool.h>
#include "ial.h"

/**
 * Typ polozky v tabulce symbolu
 */
typedef enum {
	TAB_SYM_VARIABLE, //!< TAB_SYM_VARIABLE
	TAB_SYM_CONSTANT, //!< TAB_SYM_CONSTANT
	TAB_SYM_FUNCTION //!< TAB_SYM_FUNCTION
} tTabSymDataType;

/**
 * Typy promennych
 */
typedef enum {
	TAB_SYM_VAR_INTEGER, //!< TAB_SYM_VAR_INTEGER
	TAB_SYM_VAR_DOUBLE, //!< TAB_SYM_VAR_DOUBLE
	TAB_SYM_VAR_STRING, //!< TAB_SYM_VAR_STRING
	TAB_SYM_VAR_BOOLEAN, //!< TAB_SYM_VAR_BOOLEAN
	TAB_SYM_VAR_AUTO    //!< TAB_SYM_VAR_AUTO
} tTabSymVarDataType;

/**
 * Navratove typy funkci.
 */
typedef enum {
	TAB_SYM_VAR_NO_AUTO_INTEGER,    //!< TAB_SYM_VAR_NO_AUTO_INTEGER
	TAB_SYM_VAR_NO_AUTO_DOUBLE, //!< TAB_SYM_VAR_NO_AUTO_DOUBLE
	TAB_SYM_VAR_NO_AUTO_STRING, //!< TAB_SYM_VAR_NO_AUTO_STRING
	TAB_SYM_VAR_NO_AUTO_BOOLEAN //!< TAB_SYM_VAR_NO_AUTO_BOOLEAN
} tTabSymVarNoAutoDataType;

/**
 * Informace k promenne.
 */
typedef struct {
	tTabSymVarDataType dataType;
	bool defined;
} tVariableInfo;

/**
 * Hodnoty konstant.
 */
typedef union {
	int intVal;
	double doubleVal;
	bool boolVal;
	string* stringVal;
} unionValue;

/**
 * Informace ke konstante.
 */
typedef struct {
	tTabSymVarNoAutoDataType dataType;
	unionValue value;

} tConstantInfo;

/**
 * Prvek v listu parametru.
 */
typedef struct tParamListElem {
	struct tParamListElem *next;
	string* idName;
	tTabSymVarNoAutoDataType dataType;
}*tParamListElemPtr;

/**
 * List parametru.
 */
typedef struct {
	tParamListElemPtr first;
}*tParamListPtr;



/**
 * Informace k funkci.
 */
typedef struct {
	tParamListPtr* params;		// parametry funkce
	tTabSymVarNoAutoDataType retType;	// navratovy typ funkce
	tTabSym* locTab;			// lokalni tabulka symbolu
	tTabSymList* tabBlockList;	// list tabulek bloku
	tInsTape* instTape;			// instrukcni paska
	bool defined;				// byla definovana

} tFuncInfo;

/**
 * Data polozky v tabulce.
 */
typedef struct {
	tTabSymDataType type;	//druh identifikatoru
	union {
		tVariableInfo *var;
		tConstantInfo * constant;
		tFuncInfo * func;
	} info;
} tTabSymElemData;

/**
 * Typy tabulky symbolu.
 */
typedef enum {
	TAB_SYM_GLOB,	//!< TAB_SYM_GLOB
	TAB_SYM_LOC, //!< TAB_SYM_LOC
} tTabSymTypes;

/**
 * Tabulka symbolu
 */
typedef struct {
	tTabSymTypes tabType;
	tBSTNodePtr root;
} tTabSym;

/**
 * Vytvori tabulku symbolu.
 * @param tabType[in]	-	typ tabulky symbolu
 * @return	Ukazatel na vytvorenou tabulku. Pokud chyba vraci NULL.
 */
tTabSym* tabSymCreate(tTabSymTypes tabType);

/**
 * Vyhleda prvek v tabulce symbolu.
 * @param table[in]		-	tabulka symbolu
 * @param key[in]		-	identifikator
 * @return	Vraci vyhledany prvek v tabulce, pri neuspechu vraci NULL.
 */
tTabSymElemData* tabSymSearch(tTabSym* table, string* key);

/**
 * Vklada do tabulky symbolu promennou.
 * @param table[in]		-	tabulka symbolu
 * @param key[in]		-	identifikator
 * @param varInfo[in]	-	informace o promenne
 * @return	Vraci 0 pokud chyba, jinak 1.
 */
int tabSymInsertVar(tTabSym* table, string* key, tVariableInfo* varInfo);

/**
 * Vklada do tabulky symbolu konstantu.
 * @param table[in]		-	tabulka symbolu
 * @param key[in]		-	identifikator
 * @param constInfo[in]	-	informace o konstante
 * @return	Vraci 0 pokud chyba, jinak 1.
 */
int tabSymInsertConst(tTabSym* table, string* key, tConstantInfo* constInfo);

/**
 * Vklada do tabulky symbolu funkci.
 * @param table[in]		-	tabulka symbolu
 * @param key[in]		-	identifikator
 * @param funcInfo[in]	-	informace o funkci
 * @return	Vraci 0 pokud chyba, jinak 1.
 */
int tabSymInsertFunc(tTabSym* table, string* key, tFuncInfo* funcInfo);

/**
 * Odstrani tabulku. (uvolni z pameti)
 * @param table[in]		-	tabulka symbolu
 */
void tabSymFree(tTabSym* table);

#endif /* TABSYM_H_ */

/*** End of file: tabSym.h ***/
