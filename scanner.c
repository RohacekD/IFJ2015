#include "scanner.h"
#include "str.h"
#include <stdio.h>
/*
 * V��et mo�n�ch stav� FSM.
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
 * Musi vracet n�jakou reprezentaci tokenu
 * Dost�vat ukazatel na otev�en� soubor (DI)
 */
int getToken(int* p, FILE* source) {

	/* @var prevRest int posledn� na�ten� znak (Zbytek posledn�ho pr�chodu) */
	static int prevRest = -1;
	/* @var c int actual character*/
	static int c = -1;

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
			else if (c == "\"") {
				state = STRING;
			}
			else if (c == "/") {

			}
			break;
		case STRING:
			if (c == EOF) {
				//error
			}
			else if (c == "\\") {
				state = STRING_ESCAPE;
			}
			else if (c == "\"") {//konec retezce uloz ho

			}
			else if (c < 1 || c>255) {//nejaky znak mimo ASCII chyba

			}
			else {//uloz si znak

			}
			break;
		case DIVISION_COMMENT:
			if (c == "*") {
				state = BLOCK_COMMENT;
			}
			else if (c == "/") {
				state = LINE_COMMENT;
			}
			else {//jednoznakovy operator deleni

			}
			break;
		case BLOCK_COMMENT:
			if (c == EOF) {
				//TODO: error
			}
			else if (c=="/") {
				state = NESTED_BLOCK_COMMENT_CHECK;
			}
			else if (c=="*") {
				state = END_BLOCK_COMMENT;
			}
			break;
		case LINE_COMMENT:
			if (c == "\n") {//konec radkoveho komentare hledame dalsi lexem
				state = START;
			}
			else if (c == EOF) {//TODO: je to chyba??

			}
			else {//komentar pokracuje
				
			}
			break;
		case END_BLOCK_COMMENT:
			if (c == "*") {
				//z�staneme tady koment�� m��e kon�it x hv�zdi�kama
			}
			else if (c == EOF) {
				//todo: error
			}
			else if (c == "/") {//konec blokov�ho koment��e jdeme hledat dal�� lex�m
				state = START;
			}
			else {
				state = BLOCK_COMMENT;
			}
			break;
		case NESTED_BLOCK_COMMENT_CHECK:
			if (c == "*") {
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

void f() {

}