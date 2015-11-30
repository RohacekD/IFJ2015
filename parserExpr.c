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
 * @param tableListElem[in]	-	element listu tabulek v aktualnim zanoreni
 * @param termKind[in]	-	druh terminalu
 * @param token[in]		-	ukazatel na token
 * @param id[out]		-	ukazatel na string, kde da ukazatel na alkovany string s id
 * @return Pokud vse v poradku vraci ERR_OK. Pri chybe vraci error kody (ERR_INTERNAL, ERR_SEM_DEF).
 */
int semHandleNewToken(tTabSym* globTable, tTabSym* table, tTabSymListElemPtr tableListElem, tParExpTerminals termKind, tToken* token,
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

			id = tabSymListCreateTmpSymbol(tableListElem,table);
			tConstantInfo* constInfo = tabSymCreateConstantInfo(dataType, token->value);

			if (id == NULL) {
				return ERR_INTERNAL;	//chyba
			}
			if(tConstantInfo == NULL){
				strFree(id);
				free(id);
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

			if(tabSymListSearch(tableListElem, table, token->value->stringVal) ==NULL){
				//nenalezeno semanticka chyba
				return ERR_SEM_DEF;
			}
			newString = malloc(sizeof(string));
			if (newString == NULL) {
				//chyba pri alokaci
				return ERR_INTERNAL;
			}
			if(strInit(newString)){
				free(newString);
				return ERR_INTERNAL;
			}
			if(strCopyString(newString, token->value->stringVal)){
				strFree(newString);
				free(newString);
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
			free(newString);
			return ERR_INTERNAL;
		}
		if(strCopyString(newString, token->value->stringVal)){
			strFree(newString);
			free(newString);
			return ERR_INTERNAL;
		}
		id=newString;
	}
	return ERR_OK;
}

/**
 * SLouzi pro kodove oznaceni stavu v automatu S_F znamena konecny stav
 */
typedef enum{
	S_START,                         //!< S_START
	S_F_I,                           //!< S_F_I
	S_MINUS,                         //!< S_MINUS
	S_F_MINUS_E,                     //!< S_F_MINUS_E
	S_NOT,                   		 //!< S_NOT
	S_F_NOT_E,               		 //!< S_F_NOT_E
	S_INC,                           //!< S_INC
	S_F_INC_E,                       //!< S_F_INC_E
	S_DEC,                           //!< S_DEC
	S_F_DEC_E,                       //!< S_F_DEC_E
	S_FUN,                           //!< S_FUN
	S_FUN_OPEN_BRACKET,              //!< S_FUN_OPEN_BRACKET
	S_FUN_OPEN_BRACKET_E,            //!< S_FUN_OPEN_BRACKET_E
	S_F_FUN,                         //!< S_F_FUN
	S_OPEN_BRACKET,                  //!< S_OPEN_BRACKET
	S_OPEN_BRACKET_E,                //!< S_OPEN_BRACKET_E
	S_F_OPEN_BRACKET_E_CLOSE_BRACKET,//!< S_F_OPEN_BRACKET_E_CLOSE_BRACKET
	S_E,                             //!< S_E
	S_E_PLUS,                        //!< S_E_PLUS
	S_F_E_PLUS_E,                    //!< S_F_E_PLUS_E
	S_E_MINUS,                       //!< S_E_MINUS
	S_F_E_MINUS_E,                   //!< S_F_E_MINUS_E
	S_E_MUL,                         //!< S_E_MUL
	S_F_E_MUL_E,                     //!< S_F_E_MUL_E
	S_E_DIV,                         //!< S_E_DIV
	S_F_E_DIV_E,                     //!< S_F_E_DIV_E
	S_E_EQU,                         //!< S_E_EQU
	S_F_E_EQU_E,                     //!< S_F_E_EQU_E
	S_E_NEQU,                        //!< S_E_NEQU
	S_F_E_NEQU_E,                    //!< S_F_E_NEQU_E
	S_E_LESS,                        //!< S_E_LESS
	S_F_E_LESS_E,                    //!< S_F_E_LESS_E
	S_E_GRATER,                      //!< S_E_GRATER
	S_F_E_GREATER_E,                 //!< S_F_E_GREATER_E
	S_E_LESSEQU,                     //!< S_E_LESSEQU
	S_F_E_LESSEQU_E,                 //!< S_F_E_LESSEQU_E
	S_E_GRATEREQU,                   //!< S_E_GRATEREQU
	S_F_E_GREATEREQU_E,              //!< S_F_E_GREATEREQU_E
	S_F_E_INC,                       //!< S_F_E_INC
	S_F_E_DEC,                       //!< S_F_E_DEC
	S_E_AND,                         //!< S_E_AND
	S_F_E_AND_E,                     //!< S_F_E_AND_E
	S_E_OR,                          //!< S_E_OR
	S_F_E_OR_E                       //!< S_F_E_OR_E
}ruleAutomateStates;

int chooseRule(tPrecStack* stack){

	tPrecStackElemPtr act = stack->top;
	ruleAutomateStates actState=S_START;
	ruleAutomateStates nextState;
	/*
	 * Konecny automat, ktery zpracovava obsah zasobniku po < a snazi se najit pravidlo,
	 * ktery by takoveto poslopnosti odpovidalo.
	 */
	while(act!=NULL && act->data.type!=PREC_STACK_SIGN && act->data.type!=PREC_STACK_ENDMARK){
		switch (actState) {
			case S_START:
				if(act->data.type==PREC_STACK_NONTERMINAL){
					//neterminal
					actState=S_E;
				}else{
					//terminal
					switch (act->data.key) {
						case TERMINAL_IDENTIFICATOR:
							actState=S_F_I;
							break;
						case TERMINAL_UNARY_MINUS:
							actState=S_MINUS;
							break;
						case TERMINAL_NOT:
							actState=S_NOT;
							break;
						case TERMINAL_INCREMENT_PREFIX:
							actState=S_INC;
							break;
						case TERMINAL_DECREMENT_PREFIX:
							actState=S_DEC;
							break;
						case TERMINAL_FUNCTION_IDENTIFICATOR:
							actState=S_FUN;
							break;
						case TERMINAL_OPEN_BRACKET:
							actState=S_OPEN_BRACKET;
							break;
						default:
							//chyba
							return 0;
							break;
					}
				}
				break;
			case S_MINUS:
				if(act->data.type==PREC_STACK_NONTERMINAL){
					actState=S_F_MINUS_E;
				}else{
					//chyba
					return 0;
				}
				break;
			case S_NOT:
				if(act->data.type==PREC_STACK_NONTERMINAL){
					actState=S_F_NOT_E;
				}else{
					//chyba
					return 0;
				}
				break;
			case S_INC:
				if(act->data.type==PREC_STACK_NONTERMINAL){
					actState=S_F_INC_E;
				}else{
					//chyba
					return 0;
				}
				break;
			case S_FUN:
				if(act->data.type==PREC_STACK_TERMINAL &&
						act->data.key==TERMINAL_OPEN_BRACKET){
					actState=S_FUN_OPEN_BRACKET;
				}else{
					//chyba
					return 0;
				}
				break;
			case S_FUN_OPEN_BRACKET:
				if(act->data.type==PREC_STACK_NONTERMINAL){
					actState=S_FUN_OPEN_BRACKET_E;
				}else{
					//chyba
					return 0;
				}
				break;

			case S_FUN_OPEN_BRACKET_E:
				if(act->data.type==PREC_STACK_TERMINAL &&
					act->data.key==TERMINAL_CLOSE_BRACKET){
					actState=S_F_FUN;
				}else if(act->data.type==PREC_STACK_TERMINAL &&
					act->data.key==TERMINAL_COMMA){
					actState=S_FUN_OPEN_BRACKET;
				}else{
					//chyba
					return 0;
				}
				break;
			case S_DEC:
				if(act->data.type==PREC_STACK_NONTERMINAL){
					actState=S_F_DEC_E;
				}else{
					//chyba
					return 0;
				}
				break;
			case S_OPEN_BRACKET:
				if(act->data.type==PREC_STACK_NONTERMINAL){
					actState=S_OPEN_BRACKET_E;
				}else{
					//chyba
					return 0;
				}
				break;
			case S_OPEN_BRACKET_E:
				if(act->data.type==PREC_STACK_TERMINAL &&
						act->data.key==TERMINAL_CLOSE_BRACKET){
					actState=S_F_OPEN_BRACKET_E_CLOSE_BRACKET;
				}else{
					//chyba
					return 0;
				}
				break;
			case S_E:
				if(act->data.type==PREC_STACK_TERMINAL){
					switch (act->data.key) {
						case TERMINAL_ADDITION:
							actState=S_E_PLUS;
							break;
						case TERMINAL_SUBTRACTION:
							actState=S_E_MINUS;
							break;
						case TERMINAL_MULTIPLICATION:
							actState=S_E_MUL;
							break;
						case TERMINAL_DIVISION:
							actState=S_E_DIV;
							break;
						case TERMINAL_EQUAL:
							actState=S_E_EQU;
							break;
						case TERMINAL_NOT_EQUAL:
							actState=S_E_NEQU;
							break;
						case TERMINAL_LESS_THAN:
							actState=S_E_LESS;
							break;
						case TERMINAL_GREATER_THAN:
							actState=S_E_GRATER;
							break;
						case TERMINAL_LESS_THAN_OR_EQUAL:
							actState=S_E_LESSEQU;
							break;
						case TERMINAL_GREATER_THAN_OR_EQUAL:
							actState=S_E_GRATEREQU;
							break;
						case TERMINAL_INCREMENT_POSTFIX:
							actState=S_F_E_INC;
							break;
						case TERMINAL_DECREMENT_POSTFIX:
							actState=S_F_E_DEC;
							break;
						case TERMINAL_AND:
							actState=S_E_AND;
							break;
						case TERMINAL_OR:
							actState=S_E_OR;
							break;
						default:
							//chyba
							return 0;
					}
				}else{
					//chyba
					return 0;
				}
				break;

			case S_E_PLUS:
				nextState=S_F_E_PLUS_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_MINUS:
				nextState=S_F_E_MINUS_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_MUL:
				nextState=S_F_E_MUL_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_DIV:
				nextState=S_F_E_DIV_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_EQU:
				nextState=S_F_E_EQU_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_NEQU:
				nextState=S_F_E_NEQU_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_LESS:
				nextState=S_F_E_LESS_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_GRATER:
				nextState=S_F_E_GREATER_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_LESSEQU:
				nextState=S_F_E_LESSEQU_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_GRATEREQU:
				nextState=S_F_E_GREATEREQU_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_AND:
				nextState=S_F_E_AND_E;
				goto NEXT_STATE_LOGIC_S_E;
			case S_E_OR:
				nextState=S_F_E_OR_E;
		NEXT_STATE_LOGIC_S_E:
				if(act->data.type==PREC_STACK_NONTERMINAL){
					actState=nextState;
				}else{
					//chyba
					return 0;
				}
				break;

			case S_F_I:
			case S_F_MINUS_E:
			case S_F_NOT_E:
			case S_F_INC_E:
			case S_F_DEC_E:
			case S_F_FUN:
			case S_F_OPEN_BRACKET_E_CLOSE_BRACKET:
			case S_F_E_PLUS_E:
			case S_F_E_MINUS_E:
			case S_F_E_MUL_E:
			case S_F_E_DIV_E:
			case S_F_E_EQU_E:
			case S_F_E_NEQU_E:
			case S_F_E_LESS_E:
			case S_F_E_GREATER_E:
			case S_F_E_LESSEQU_E:
			case S_F_E_GREATEREQU_E:
			case S_F_E_INC:
			case S_F_E_DEC:
			case S_F_E_AND_E:
			case S_F_E_OR_E:
				//chyba mel byt uz konec
				return 0;

			default:
				//chyba ovsem ne syntakticka
				return 0;
				break;
		}
		act=act->next;
	}

	//mame stav potrebujeme pravidlo

}

int parseExpression(tTabSym* globTable, tTabSymListElemPtr tableListElem, tTabSym* table, tInsTape* tape,
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
