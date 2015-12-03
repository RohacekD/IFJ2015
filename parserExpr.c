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
#include "insTape.h"
#include "error.h"
#include "genInstructs.h"

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
 * @param table[in]		-	ukazatel na lokalni tabulku symbolu
 * @param insertToTable[in]	-	Do teto tabulky se vkladaji nove symboly
 * @param tableListElem[in]	-	element listu tabulek v aktualnim zanoreni
 * @param termKind[in]	-	druh terminalu
 * @param token[in]		-	ukazatel na token
 * @param id[out]		-	ukazatel na string, kde da ukazatel na alkovany string s id
 * @return Pokud vse v poradku vraci ERR_OK. Pri chybe vraci error kody (ERR_INTERNAL, ERR_SEM_DEF).
 */
int semHandleNewToken(tTabSym* table, tTabSym* insertToTable, tTabSymListElemPtr tableListElem, tParExpTerminals termKind, tToken* token,
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
			if(tabSymInsertConst(insertToTable, id, constInfo)==0){
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
		if(((functionData=tabSymSearch(globalTable, token->value->stringVal))==NULL) ||
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
	S_MINUS,                         //!< S_MINUS
	S_NOT,                   		 //!< S_NOT
	S_INC,                           //!< S_INC
	S_DEC,                           //!< S_DEC
	S_FUN,                           //!< S_FUN
	S_FUN_OPEN_BRACKET,              //!< S_FUN_OPEN_BRACKET
	S_FUN_OPEN_BRACKET_E,            //!< S_FUN_OPEN_BRACKET_E
	S_OPEN_BRACKET,                  //!< S_OPEN_BRACKET
	S_OPEN_BRACKET_E,                //!< S_OPEN_BRACKET_E
	S_E,                             //!< S_E
	S_E_PLUS,                        //!< S_E_PLUS
	S_E_MINUS,                       //!< S_E_MINUS
	S_E_MUL,                         //!< S_E_MUL
	S_E_DIV,                         //!< S_E_DIV
	S_E_EQU,                         //!< S_E_EQU
	S_E_NEQU,                        //!< S_E_NEQU
	S_E_LESS,                        //!< S_E_LESS
	S_E_GRATER,                      //!< S_E_GRATER
	S_E_LESSEQU,                     //!< S_E_LESSEQU
	S_E_GRATEREQU,                   //!< S_E_GRATEREQU
	S_E_AND,                         //!< S_E_AND
	S_E_OR,                          //!< S_E_OR
	/**
	 * Nasleduji koncove stavy, je nutne, aby byly takto za sebou se zacinajicim
	 * S_F_MINUS_E. Protoze se porovnava na prislusnost do koncovych stavu dle:
	 * x>=S_F_MINUS_E
	 */
	S_F_MINUS_E,                     //!< S_F_MINUS_E						E->-E
	S_F_I,                           //!< S_F_I								E->i
	S_F_NOT_E,               		 //!< S_F_NOT_E							E->!E
	S_F_INC_E,                       //!< S_F_INC_E							E->++E
	S_F_DEC_E,                       //!< S_F_DEC_E							E->--E
	S_F_FUN,                         //!< S_F_FUN							E->funkce
	S_F_OPEN_BRACKET_E_CLOSE_BRACKET,//!< S_F_OPEN_BRACKET_E_CLOSE_BRACKET	E->(E)
	S_F_E_PLUS_E,                    //!< S_F_E_PLUS_E						E->E+E
	S_F_E_MINUS_E,                   //!< S_F_E_MINUS_E						E->E-E
	S_F_E_MUL_E,                     //!< S_F_E_MUL_E						E->E*E
	S_F_E_DIV_E,                     //!< S_F_E_DIV_E						E->E/E
	S_F_E_EQU_E,                     //!< S_F_E_EQU_E						E->E==E
	S_F_E_NEQU_E,                    //!< S_F_E_NEQU_E						E->E!=E
	S_F_E_LESS_E,                    //!< S_F_E_LESS_E						E->E<E
	S_F_E_GREATER_E,                 //!< S_F_E_GREATER_E					E->E>E
	S_F_E_LESSEQU_E,                 //!< S_F_E_LESSEQU_E					E->E<=E
	S_F_E_GREATEREQU_E,              //!< S_F_E_GREATEREQU_E				E->E>=E
	S_F_E_INC,                       //!< S_F_E_INC							E->E++
	S_F_E_DEC,                       //!< S_F_E_DEC							E->E--
	S_F_E_AND_E,                     //!< S_F_E_AND_E						E->E&&E
	S_F_E_OR_E                       //!< S_F_E_OR_E						E->E||E
}ruleAutomateStates;

/**
 * Vybere pravidlo na zaklade obsahu stacku
 * @param stack[in]	-	Stack Obsahujici terminaly a neterminaly ke zjisteni pravdila
 * @param rule[out]	-	Pokud nedojde k chybe ulozi zde kod pravidla.
 * 						Kod pravidla se vypocitava na zaklade ruleAutomateStates a to vyhradne z konecnych stavu,
 * 						kde vzorec pro vypocet pravidla vypada takto pravidlo=state-S_F_MINUS_E kde state je z mnoziny
 * 						koncovych stavu ruleAutomateStates.
 * @return	Vraci 0- pri syntakticke chybe, -1- pri chybe a 1 pokud je vse v poradku.
 */

int chooseRule(tPrecStack* stack, int* rule){

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
				return -1;
				break;
		}
		act=act->next;
	}

	//mame stav potrebujeme pravidlo

	//stav musi byt konecny
	if(actState<S_F_MINUS_E){
		//chyba stav neni koncovy
		return 0;
	}

	*rule=actState-S_F_E_MINUS_E;
	return 1;

}

/***
 * Vytvori novy neterminal a vlozi ho do tabulky pod symbolem pomocne promenne.
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky jej vlozi.
 * @param dataType[in]				-	Datovy typ noveho neterminalu.
 * @return	Ukzatel na symbol primo v tabulce symbolu. NULL pri chybe.
 */
string* createNewNoterminal(tTabSymListElemPtr* startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tTabSymVarDataType dataType,
		tPrecStack stack){
	//vytvorime novy tmp
	string* newTmp=tabSymListCreateTmpSymbol(startTabSymListElem, tabSym);
	if(newTmp==NULL){
		//chyba
		return NULL;
	}
	//vytvorime variableinfo pro novy tmp
	tVariableInfo* varInfo = tabSymCreateVariableInfo();
	if(varInfo==NULL){
		goto INTERNAL_ERROR;
	}

	//nastavime datovy typ
	varInfo->dataType=dataType;

	//vlozime vytvorene tmp do tabulky
	if(tabSymInsertVar(insertToTable,newTmp,varInfo)==0){
		//chyba
		goto INTERNAL_ERROR_2;
	}
	string* adrOfString=tabSymListGetPointerToKey(startTabSymListElem, tabSym, newTmp);

	//ulozime neterminal na vrchol stacku
	if(precStackPushElementOfKind(stack, PREC_STACK_NONTERMINAL, 0, newTmp)==0){
		goto INTERNAL_ERROR_2;
	}


	return adrOfString;

INTERNAL_ERROR_2:
//mazeme i varInfo
	free(varInfo);
INTERNAL_ERROR:
//vracime interni chybu a uvolnujeme newTmp
	strFree(newTmp);
	free(newTmp);
	return NULL;
}

/**
 * Vraci prevedeny datovy typ symbolu(elementu) v tabulce na tTabSymVarDataType.
 * @param elemData[in]	-	symbol(element) v tabulce
 * @param dataType[out]	-	Zde bude ulozen datovy typ. Pokud chyba nemanipuluje.
 * @return	Vraci dle ERR_CODES.
 */
ERR_CODES getDataTypeOfSymbol(tTabSymElemData* elemData, tTabSymVarDataType dataType){
	//vybereme informaci o typu
	switch (elemData->type) {
		case TAB_SYM_VARIABLE:
			if(elemData->info.var->dataType==TAB_SYM_VAR_AUTO){
				return ERR_SEM_COM;
			}
			dataType=elemData->info.var->dataType;
			break;
		case TAB_SYM_CONSTANT:
			switch (elemData->info.constant->dataType) {
				case TAB_SYM_VAR_NO_AUTO_INTEGER:
					dataType=TAB_SYM_VAR_INTEGER;
					break;
				case TAB_SYM_VAR_NO_AUTO_DOUBLE:
					dataType=TAB_SYM_VAR_DOUBLE;
					break;
				case TAB_SYM_VAR_NO_AUTO_STRING:
					dataType=TAB_SYM_VAR_STRING;
					break;
				case TAB_SYM_VAR_NO_AUTO_BOOLEAN:
					dataType=TAB_SYM_VAR_BOOLEAN;
					break;
				default:
					//chyba
					return ERR_INTERNAL;
					break;
			}
			break;
		default:
			//chyba
			return ERR_INTERNAL;
			break;
	}
	return 1;
}

//E->i
ERR_CODES genInsTermToNoterm(tTabSymListElemPtr* startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//ziskame data o terminalu(promenne konstante)
	tPrecStackData* topElemData=precStackTop(stackForGen);

	//hledame symbol neterminalu v tabulce
	tTabSymElemData* elemData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);

	if(elemData){
		/*
		 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
		 * kontrola na definici, byla jiz provedena
		 */

		return ERR_INTERNAL;
	}


	//ziskame kod datoveho typu pro vytvoreni noveho neterminalu
	tTabSymVarDataType codeOfDataType;
	ERR_CODES errCode=getDataTypeOfSymbol(elemData, codeOfDataType);
	if(errCode!=ERR_OK){
		return errCode;
	}

	//ziskame pointery primo do tabulky
	string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemData->id);

	//vytvorime novy neterminal, ktery bude slouzit, jako cilova adresa
	string* adr3=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, codeOfDataType, stack);

	if(adr1==NULL || adr3){
		return ERR_INTERNAL;
	}

	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, I_ASSIGN, adr1, NULL, adr3)==0){
		return ERR_INTERNAL;
	}
	//vlozim novy neterminal na precedencni stack
	/**
		 * Vlozi element typu type s klicem key na vrchol zasobniku stack,
		 * @param[in] stack		-	Zasobnik, do ktereho vklada.
		 * @param[in] type		-	Typ elementu(tStackElemType).
		 * @param[in] key		-	Klic elementu.
		 * @return 0 pri chybe, jinak 1.
		 */
		int precStackPushElementOfKind(stack, , int key, string* id);

	return ERR_OK;
}


ERR_CODES genInsUnaryMinus(tTabSymListElemPtr* startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//-E
	//prvni je minus dame jej pryc
	precStackPop(stackForGen);

	//ziskame data o neterminalu
	tPrecStackData* topElemData=precStackTop(stackForGen);
	//hledame symbol neterminalu v tabulce
	tTabSymElemData* elemData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);
	if(elemData){
			/*
			 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
			 * kontrola na definici, byla jiz provedena
			 */
			return ERR_INTERNAL;
	}

	//ziskame kod datoveho typu pro semantickou kontrolu a vytvoreni noveho neterminalu
	tTabSymVarDataType codeOfDataType;
	ERR_CODES errCode=getDataTypeOfSymbol(elemData, codeOfDataType);
	if(errCode!=ERR_OK){
		return errCode;
	}

	/**
	 * Provedeme semantickou kontrolu,
	 * prijmame pouze int||double||boolean
	 */
	if(codeOfDataType!=TAB_SYM_VAR_INTEGER && codeOfDataType!=TAB_SYM_VAR_DOUBLE
			&& codeOfDataType!=TAB_SYM_VAR_BOOLEAN){
		return ERR_SEM_COM;
	}

	//vysledek s bool budem chtit v int
	if(codeOfDataType==TAB_SYM_VAR_BOOLEAN){
		codeOfDataType=TAB_SYM_VAR_INTEGER;
	}


	//ziskame pointery primo do tabulky symbolu
	string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemData->id);

	//vytvorime novy neterminal, ktery bude slouzit, jako cilova adresa
	string* adr3=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, codeOfDataType, stack);

	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, I_UMINUS, adr1, NULL, adr3)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;

}

ERR_CODES genInsNot(tTabSymListElemPtr* startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//E->!E
	//prvni je vykricnik dame jej pryc
	precStackPop(stackForGen);
	//ziskame data o neterminalu
	tPrecStackData* topElemData=precStackTop(stackForGen);
	//hledame symbol neterminalu v tabulce
	tTabSymElemData* elemData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);
	if(elemData){
			/*
			 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
			 * kontrola na definici, byla jiz provedena
			 */
			return ERR_INTERNAL;
	}
	//ziskame kod datoveho typu pro semantickou kontrolu
	tTabSymVarDataType codeOfDataType;
	ERR_CODES errCode=getDataTypeOfSymbol(elemData, codeOfDataType);
	if(errCode!=ERR_OK){
		return errCode;
	}


	/**
	 * Provedeme semantickou kontrolu,
	 * prijmame pouze int||double||boolean
	 */
	if(codeOfDataType!=TAB_SYM_VAR_INTEGER && codeOfDataType!=TAB_SYM_VAR_DOUBLE
			&& codeOfDataType!=TAB_SYM_VAR_BOOLEAN){
		return ERR_SEM_COM;
	}

	//ziskame pointery primo do tabulky symbolu
	string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemData->id);

	//vytvorime novy neterminal, ktery bude slouzit, jako cilova adresa(vysledkem je bool)
	string* adr3=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, TAB_SYM_VAR_BOOLEAN, stack);

	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, I_LOG_NOT, adr1, NULL, adr3)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;
}

ERR_CODES genInsIncPre(tTabSymListElemPtr* startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//E->++E
	//TODO
}

ERR_CODES genInsDecPre(tTabSymListElemPtr* startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//E->--E
	//TODO
}
ERR_CODES genInsFunc(tTabSymListElemPtr* startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//E->--E
	//TODO
}


void manageRule(){
	/**
		 * Nasleduji koncove stavy, je nutne, aby byly takto za sebou se zacinajicim
		 * S_F_MINUS_E. Protoze se porovnava na prislusnost do koncovych stavu dle:
		 * x>=S_F_MINUS_E
		 */
		S_F_MINUS_E,                     //!< S_F_MINUS_E						E->-E
		S_F_I,                           //!< S_F_I								E->i
		S_F_NOT_E,               		 //!< S_F_NOT_E							E->!E
		S_F_INC_E,                       //!< S_F_INC_E							E->++E
		S_F_DEC_E,                       //!< S_F_DEC_E							E->--E
		S_F_FUN,                         //!< S_F_FUN							E->funkce
		S_F_OPEN_BRACKET_E_CLOSE_BRACKET,//!< S_F_OPEN_BRACKET_E_CLOSE_BRACKET	E->(E)
		S_F_E_PLUS_E,                    //!< S_F_E_PLUS_E						E->E+E
		S_F_E_MINUS_E,                   //!< S_F_E_MINUS_E						E->E-E
		S_F_E_MUL_E,                     //!< S_F_E_MUL_E						E->E*E
		S_F_E_DIV_E,                     //!< S_F_E_DIV_E						E->E/E
		S_F_E_EQU_E,                     //!< S_F_E_EQU_E						E->E==E
		S_F_E_NEQU_E,                    //!< S_F_E_NEQU_E						E->E!=E
		S_F_E_LESS_E,                    //!< S_F_E_LESS_E						E->E<E
		S_F_E_GREATER_E,                 //!< S_F_E_GREATER_E					E->E>E
		S_F_E_LESSEQU_E,                 //!< S_F_E_LESSEQU_E					E->E<=E
		S_F_E_GREATEREQU_E,              //!< S_F_E_GREATEREQU_E				E->E>=E
		S_F_E_INC,                       //!< S_F_E_INC							E->E++
		S_F_E_DEC,                       //!< S_F_E_DEC							E->E--
		S_F_E_AND_E,                     //!< S_F_E_AND_E						E->E&&E
		S_F_E_OR_E                       //!< S_F_E_OR_E						E->E||E
}

int parseExpression(tTabSymListElemPtr tableListElem, tTabSym* table, tInsTape* tape,
		tTabSymVarNoAutoDataType expDataType, FILE* scanFile) {
	string* id; //pro vytvareni identifikatoru v tabulce symbolu

	//vybereme si tabulku, do ktere se budou vkladat tmp symboly (pomocne promenne)
	tTabSym* insertToTable=tableListElem->table;
	if(insertToTable==NULL){
		insertToTable=table;
	}

	//vytvorime zasobniky
	tPrecStack stack;
	precStackInit(stack);
	tPrecStack* revertedTopStack;	//pouziva se pro vyber pravidla

	//pridame ukoncujici znak
	precStackPushElementOfKind(stack, PREC_STACK_ENDMARK, 0);

	int rule; // vybrane pravidlo

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
			if(semHandleNewToken(insertToTable, table, b, &token, id)!=ERR_OK){
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

			if(semHandleNewToken(insertToTable, table, b, token, id)!=ERR_OK){
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

			//pokusime se ziskat stack obsahujici <y a smazeme jej z vrcholu
			revertedTopStack=precStackCreateRevertedTopReduc(stack);
			if(revertedTopStack==NULL){
				//TODO:ERROR
			}
			switch (chooseRule(revertedTopStack, &rule)) {
				case 1:
					// v poradku
					manageRule(revertedTopStack, rule, tableListElem, table, tape);//zmeni <y za neterminal, provede semantickou kontrolu a vygeneruje instrukci.
					break;
				case 0:
					// syntakticka chyba
					//TODO:ERROR
					break;
				case -1:
					// vnitrni chyba
					//TODO:ERROR
					break;
				default:
					// vnitrni chyba
					//TODO:ERROR
					break;
			}

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
