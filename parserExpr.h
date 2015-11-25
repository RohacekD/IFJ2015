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

/**
 * Provede precedencni syntaktickou, semantickou analyzu a generovani tri adresneho kodu.
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
int parseExpression(tTabSym* table, tInsTape* tape, tTabSymVarNoAutoDataType expDataType, FILE* scanFile);



#endif /* PARSEREXPR_H_ */



/*** End of file: parserExpr.h ***/
