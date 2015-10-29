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
	END_BLOCK_COMMENT
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
int getToken(int* p, FILE* source) {

	/* @var prevRest int poslední načtený znak (Zbytek posledního průchodu) */
	static int prevRest = -1;
	/* @var c int actual character*/
	static int c = -1;
	tToken token = {
		.typ = END_OF_FILE,
	};
	string s;
	//strInit vraci 1 při chybe
	if (strInit(&s)) {
		//return ERROR_ALOK;
	}

	int state = START;
	while (1) {
		c = getc(source);
		switch (state)
		{
		case START:
			if (c == 35) {
				state = STRING;
			}
			else if (isspace(c)) {
				state = START;
			}
			else if (c == '\"') {
				state = STRING;
			}
			else if (c == '/') {

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
			else if (c == '\"') {//konec retezce uloz ho

			}
			else if (c < 1 || c>255) {//nejaky znak mimo ASCII chyba

			}
			else {//uloz si znak
				strAddChar(&s, c);
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

			}
			break;
		case BLOCK_COMMENT:
			if (c == EOF) {
				//TODO: error
			}
			else if (c=='/') {
				state = NESTED_BLOCK_COMMENT_CHECK;
			}
			else if (c=='*') {
				state = END_BLOCK_COMMENT;
			}
			break;
		case LINE_COMMENT:
			if (c=='\n') {//konec radkoveho komentare hledame dalsi lexem
				state = START;
			}
			else if (c == EOF) {//TODO: je to chyba??

			}
			else {//komentar pokracuje
				
			}
			break;
		case END_BLOCK_COMMENT:
			if (c == '*') {
				//zůstaneme tady komentář může končit x hvězdičkama
			}
			else if (c == EOF) {
				//todo: error
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
				//TODO: error 
			}
			else if (c == EOF) {
				//TODO: error
			}
			else {
				state = BLOCK_COMMENT;
			}
			break;
		default:
			break;
		}
	}
}

int strToInt(string* forConversion, int* val){
	/** todo rozdelane, mozno vymazat cele
	number = strtol(cString, &pEnd, 0);
	//kontrola
	if (errno == ERANGE || *pEnd != '\0' || number > INT_MAX || number < INT_MIN) {
		return 0;
	}
	*converted = (int) number;
	*converted */
}

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
