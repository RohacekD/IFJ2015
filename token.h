#ifndef TOKEN_H
#define TOKEN_H
#include "str.h"
/*
Tabulka tokenů - typ a union mezi pointrem do tabulky symbolů a ordinální hodnotou
typ je enum ("integer","double" nebo "dělení", "násobení", etc.)
typ může být i operátor (*,/, etc)
getToken vrací "STAV" jestli je v pořádku, nesestavil token a parametrem ukazatel do výše zmíněné tabulky tokenů

*/

enum tTokenTypes
{
	INTEGER,
	DOUBLE,
	STRING,
	BOOL,
	DIVISION,
	MULTIPLY,
	ADD,
	SUBTRACTION,
	GREATER,
	GREATER_EQUAL,
	LESS,
	LESS_EQUAL,
	EQUAL,
	NOT_EQUAL,
	INCREMENTATION,
	DECREMENTATION,
	LOG_AND,
	LOG_OR,
	LOG_NOT,
	MINUS,
	KEYW_INT,
	END_OF_FILE
};

typedef struct {
	int typ;
	union 
	{
		//nejak implementovany pointer do tabulky symbolu
		int intVal;
		double doubleVal;
		string stringVal;
		//etc
	};
} tToken;
#endif