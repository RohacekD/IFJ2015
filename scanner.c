#include "scanner.h"
#include "str.h"
#include <stdio.h>
/*
 * Výèet možných stavù FSM.
 */
enum states {
	START,
	STRING,
	STRING_END,
	STRING_ESCAPE,
	ERROR
};
/* @var int citac radku*/
int line = 1;


/*
* KEY WORDS
* auto, cin, cout, double, else,
* for, if, int, return, string
*
*
*/
/*
 * Musi vracet nìjakou reprezentaci tokenu
 * Dostávat ukazatel na otevøený soubor (DI)
 */
int getToken(int* p, FILE* source) {

	/* @var prevRest int poslední naètený znak (Zbytek posledního prùchodu) */
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
		default:
			break;
		}
	}
}