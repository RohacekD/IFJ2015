//hlavickovy soubor pro praci s nekonecne dlouhymi retezci

#ifndef STR_H_
#define STR_H_

#include <string.h>
#include <malloc.h>

#include "error.h"

typedef struct
{
	char* str;		// misto pro dany retezec ukonceny znakem '\0'
	int length;		// skutecna delka retezce
	int allocSize;	// velikost alokovane pameti
} string;

int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strConConstString(string *s1, char *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
int strGetLength(string *s);
int strCharToString(string* s, char* c);

/**
 * vestavena funkce jazyka IFJ2015
 * @param s vstupni retezec
 * @param i zacatek podretezce
 * @param n delka podretezce
 * @param err simulace exception C++11; 
 * @return vraci podretezec
 */
string substr(string s, int i, int n, ERR_CODES *err);

/**
 * vestavena funkce jazyka IFJ2015
 * @param s1
 * @param s2
 * @return konkatenace retezcu
 */
string concat(string s1, string s2);

/**
 * @param s retezec
 * @return vrati delku retezce
 */        
int length(string s);
#endif /* STR_H_ */
