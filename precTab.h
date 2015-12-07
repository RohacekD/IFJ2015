/*
 * precTab.h
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: Hlavickovy soubor (pro precTab.c).
 */
/**
 * @file precTab.h
 * @author xdocek09
 * @brief Hlavickovy soubor (pro precTab.c).
 */

#ifndef PRECTAB_H_
#define PRECTAB_H_

typedef enum{
	TERMINAL_ADDITION,					// +
	TERMINAL_SUBTRACTION,				// -
	TERMINAL_MULTIPLICATION,			// *
	TERMINAL_DIVISION,					// /
	TERMINAL_EQUAL,						// ==
	TERMINAL_NOT_EQUAL,					// !=
	TERMINAL_LESS_THAN,					// <
	TERMINAL_GREATER_THAN,				// >
	TERMINAL_LESS_THAN_OR_EQUAL,		// <=
	TERMINAL_GREATER_THAN_OR_EQUAL,	 	// >=
	TERMINAL_OPEN_BRACKET,				// (
	TERMINAL_CLOSE_BRACKET,				// )
	TERMINAL_UNARY_MINUS,				// -
	TERMINAL_INCREMENT_POSTFIX,			// postfix ++
	TERMINAL_DECREMENT_POSTFIX,			// postfix --
	TERMINAL_INCREMENT_PREFIX,			// prefix ++
	TERMINAL_DECREMENT_PREFIX,			// prefix --
	TERMINAL_NOT,						// !
	TERMINAL_AND,						// &&
	TERMINAL_OR,						// ||
	TERMINAL_FUNCTION_IDENTIFICATOR,	// jmeno funkce
	TERMINAL_COMMA,						// ,
	TERMINAL_IDENTIFICATOR,				// jmeno promenne nebo konstanta
	TERMINAL_ENDMARK					// ukoncovaci
}tParExpTerminals;

/**
 * Vraci pravidlo pro dany terminal a token.
 * @param[in] terminal	-	kod terminalu (nejblize vrcholu zasobniku)
 * @param[in] token		-	kod tokenu (prichoziho)
 * @return Vraci nasledujici pravidla:
 * 	0 - chyba
 * 	'<'
 * 	'>'
 * 	'='
 */
int precGetRule(int terminal, int token);

#endif /* PRECTAB_H_ */
/*** End of file: precTab.h ***/
