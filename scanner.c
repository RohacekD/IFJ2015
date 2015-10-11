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

smth getTocen(char*, smth, smth) {
	int state = START;
	/* @var c char actual character*/
	char c;
	while (1) {
		c = getc(char*);
		
		switch (state)
		{
		case START:
			if (c == 35) {
				state = STRING;
			}
			break;
		default:
			break;
		}
	}
}