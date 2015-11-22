#include "scanner.h"
#include "error.h"
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
	//NOT_EQUAL,
	ESCAPE,
	BINARY_NUMBER,
	OCTAL_NUMBER,
	HEX_DEC_NUMBER,
	INT_PART,
	FLOAT_OR_INT,
	FLOAT_PART,
	EXPONENT,
	EXPONENT_CHECK,
	EXPONENT_PLUS_MINUS_CHECK
};
/* @var int citac radku*/
int line = 1;
int character = 0;

/*
* KEY WORDS
* auto, cin, cout, double, else,
* for, if, int, return, string
*/
int getToken(tToken *Token, FILE* source) {
	/* @var c int actual character*/
	int c = -1;
	/* @var prevRest int poslední načtený znak (Zbytek posledního průchodu) */
	static int pom = -1;

	string s;
	//strInit vraci 1 při chybe
	if (strInit(&s)) {
		//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
	}

	int state = START;
	while (1) {
		if (pom > 0) {//pozustatek z minuleho cteni
			c = pom;
			pom = -1;
		}
		else {
			c = getc(source);
			character++;
			if (c == '\n') {
				line++;
				character = 0;
			}
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
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			//ulozi prvni nulu v cisle
			else if (c == '0') {
				state = FLOAT_OR_INT;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c >= '1' && c <= '9') {
				state = INT_PART;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c == ';') {
				Token->typ = SEMICOLON;
				strFree(&s);
				return 1;
			}
			else if (c == '\\') {
				state = ESCAPE;
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
				strFree(&s);
				return 1;
			}
			else if (c == '!' || c == '<' || c == '>' || c == '=') {
				state = TWO_CHAR_OPER;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c == ')') {
				Token->typ = PARENTHESIS_CLOSING;
				strFree(&s);
				return 1;
			}
			else if (c == '(') {
				Token->typ = PARENTHESIS_OPENING;
				strFree(&s);
				return 1;
			}
			else if (c == '}') {
				Token->typ = BRACES_CLOSING;
				strFree(&s);
				return 1;
			}
			else if (c == '{') {
				Token->typ = BRACES_OPENING;
				strFree(&s);
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
				strFree(&s);
				return 1;
			}
			else {
				fprintf(stderr, "Unknown symbol\n");
			}
			break;
		case INT_PART:
			if (c >= '0' && c <= '9') {
				state = INT_PART;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c == 'e' || c == 'E') {
				state = EXPONENT_CHECK;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c == '.') {
				state = FLOAT_PART;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else {
				int val = 42;
				if (!strToInt(&s, &val)) {
					//FatalError(99, "%s-%d: Chyba pri nacteni celeho cisla.", ERROR_MESSAGES[ERROR_ALLOC], line);
					strFree(&s);
					return 42;
				}
				pom = c;
				Token->typ = TYPE_INTEGER;
				Token->value.intVal = val;
				strFree(&s);
				return 1;
			}
			break;
		case FLOAT_OR_INT:
			//prvni nula byla ulozena pocatecnim stavu ted je necham byt
			if (c == '0') {
				state = FLOAT_OR_INT;
			}
			else if (c >= '1' && c<='9') {
				state = INT_PART;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c == '.') {
				state = FLOAT_PART;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c == 'e' || c == 'E') {
				state = EXPONENT_CHECK;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else {
				int val = 42;
				if (!strToInt(&s, &val)) {
					//todo error
				strFree(&s);
					return 42;
				}
				Token->typ = TYPE_INTEGER;
				Token->value.intVal = val;
				strFree(&s);
				pom = c;
				return 1;
			}
			break;
		case FLOAT_PART:
			if (c >= '0' && c <= '9') {
				state = FLOAT_PART;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c == 'e' || c == 'E') {
				state = EXPONENT_CHECK;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else {
				double val = 42;
				if (!strToDouble(&s, &val)) {
					//todo error
					strFree(&s);
					return 42;
				}
				Token->typ = TYPE_DOUBLE;
				Token->value.doubleVal = val;
				strFree(&s);
				pom = c;
				return 1;
			}
			break;
		case EXPONENT_CHECK:
			if (c >= '0' && c <= '9') {
				state = EXPONENT;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else if (c == '+' || c == '-') {
				state = EXPONENT_PLUS_MINUS_CHECK;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else {
				//error
				pom = c;
				strFree(&s);
				return 42;
			}
			break;
		case EXPONENT_PLUS_MINUS_CHECK:
			if (c >= '0' && c <= '9') {
				state = EXPONENT;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else {
				//error
				pom = c;
				strFree(&s);
				return 42;
			}
			break;
		case EXPONENT:
			if (c >= '0' && c <= '9') {
				state = EXPONENT;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else {
				double val = 42;
				if (!strToDouble(&s, &val)) {
					//todo error
					strFree(&s);
					return 42;
				}
				Token->typ = TYPE_DOUBLE;
				Token->value.doubleVal = val;
				pom = c;
				strFree(&s);
				return 1;
			}
			break;
		case ESCAPE:
			switch (c) {
			case 'b':
			case 'B':
				state = BINARY_NUMBER;
				break;
			case '0':
				state = OCTAL_NUMBER;
				break;
			case 'x':
			case 'X':
				state = HEX_DEC_NUMBER;
				break;
			default:
				//TODO: error
				strFree(&s);
				return 42;
				break;
			}
			break;
		case BINARY_NUMBER:
			if (c == '0' || c == '1') {
				//zahazuji nevyznamne nuly
				if (!(c == '0' && strGetLength(&s) == 0)) {
					if (strAddChar(&s, c)) {
						//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
					}
				}
			}
			else {
				pom = c;
				int val = 0;
				if (!strBinToInt(&s, &val)) {
					strFree(&s);
					return 42;
				}
				Token->typ = TYPE_INTEGER;
				Token->value.intVal = val;
				strFree(&s);
				return 1;
			}
			break;
		case OCTAL_NUMBER:
			if (c >= '0' && c <= '7') {
				if (!(c == '0' && strGetLength(&s) == 0)) {
					if (strAddChar(&s, c)) {
						//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
					}
				}
			}
			else {
				pom = c;
				int val = 0;
				if (!strOctToInt(&s, &val)) {
					strFree(&s);
					return 42;
				}
				Token->typ = TYPE_INTEGER;
				Token->value.intVal = val;
				strFree(&s);
				return 1;
			}
			break;
		case HEX_DEC_NUMBER:
			if (c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F') {
				if (!(c == '0' && strGetLength(&s)==0)) {
					if (strAddChar(&s, c)) {
						//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
					}
				}
			}
			else {
				pom = c;
				int val = 0;
				if (!strHexToInt(&s, &val)) {
					strFree(&s);
					return 42;
				}
				Token->typ = TYPE_INTEGER;
				Token->value.intVal = val;
				strFree(&s);
				return 1;
			}
			break;
		case LOGICAL_AND:
			if (c == '&') {
				Token->typ = LOG_AND_OPER;
				strFree(&s);
				return 1;
			}
			else {
				/*Abych se zotavil po teto chybe a docetl soubor*/
				pom = c;
				strFree(&s);
				return 42;
			}
			break;
		case LOGICAL_OR:
			if (c == '|') {
				Token->typ = LOG_OR_OPER;
				strFree(&s);
				return 1;
			}
			else {
				/*Abych se zotavil po teto chybe a docetl soubor*/
				pom = c;
				strFree(&s);
				return 42;
			}
			break;
		case STRING:
			if (c == EOF) {
				//error
			}
			else if (c == '\\') {
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
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
				strFree(&s);
				return 1;
			}
			else if (c < 1 || c>255) {//nejaky znak mimo ASCII chyba

			}
			else {//uloz si znak
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			break;
		case IDENTIFICATOR:
			if (isalnum(c) || c == '_') {
				state = state;
				if (strAddChar(&s, c)) {
					//FatalError(99, ERROR_MESSAGES[ERROR_ALLOC]);
				}
			}
			else {
				pom = c;
				Token->typ = TYPE_IDENTIFICATOR;
				if (strInit(&(Token->value.stringVal))) {
					//return stringval
				}
				if (strCopyString(&(Token->value.stringVal), &s)) {
					strFree(&s);
					return 0;//todo
				}
				strFree(&s);
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
				strFree(&s);
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
				strFree(&s);
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
				strFree(&s);
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
				strFree(&s);
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
			if (c == '=') {
				switch (s.str[0]) {
				case '!':
					Token->typ = NOT_EQUAL_OPER;
					break;
				case '<':
					Token->typ = LESS_EQUAL;
					break;
				case '>':
					Token->typ = GREATER_EQUAL;
					break;
				case '=':
					Token->typ = EQUAL;
					break;
				default:
					break;
				}
			}
			else {
				pom = c;
				switch (s.str[0]) {
				case '!':
					Token->typ = LOG_NOT_OPER;
						break;
				case '<':
					Token->typ = LESS;
						break;
				case '>':
					Token->typ = GREATER;
						break;
				case '=':
					Token->typ = SET_OPER;
						break;
				default:
					break;
				}
			}
			strFree(&s);
			return 1;
			break;
		case DECREMENT_OPERATOR:
			if (c == '-') {
				Token->typ = DECREMENTATION;
				strFree(&s);
				return 1;
			}
			else {
				pom = c;
				Token->typ = MINUS;
				strFree(&s);
				return 1;
			}
			break;
		case INCREMENT_OPERATOR:
			if (c == '+') {
				Token->typ = INCREMENTATION;
				strFree(&s);
				return 1;
			}
			else {
				pom = c;
				Token->typ = PLUS;
				strFree(&s);
				return 1;
			}
			break;
		default:
			break;
		}
	}
}

int strToInt(string* forConversion, int* val){
	char* ptr = NULL;
	*val = (int)strtol(forConversion->str, &ptr, 10);
	if (ptr == forConversion->str || *ptr != '\0') return 0;
	return 1;
}

int strBinToInt(string* forConversion, int* val) {
	char* ptr = NULL;

	*val = (int)strtol(forConversion->str, &ptr, 2);
	if (ptr == forConversion->str || *ptr != '\0') return 0;
	if (*val > 255 || *val < 1) return 0;
	return 1;
}
int strHexToInt(string* forConversion, int* val) {
	char* ptr = NULL;

	*val = (int)strtol(forConversion->str, &ptr, 16);
	if (ptr == forConversion->str || *ptr != '\0') return 0;
	if (*val > 255 || *val < 1) return 0;
	return 1;
}
int strOctToInt(string* forConversion, int* val) {
	char* ptr = NULL;

	*val = (int)strtol(forConversion->str, &ptr, 8);
	if (ptr == forConversion->str || *ptr != '\0') return 0;
	if (*val > 255 || *val < 1) return 0;
	return 1;
}

int strToDouble(string* forConversion, double* val) {
	char* ptr = NULL;
	*val = (double)strtod(forConversion->str, &ptr);
	if (ptr == forConversion->str || *ptr != '\0') return 0;
	return 1;
}

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