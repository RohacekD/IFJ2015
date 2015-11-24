#ifndef TOKEN_H
#define TOKEN_H
#include "str.h"
#include <stdbool.h>
/*
Tabulka tokenů - typ a union mezi pointrem do tabulky symbolů a ordinální hodnotou
typ je enum ("integer","double" nebo "dělení", "násobení", etc.)
typ může být i operátor (*,/, etc)
getToken vrací "STAV" jestli je v pořádku, nesestavil token a parametrem ukazatel do výše zmíněné tabulky tokenů

*/

typedef enum tTokenTypes
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
	SET_OPER,
	INCREMENTATION,
	DECREMENTATION,
	LOG_AND_OPER,
	LOG_OR_OPER,
	LOG_NOT_OPER,
	MINUS,
	PLUS,
	PARENTHESIS_OPENING,
	PARENTHESIS_CLOSING,
	BRACES_OPENING,
	BRACES_CLOSING,
	SEMICOLON,
	COMMA,

	KEYW_AUTO,
	KEYW_CIN,
	KEYW_COUT,
	KEYW_INT,
	KEYW_DOUBLE,
	KEYW_STRING,
	KEYW_IF,
	KEYW_ELSE,
	KEYW_FOR,
	KEYW_WHILE,
	KEYW_RETURN,

	END_OF_FILE
}TokenTypes;
typedef union utokenValue
{
	//nejak implementovany pointer do tabulky symbolu
	int intVal;
	double doubleVal;
	string stringVal;
	bool boolVal;
}tokenValue;
typedef struct stToken{
	TokenTypes typ;
	tokenValue value;
} tToken;
#endif