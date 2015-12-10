#ifndef INTERPRET_H_
#define INTERPRET_H_
#include "insTape.h"
#include "frameStack.h"
#include "error.h"
#include "tabSym.h"
#include "scanner.h"
#include <ctype.h>
/*
 * Dostane instrukci a od te zacne vykonavat program.
 * Vykonava skokove instrukce ve svem tele
 * @param ins[in]	-	instrukce od ktere zacit vykonavat
 * @return Vraci ERR_OK pokud nastane OK, prislusny error kod;
 */
int executeTape(tInsTapeInsPtr);
/*
 * Dostane instrukci a vykona ji, nasledne posune na dalsi instrukci
 * @param ins[in]	-	instrukce ktera se ma provest.
 * 
 * @return Vraci 0 pokud nastane chyba, jinak 1;
 */
int executeIns(tInsTapeInsPtr* , tStack* );

void tTabSymToFrame(tBSTNodePtr node, tFrameContainer* frameContainer, bool isFunction);

int setParams(tInsTapeInsPtr* ins, tStack* stack);

#endif //INTERPRET_H_
