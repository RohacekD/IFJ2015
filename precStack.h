/*
 * precStack.h
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: Hlavickovy soubor (pro precStack.c).
 */
/**
 * @file precStack.h
 * @author xdocek09
 * @brief Hlavickovy soubor (pro stack.c).
 */
#ifndef PRECSTACK_H_
#define PRECSTACK_H_
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include "precTab.h"
#include "str.h"

typedef enum
{
	PREC_STACK_TERMINAL,
	PREC_STACK_NONTERMINAL,
	PREC_STACK_SIGN,		//'<'
	PREC_STACK_ENDMARK
}tPrecStackElemType;

typedef struct{
	tPrecStackElemType type;
	int key;				// klic, daneho neterminalu, terminalu
	string* id;				// identifikator pro tabulku symbolu
} tPrecStackData;

typedef struct tStackElem {
	tPrecStackData data;
    struct tStackElem *next;
} *tPrecStackElemPtr;

typedef struct {
	tPrecStackElemPtr top;
} tPrecStack;


/**
 * Inicializace zasobniku.
 * @param[in] stack	-	zasobnik pro inicializovani
 */
void precStackInit(tPrecStack * stack);

/**
 * Vlozi data na vrchol zasobniku stack,
 * @param[in] stack		-	Zasobnik, do ktereho vklada.
 * @param[in] data		-	Data, ktera vklada.
 * @return 0 pri chybe, jinak 1.
 */
int precStackPush(tPrecStack * stack, tPrecStackData data);

/**
 * Vlozi element typu type s klicem key na vrchol zasobniku stack,
 * @param[in] stack		-	Zasobnik, do ktereho vklada.
 * @param[in] type		-	Typ elementu(tStackElemType).
 * @param[in] key		-	Klic elementu.
 * @return 0 pri chybe, jinak 1.
 */
int precStackPushElementOfKind(tPrecStack * stack, tPrecStackElemType type, int key, string* id);

/**
 * Vlozi element typu type s klicem key pred nejvrchnejsi terminal zasobniku stack,
 * @param[in] stack		-	Zasobnik, do ktereho vklada.
 * @param[in] type		-	Typ elementu(tStackElemType).
 * @param[in] key		-	Klic elementu.
 * @return 0 pri chybe, jinak 1.
 */
int precStackPushElemBeforeTopTerm(tPrecStack * stack, tPrecStackElemType type, int key, string* id);

/**
 * Rusi prvek na vrcholu zasobniku stack.
 * @param[in] stack	-	Zasobnik u ktereho rusi vrchni prvek.
 */
void precStackPop(tPrecStack * stack);

/**
 * Ziska data z vrcholu zasobniku,
 * @param[in] stack		-	Zasobnik, ze ktereho cte.
 * @return	Vraci odkaz na data nebo NULL pri chybe (prazdny zasobnik).
 */
tPrecStackData* precStackTop(tPrecStack * stack);

/**
 * Vraci prvek z vrcholu zasobniku.
 * @param[in] stack		-	Zasobnik, ze ktereho cte.
 * @return	Pokud je chyba (prazdny zasobnik) vraci NULL,
 * 			jinak pointer na prvek z vrcholu zasobniku.
 */
tPrecStackElemPtr precStackTopElement(tPrecStack * stack);

/**
 * Vraci nejvrchnejsi terminal ze zasobniku.
 * @param[in] stack			-	zasobnik, z ktereho vraci terminal
 * @param[out] terminal		-	Do tohoto nacte nejvrchnejsi terminal.
 * @return	Pokud je chyba (terminal nenalezen) vraci 0, jinak 1.
 */
int precStackTopTerminal(tPrecStack* stack, tParExpTerminals* terminal);

/**
 * Vytvori precStack ze vsech polozek z vrcholu zasobniku do < (bez).
 * Revertuje jejich poradi. Z puvodni zasobniku zmizi a to i vcetne <.
 * @param stack[in]	-	precStack, ze ktereho bere data
 * @return Vraci novy precStack, pri chybe NULL.
 */
tPrecStack * precStackCreateRevertedTopReduc(tPrecStack * stack);

/**
 * Kontroluje prazdnost zasobniku.
 * @param[in] stack	-	Zasobnik, ktery kontroluje.
 * @return	1 pokud je prazdny, jinak 0.
 */
int precStackEmpty(tPrecStack * stack);

/**
 * Vyprazdni zasobnik a uvede jej do stavu po inicializaci.
 * @param stack[in]	-	Zasobnik, ktery vyprazdnuje.
 */
void precStackDispose(tPrecStack * stack);

/**
 * Uvolni z pameti cely stack. Nejneom obsah.
 * @param stack[in]	-	Stack pro uvolneni z pameti.
 */
void precStackFree(tPrecStack ** stack);

#endif /* PRECSTACK_H_ */
/*** End of file: precStack.h ***/
