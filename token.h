#ifndef TOKEN_H
#define TOKEN_H
#include "str.h"
/*
Tabulka token� - typ a union mezi pointrem do tabulky symbol� a ordin�ln� hodnotou
typ je enum ("integer","double" nebo "d�len�", "n�soben�", etc.)
typ m��e b�t i oper�tor (*,/, etc)
getToken vrac� "STAV" jestli je v po��dku, nesestavil token a parametrem ukazatel do v��e zm�n�n� tabulky token�

*/

enum tTokenTypes
{
	INTEGER,
	DOUBLE,
	STRING,
	??,
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