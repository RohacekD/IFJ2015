#ifndef INTERPRET_H_
#define INTERPRET_H_
#include "insTape.h"
#include "frameStack.h"
#include "error.h"
/*
 * Dostane instrukci a od te zacne vykonavat program.
 * @param ins[in]	- instrukce od ktere zacit vykonavat
 * @return Vraci 0 pokud nastane chyba, jinak 1;
 */
int executeTape(tInsTapeInsPtr);
/*
 * Dostane instrukci a 
 * @param ins[in]	- instrukce ktera se ma provest.
 * 
 * @return Vraci 0 pokud nastane chyba, jinak 1;
 */
int executeIns(tInsTapeInsPtr, tDLList*);

int findVariable(tDLList*, string*);


#endif //INTERPRET_H_