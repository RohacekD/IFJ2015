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

#include <stdlib.h>
#include <stdbool.h>
#include "parserExpr.h"
#include "precStack.h"
#include "token.h"
#include "precTab.h"
#include "tabSym.h"
#include "error.h"


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
int prepareNextToken(tPrecStack* stack, FILE* scanFile,
		tParExpTerminals* terminalCode, tToken* token) {
	tPrecStackData* precStackdata;
	tToken nextToken;

	if (getToken(token, scanFile) != 1) {
		return 0;
	}

	typedef union utokenValue
	{
		//nejak implementovany pointer do tabulky symbolu
		int intVal;
		double doubleVal;
		string stringVal;
		bool boolVal;
	}tokenValue;



	typedef struct stToken{
		TokenTypes typ;
		tokenValue value;
	} tToken;

	switch (token->typ) {
	case PLUS:
		// +
		*terminalCode = TERMINAL_ADDITION;
		break;
	case MINUS:
		// -
		precStackdata = precStackTop(stack);
		if (precStackdata->type == PREC_STACK_ENDMARK
				|| (precStackdata->type == PREC_STACK_TERMINAL
						&& precStackdata->key == TERMINAL_OPEN_BRACKET)) {
			//na vrcholu zasobniku je endmark, nebo terminal oteviraci zavorka
			*terminalCode = TERMINAL_UNARY_MINUS;
		} else {
			//jinak se jedna o klasicke odecitani
			*terminalCode = TERMINAL_SUBTRACTION;
		}
		break;
	case MULTIPLY:
		// *
		*terminalCode = TERMINAL_MULTIPLICATION;
		break;
	case DIVISION:
		// /
		*terminalCode = TERMINAL_DIVISION;
		break;
	case EQUAL:
		// ==
		*terminalCode = TERMINAL_EQUAL;
		break;
	case NOT_EQUAL_OPER:
		// !=
		*terminalCode = TERMINAL_NOT_EQUAL;
		break;
	case LESS:
		// <
		*terminalCode = TERMINAL_LESS_THAN;
		break;
	case GREATER:
		// >
		*terminalCode = TERMINAL_GREATER_THAN;
		break;
	case LESS_EQUAL:
		// <=
		*terminalCode = TERMINAL_LESS_THAN_OR_EQUAL;
		break;
	case GREATER_EQUAL:
		// >=
		*terminalCode = TERMINAL_GREATER_THAN_OR_EQUAL;
		break;
	case PARENTHESIS_OPENING:
		// (
		*terminalCode = TERMINAL_OPEN_BRACKET;
		break;
	case PARENTHESIS_CLOSING:
		// )
		*terminalCode = TERMINAL_CLOSE_BRACKET;
		break;
	case INCREMENTATION:
		// postfix ++
		precStackdata = precStackTop(stack);
		if (precStackdata->type == PREC_STACK_TERMINAL
				&& precStackdata->key == TERMINAL_IDENTIFICATOR) {
			//na vrcholu zasobniku mame identifikator, jedna se tedy o postfix
			*terminalCode = TERMINAL_INCREMENT_POSTFIX;
		} else {
			// prefix ++
			if (prepareNextToken(stack, scanFile, terminalCode, &nextToken)
					== 0) {
				//chyba
				return 0;
			}

			if (terminalCode == TYPE_IDENTIFICATOR) {
				ungetToken(&nextToken);	//vratime
				*terminalCode = TERMINAL_INCREMENT_PREFIX;
			} else {
				//chyba
				return 0;
			}

		}

		break;
	case DECREMENTATION:
		// postfix --
		precStackdata = precStackTop(stack);
		if (precStackdata->type == PREC_STACK_TERMINAL
				&& precStackdata->key == TERMINAL_IDENTIFICATOR) {
			//na vrcholu zasobniku mame identifikator, jedna se tedy o postfix
			*terminalCode = TERMINAL_DECREMENT_POSTFIX;
		} else {
			// prefix --
			if (prepareNextToken(stack, scanFile, terminalCode, &nextToken)
					== 0) {
				//chyba
				return 0;
			}
			if (terminalCode == TYPE_IDENTIFICATOR) {
				ungetToken(&nextToken);	//vratime
				*terminalCode = TERMINAL_DECREMENT_PREFIX;
			} else {
				//chyba
				return 0;
			}
		}
		break;
	case LOG_NOT_OPER:
		// !
		*terminalCode = TERMINAL_NOT;
		break;
	case LOG_AND_OPER:
		// &&
		*terminalCode = TERMINAL_AND;
		break;
	case LOG_OR_OPER:
		// ||
		*terminalCode = TERMINAL_OR;
		break;
	case COMMA:
		// ,
		*terminalCode = TERMINAL_COMMA;
		break;

	case TYPE_INTEGER:
	case TYPE_DOUBLE:
	case TYPE_STRING:
	case TYPE_BOOL:
		//identifikator
		*terminalCode = TERMINAL_IDENTIFICATOR;
		break;
	case TYPE_IDENTIFICATOR:
		//identifikator

		//musime zjistit, zda-li se nejedna o identifikator funkce
		*terminalCode = TERMINAL_IDENTIFICATOR;
		if (getToken(&nextToken, scanFile) != 1)
			return 0;
		if (nextToken.typ == PARENTHESIS_OPENING) {
			// jmeno funkce
			*terminalCode = TERMINAL_FUNCTION_IDENTIFICATOR;
		}
		ungetToken(&nextToken);	//vratime

		break;
	case END_OF_FILE:
		*terminalCode = TERMINAL_ENDMARK;
		break;
	default:
		//vse ostatni je endmark, ale musime vratit token pro dalsi pokracovani
		*terminalCode = TERMINAL_ENDMARK;
		break;
	}
	return 1;
}

/**
 * Provadi semanticke akce nad nove prijatym tokenem.
 * 	Ulozi konstantu do tabulky symbolu.
 * 	Kontroluje jestli se promenna jiz nachazi v tabulce symbolu.
 * 	Kontroluje jestli je funkce jiz definovana.
 * @param globTable[in]	-	ukazatel na globalni tabulku symbolu
 * @param table[in]		-	ukazatel na lokalni tabulku symbolu
 * @param termKind[in]	-	druh terminalu
 * @param token[in]		-	ukazatel na token
 * @param id[out]		-	ukazatel na string, kde da ukazatel na alkovany string s id
 * @return Pokud vse v poradku vraci ERR_OK. Pri chybe vraci error kody (ERR_INTERNAL, ERR_SEM_DEF).
 */
int semHandleNewToken(tTabSym* globTable, tTabSym* table, tParExpTerminals termKind, tToken* token,
		string* id) {
	string* newString;
	if (termKind == TERMINAL_IDENTIFICATOR) {
		tTabSymVarNoAutoDataType dataType;
		switch (token->typ) {
		case TYPE_INTEGER:
			dataType = TAB_SYM_VAR_NO_AUTO_INTEGER;
			goto CONSTANT_HANDLE;
		case TYPE_DOUBLE:
			dataType = TAB_SYM_VAR_NO_AUTO_DOUBLE;
			goto CONSTANT_HANDLE;
		case TYPE_STRING:
			dataType = TAB_SYM_VAR_NO_AUTO_STRING;
			goto CONSTANT_HANDLE;
		case TYPE_BOOL:
			/*
			 * jedna se o konstantu
			 * je nutne vytvorit novy $tmp
			 */
			dataType = TAB_SYM_VAR_NO_AUTO_BOOLEAN;

			CONSTANT_HANDLE:

			id = tabSymCreateTmpSymbol(table);
			tConstantInfo* constInfo = tabSymCreateConstantInfo(dataType, token->value);

			if (id == NULL || tConstantInfo == NULL) {
				return ERR_INTERNAL;	//chyba
			}

			//vlozime do tabulky
			if(tabSymInsertConst(table, id, constInfo)==0){
				return ERR_INTERNAL;
			}

			break;
		default:
			/*
			 * Jedna se o promennou musime zkontrolovat,
			 * jestli je jiz v tabulce, jinak se jedna o semantickou chybu
			 * nedefinovana promenna.
			 */
			if(tabSymSearch(table, token->value->stringVal)==NULL){
				//nenalezeno semanticka chyba
				return ERR_SEM_DEF;
			}
			newString = malloc(sizeof(string));
			if (newString == NULL) {
				//chyba pri alokaci
				return ERR_INTERNAL;
			}
			if(strInit(newString)){
				return ERR_INTERNAL;
			}
			if(strCopyString(newString, token->value->stringVal)){
				return ERR_INTERNAL;
			}
			id=newString;
			break;
		}
	}else if(termKind == TERMINAL_FUNCTION_IDENTIFICATOR){
		/**
		 * Jedna se o funkcni identifikator, zkontrolujeme jestli se nachazi v tabulce
		 * symbolu a jestli je definovana.
		 */
		tTabSymElemData functionData;
		if(((functionData=tabSymSearch(globTable, token->value->stringVal))==NULL) ||
				functionData->info.func->defined==true){
			//nenalezeno/nedefinovano semanticka chyba
			return ERR_SEM_DEF;
		}
		newString = malloc(sizeof(string));
		if (newString == NULL) {
			//chyba pri alokaci
			return ERR_INTERNAL;
		}
		if(strInit(newString)){
			return ERR_INTERNAL;
		}
		if(strCopyString(newString, token->value->stringVal)){
			return ERR_INTERNAL;
		}
		id=newString;
	}
	return ERR_OK;
}

int parseExpression(tTabSym* globTable, tTabSym* table, tInsTape* tape,
		tTabSymVarNoAutoDataType expDataType, FILE* scanFile) {
	string* id; //pro vytvareni identifikatoru v tabulce symbolu

	//vytvorime zasobnik
	tPrecStack stack;
	precStackInit(stack);

	//pridame ukoncujici znak
	precStackPushElementOfKind(stack, PREC_STACK_ENDMARK, 0);

	tParExpTerminals a;	//nejvrchnejis terminal na zasobniku
	tParExpTerminals b;	//aktualni druh tokenu na vstupu
	tToken token;	//aktualni token na vstupu

	if(prepareNextToken(&stack, scanFile, &b, &token)==0){	//ziskama pocatecni token
		//TODO:ERROR
	}
	precStackTopTerminal(&stack, &a);
	do {
		switch (precGetRule(a, b)) {
		case '=':
			//dej na zasobnik prijaty token a precti dalsi ze vstupu
			if(semHandleNewToken(globTable, table, b, &token, id)!=ERR_OK){
				//TODO:ERROR
			}
			if(precStackPushElementOfKind(stack, PREC_STACK_TERMINAL, b, id)==0){
				//TODO:ERROR
			}

			//TODO: clean actual token

			if(prepareNextToken(&stack, scanFile, &b, &token)==0){
				//TODO:ERROR
			}
			break;
		case '<':
			/*
			 * zaměň a za a< na zásobníku
			 * a dej na zasobnik prijaty token a precti dalsi ze vstupu
			 */
			precStackPushElemBeforeTopTerm(stack, PREC_STACK_SIGN, '<', NULL);

			if(semHandleNewToken(globTable, table, b, token, id)!=ERR_OK){
				//TODO:ERROR
			}

			if(precStackPushElementOfKind(stack, PREC_STACK_TERMINAL, b, id)==0){
				//TODO:ERROR
			}

			//TODO: clean actual token
			if(prepareNextToken(&stack, scanFile, &b, &token)==0){
				//TODO:ERROR
			}
			break;
		case '>':
			/*
			 * if <y je na vrcholu zásobníku and r: A → y ∈ P
			 *	then zaměň <y za A & vypiš r na výstup
			 *	else chyba
			 */
			break;
		default:
			//chyba

			break;
		}
		precStackTopTerminal(&stack, &a);
	} while (b!=TERMINAL_ENDMARK || a!=TERMINAL_ENDMARK);//b = $ and top = $

	return 1;
}

/*** End of file: parserExpr.c ***/
