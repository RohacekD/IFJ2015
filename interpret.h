#ifndef INTERPRET_H_
#define INTERPRET_H_
#include "insTape.h"
#include "frameStack.h"
#include "error.h"
#include "tabSym.h"
/*
 * Dostane instrukci a od te zacne vykonavat program.
 * Vykonava skokove instrukce ve svem tele
 * @param ins[in]	-	instrukce od ktere zacit vykonavat
 * @return Vraci 0 pokud nastane chyba, jinak 1;
 */
int executeTape(tInsTapeInsPtr);
/*
 * Dostane instrukci a vykona ji, nasledne posune na dalsi instrukci
 * @param ins[in]	-	instrukce ktera se ma provest.
 * 
 * @return Vraci 0 pokud nastane chyba, jinak 1;
 */
int executeIns(tInsTapeInsPtr* , tStack* );

void tTabSymToFrame(tBSTNodePtr node, tFrameContainer* frameContainer);

#endif //INTERPRET_H_