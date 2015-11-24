/*
 * parserExpr.c
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: 
 */
/**
 * @file parserExpr.c
 * @author xdocek09
 * @brief 
 */

#include "parserExpr.h"

#include "precStack.h"
#include <stdlib.h>
#include "token.h"

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
	TERMINAL_IDENTIFICATOR,				// jmeno promenne nebo konstanta
	TERMINAL_FUNCTION_IDENTIFICATOR,	// jmeno funkce
	TERMINAL_COMMA						// ,
}tParExpTerminals;

/**
 * Pripravi dalsi token v rade.
 * @return Vraci kod terminalu pro dany token.
 */
tParExpTerminals prepareNextToken(){

}


int parseExpression(){
	//vytvorime zasobnik
	tPrecStack stack;
	precStackInit(stack);

	//pridame ukoncujici znak
	precStackPushElementOfKind(stack, PREC_STACK_ENDMARK,0);

	int a;
	int b=getToken();
	do{
		a=precStackTopTerminal(&stack, &a);
		switch (precGetRule(a,b)) {
			case '=':
				precStackPushElementOfKind(stack, PREC_STACK_TERMINAL,b);
				b=getToken();
				break;
			case '<':
				//zaměň a za a< na zásobníku
				precStackPushElemBeforeTopTerm(stack,PREC_STACK_SIGN,'<');

				precStackPushElementOfKind(stack, PREC_STACK_TERMINAL,b);
				b=getToken();
				break;
			case '>':

				break;
			default:
				//chyba

				break;
		}
	}while();

	return 1;
}


/*** End of file: parserExpr.c ***/
