﻿#ifndef SCANNER_H
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
 * Hlavni fce lexykalniho analyzatoru pokud nastane chyba nevrati naalokovany token
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

int strBinToInt(string* forConversion, int* val);
int strHexToInt(string* forConversion, int* val);
int strOctToInt(string* forConversion, int* val);
int unescapeStr(string*);
int isKeyWord(string*);

/*
 * uvolni misto po tokenu
 * @param token[in] token pro vycisteni
 */
void freeTokenMem(tToken);

typedef struct tTQelem{
	tToken token;
	struct tTQelem* lptr;
	struct tTQelem* rptr;
} *tTokenQueueElem;

typedef struct {
	tTokenQueueElem First;//prvni prvek (ten co jde ven)
	tTokenQueueElem Last;//posledni prvek (ten co prisel posledni
} tTokenQueue;
extern tTokenQueue *TQueue;
/*
 * Inicializuje frontu v glob promenne 
 */
void TQInit();
//void TQEnqueue(tToken*);toto je nas unget
/*
 * Tuto fci volat jen z scanneru. 
 * @param token[out] vrati token pokud nejaky ve fronte je jiank NULL
 */
void TQDequeue(tToken*);//toto volat jen ze scanneru
/**
* Uvolni pamet po fronte tokenu
*/
void freeTokenQueue();

/*
 * Zkopíruje token na frontu a vycisti ho
 * @param token[in] vraceny token
 */
void ungetToken(tToken*);

/**
 * Uvolni pamet po fronte tokenu
 */
void freeTokenQueue();
#endif
