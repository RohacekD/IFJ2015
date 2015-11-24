﻿#include "scanner.h"
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
	STRING_BINARY_NUMBER,
	STRING_OCTAL_NUMBER,
	STRING_HEX_DEC_NUMBER,
	INT_PART,
	FLOAT_OR_INT,
	FLOAT_PART,
	EXPONENT,
	EXPONENT_CHECK,
	EXPONENT_PLUS_MINUS_CHECK
};
/* @var int citac radku*/
int line = 1;
/* @var int citac znaku na radku*/
int character = 0;
/* @var int priznak chyby */
int errorFlag = 0;

int getToken(tToken *Token, FILE* source) {
	/* @var c int actual character*/
	int c = -1;
	/* @var prevRest int poslední načtený znak (Zbytek posledního průchodu) */
	static int pom = -1;
	/* @var string s Uklada aktualne cteny string pokud je treba, pri kazdem 
	 * konci teto funkce musi byt volano strFree.
	 */
	string s;
	//strInit vraci 1 při chybe
	if (strInit(&s)) {
		FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
	}
	string escape;


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
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			//ulozi prvni nulu v cisle
			else if (c == '0') {
				state = FLOAT_OR_INT;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else if (c >= '1' && c <= '9') {
				state = INT_PART;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
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
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
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
			else if (c == ',') {
				Token->typ = COMMA;
				strFree(&s);
				return 1;
			}
			else if (c==EOF) {
				Token->typ = END_OF_FILE;
				strFree(&s);
				if (errorFlag) {
					FatalError(errorFlag, ERR_MESSAGES[ERR_LEX]);
				}
				return 1;
			}
			else {
				errorFlag = 1;
				Warning("%sLine - %d:%d\t-  Unknown symbol.\n", ERR_MESSAGES[ERR_LEX], line, character);
			}
			break;
		case INT_PART:
			if (c >= '0' && c <= '9') {
				state = INT_PART;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else if (c == 'e' || c == 'E') {
				state = EXPONENT_CHECK;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else if (c == '.') {
				state = FLOAT_PART;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else {
				int val = 42;
				if (!strToInt(&s, &val)) {
					FatalError(99, "%s-%d: Chyba pri nacteni celeho cisla.", ERR_MESSAGES[ERR_ALLOC], line);
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
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else if (c == '.') {
				state = FLOAT_PART;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else if (c == 'e' || c == 'E') {
				state = EXPONENT_CHECK;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else {
				int val = 42;
				if (!strToInt(&s, &val)) {
					errorFlag = 1;
					Warning("%sLine - %d:%d\t-  Nepodarilo se nacist ciselny literal.\n",ERR_MESSAGES[ERR_LEX], line, character);
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
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else if (c == 'e' || c == 'E') {
				state = EXPONENT_CHECK;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else {
				double val = 42;
				if (!strToDouble(&s, &val)) {
					errorFlag = 1;
					Warning("%sLine - %d:%d\t-  Nepodarilo se nacist ciselny literal.\n",ERR_MESSAGES[ERR_LEX], line, character);
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
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else if (c == '+' || c == '-') {
				state = EXPONENT_PLUS_MINUS_CHECK;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			//nemuzeme napsat exponent a neudat ho
			else{
				errorFlag = 1;
				Warning("%sLine - %d:%d\t-  Exponent musi obsahovat validni cislici.\n",ERR_MESSAGES[ERR_LEX], line, character);
				pom = c;
				strFree(&s);
				return 42;
			}
			break;
		case EXPONENT_PLUS_MINUS_CHECK:
			if (c >= '0' && c <= '9') {
				state = EXPONENT;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else {
				errorFlag = 1;
				Warning("%sLine - %d:%d\t-  Exponent musi obsahovat validni cislici.\n",ERR_MESSAGES[ERR_LEX], line, character);
				pom = c;
				strFree(&s);
				return 42;
			}
			break;
		case EXPONENT:
			if (c >= '0' && c <= '9') {
				state = EXPONENT;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			else {
				double val = 42;
				if (!strToDouble(&s, &val)) {
					errorFlag = 1;
					Warning("%sLine - %d:%d\t-  Nepodarilo se nacist ciselny literal.\n",ERR_MESSAGES[ERR_LEX], line, character);
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
				errorFlag = 1;
				Warning("%sLine - %d:%d\t-  Ocekavan symbol pro ciselnou soustavu.\n",ERR_MESSAGES[ERR_LEX], line, character);
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
						FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
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
				//zahazuji nevyznamne nuly
				if (!(c == '0' && strGetLength(&s) == 0)) {
					if (strAddChar(&s, c)) {
						FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
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
				//zahazuji nevyznamne nuly
				if (!(c == '0' && strGetLength(&s)==0)) {
					if (strAddChar(&s, c)) {
						FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
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
				pom = c;
				strFree(&s);
				errorFlag = 1;
				Warning("%sLine - %d:%d\t-  Binarni and neni podporovan v IFJ2015.\n",ERR_MESSAGES[ERR_LEX], line, character-1);
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
				errorFlag = 1;
				Warning("%sLine - %d:%d\t-  Binarni or neni podporovan v IFJ2015.\n",ERR_MESSAGES[ERR_LEX], line, character - 1);
				pom = c;
				strFree(&s);
				return 42;
			}
			break;
		case STRING:
			if (c == EOF) {
				errorFlag = 1;
				Warning("%sLine - %d:%d\t-  Necekany konec souboru.\n",ERR_MESSAGES[ERR_LEX], line, character);
			}
			else if (c == '\\') {
				/*if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}*/
				
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
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			break;
		case STRING_ESCAPE:
			switch (c) {
			case 'b':
			case 'B':
				if (strInit(&escape)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
				state = STRING_BINARY_NUMBER;
				break;
			case '0':
				if (strInit(&escape)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
				state = STRING_OCTAL_NUMBER;
				break;
			case 'x':
			case 'X':
				if (strInit(&escape)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
				state = STRING_HEX_DEC_NUMBER;
				break;
			case '\\':
				if (strAddChar(&s, '\\')) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
				state = STRING;
				break;
			case '\n':
				if (strAddChar(&s, '\n')) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
				state = STRING;
				break;
			case '\t':
				if (strAddChar(&s, '\t')) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
				state = STRING;
				break;
			case '\"':
				if (strAddChar(&s, '\"')) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
				state = STRING;
				break;
			default:
				errorFlag = 1;
				Warning("%sLine - %d:%d\t-  Ocekavan symbol pro ciselnou soustavu.\n", ERR_MESSAGES[ERR_LEX], line, character);
				strFree(&s);
				return 42;
				break;
			}
			break;
		case STRING_BINARY_NUMBER:
			if ((c == '0' || c == '1')) {
				if (strGetLength(&escape) < 8) {
					if (strAddChar(&escape, c)) {
						FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
					}
				}
				else {
					int val;
					if (strBinToInt(&escape, &val) && val >= 1 && val <= 255) {
						if (strAddChar(&s, (char)val)) {
							FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
						}
					}
					else {
						errorFlag = 1;
						Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
					}
					strFree(&escape);
					state = STRING;
				}
			}
			else {
				if (strGetLength(&escape) < 8) {
					errorFlag = 1;
					Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
				}
				if (strGetLength(&escape) == 8) {
					int val;
					if (strBinToInt(&escape, &val) && val >= 1 && val <= 255) {
						if (strAddChar(&s, (char)val)) {
							FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
						}
					}
					else {
						errorFlag = 1;
						Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
					}
				}
				pom = c;
				strFree(&escape);
				state = STRING;
			}
			break;
		case STRING_OCTAL_NUMBER:
			if ((c >= '0' && c <= '7')) {
				if (strGetLength(&escape) < 3) {
					if (strAddChar(&escape, c)) {
						FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
					}
				}
				else {
					int val;
					if (strOctToInt(&escape, &val) && val >= 1 && val <= 255) {
						if (strAddChar(&s, (char)val)) {
							FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
						}
					}
					else {
						errorFlag = 1;
						Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
					}
					strFree(&escape);
					state = STRING;
				}
			}
			else {
				if (strGetLength(&escape) < 3) {
					errorFlag = 1;
					Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
				}
				if (strGetLength(&escape) == 3) {
					int val;
					if (strOctToInt(&escape, &val) && val >= 1 && val <= 255) {
						if (strAddChar(&s, (char)val)) {
							FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
						}
					}
					else {
						errorFlag = 1;
						Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
					}
				}
				pom = c;
				strFree(&escape);
				state = STRING;
			}
			break;
		case STRING_HEX_DEC_NUMBER:
			if ((c >= '0' && c <= '9')||(c >= 'a' && c <= 'f')|| (c >= 'A' && c <= 'F')) {
				if (strGetLength(&escape) < 2) {
					if (strAddChar(&escape, c)) {
						FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
					}
				}
				else {
					int val;
					if (strHexToInt(&escape, &val) && val >= 1 && val <= 255) {
						if (strAddChar(&s, (char)val)) {
							FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
						}
					}
					else {
						errorFlag = 1;
						Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
					}
					strFree(&escape);
					state = STRING;
				}
			}
			else {
				if (strGetLength(&escape) < 2) {
					errorFlag = 1;
					Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
				}
				if (strGetLength(&escape) == 2) {
					int val;
					if (strHexToInt(&escape, &val) && val >= 1 && val <= 255) {
						if (strAddChar(&s, (char)val)) {
							FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
						}
					}
					else {
						errorFlag = 1;
						Warning("%sLine - %d:%d\t-  Retezec \"%s\" neni platnou escape sekvenci.\n", ERR_MESSAGES[ERR_LEX], line, character, escape.str);
					}
				}
				pom = c;
				strFree(&escape);
				state = STRING;
			}
			break;
		case IDENTIFICATOR:
			//ctu cislice, pismena nebo podtrzitka
			if (isalnum(c) || c == '_') {
				state = state;
				if (strAddChar(&s, c)) {
					FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
				}
			}
			//konec identifikatoru
			else {
				pom = c;
				int isIdent = isKeyWord(&s);
				Token->typ = isIdent;
				if (isIdent == TYPE_IDENTIFICATOR) {
					if (strInit(&(Token->value.stringVal))) {
						FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
					}
					if (strCopyString(&(Token->value.stringVal), &s)) {
						strFree(&s);
						FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
					}
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
				Warning("%sLine - %d:%d\t-  Necekany konec souboru.\n", ERR_MESSAGES[ERR_LEX], line, character);
				strFree(&s);
				return 42;
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
				Warning("%sLine - %d:%d\t-  Necekany konec souboru.\n", ERR_MESSAGES[ERR_LEX], line, character);
				strFree(&s);
				return 42;
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
				Warning("%sLine - %d:%d\t-  Necekany konec souboru.\n", ERR_MESSAGES[ERR_LEX], line, character);
				strFree(&s);
				return 42;
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
* KEY WORDS
* auto, cin, cout, double, else,
* for, if, int, return, string
*/
/*
 * Pole vsech klicovych slov
 * @warning nechte v tomto poradi dulezite pro funkcnost
 */
const char* keyWords[] = {
	"auto",
	"cin",
	"cout",
	"int",
	"double",
	"string",
	"if",
	"else",
	"for",
	"while",
	"return",
	""
};

/*
 * @depractated
 * @bug this whole function is one big bug, do not use
 */
int unescapeStr(string* s) {
	string ret, help;
	//strInit(&help);
	strInit(&ret);
	int i = 0;
	while (s->str[i++]!='\0') {
		if (s->str[i] == '\\') {
			i++;
		}
		else {
			if (strAddChar(&ret, s->str[i])) {
				FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
			}
		}
	}
}

/*
 * Prohleda tabulku klicovych slov a zjisti zda prijaty string neni mezi nimi 
 *
 * @uses keyWords
 * @param string* s zkoumany string
 * @return TokenTypes Type of token (TYPE_IDENTIFICATOR|[KEYW_AUTO-KEYW_RETURN])
 *
 */
int isKeyWord(string* s) {
	int ret = 0;
	int i = 0;
	for (i = 0; i < 12; i++) {
		if (strCmpConstStr(s, keyWords[i])==0) {
			break;
		}
	}
	if (i == 12) {
		return TYPE_IDENTIFICATOR;
	}
	return KEYW_AUTO + i;
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