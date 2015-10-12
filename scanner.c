#include "scanner.h"

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
smth getTocen(char*, smth, smth) {

	/* @var prevRest int poslední naètený znak (Zbytek posledního prùchodu) */
	static int prevRest = -1;
	/* @var c int actual character*/
	static int c = -1;

	int state = START;
	while (1) {
		c = getc(FILE*);
		
		switch (state)
		{
		case START:
			if (c == 35) {
				state = STRING;
			}
			else if (isspace(c)) {
				state = START;
			}
			break;
		default:
			break;
		}
	}
}