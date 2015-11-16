#include "scanner.h"
#include "str.h"
#include <stdio.h>
/*
 * Výčet možných stavů FSM.
 */
enum states {
	START,
	STRING,
	STRING_END,
	STRING_ESCAPE,
	ERROR,
	DIVISION_COMMENT,
	BLOCK_COMMENT,
	LINE_COMMENT,
	NESTED_BLOCK_COMMENT_CHECK,
	END_BLOCK_COMMENT,
	TWO_CHAR_OPER,
	DECREMENT_OPERATOR,
	INCREMENT_OPERATOR,
	LOGICAL_AND,
	LOGICAL_OR,
	IDENTIFICATOR,
	NOT_EQUAL
};
/* @var int citac radku*/
int line = 1;
//TODO Doca's kravinka

/*
* KEY WORDS
* auto, cin, cout, double, else,
* for, if, int, return, string
*
*
*/
/*
 * Musi vracet nějakou reprezentaci tokenu
 * Dostávat ukazatel na otevřený soubor (DI)
 */
int getToken(tToken *Token, FILE* source) {
	/* @var c int actual character*/
	int c = -1;
	/* @var prevRest int poslední načtený znak (Zbytek posledního průchodu) */
	static int pom = -1;

	string s;
	//strInit vraci 1 při chybe
	if (strInit(&s)) {
		//return ERROR_ALOK;
	}

	int state = START;
	while (1) {
		if (pom > 0) {//pozustatek z minuleho cteni
			c = pom;
			pom = -1;
		}
		else {
			c = getc(source);
		}

		switch (state)
		{
		case START:
			if (c == 35) {
				state = STRING;
			}
			else if (isspace(c)) {
				state = START;
			}
			else if (isalpha(c) || c == '_') {
				state = IDENTIFICATOR;
				if (strAddChar(&s, c)) {
					//return ERROR_ALLOC;
				}
			}
			else if (c == '\"') {
				state = STRING;
			}
			else if (c == '/') {
				state = DIVISION_COMMENT;
			}
			else if (c == '-') {
				state = DECREMENT_OPERATOR;
			}
			else if (c == '+') {
				state = INCREMENT_OPERATOR;
			}
			else if (c == '*') {
				Token->typ = MULTIPLY;
				return 1;
			}
			else if (c == '!') {
				state = NOT_EQUAL;
			}
			else if (c == ')') {
				Token->typ = PARENTHESIS_CLOSING;
				return 1;
			}
			else if (c == '(') {
				Token->typ = PARENTHESIS_OPENING;
				return 1;
			}
			else if (c == '|') {
				state = LOGICAL_OR;
			}
			else if (c == '&') {
				state = LOGICAL_AND;
			}
			else if (c==EOF) {
				Token->typ = END_OF_FILE;
				return 1;
			}
			break;
		case NOT_EQUAL:
			if (c == '=') {
				Token->typ = NOT_EQUAL_OPER;
				return 1;
			}
			else {
				pom = c;
				return 42;
			}
			break;
		case LOGICAL_AND:
			if (c == '&') {
				Token->typ = LOG_AND_OPER;
				return 1;
			}
			else {
				return 42;
			}
			break;
		case LOGICAL_OR:
			if (c == '|') {
				Token->typ = LOG_OR_OPER;
				return 1;
			}
			else {
				return 42;
			}
			break;
		case STRING:
			if (c == EOF) {
				//error
			}
			else if (c == '\\') {
				if (strAddChar(&s, c)) {
					//return ERROR_ALLOC;
				}
				state = STRING_ESCAPE;
			}
			else if (c == '"') {//konec retezce uloz ho
				Token->typ = TYPE_STRING;
				if (strInit(&Token->value.stringVal)) {
					//return stringval
				}
				if (strCopyString(&Token->value.stringVal, &s)) {
					//return error;
				}
				return 1;
			}
			else if (c < 1 || c>255) {//nejaky znak mimo ASCII chyba

			}
			else {//uloz si znak
				if (strAddChar(&s, c)) {
					//return ERROR_ALLOC;
				}
			}
			break;
		case IDENTIFICATOR:
			if (isalnum(c) || c == '_') {
				state = state;
				if (strAddChar(&s, c)) {
					//return ERROR_ALLOC;
				}
			}
			else {
				pom = c;
				Token->typ = TYPE_IDENTIFICATOR;
				if (strInit(&(Token->value.stringVal))) {
					//return stringval
				}
				if (strCopyString(&(Token->value.stringVal), &s)) {
					return 0;//todo
				}
				return 1;//todo
			}
			break;
		case DIVISION_COMMENT:
			if (c == '*') {
				state = BLOCK_COMMENT;
			}
			else if (c == '/') {
				state = LINE_COMMENT;
			}
			else {//jednoznakovy operator deleni
				Token->typ = DIVISION;
				pom = c;
				return 1;//todo return success
			}
			break;
		case LINE_COMMENT:
			if (c == '\n') {//konec radkoveho komentare hledame dalsi lexem
				state = START;
			}
			else if (c == EOF) {//nemusi byt lf pred koncem souboru
				pom = c;
				state = START;
			}
			else {//komentar pokracuje
				  //reflection nepodporujeme takže komentáře zahodíme
			}
			break;
		case BLOCK_COMMENT:
			if (c == EOF) {
				//blokovy komentár chceme ukoncit
				return 0;
			}
			else if (c=='/') {
				state = NESTED_BLOCK_COMMENT_CHECK;
			}
			else if (c=='*') {
				state = END_BLOCK_COMMENT;
			}
			break;
		case END_BLOCK_COMMENT:
			if (c == '*') {
				//zůstaneme tady komentář může končit x hvězdičkama
			}
			else if (c == EOF) {
				return 42;//todo
			}
			else if (c == '/') {//konec blokového komentáře jdeme hledat další lexém
				state = START;
			}
			else {
				state = BLOCK_COMMENT;
			}
			break;
		case NESTED_BLOCK_COMMENT_CHECK:
			if (c == '*') {
				return 42;//TODO: error 
			}
			else if (c == EOF) {
				//TODO: error
			}
			else {
				state = BLOCK_COMMENT;
			}
			break;
		case TWO_CHAR_OPER:
			break;
		case DECREMENT_OPERATOR:
			if (c == '-') {
				Token->typ = DECREMENTATION;
				return 1;
			}
			else {
				pom = c;
				Token->typ = MINUS;
				return 1;
			}
			break;
		case INCREMENT_OPERATOR:
			if (c == '+') {
				Token->typ = INCREMENTATION;
				return 1;
			}
			else {
				pom = c;
				Token->typ = PLUS;
				return 1;
			}
			break;
		default:
			break;
		}
	}
}
/*
int strToInt(string* forConversion, int* val){
	/** todo rozdelane, mozno vymazat cele
	number = strtol(cString, &pEnd, 0);
	//kontrola
	if (errno == ERANGE || *pEnd != '\0' || number > INT_MAX || number < INT_MIN) {
		return 0;
	}
	*converted = (int) number;
	*converted */
/*}*/
/*
int strToBool(string* forConversion, bool* val){
	const char trueString="true";
	const char falseString="false";
	char * compareWith;

	if(forConversion->length==4){
		//dle delky porovnavame na true
		compareWith=&trueString;
	}else if(forConversion->length!=5){
		//dle delky porovnavame na false
		compareWith=&falseString;
	}else{
		//chyba nesedi delka
		return 0;
	}

	//zkontrolujeme na schodu, pokud se lisi vratime chybu
	for(int i=0;i<forConversion->length; i++){
		if(forConversion->str[i]==compareWith[i]){
			return 0;
		}
	}

	//vse v poradku vratime prevedenou hodnu ve val.
	if(forConversion->length==4){
		*val=true;
	}else{
		*val=false;
	}

	return 1;
}
*/