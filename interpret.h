#ifndef INTERPRET_H_
#define INTERPRET_H_
#include "insTape.h"
#include "frameStack.h"
/*
 * Dostane instrukci a od te zacne vykonavat program.
 * @param ins[in]	- instrukce od ktere zacit vykonavat
 * @return Vraci 0 pokud nastane chyba, jinak 1;
 */
int executeTape(tInsTapeInsPtr);
/*
 * Dostane instrukci a 
 * @param ins[in]	- instrukce ktera se ma provest.
 */
int executeIns(tInsTapeInsPtr);

int findVariable(tFrame*, char*);


#endif //INTERPRET_H_