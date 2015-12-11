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

extern tTabSym *globalTable;

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
	tPrecStackData* precStackdata=NULL;
	tToken nextToken;

	if (getToken(token, scanFile) != 1) {
		return 0;
	}

	switch ((*token)->typ) {
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

			if (*terminalCode == TERMINAL_IDENTIFICATOR) {
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
			if (*terminalCode == TERMINAL_IDENTIFICATOR) {
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
		if (nextToken->typ == PARENTHESIS_OPENING) {
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
 * @param id[out]		-	ukazatel na string, kde da ukazatel na alokovany string s id
 * @return Pokud vse v poradku vraci ERR_OK. Pri chybe vraci error kody (ERR_INTERNAL, ERR_SEM_DEF).
 */
int semHandleNewToken(tTabSym* table, tTabSym* insertToTable, tTabSymListElemPtr tableListElem, tParExpTerminals termKind, tToken token,
		string** id) {
	string* newString=NULL;
	if (termKind == TERMINAL_IDENTIFICATOR) {
		tTabSymVarNoAutoDataType dataType;
		//vytvorime si value
		unionValue value;
		switch (token->typ) {
		case TYPE_INTEGER:
			dataType = TAB_SYM_VAR_NO_AUTO_INTEGER;
			value.intVal=token->value.intVal;
			goto CONSTANT_HANDLE;
		case TYPE_DOUBLE:
			dataType = TAB_SYM_VAR_NO_AUTO_DOUBLE;
			value.doubleVal=token->value.doubleVal;
			goto CONSTANT_HANDLE;
		case TYPE_STRING:
			dataType = TAB_SYM_VAR_NO_AUTO_STRING;
			string* stringForValue=malloc(sizeof(string));
			strInit(stringForValue);
			strCopyString(stringForValue, &(token->value.stringVal));
			value.stringVal=stringForValue;

			goto CONSTANT_HANDLE;
		case TYPE_BOOL:
			/*
			 * jedna se o konstantu
			 * je nutne vytvorit novy tmp
			 */
			dataType = TAB_SYM_VAR_NO_AUTO_BOOLEAN;
			value.boolVal=token->value.boolVal;
			CONSTANT_HANDLE:

			*id = tabSymListCreateTmpSymbol(tableListElem,table);
			if (*id == NULL) {
				return ERR_INTERNAL;	//chyba
			}
			tConstantInfo* constInfo = tabSymCreateConstantInfo(dataType, value);


			if(constInfo == NULL){
				strFree(*id);
				free(*id);
				return ERR_INTERNAL;	//chyba
			}

			//vlozime do tabulky
			if(tabSymInsertConst(insertToTable, *id, constInfo)==0){
				return ERR_INTERNAL;
			}

			break;
		default:
			/*
			 * Jedna se o promennou musime zkontrolovat,
			 * jestli je jiz v tabulce, jinak se jedna o semantickou chybu
			 * nedefinovana promenna.
			 */

			if(tabSymListSearch(tableListElem, table, &(token->value.stringVal)) ==NULL){
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
			if(strCopyString(newString, &(token->value.stringVal))){
				strFree(newString);
				free(newString);
				return ERR_INTERNAL;
			}
			*id=newString;
			break;
		}
	}else if(termKind == TERMINAL_FUNCTION_IDENTIFICATOR){
		/**
		 * Jedna se o funkcni identifikator, zkontrolujeme jestli se nachazi v tabulce
		 * symbolu.
		 */
		tTabSymElemData* functionData=NULL;
		if(((functionData=tabSymSearch(globalTable, &(token->value.stringVal)))==NULL)){
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
		if(strCopyString(newString, &(token->value.stringVal))){
			strFree(newString);
			free(newString);
			return ERR_INTERNAL;
		}
		*id=newString;
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
	S_F_INC_I,                       //!< S_F_INC_E							E->++E
	S_F_DEC_I,                       //!< S_F_DEC_E							E->--E
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
	S_F_I_INC,                       //!< S_F_E_INC							E->E++
	S_F_I_DEC,                       //!< S_F_E_DEC							E->E--
	S_F_E_AND_E,                     //!< S_F_E_AND_E						E->E&&E
	S_F_E_OR_E                       //!< S_F_E_OR_E						E->E||E
}ruleAutomateStates;

/**
 * Vybere pravidlo na zaklade obsahu stacku
 * @param stack[in]	-	Stack Obsahujici terminaly a neterminaly ke zjisteni pravdila
 * @param rule[out]	-	Pokud nedojde k chybe ulozi zde kod pravidla.
 * 						Kod pravidla odpovida kodum konecnych stavu v ruleAutomateStates.
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
			case S_F_I:
				switch (act->data.key) {
					case TERMINAL_INCREMENT_POSTFIX:
						actState=S_F_I_INC;
						break;
					case TERMINAL_DECREMENT_POSTFIX:
						actState=S_F_I_DEC;
						break;
					default:
						//chyba
						return 0;
						break;
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
				if(act->data.type==PREC_STACK_TERMINAL && act->data.key==TERMINAL_IDENTIFICATOR){
					actState=S_F_INC_I;
				}else{
					//chyba
					return 0;
				}
				break;

			case S_DEC:
				if(act->data.type==PREC_STACK_TERMINAL && act->data.key==TERMINAL_IDENTIFICATOR){
					actState=S_F_DEC_I;
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
				}else if(act->data.type==PREC_STACK_TERMINAL &&
						act->data.key==TERMINAL_CLOSE_BRACKET){
						actState=S_F_FUN;
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


			case S_F_MINUS_E:
			case S_F_NOT_E:
			case S_F_INC_I:
			case S_F_DEC_I:
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
			case S_F_I_INC:
			case S_F_I_DEC:
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

	*rule=actState;
	return 1;

}

typedef enum{
	BUILD_IN_FUNC_NO_MATCH,	//nedoslo ke schode
	BUILD_IN_FUNC_ID_LENGTH,
	BUILD_IN_FUNC_ID_SUBSTR,
	BUILD_IN_FUNC_ID_CONCAT,
	BUILD_IN_FUNC_ID_FIND,
	BUILD_IN_FUNC_ID_SORT
}tBuildInFunctions;
/**
 * Provede porovnani, na shodnost indentifikatoru s identifikatory
 * vestavenych funkci.
 * @param toCompare[in]	-	id pro porovnani
 * @return	Vraci tBuildInFunctions kody.
 */
tBuildInFunctions compareBuildInFunctions(string* toCompare){
	char IDLength[]="length";
	char IDSubstr[]="substr";
	char IDConcat[]="concat";
	char IDFind[]="find";
	char IDSort[]="sort";

	if(strCmpConstStr(toCompare, IDLength)==0) return BUILD_IN_FUNC_ID_LENGTH;
	if(strCmpConstStr(toCompare, IDSubstr)==0) return BUILD_IN_FUNC_ID_SUBSTR;
	if(strCmpConstStr(toCompare, IDConcat)==0) return BUILD_IN_FUNC_ID_CONCAT;
	if(strCmpConstStr(toCompare, IDFind)==0) return BUILD_IN_FUNC_ID_FIND;
	if(strCmpConstStr(toCompare, IDSort)==0) return BUILD_IN_FUNC_ID_SORT;
	return BUILD_IN_FUNC_NO_MATCH;
}

/***
 * Vytvori novy neterminal a vlozi ho do tabulky pod symbolem pomocne promenne a ulozi ho na vrchol zasobniku.
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky jej vlozi.
 * @param dataType[in]				-	Datovy typ noveho neterminalu.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vlozi novy neterminal. Pokud NULL, tak nevklada.
 * @return	Ukzatel na symbol primo v tabulce symbolu. NULL pri chybe.
 */
string* createNewNoterminal(tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tTabSymVarDataType dataType,
		tPrecStack* stack){
	//vytvorime novy tmp
	string* newTmp=tabSymListCreateTmpSymbol(startTabSymListElem, tabSym);
	if(newTmp==NULL){
		//chyba
		return NULL;
	}
	//vytvorime variableinfo pro novy tmp
	tVariableInfo* varInfo = tabSymCreateVariableInfo(dataType);
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
	if(stack!=NULL && precStackPushElementOfKind(stack, PREC_STACK_NONTERMINAL, 0, newTmp)==0){
		goto INTERNAL_ERROR_2;
	}

	//vycistime newTmp
	strFree(newTmp);
	free(newTmp);
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
ERR_CODES getDataTypeOfSymbol(tTabSymElemData* elemData, tTabSymVarDataType* dataType){
	//vybereme informaci o typu
	switch (elemData->type) {
		case TAB_SYM_VARIABLE:
			if(elemData->info.var->dataType==TAB_SYM_VAR_AUTO){
				return ERR_SEM_COM;
			}
			*dataType=elemData->info.var->dataType;
			break;
		case TAB_SYM_CONSTANT:
			switch (elemData->info.constant->dataType) {
				case TAB_SYM_VAR_NO_AUTO_INTEGER:
					*dataType=TAB_SYM_VAR_INTEGER;
					break;
				case TAB_SYM_VAR_NO_AUTO_DOUBLE:
					*dataType=TAB_SYM_VAR_DOUBLE;
					break;
				case TAB_SYM_VAR_NO_AUTO_STRING:
					*dataType=TAB_SYM_VAR_STRING;
					break;
				case TAB_SYM_VAR_NO_AUTO_BOOLEAN:
					*dataType=TAB_SYM_VAR_BOOLEAN;
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
	return ERR_OK;
}

/**
 * Genereuje instrukci pro pravidlo E->i a provadi taky semantickou kontrolu.
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky vklada nove pomocne soubory.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada novy neterminal.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return	Vraci ERR_CODES
 */
ERR_CODES genInsTermToNoterm(tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//ziskame data o terminalu(promenne konstante)
	tPrecStackData* topElemData=precStackTop(stackForGen);

	//hledame symbol neterminalu v tabulce
	tTabSymElemData* elemData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);

	if(elemData == NULL){
		/*
		 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
		 * kontrola na definici, byla jiz provedena
		 */

		return ERR_INTERNAL;
	}


	//ziskame kod datoveho typu pro vytvoreni noveho neterminalu
	tTabSymVarDataType codeOfDataType;
	ERR_CODES errCode=getDataTypeOfSymbol(elemData, &codeOfDataType);
	if(errCode!=ERR_OK){
		return errCode;
	}

	//ziskame pointery primo do tabulky
	string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemData->id);

	//vytvorime novy neterminal, ktery bude slouzit, jako cilova adresa
	string* adr3=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, codeOfDataType, stack);
	if(adr3==NULL){
		return ERR_INTERNAL;
	}

	if(adr1==NULL || adr3==NULL){
		return ERR_INTERNAL;
	}

	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, I_ASSIGN, adr1, NULL, adr3)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;
}

/**
 * Genereuje instrukci pro pravidlo E->-E a provadi taky semantickou kontrolu.
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky vklada nove pomocne soubory.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada novy neterminal.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return	Vraci ERR_CODES
 */
ERR_CODES genInsUnaryMinus(tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//prvni je minus dame jej pryc
	precStackPop(stackForGen);

	//ziskame data o neterminalu
	tPrecStackData* topElemData=precStackTop(stackForGen);
	//hledame symbol neterminalu v tabulce
	tTabSymElemData* elemData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);
	if(elemData == NULL){
			/*
			 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
			 * kontrola na definici, byla jiz provedena
			 */
			return ERR_INTERNAL;
	}

	//ziskame kod datoveho typu pro semantickou kontrolu a vytvoreni noveho neterminalu
	tTabSymVarDataType codeOfDataType;
	ERR_CODES errCode=getDataTypeOfSymbol(elemData, &codeOfDataType);
	if(errCode!=ERR_OK){
		return errCode;
	}

	/**
	 * Provedeme semantickou kontrolu,
	 * prijmame pouze int||double||boolean
	 */
	if(codeOfDataType==TAB_SYM_VAR_STRING || codeOfDataType==TAB_SYM_VAR_AUTO){
		return ERR_SEM_COM;
	}

	//vysledek s bool budem chtit v int
	if(codeOfDataType==TAB_SYM_VAR_BOOLEAN){
		codeOfDataType=TAB_SYM_VAR_INTEGER;
	}

	//ziskame pointery primo do tabulky symbolu
	string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemData->id);

	//ulozime neterminal na vrchol stacku
	if(stack!=NULL && precStackPushElementOfKind(stack, PREC_STACK_NONTERMINAL, 0, topElemData->id)==0){
		return ERR_INTERNAL;
	}

	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, I_UMINUS, adr1, NULL, adr1)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;

}
/**
 * Genereuje instrukci pro pravidlo E->!E a provadi taky semantickou kontrolu.
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky vklada nove pomocne soubory.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada novy neterminal.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return	Vraci ERR_CODES
 */
ERR_CODES genInsNot(tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){

	//prvni je vykricnik dame jej pryc
	precStackPop(stackForGen);
	//ziskame data o neterminalu
	tPrecStackData* topElemData=precStackTop(stackForGen);
	//hledame symbol neterminalu v tabulce
	tTabSymElemData* elemData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);
	if(elemData == NULL){
			/*
			 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
			 * kontrola na definici, byla jiz provedena
			 */
			return ERR_INTERNAL;
	}
	//ziskame kod datoveho typu pro semantickou kontrolu
	tTabSymVarDataType codeOfDataType;
	ERR_CODES errCode=getDataTypeOfSymbol(elemData, &codeOfDataType);
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
	string* adr3=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, TAB_SYM_VAR_INTEGER, stack);
	if(adr3==NULL){
		return ERR_INTERNAL;
	}

	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, I_LOG_NOT, adr1, NULL, adr3)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;
}

/**
 * Genereuje instrukci pro pravidla E->++i || E->--i a provadi taky semantickou kontrolu.
 * @param rule[in]					-	Na zaklade tohoto vybira o ktere pravidlo se jedna (E->++i || E->--i).
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky vklada nove pomocne soubory.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada novy neterminal.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return	Vraci ERR_CODES
 */
ERR_CODES genInsPrefix(ruleAutomateStates rule, tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//prvni prefix dame jej pryc
	precStackPop(stackForGen);
	//ziskame data o terminalu
	tPrecStackData* topElemData=precStackTop(stackForGen);
	//hledame symbol neterminalu v tabulce
	tTabSymElemData* elemData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);
	if(elemData == NULL){
		/*
		 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
		 * kontrola na definici, byla jiz provedena
		 */
		return ERR_INTERNAL;
	}
	//zkontrolujeme jestli se jedna o promennou
	if(elemData->type!=TAB_SYM_VARIABLE){
		return ERR_SEM_REST;
	}

	//ziskame kod datoveho typu pro semantickou kontrolu
	tTabSymVarDataType codeOfDataType;
	ERR_CODES errCode=getDataTypeOfSymbol(elemData, &codeOfDataType);
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
	if(rule==S_F_DEC_I && codeOfDataType==TAB_SYM_VAR_BOOLEAN){
		/*
		 * The operand of prefix -- is modified by subtracting 1.  The operand shall not be of type bool.
		 * The requirements on the operand of prefix -- and the properties of its result are otherwise the
		 * same as those of prefix ++ . [ Note: For postfix increment and decrement, see 5.2.6.— end note]
		 */
		return ERR_SEM_COM;
	}
	//ziskame pointery primo do tabulky symbolu
	string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemData->id);

	//vytvorime novy neterminal, ktery bude slouzit, jako cilova adresa
	string* adr3=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, codeOfDataType, stack);
	if(adr3==NULL){
		return ERR_INTERNAL;
	}

	tInstructTypes insType;
	if(rule==S_F_DEC_I){
		insType=I_DEC;
	}else{
		insType=I_INC;
	}

	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, insType, adr1, NULL, adr3)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;

}
/**
 * Genereuje instrukci pro pravidla E->i++ || /E->i-- a provadi taky semantickou kontrolu.
 * @param rule[in]					-	Na zaklade tohoto vybira o ktere pravidlo se jedna (E->i++ || /E->i--).
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky vklada nove pomocne soubory.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada novy neterminal.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return	Vraci ERR_CODES
 */
ERR_CODES genInsPostfix(ruleAutomateStates rule, tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//ziskame data o terminalu
	tPrecStackData* topElemData=precStackTop(stackForGen);
	//hledame symbol neterminalu v tabulce
	tTabSymElemData* elemData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);
	if(elemData == NULL){
		/*
		 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
		 * kontrola na definici, byla jiz provedena
		 */
		return ERR_INTERNAL;
	}
	//zkontrolujeme jestli se jedna o promennou
	if(elemData->type!=TAB_SYM_VARIABLE){
		return ERR_SEM_REST;
	}

	//ziskame kod datoveho typu pro semantickou kontrolu
	tTabSymVarDataType codeOfDataType;
	ERR_CODES errCode=getDataTypeOfSymbol(elemData, &codeOfDataType);
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
	if(rule==S_F_I_DEC && codeOfDataType==TAB_SYM_VAR_BOOLEAN){
		/*
		 * chyba dle c++: The operand of postfix -- is decremented analogously to the postfix ++
		 * operator, except that the operand shall not be of type bool.
		 * [Note: For prefix increment and decrement, see 5.3.2. — end note]
		 */
		return ERR_SEM_COM;
	}
	//ziskame pointery primo do tabulky symbolu
	string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemData->id);

	//vytvorime novy neterminal, ktery bude slouzit, jako cilova adresa
	string* adr3=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, codeOfDataType, stack);
	if(adr3==NULL){
			return ERR_INTERNAL;
	}

	tInstructTypes insType;
	if(rule==S_F_I_DEC){
		insType=I_DEC;
	}else{
		insType=I_INC;
	}

	//musime vlozit dve instrukce prvni priradime, do tmp promene
	//ve druhe inkrementujeme promennou
	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, I_ASSIGN, adr1, NULL, adr3)==0){
		return ERR_INTERNAL;
	}
	//vlozime instrukci na pasku
	if(insTapeInsertLast(insTape, insType, adr1, NULL, adr1)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;
}

/**
 * Generuje instrukce pro vkladani parametru funkce a provadi semantickou kontrolu parametru.
 * @param params[in]				-	List parametru volane funkce. Pouziva se pro semantickou kontrolu.
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return Vraci ERR_CODES
 */
ERR_CODES genInsFuncParams(tParamListPtr params, tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tPrecStack* stackForGen, tInsTape* insTape){

	//vezmeme prvni parametr
	tPrecStackData* actParamData=precStackTop(stackForGen);


	//nejprve provedeme kontrolu pro prazdne parametry
	//je-li seznam parametru prazdny musi byt na vrcholu zasobniku )

	if(params->first==NULL && actParamData->type==PREC_STACK_TERMINAL && actParamData->key==TERMINAL_CLOSE_BRACKET){
		return ERR_OK;
	}
	//je li na vrcholu zasobniku ) musime zkontrolovat, jestli ma byt opravdu seznam param prazdny
	if(actParamData->type==PREC_STACK_TERMINAL && actParamData->key==TERMINAL_CLOSE_BRACKET && params->first!=NULL){
		//nema byt prazdny chyba
		return ERR_SEM_COM;
	}


	tTabSymElemData* actParam= tabSymListSearch(startTabSymListElem,tabSym, actParamData->id);
	tTabSymVarDataType codeOfDataType;

	//budeme prochazet paramlist a parovat s neterminaly na stacku
	//nastavime aktivitu na prvni prvek
	first(params);


	while(params->act!=NULL && actParamData!=NULL){
		//hledame symbol neterminalu v tabulce
		actParam= tabSymListSearch(startTabSymListElem,tabSym, actParamData->id);
		if(actParam == NULL){
			/*
			 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
			 * kontrola na definici, byla jiz provedena
			 */
			return ERR_INTERNAL;
		}
		ERR_CODES errCode=getDataTypeOfSymbol(actParam, &codeOfDataType);
		if(errCode!=ERR_OK){
			return errCode;
		}

		//semanticka kontrola typu
		if(((tTabSymVarDataType)params->act->dataType==TAB_SYM_VAR_STRING && codeOfDataType!=TAB_SYM_VAR_STRING) ||
							((tTabSymVarDataType)params->act->dataType!=TAB_SYM_VAR_STRING && codeOfDataType==TAB_SYM_VAR_STRING)){
			// nelze provest implicitni konverzi mezi stringem a dalsimi typy
			return ERR_SEM_COM;
		}
		//ziskame pointery primo do tabulky symbolu
		string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, actParamData->id);

		//vygenerujeme instrukci
		// adr1 -z volajici funkce, , adr3-z volane | set parametr - musis se podivat o jeden niz
		if(insTapeInsertLast(insTape, I_SP, adr1, NULL, params->act->idName)==0){
			return ERR_INTERNAL;
		}

		//dalsi musi byt , nebo ) proto to muzeme odstranit
		precStackPop(stackForGen);
		precStackPop(stackForGen);
		actParamData=precStackTop(stackForGen);
		succ(params);
	}
	//zkontrolujeme jestli je vse semanticky v poradku
	if(params->act!=NULL || actParamData!=NULL){
		return ERR_SEM_COM;
	}
	return ERR_OK;
}


/**
 * Genereuje instrukci pro pravidlo E->f(E...) a provadi take semantickou kontrolu.
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky vklada nove pomocne soubory.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada novy neterminal.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return	Vraci ERR_CODES
 */
ERR_CODES genInsFunc(tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	//E->f(...)	...-seznam parametru oddelenych carkou muze byt i prazdny
	static const unsigned int POOL_SIZE=4;	//velikost poolu adres

	//ziskame data o identifikatoru funkce
	tPrecStackData* topElemData=precStackTop(stackForGen);
	if(topElemData==NULL){
		return ERR_INTERNAL;
	}

	tTabSymVarDataType dataTypeOfResult;	//datovy typ vysledku

	tBuildInFunctions buildInKind=compareBuildInFunctions(topElemData->id);
	tTabSymElemData* funcData=NULL;
	tFuncInfo* funcInfo=NULL;
	//jedna se o vestavenou funkci?
	if(buildInKind!=BUILD_IN_FUNC_NO_MATCH){
		funcData=tabSymSearch(globalTable, topElemData->id);
		funcInfo=funcData->info.func;

		dataTypeOfResult=funcInfo->retType;	// datovy typ pro ulozeni hodnoty funkce

		string* adrPool[4]={NULL};

		//budeme prochazet paramlist a parovat s neterminaly na stacku
		tParamListPtr params=funcInfo->params;
		if(params==NULL){
			//chyba
			return ERR_INTERNAL;
		}
		//nastavime aktivitu na prvni prvek
		first(params);
		//vezmeme prvni parametr
		precStackPop(stackForGen);//odstranime ID funkce
		precStackPop(stackForGen);//Odstranime (

		tPrecStackData* actParamData=precStackTop(stackForGen);
		tTabSymElemData* actParam= tabSymListSearch(startTabSymListElem,tabSym, actParamData->id);
		tTabSymVarDataType codeOfDataType;



		for(unsigned int i=0; i<POOL_SIZE && params->act!=NULL && actParamData!=NULL;i++){
			//hledame symbol neterminalu v tabulce
			actParam= tabSymListSearch(startTabSymListElem,tabSym, actParamData->id);
			if(actParam == NULL){
				/*
				* chyba nenalezeno, ale jedna se o interni chybu, protoze,
				* kontrola na definici, byla jiz provedena
				*/
				return ERR_INTERNAL;
			}
			ERR_CODES errCode=getDataTypeOfSymbol(actParam, &codeOfDataType);
			if(errCode!=ERR_OK){
				return errCode;
			}
			//semanticka kontrola typu
			if(((tTabSymVarDataType)params->act->dataType==TAB_SYM_VAR_STRING && codeOfDataType!=TAB_SYM_VAR_STRING) ||
					((tTabSymVarDataType)params->act->dataType!=TAB_SYM_VAR_STRING && codeOfDataType==TAB_SYM_VAR_STRING)){
				// nelze provest implicitni konverzi mezi stringem a dalsimi typy
				return ERR_SEM_COM;
			}
			//ziskame pointery primo do tabulky symbolu a ulozime do pool
			adrPool[i]=tabSymListGetPointerToKey(startTabSymListElem, tabSym, actParamData->id);

			//dalsi musi byt , nebo ) proto to muzeme odstranit
			precStackPop(stackForGen);
			precStackPop(stackForGen);
			actParamData=precStackTop(stackForGen);
			succ(params);
		}
		//zkontrolujeme jestli je vse semanticky v poradku
		if(params->act!=NULL || actParamData!=NULL){
			return ERR_SEM_COM;
		}

		//vytvorime novy neterminal, ktery bude slouzit, jako return pro funkci (vysledkem je string)
		string* saveTo=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, dataTypeOfResult, stack);
		if(saveTo==NULL){
			return ERR_INTERNAL;
		}
		//vygenerujeme prislusne instrukce
		switch (buildInKind) {
			case BUILD_IN_FUNC_ID_CONCAT:
				if(insTapeInsertLast(insTape, I_CONCAT, adrPool[0], adrPool[1], saveTo)==0){
					return ERR_INTERNAL;
				}

				break;
			case BUILD_IN_FUNC_ID_FIND:
				if(insTapeInsertLast(insTape, I_FIND, adrPool[0], adrPool[1], saveTo)==0){
					return ERR_INTERNAL;
				}
				break;
			case BUILD_IN_FUNC_ID_LENGTH:
				if(insTapeInsertLast(insTape, I_LENGTH, adrPool[0], NULL, saveTo)==0){
					return ERR_INTERNAL;
				}
				break;
			case BUILD_IN_FUNC_ID_SORT:
				if(insTapeInsertLast(insTape, I_SORT, adrPool[0], NULL, saveTo)==0){
					return ERR_INTERNAL;
				}
				break;
			case BUILD_IN_FUNC_ID_SUBSTR:
				if(insTapeInsertLast(insTape, I_SUBSTR, adrPool[0], adrPool[1], adrPool[2])==0){
					return ERR_INTERNAL;
				}
				if(insTapeInsertLast(insTape, I_SUBSTR_DEST, NULL, NULL, saveTo)==0){
					return ERR_INTERNAL;
				}
				break;
			default:
				break;
		}
		//vse ok
		return ERR_OK;
	}

	//nejedna se o vestavenou funkci

	//hledame symbol funkce v globalni tabulce
	funcData=tabSymSearch(globalTable, topElemData->id);
	if(funcData == NULL){
		/*
		 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
		 * kontrola na definici, byla jiz provedena
		 */
		return ERR_INTERNAL;
	}

	funcInfo=funcData->info.func;
	dataTypeOfResult=funcInfo->retType;	// datovy typ pro ulozeni hodnoty funkce
	//vytvorime novy neterminal, ktery bude slouzit, jako return pro funkci (vysledkem je string)
	string* saveTo=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, dataTypeOfResult, stack);
	if(saveTo==NULL){
		return ERR_INTERNAL;
	}

	//call function adr1 - locTab adr2 - ins adr3 - adresa kam ulozit vysledek fce(u main NULL)
	if(insTapeInsertLast(insTape, I_CF, funcInfo->locTab, funcInfo->instTape->first, saveTo)==0){
		return ERR_INTERNAL;
	}
	//pop pro parametry
	precStackPop(stackForGen);//odstranime ID funkce
	precStackPop(stackForGen);//Odstranime (

	//zavolame zpracovani parametru
	return genInsFuncParams(funcInfo->params, startTabSymListElem, tabSym,stackForGen, insTape);
}

/**
 * Zpracovava E->(E). Pouze vlozi E na vrchol zasobniku stack.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada neterminal v zavorkach.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @return Vraci ERR_CODES
 */
ERR_CODES genInsEInBrackets(tPrecStack* stack, tPrecStack* stackForGen){
	//E->(E) pouze vlozi E na vrchol zasobniku
	//prvni je ( odstranime
	precStackPop(stackForGen);
	//ziskame neterminal E
	tPrecStackData* actData = precStackTop(stackForGen);
	if(actData==NULL){
		return ERR_INTERNAL;
	}
	//vlozime jej na vrchol zasobniku stack
	if(precStackPush(stack,*actData)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;
}
/**
 * Genereuje instrukci pro pravidla:
 * E->E+E || E->E-E || E->E*E || E->E/E || E->E==E || E->E!=E || E->E<E || E->E>E || E->E<=E || E->E>=E || E->E&&E || E->E||E
 * a provadi take semantickou kontrolu.
 * @param rule[in]					-	Na zaklade tohoto vybira o ktere pravidlo se jedna (E->E+E || E->E-E ...).
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky vklada nove pomocne soubory.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada novy neterminal.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return	Vraci ERR_CODES
 */
ERR_CODES genInsBinaryOpers(ruleAutomateStates rule, tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){
	ERR_CODES errCode;
	//prvni je neterminal
	//ziskame data o neterminalu/levem operandu
	tPrecStackData* topElemData=precStackTop(stackForGen);
	if(topElemData==NULL){
		return ERR_INTERNAL;
	}

	//ziskame pointery leveho operandu primo do tabulky symbolu
	string* adr1=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemData->id);

	//hledame symbol neterminalu v tabulce
	tTabSymElemData* leftOperandData= tabSymListSearch(startTabSymListElem,tabSym, topElemData->id);
	if(leftOperandData==NULL){
		/*
		 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
		 * kontrola na definici, byla jiz provedena
		 */
		return ERR_INTERNAL;
	}

	//ziskame kod datoveho typu pro semantickou kontrolu
	tTabSymVarDataType leftOperCodeOfDataType;
	errCode=getDataTypeOfSymbol(leftOperandData, &leftOperCodeOfDataType);
	if(errCode!=ERR_OK){
		return errCode;
	}
	precStackPop(stackForGen);
	//Nasleduje operator ten, ale nepotrebujeme zname ho z pravidla
	precStackPop(stackForGen);

	//ziskame data o neterminalu/pravem operandu
	tPrecStackData* topElemDataNext=precStackTop(stackForGen);
	if(topElemDataNext==NULL){
		return ERR_INTERNAL;
	}
	//ziskame pointery leveho operandu primo do tabulky symbolu
	string* adr2=tabSymListGetPointerToKey(startTabSymListElem, tabSym, topElemDataNext->id);

	//hledame symbol neterminalu v tabulce
	tTabSymElemData* rightOperandData= tabSymListSearch(startTabSymListElem,tabSym, topElemDataNext->id);
	if(rightOperandData==NULL){
		/*
		 * chyba nenalezeno, ale jedna se o interni chybu, protoze,
		 * kontrola na definici, byla jiz provedena
		 */
		return ERR_INTERNAL;
	}
	//ziskame kod datoveho typu pro semantickou kontrolu
	tTabSymVarDataType rightOperCodeOfDataType;
	errCode=getDataTypeOfSymbol(rightOperandData, &rightOperCodeOfDataType);
	if(errCode!=ERR_OK){
		return errCode;
	}





	//mame oba operandy a dle pravidla i operator, nasleduje provedeni semanticke kontroly

	if(leftOperCodeOfDataType==TAB_SYM_VAR_AUTO || rightOperCodeOfDataType==TAB_SYM_VAR_AUTO){
		//pokud je auto
		return ERR_SEM_COM;
	}

	if((leftOperCodeOfDataType==TAB_SYM_VAR_STRING && rightOperCodeOfDataType!=TAB_SYM_VAR_STRING) ||
			(rightOperCodeOfDataType==TAB_SYM_VAR_STRING && leftOperCodeOfDataType!=TAB_SYM_VAR_STRING)){
		//pokud je jeden z operandu string musi byt i ten druhy string
		return ERR_SEM_COM;
	}


	//implicitni konverze
	if(leftOperCodeOfDataType!=TAB_SYM_VAR_STRING && rightOperCodeOfDataType!=TAB_SYM_VAR_STRING){
		//provadime implicitni konverze, nesmi byt ani jeden z operandu string


		if(leftOperCodeOfDataType!=rightOperCodeOfDataType){
			//operandy se nerovnaji
			if(leftOperCodeOfDataType==TAB_SYM_VAR_BOOLEAN){
				/*levy je boolean vysledny typ je typ praveho
				 * Vysvetleni:
				 * LEVY		PRAVY	|	KONVERZE NA
				 * ---------------------------------
				 * BOOL		BOOL	|	BOOL			//nemame kvuli zanoreni v podmince
				 * BOOL		INT		|	INT
				 * BOOL		DOUBLE	|	DOUBLE
				 */
				leftOperandData->type=rightOperCodeOfDataType;	//prepiseme jeho dataovy typ
			}
			if(leftOperCodeOfDataType==TAB_SYM_VAR_DOUBLE){
				/*levy je double vysledny typ je typ double
				 * Vysvetleni:
				 * LEVY		PRAVY	|	KONVERZE NA
				 * ---------------------------------
				 * DOUBLE	BOOL	|	DOUBLE
				 * DOUBLE	INT		|	DOUBLE
				 * DOUBLE	DOUBLE	|	DOUBLE			//nemame kvuli zanoreni v podmince
				 */
				rightOperandData->type=TAB_SYM_VAR_DOUBLE;
			}
			if(leftOperCodeOfDataType==TAB_SYM_VAR_INTEGER){
				/*levy je integer
				 * Vysvetleni:
				 * LEVY		PRAVY	|	KONVERZE NA
				 * ---------------------------------
				 * INT		BOOL	|	INT
				 * INT		INT		|	INT				//nemame kvuli zanoreni v podmince
				 * INT		DOUBLE	|	DOUBLE
				 */


				if(rightOperCodeOfDataType==TAB_SYM_VAR_DOUBLE){
					//levy je integer a pravy double.
					leftOperandData->type=TAB_SYM_VAR_DOUBLE;	//prepiseme jeho dataovy typ
				}else{
					rightOperandData->type=TAB_SYM_VAR_INTEGER;
				}

			}
		}
	}

	tInstructTypes insType;

	switch (rule) {
		case S_F_E_PLUS_E:
			insType=I_PLUS;
			break;
		case S_F_E_MINUS_E:
			insType=I_MINUS;
			break;
		case S_F_E_MUL_E:
			insType=I_MUL;
			break;
		case S_F_E_DIV_E:
			insType=I_DIV;
			break;
		case S_F_E_EQU_E:
			insType=I_EQUAL;
			break;
		case S_F_E_NEQU_E:
			insType=I_NOTEQUAL;
			break;
		case S_F_E_LESS_E:
			insType=I_LESSER;
			break;
		case S_F_E_GREATER_E:
			insType=I_GREATER;
			break;
		case S_F_E_LESSEQU_E:
			insType=I_LEQUAL;
			break;
		case S_F_E_GREATEREQU_E:
			insType=I_GEQUAL;
			break;
		case S_F_E_AND_E:
			insType=I_LOG_AND;
			break;
		case S_F_E_OR_E:
			insType=I_LOG_OR;
			break;
		default:
			//spatne predane pravidlo
			return ERR_INTERNAL;
			break;
	}


	if(rule==S_F_E_EQU_E || rule==S_F_E_NEQU_E || rule==S_F_E_LESS_E || rule==S_F_E_GREATER_E || rule==S_F_E_LESSEQU_E
				|| rule==S_F_E_GREATEREQU_E || rule==S_F_E_AND_E  || rule==S_F_E_OR_E){
		//pro logicke operace vytvorime novy tmp
		//vytvorime novy neterminal, ktery bude slouzit, jako vysledek
		string* adr3=createNewNoterminal(startTabSymListElem, tabSym, insertToTable, TAB_SYM_VAR_INTEGER, stack);
		if(adr3==NULL){
			return ERR_INTERNAL;
		}
		//vlozime instrukci na pasku
		if(insTapeInsertLast(insTape, insType, adr1, adr2, adr3)==0){
			return ERR_INTERNAL;
		}
	}else{
		//pro ostatni recyklujeme levy
		//ulozime neterminal na vrchol stacku

		string leftOperSymbol;	//pri znovu pouziti tmp promenne
		strInit(&leftOperSymbol);
		strCopyString(&leftOperSymbol, adr1);

		if(stack!=NULL && precStackPushElementOfKind(stack, PREC_STACK_NONTERMINAL, 0, &leftOperSymbol)==0){
			strFree(&leftOperSymbol);
			return ERR_INTERNAL;
		}

		if(insTapeInsertLast(insTape, insType, adr1, adr2, adr1)==0){
			strFree(&leftOperSymbol);
			return ERR_INTERNAL;
		}
	}

	return ERR_OK;
}

/**
 * Obhospodarovava pravidla. Vola prislusne funkce pro jejich zpracovani.
 * @param rule[in]					-	Na zaklade tohoto vybira o ktere pravidlo se jedna.
 * @param startTabSymListElem[in]	-	Slouzi pro vyhledavani symbolu.
 * @param tabSym[in]				-	Slouzi pro vyhledavani symbolu.
 * @param insertToTable[in]			-	Do teto tabulky vklada nove pomocne soubory.
 * @param stack[in]					-	Na vrchol tohoto zasobniku vklada novy neterminal.
 * @param stackForGen[in]			-	Stack, ktery je pomocny pro generovani (<y).
 * @param insTape[in]				-	Paska instrukci, na kterou vlozi nove instrukce.
 * @return	Vraci ERR_CODES
 */
ERR_CODES manageRule(ruleAutomateStates rule, tTabSymListElemPtr startTabSymListElem, tTabSym* tabSym, tTabSym* insertToTable, tPrecStack* stack, tPrecStack* stackForGen, tInsTape* insTape){

	//na zaklade koncoveho stavu automatu vybere funkce pro zpracovani
	switch (rule) {
		case S_F_I:
			return genInsTermToNoterm(startTabSymListElem, tabSym, insertToTable, stack, stackForGen, insTape);
		case S_F_MINUS_E:
			return genInsUnaryMinus(startTabSymListElem, tabSym, stack, stackForGen,insTape);
			break;
		case S_F_NOT_E:
			return genInsNot(startTabSymListElem, tabSym, insertToTable, stack, stackForGen, insTape);
			break;

		case S_F_INC_I:
		case S_F_DEC_I:
			return genInsPrefix(rule, startTabSymListElem, tabSym, insertToTable, stack, stackForGen, insTape);
			break;

		case S_F_I_INC:
		case S_F_I_DEC:
			return genInsPostfix(rule, startTabSymListElem, tabSym, insertToTable, stack, stackForGen, insTape);
			break;

		case S_F_FUN:
			return genInsFunc(startTabSymListElem, tabSym, insertToTable, stack, stackForGen, insTape);
			break;

		case S_F_OPEN_BRACKET_E_CLOSE_BRACKET:
			return genInsEInBrackets(stack, stackForGen);
			break;

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
		case S_F_E_AND_E:
		case S_F_E_OR_E:
			return genInsBinaryOpers(rule, startTabSymListElem, tabSym, insertToTable, stack, stackForGen, insTape);
			break;
		default:
			return ERR_INTERNAL;
			break;
	}

}

/**
 * Slouzi pro vytvoreni tmp promene, pro parser
 * @param stack[in]			-	Odtud vezme z vrcholu neterminal
 * @param tableListElem[in]	-	Slouzi pro najiti symbolu v hiararchii tabulek.
 * @param table[in]			-	Slouzi pro najiti symbolu v hiararchii tabulek.
 * @param insertToTable[in]	-	Do teto tabulky vlozi nove vygenerovnay tmp.
 * @param tape[in]			-	Na tuto pasku generuje instrukci prirazeni do nove vygenerovane tmp.
 * @return vraci ERR_CODES
 */
ERR_CODES genEndInstruction(tPrecStack* stack, tTabSymListElemPtr tableListElem, tTabSym* table, tTabSym* insertToTable, tInsTape* tape){
	//vezmeme neterminal z vrcholu zasobniku
	ERR_CODES errRet;
	tPrecStackData* topElemData=precStackTop(stack);
	tTabSymElemData* dataOfLast;
	//zjistime jeho datovy typ
	if((dataOfLast = tabSymListSearch(tableListElem, table, topElemData->id)) == NULL) {
		return ERR_INTERNAL;
	}
	tTabSymVarDataType expDataTypeFor;
	errRet=getDataTypeOfSymbol(dataOfLast, &expDataTypeFor);
	if(errRet!=ERR_OK){
		return errRet;
	}


	//vytvorime novy neterminal, ktery bude slouzit, jako cilova adresa
	string* adr3=createNewNoterminal(tableListElem, table, insertToTable, expDataTypeFor, NULL);
	if(adr3==NULL){
		return ERR_INTERNAL;
	}
	//ziskame pointery leveho operandu primo do tabulky symbolu
	string* adr1=tabSymListGetPointerToKey(tableListElem, table, topElemData->id);

	//vlozime instrukci na pasku
	if(insTapeInsertLast(tape, I_ASSIGN, adr1, NULL, adr3)==0){
		return ERR_INTERNAL;
	}
	return ERR_OK;
}

int parseExpression(tTabSymListElemPtr tableListElem, tTabSym* table, tInsTape* tape,
		tTabSymVarNoAutoDataType* expDataType, FILE* scanFile) {
	string* id=NULL; //pro vytvareni identifikatoru v tabulce symbolu
	ERR_CODES errRet;	//chybovy kod
	//vybereme si tabulku, do ktere se budou vkladat tmp symboly (pomocne promenne)
	tTabSym* insertToTable=NULL;
	if(tableListElem!=NULL){
		insertToTable=tableListElem->table;
	}
	if(insertToTable==NULL){
		insertToTable=table;
	}

	//vytvorime zasobniky
	tPrecStack stack;
	precStackInit(&stack);
	tPrecStack* revertedTopStack=NULL;	//pouziva se pro vyber pravidla

	//pridame ukoncujici znak
	precStackPushElementOfKind(&stack, PREC_STACK_ENDMARK, TERMINAL_ENDMARK, NULL);

	int rule; // vybrane pravidlo

	tParExpTerminals a;	//nejvrchnejis terminal na zasobniku
	tParExpTerminals b;	//aktualni druh tokenu na vstupu
	tToken token;	//aktualni token na vstupu

	if(prepareNextToken(&stack, scanFile, &b, &token)==0){	//ziskame pocatecni token
		//chyba lexikalni
		return ERR_LEX;
	}

	if(precStackTopTerminal(&stack, &a)==0){
		//vlozili jsme ENDMARK NEMUZE, BYT POKUD ANO CHYBA
		freeTokenMem(&token);
		return ERR_INTERNAL;
	}
	do {
		switch (precGetRule(a, b)) {
		case '=':
			//dej na zasobnik prijaty token a precti dalsi ze vstupu
			if((errRet=semHandleNewToken(table, insertToTable, tableListElem, b, token, &id))!=ERR_OK){
				goto ERROR_HANDLER;
			}
			if(precStackPushElementOfKind(&stack, PREC_STACK_TERMINAL, b, id)==0){
				errRet=ERR_INTERNAL;
				goto ERROR_HANDLER;
			}

			//vycistime aktualni token
			freeTokenMem(&token);
			//vycistime i id
			strFree(id);
			free(id);
			id=NULL;

			if(prepareNextToken(&stack, scanFile, &b, &token)==0){
				errRet=ERR_LEX;
				goto ERROR_HANDLER;
			}
			break;
		case '<':
			/*
			 * zaměň a za a< na zásobníku
			 * a dej na zasobnik prijaty token a precti dalsi ze vstupu
			 */
			precStackPushElemBeforeTopTerm(&stack, PREC_STACK_SIGN, '<', NULL);

			if((errRet=semHandleNewToken(table, insertToTable, tableListElem, b, token, &id))!=ERR_OK){
				goto ERROR_HANDLER;
			}

			if(precStackPushElementOfKind(&stack, PREC_STACK_TERMINAL, b, id)==0){
				errRet=ERR_INTERNAL;
				goto ERROR_HANDLER;
			}

			//vycistime aktualni token
			freeTokenMem(&token);
			//vycistime i id
			strFree(id);
			free(id);
			id=NULL;

			if(prepareNextToken(&stack, scanFile, &b, &token)==0){
				errRet=ERR_LEX;
				goto ERROR_HANDLER;
			}
			break;
		case '>':
			/*
			 * if <y je na vrcholu zásobníku and r: A → y ∈ P
			 *	then zaměň <y za A & vypiš r na výstup
			 *	else chyba
			 */

			//pokusime se ziskat stack obsahujici <y a smazeme jej z vrcholu
			revertedTopStack=precStackCreateRevertedTopReduc(&stack);
			if(revertedTopStack==NULL){
				errRet=ERR_INTERNAL;
				goto ERROR_HANDLER;
			}
			switch (chooseRule(revertedTopStack, &rule)) {
				case 1:
					// v poradku

					//zmeni <y za neterminal, provede semantickou kontrolu a vygeneruje instrukci.
					errRet=manageRule(rule, tableListElem, table, insertToTable, &stack, revertedTopStack, tape);
					precStackFree(&revertedTopStack);	//vymazem obsah
					if(errRet!=ERR_OK){
						//chyba
						goto ERROR_HANDLER;
					}
					break;
				case 0:
					// syntakticka chyba
					errRet=ERR_SYNTAX;
					goto ERROR_HANDLER;
				case -1:
					// vnitrni chyba
					errRet=ERR_INTERNAL;
					goto ERROR_HANDLER;
					break;
				default:
					// vnitrni chyba
					errRet=ERR_INTERNAL;
					goto ERROR_HANDLER;
					break;
			}

			break;
		default:
			if(a==TERMINAL_ENDMARK && b==TERMINAL_CLOSE_BRACKET){
				//pokud je na vrchu zasobniku $ a prichazi )
				ungetToken(&token);	//pro dalsi zpracovani
				//musime jeste vygenerovat konecnou instrukci pro parser
				errRet=genEndInstruction(&stack, tableListElem, table, insertToTable, tape);
			}else{
				//chyba
				errRet=ERR_SYNTAX;
			}
			goto ERROR_HANDLER;
		}
		precStackTopTerminal(&stack, &a);
	} while (b!=TERMINAL_ENDMARK || a!=TERMINAL_ENDMARK);//b = $ and top = $

	if(precStackTop(&stack)->type==PREC_STACK_ENDMARK){
		//jedna se o prazdny vyraz
		errRet=ERR_SYNTAX;	//chyba
		goto ERROR_HANDLER;
	}

	//musime jeste vygenerovat konecnou instrukci pro parser
	errRet=genEndInstruction(&stack, tableListElem, table, insertToTable, tape);
	if(errRet!=ERR_OK){
		goto ERROR_HANDLER;
	}
	ungetToken(&token);	//pro dalsi zpracovani
	precStackDispose(&stack);
	precStackFree(&revertedTopStack);

	//vratime datovy typ vyrazu
	string* lastGeneratedTMP;
	tTabSymElemData* dataOfLast;
	if((lastGeneratedTMP = tabSymListLastCreateTmpSymbol(tableListElem, table)) == NULL) {
		return ERR_INTERNAL;
	}

	if((dataOfLast = tabSymListSearch(tableListElem, table, lastGeneratedTMP)) == NULL) {
			return ERR_INTERNAL;
	}
	tTabSymVarDataType expDataTypeFor;
	errRet=getDataTypeOfSymbol(dataOfLast, &expDataTypeFor);
	if(errRet!=ERR_OK){
		return errRet;
	}
	*expDataType=expDataTypeFor;
	strFree(lastGeneratedTMP);
	free(lastGeneratedTMP);

	return ERR_OK;

ERROR_HANDLER:

	freeTokenMem(&token);
	precStackDispose(&stack);
	precStackFree(&revertedTopStack);
	//vycistime i id
	strFree(id);
	free(id);
	return errRet;

}

/*** End of file: parserExpr.c ***/
