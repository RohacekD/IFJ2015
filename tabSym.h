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
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include "insTape.h"
#include "ial.h"
#include "str.h"
/**
 * Typ polozky v tabulce symbolu
 */
typedef enum {
	TAB_SYM_VARIABLE,	//!< TAB_SYM_VARIABLE
	TAB_SYM_CONSTANT,	//!< TAB_SYM_CONSTANT
	TAB_SYM_FUNCTION	//!< TAB_SYM_FUNCTION
} tTabSymDataType;

/**
 * Typy promennych, nesmi se menit poradim
 */
typedef enum {
	TAB_SYM_VAR_INTEGER,	//!< TAB_SYM_VAR_INTEGER
	TAB_SYM_VAR_DOUBLE,		//!< TAB_SYM_VAR_DOUBLE
	TAB_SYM_VAR_STRING,		//!< TAB_SYM_VAR_STRING
	TAB_SYM_VAR_BOOLEAN,	//!< TAB_SYM_VAR_BOOLEAN
	TAB_SYM_VAR_AUTO		//!< TAB_SYM_VAR_AUTO
} tTabSymVarDataType;

/**
 * Navratove typy funkci, nesmi se menit poradim.
 */
typedef enum {
	TAB_SYM_VAR_NO_AUTO_INTEGER,    //!< TAB_SYM_VAR_NO_AUTO_INTEGER
	TAB_SYM_VAR_NO_AUTO_DOUBLE,		//!< TAB_SYM_VAR_NO_AUTO_DOUBLE
	TAB_SYM_VAR_NO_AUTO_STRING,		//!< TAB_SYM_VAR_NO_AUTO_STRING
	TAB_SYM_VAR_NO_AUTO_BOOLEAN		//!< TAB_SYM_VAR_NO_AUTO_BOOLEAN
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
typedef struct tParamList{
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


struct tTabSymList;

typedef struct tTabSymListElem{
	tTabSym* table;
	struct tTabSymListElem* next;
	struct tTabSymList* childList;
	struct tTabSymListElem* parentElement;
}*tTabSymListElemPtr;

typedef struct tTabSymList{
	tTabSymListElemPtr first;
}tTabSymList;

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
 *  funkce inicializuje strukturu pro seznam parametru
 * @param list[in]      -   seznam parametru 
 * @return      -   vraci 1, pokud probehla funkce v poradku, jinak 0
 */
int initList(tParamListPtr list);

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
int postInsert(tParamListPtr list, string *idName, tTabSymVarNoAutoDataType dataType);

/**
 * Funkce, ktera vlozi prvek do seznamu na posledni pozici
 *      z duvodu zachovani poradi parametru
 * @param list          seznam parametru
 * @param idName        jmeno identifikatoru
 * @param dataType      datovy typ argumentu
 * @return              0 - nastala chyba
 *                      1 - funkce probehla v poradku
 */
int insertEl(tParamListPtr list, string *idName, tTabSymVarNoAutoDataType dataType);

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


/**
 * Alokuje pamet pro tTabSymList.
 * @return Ukazatel na nove alokovanou strukturu. Pokud chyba NULL.
 */
tTabSymList* tabSymListCreate();

/**
 * Vlozi do listu tabulek novou tabulku.
 * @param tabList[in]		-	List, do ktereho bude vlozena tabulka.
 * @param table[in]			-	Ukazatel na tabulku, kterou vlozi.
 * @param parentElement[in]	-	Ukazatel na rodicovsky prvek.
 * @return	Vraci ukazatel na nove vnoreny prvek. Pokud chyba NULL.
 */
tTabSymListElemPtr tabSymListInsertTable(tTabSymList* tabList, tTabSym* table, tTabSymListElemPtr parentElement);

/**
 * Vyhledava symbol key rekurzivne u rodicu daneho startTabSymListElem.
 * Pokud nenajde prohledava nakonec loctable. Vraci prvni nalezeny.
 * @param startTabSymListElem[in]	-	startovaci prvek
 * @param locTable[in]				-	Tabulka, kterou prohleda jako posledni,
 * 										tato tabulka je mimo hierarchii rodicu.
 * 										Napriklad lokalni tabulka funkce.
 * @param key[in]					-	Klic, ktery chceme vyhledat
 * @return	Vraci ukazatel na tTabSymElemData. Nebo NULL, pokud nenajde.
 */
tTabSymElemData* tabSymListSearch(tTabSymListElemPtr startTabSymListElem, tTabSym* locTable, string* key);


/**
 * Slouzi pro ziskani ukazatele na key primo v tabulce .
 * @param startTabSymListElem[in]	-	startovaci prvek
 * @param locTable[in]				-	Tabulka, kterou prohleda jako posledni,
 * 										tato tabulka je mimo hierarchii rodicu.
 * 										Napriklad lokalni tabulka funkce.
 * @param key[in]					-	Klic, ktery chceme vyhledat
 * @return Vraci ukazatel na string. Nebo NULL, pokud nenajde.
 */
string* tabSymListGetPointerToKey(tTabSymListElemPtr startTabSymListElem, tTabSym* locTable, string* key);

/**
 * Vytvori nazev docasne promenne. ($tmpI) Zohlednuje $tmp vsech rodicu i loctable.
 * Zvysuje counter tabulky symbolu v generateFor.
 * @param generateFor[in]	-	ukazatel na element listu tabulek symbolu
 * @param locTable[in]		-	lokalni tabulka symbolu
 * @return	Ukazatel na vygenerovany symbol. Pokud chyba NULL.
 */

string* tabSymListCreateTmpSymbol(tTabSymListElemPtr generateFor, tTabSym* locTable);

/**
 * Vrati nazev naposledy vytvoreneho $tmp v generateFor. (jedna se pouze o vygenerovany nazev)
 * @param generateFor[in]	-	ukazatel na element listu tabulek symbolu
 * @param locTable[in]		-	lokalni tabulka symbolu
 * @return	Ukazatel na nazev posledne vygenerovaneho symbolu. Pokud chyba NULL.
 */
string* tabSymListLastCreateTmpSymbol(tTabSymListElemPtr generateFor, tTabSym* locTable);

/**
 * Uvolni list z pameti.
 * @param tabList[in]	-	List pro uvolneni.
 */
void tabSymListFree(tTabSymList* tabList);


#endif /* TABSYM_H_ */

/*** End of file: tabSym.h ***/
