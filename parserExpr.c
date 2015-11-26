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
	TERMINAL_COMMA,						// ,
	TERMINAL_ENDMARK					// ukoncovaci
}tParExpTerminals;


/**
 * Pripravi dalsi token v rade.
 * 	Prevadi minus na unarni minus a vsechny nedefinovane tokeny
 * 	(ktere nemaji ekvivalent v tParExpTerminals) na ENDMARK.
 * @param stack[in]			-	Stack pro precedencni analyzu
 * @param scanFile[in]		-	Soubor pro scanner.
 * @param terminalCode[out]	-	Zde vraci kod odpovidajiciho terminalu
 * @param token[out]		-	zde vraci strukturu tToken
 * @return 1 OK, 0 chyba
 */
int prepareNextToken(tPrecStack* stack, FILE* scanFile, tParExpTerminals* terminalCode, tToken* token){
	tPrecStackData* precStackdata;
	tToken nextToken;

	if(getToken(token, scanFile)!=1){
		return 0;
	}

	switch (token->typ) {
		case PLUS:
			// +
			return TERMINAL_ADDITION;
			break;
		case MINUS:
			// -
			precStackdata=precStackTop(stack);
			if(precStackdata->type==PREC_STACK_ENDMARK ||
					(precStackdata->type==PREC_STACK_TERMINAL && precStackdata->key==TERMINAL_OPEN_BRACKET)){
				//na vrcholu zasobniku je endmark, nebo terminal oteviraci zavorka
				*terminalCode=TERMINAL_UNARY_MINUS;
			}else{
				//jinak se jedna o klasicke odecitani
				*terminalCode=TERMINAL_SUBTRACTION;
			}
			break;
		case MULTIPLY:
			// *
			*terminalCode=TERMINAL_MULTIPLICATION;
			break;
		case DIVISION:
			// /
			*terminalCode=TERMINAL_DIVISION;
			break;
		case EQUAL:
			// ==
			*terminalCode=TERMINAL_EQUAL;
			break;
		case NOT_EQUAL_OPER:
			// !=
			*terminalCode=TERMINAL_NOT_EQUAL;
			break;
		case LESS:
			// <
			*terminalCode=TERMINAL_LESS_THAN;
			break;
		case GREATER:
			// >
			*terminalCode=TERMINAL_GREATER_THAN;
			break;
		case LESS_EQUAL:
			// <=
			*terminalCode=TERMINAL_LESS_THAN_OR_EQUAL;
			break;
		case GREATER_EQUAL:
			// >=
			*terminalCode=TERMINAL_GREATER_THAN_OR_EQUAL;
			break;
		case PARENTHESIS_OPENING:
			// (
			*terminalCode=TERMINAL_OPEN_BRACKET;
			break;
		case PARENTHESIS_CLOSING:
			// )
			*terminalCode=TERMINAL_CLOSE_BRACKET;
			break;
		case INCREMENTATION:
			// postfix ++
			precStackdata=precStackTop(stack);
			if(precStackdata->type==PREC_STACK_TERMINAL &&
					precStackdata->key==TERMINAL_IDENTIFICATOR){
				//na vrcholu zasobniku mame identifikator, jedna se tedy o postfix
				*terminalCode=TERMINAL_INCREMENT_POSTFIX;
			}else{
				// prefix ++
				if(prepareNextToken(stack, scanFile, terminalCode, &nextToken)==0){
					//chyba
					return 0;
				}

				if(terminalCode==TYPE_IDENTIFICATOR){
					ungetToken(&nextToken);	//vratime
					*terminalCode=TERMINAL_INCREMENT_PREFIX;
				}else{
					//chyba
					return 0;
				}

			}

			break;
		case DECREMENTATION:
			// postfix --
			precStackdata=precStackTop(stack);
			if(precStackdata->type==PREC_STACK_TERMINAL &&
					precStackdata->key==TERMINAL_IDENTIFICATOR){
				//na vrcholu zasobniku mame identifikator, jedna se tedy o postfix
				*terminalCode=TERMINAL_DECREMENT_POSTFIX;
			}else{
				// prefix --
				if(prepareNextToken(stack, scanFile, terminalCode, &nextToken)==0){
					//chyba
					return 0;
				}
				if(terminalCode==TYPE_IDENTIFICATOR){
					ungetToken(&nextToken);	//vratime
					*terminalCode=TERMINAL_DECREMENT_PREFIX;
				}else{
					//chyba
					return 0;
				}
			}
			break;
		case LOG_NOT_OPER:
			// !
			*terminalCode=TERMINAL_NOT;
			break;
		case LOG_AND_OPER:
			// &&
			*terminalCode=TERMINAL_AND;
			break;
		case LOG_OR_OPER:
			// ||
			*terminalCode=TERMINAL_OR;
			break;
		case COMMA:
			// ,
			*terminalCode=TERMINAL_COMMA;
			break;

		case TYPE_INTEGER:
		case TYPE_DOUBLE:
		case TYPE_STRING:
		case TYPE_BOOL:
			//identifikator
			*terminalCode=TERMINAL_IDENTIFICATOR;
			break;
		case TYPE_IDENTIFICATOR:
			//identifikator

			//musime zjistit, zda-li se nejedna o identifikator funkce
			*terminalCode=TERMINAL_IDENTIFICATOR;
			if(getToken(&nextToken, scanFile)!=1)return 0;
			if(nextToken.typ==PARENTHESIS_OPENING){
				// jmeno funkce
				*terminalCode=TERMINAL_FUNCTION_IDENTIFICATOR;
			}

			break;
		case END_OF_FILE:
			*terminalCode=TERMINAL_ENDMARK;
			break;
		default:
			//vse ostatni je endmark, ale musime vratit token pro dalsi pokracovani
			*terminalCode=TERMINAL_ENDMARK;
			break;
	}
	return 1;
}


int parseExpression(tTabSym* table, tInsTape* tape, tTabSymVarNoAutoDataType expDataType, FILE* scanFile){
	//vytvorime zasobnik
	tPrecStack stack;
	precStackInit(stack);

	//pridame ukoncujici znak
	precStackPushElementOfKind(stack, PREC_STACK_ENDMARK,0);

	int a;
	int b=prepareNextToken(&stack, scanFile);
	do{
		a=precStackTopTerminal(&stack, &a);
		switch (precGetRule(a,b)) {
			case '=':
				precStackPushElementOfKind(stack, PREC_STACK_TERMINAL,b);
				b=prepareNextToken(&stack, scanFile);
				break;
			case '<':
				//zaměň a za a< na zásobníku
				precStackPushElemBeforeTopTerm(stack,PREC_STACK_SIGN,'<');

				precStackPushElementOfKind(stack, PREC_STACK_TERMINAL,b);
				b=prepareNextToken(&stack, scanFile);
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
