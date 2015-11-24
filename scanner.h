#ifndef SCANNER_H
#define SCANNER_H
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "token.h"
/* @var int citac radku*/
extern int line;
/* @var int citac znaku na radku*/
extern int character;
/* @var int priznak chyby */
extern int errorFlag;
/*
 * Hlavni fce lexykalniho analyzatoru
 * @param tToken v teto strukutre je vracen nacteny token
 * @param FILE[in] inject cteneho souboru
 * @return 1 pokud prosla analyza ok
 */
int getToken(tToken*, FILE*);
/*
 prevede string v tokenu na hodnotu double
 konverze vedeckeho zapisu cisla na double
 vraci int urcujici uspech/neuspech
 v pointeru vraci hodnotu
*/
int strToDouble(string*, double*);

/**
 * Prevede obsah string na int. Pokud je obsah platnym celym cislem.
 * @param forConversion	-	string, ktery bude preveden na int
 * @param val	-	ukazatel pomoci, ktereho vraci prevedenou hodnotu (v pripade chyby neprepisuje)
 * @return	1 - pokud prevod probehne v poradku, jinak 0.
 */
int strToInt(string* forConversion, int* val);
/**
 * Prevede obsah string na bool. Pokud obsahuje true/false.
 * @param forConversion	-	string, ktery bude preveden na bool
 * @param val	-	ukazatel pomoci, ktereho vraci prevedenou hodnotu (v pripade chyby neprepisuje)
 * @return	1 - pokud prevod probehne v poradku, jinak 0.
 */
//int strToBool(string* forConversion, bool* val);
int strBinToInt(string* forConversion, int* val);
int strHexToInt(string* forConversion, int* val);
int strOctToInt(string* forConversion, int* val);
int unescapeStr(string*);
int isKeyWord(string*);
#endif
