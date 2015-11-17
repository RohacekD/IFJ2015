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
	TYPE_INTEGER,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_BOOL,
	TYPE_IDENTIFICATOR,
	DIVISION,
	MULTIPLY,
	ADD,
	SUBTRACTION,
	GREATER,
	GREATER_EQUAL,
	LESS,
	LESS_EQUAL,
	EQUAL,
	NOT_EQUAL_OPER,
	INCREMENTATION,
	DECREMENTATION,
	LOG_AND_OPER,
	LOG_OR_OPER,
	//LOG_NOT_OPER, neni v IFJ2015
	MINUS,
	PLUS,
	KEYW_INT,
	PARENTHESIS_OPENING,
	PARENTHESIS_CLOSING,
	SEMICOLON,
	END_OF_FILE
};
typedef union
{
	//nejak implementovany pointer do tabulky symbolu
	int intVal;
	double doubleVal;
	string stringVal;
	//etc
}tokenValue;
typedef struct {
	int typ;
	tokenValue value;
} tToken;
#endif
