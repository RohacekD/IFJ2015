#ifndef INTERPRET_H_
#define INTERPRET_H_
#include "insTape.h"
#include "frameStack.h"
#include "error.h"
#include "tabSym.h"
#include "scanner.h"
#include <ctype.h>
#include <math.h>
/*
 * Dostane instrukci a od te zacne vykonavat program.
 * Vykonava skokove instrukce ve svem tele
 * @link Popis instrukci je v insTape.h
 * @param ins[in]	-	instrukce od ktere zacit vykonavat
 * @return Vraci ERR_OK pokud nastane OK, prislusny error kod;
 */
int executeTape(tInsTapeInsPtr);
/*
 * Dostane instrukci a vykona ji, nasledne posune na dalsi instrukci
 * @link Popis instrukci je v insTape.h
 * @param ins[in]	-	instrukce ktera se ma provest.
 * 
 * @return Vraci 0 pokud nastane chyba, jinak 1;
 */
int executeIns(tInsTapeInsPtr* , tStack* );

/*
 * Prijme loc tabulku bloku a prevede ji na  ramec. Pokud je isFunction true tak nastavuje vsem promennym
 * deklarovanost na true. Jinak je nastavena deklarace jen pro tempove promenne (indikovano cisle na zacatku
 * jmena promenne). Vsem promenym nastavi inicializovanost na fase. Konstantam naopak na true.
 *
 * @note isFunction je zbytecne protoze parser toto neresi, ale pokud by toto bylo optimalizovano.
 * @param node[in]					- lokalni tabulka promennych
 * @param frameContainer[in|out]	- ramec ktery bude inicializovan
 * @param isFunction[in]			- indikuje jestli maji vsem promennym nastavena deklrarace na true
 */
void tTabSymToFrame(tBSTNodePtr node, tFrameContainer* frameContainer, bool isFunction);

/*
 * Priradi do funkcniho ramce (ten je na stack->Top) parametry. Zavola se z instrukce
 * I_CF. Bude posouvat tak dlouho psaku doku nedojde k posledni instrukci I_SP
 * po ukonceni fce ins ukazuje na posledni I_SP v rade. Tak aby fce executeIns()
 * mohla posunout pasku na dalsi.
 * @param ins[in]	- ukazatel na instrukci (I_CF) na pasce
 * @param stack[in]	- ukazatel na zasobnik ramcu (v stack->Top je funkcni ramec)
 * @return ERR_CODES - ERR_OK pokud probehne v poradku
 */
int setParams(tInsTapeInsPtr* ins, tStack* stack);

#endif //INTERPRET_H_
