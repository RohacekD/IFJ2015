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
#include "insTape.h"
#include "tabSymList.h"
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
        tParamListElemPtr act;
}*tParamListPtr;

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
	unsigned int tmpCounter;	//pouziva se pro index generovanych docasnych identifikatoru
} tTabSym;



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
 * Vytvari nove tVariableInfo.
 * @param dataType[in]	-	Datovy typ promenne.
 * @return	Vraci ukazatel na tVariableInfo. Pokud chyba vraci NULL.
 */
tVariableInfo* tabSymCreateVariableInfo(tTabSymVarDataType dataType);


/**
 * Vytvari nove tConstantInfo.
 * @param dataType[in]	-	Datovy typ konstanty.
 * @param value[in]		-	Hodnota konstanty.
 * @return Vraci ukazatel na tConstantInfo. Pokud chyba vraci NULL.
 */
tConstantInfo* tabSymCreateConstantInfo(tTabSymVarNoAutoDataType dataType, unionValue value);

/**
 * Vytvari nove tConstantInfo a prekopiruje do nej data z info.
 * @param info[in]	-	Info, ktere se pouzije pro nove info.
 * @return	Vraci ukazatel na tConstantInfo. Pokud chyba vraci NULL.
 */
/**
 * Vytvari nove tFuncInfo.1
 * @param params[in]		-	parametry funkce
 * @param retType[in]		-	navratovy typ funkce
 * @param locTab[in]		-	lokalni tabulka symbolu
 * @param tabBlockList[in]	-	list tabulek bloku
 * @param instTape[in]		-	instrukcni paska
 * @param defined[in]		-	byla definovana
 * @return Vraci ukazatel na tFuncInfo. Pokud chyba vraci NULL.
 */
tFuncInfo* tabSymCreateFuncInfo(tParamListPtr* params, tTabSymVarNoAutoDataType retType, tTabSym* locTab,
		tTabSymList* tabBlockList, tInsTape* instTape, bool defined);
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

/**
 * Vytvori nazev docasne promenne. ($tmpI)
 * @param table[in]	-	ukzatel na tabulku symbolu,
 * 				kde	bude pouzit vygenerovany symbol
 * @return	Ukazatel na vygenerovany symbol. Pokud chyba NULL.
 */
string* tabSymCreateTmpSymbol(tTabSym* table);

/**
 * Funkce pro inicializaci seznamu parametru
 * @param list      seznam parametru
 */
void initList(tParamListPtr list);

/**
 * Funkce odstrani seznam parametru a uvolni pamet
 * @param list      seznam parametru
 */
void paramListFree(tParamListPtr list);

/**
 * Funkce vlozi prvek za aktivni prvek
 * @param list          seznam parametru
 * @param idName        jmeno identifikatoru
 * @param dataType      datovy typ argumentu
 */
void postInsert(tParamListPtr list, string *idName, tTabSymVarNoAutoDataType dataType);

/**
 * Funkce, ktera vlozi prvek do seznamu na posledni pozici
 *      z duvodu zachovani poradi parametru
 * @param list          seznam parametru
 * @param idName        jmeno identifikatoru
 * @param dataType      datovy typ argumentu
 * @return              0 - nastala chyba
 *                      1 - funkce probehla v poradku
 */
int insertEL(tParamListPtr list, string *idName, tTabSymVarNoAutoDataType dataType);

/**
 * Funkce nastavi aktivitu na prvni prvek
 * @param list      seznam parametru
 */
void first(tParamListPtr list);

/**
 * Funkce nastavi aktivitu na nasledujici prvek
 * @param list      seznam parametru     
 */
void succ(tParamListPtr list);
/**
 *  Funkce vrati hodnotu aktivniho prvku v seznamu
 * @param list      seznam parametru
 * @return          hodnota aktivniho prvku, nebo NULL, kdyz neexistuje aktivni prvek
 */
tParamListElemPtr getActElement(tParamListPtr list);

#endif /* TABSYM_H_ */

/*** End of file: tabSym.h ***/
