/*
 * parserExpr.h
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: Hlavickovy soubor (pro parserExpr.c).
 */
/**
 * @file parserExpr.h
 * @author xdocek09
 * @brief Hlavickovy soubor (pro parserExpr.c).
 */


#ifndef PARSEREXPR_H_
#define PARSEREXPR_H_
#include <stdlib.h>
#include <stdbool.h>
#include "precStack.h"
#include "token.h"
#include "precTab.h"
#include "tabSym.h"
#include "insTape.h"
#include "error.h"
#include "genInstructs.h"

/**
 * Provede precedencni syntaktickou, semantickou analyzu a generovani tri adresneho kodu.
 * @param tableListElem[in]	-	element listu tabulek v aktualnim zanoreni
 * @param table[in]			-	tabulka symbolu
 * @param tape[in]			-	paska instrukci
 * 								Vklada instrukce na konec pasky. Pokud je paska aktivni
 * 								vklada instrukce za aktivni instrukci. V takovemto pripade
 * 								se posunuje aktivita na kazdou vlozenou instrukci, tedy posledni
 * 								vlozena instrukce bude aktivni.
 * @param expDataType[out]	-	Vysledny datovy typ vyrazu.
 * @param scanFile[in]		-	Soubor pro scanner.
 * @return kod chyby / uspechu
 */
int parseExpression(tTabSymListElemPtr tableListElem, tTabSym* table, tInsTape* tape, tTabSymVarNoAutoDataType* expDataType, FILE* scanFile);



#endif /* PARSEREXPR_H_ */



/*** End of file: parserExpr.h ***/
