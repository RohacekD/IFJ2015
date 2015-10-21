#ifndef SCANNER_H
#define SCANNER_H
#include <stdbool.h>
#include "token.h"
extern int line;
int getToken(int*, FILE*);
/*
 prevede string v tokenu na hodnotu double
 konverze vedeckeho zapisu cisla na double
 vraci int urcujici uspech/neuspech
 v pointeru vraci hodnotu
*/
int strToDouble(string, double*);
int strToInt(string, int*);
int strToBool(string, bool*);
#endif