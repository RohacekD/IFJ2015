/*
 * stack.h
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: Hlavickovy soubor (pro stack.c).
 */
/**
 * @file stack.h
 * @author xdocek09
 * @brief Hlavickovy soubor (pro stack.c).
 */
#ifndef STACK_H_
#define STACK_H_

typedef enum
{
	TERMINAL,
	NONTERMINAL,
	PRECSIGN,
	ENDMARK
}tStackElemType;

typedef struct{
	tStackElemType type;
	int key;				// klic, daneho neterminalu, terminalu

} tStackData;

typedef struct tStackElem {
	tStackData data;
    struct tStackElem *next;
} *tStackElemPtr;

typedef struct {
	tStackElemPtr top;
} tStack;


/**
 * Inicializace zasobniku.
 * @param[in] stack	-	zasobnik pro inicializovani
 */
void stackInit(tStack * stack);

/**
 * Vlozi data na vrchol zasobniku stack,
 * @param[in] stack		-	Zasobnik, do ktereho vklada.
 * @param[in] data		-	Data, ktera vklada.
 * @return 0 pri chybe, jinak 1.
 */
int stackPush(tStack * stack, tStackData data);

/**
 * Vlozi element typu type s klicem key na vrchol zasobniku stack,
 * @param[in] stack		-	Zasobnik, do ktereho vklada.
 * @param[in] type		-	Typ elementu(tStackElemType).
 * @param[in] key		-	Klic elementu.
 * @return 0 pri chybe, jinak 1.
 */
int stackPushElementOfKind(tStack * stack, tStackElemType type, int key);

/**
 * Vlozi element typu type s klicem key pred nejvrchnejsi terminal zasobniku stack,
 * @param[in] stack		-	Zasobnik, do ktereho vklada.
 * @param[in] type		-	Typ elementu(tStackElemType).
 * @param[in] key		-	Klic elementu.
 * @return 0 pri chybe, jinak 1.
 */
int stackPushElemBeforeTopTerm(tStack * stack, tStackElemType type, int key);

/**
 * Rusi prvek na vrcholu zasobniku stack.
 * @param[in] stack	-	Zasobnik u ktereho rusi vrchni prvek.
 */
void stackPop(tStack * stack);

/**
 * Nacte vrchol zasobniku stack do data,
 * @param[in] stack		-	Zasobnik, ze ktereho cte.
 * @param[out] data		-	Do tohoto nacte data z vrcholu zasobniku.
 * @return	Pokud je chyba (prazdny zasobnik) vraci 0, jinak 1.
 */
int stackTop(tStack * stack, tStackData* data);

/**
 * Vraci prvek z vrcholu zasobniku.
 * @param[in] stack		-	Zasobnik, ze ktereho cte.
 * @return	Pokud je chyba (prazdny zasobnik) vraci NULL,
 * 			jinak pointer na prvek z vrcholu zasobniku.
 */
tStackElemPtr stackTopElement(tStack * stack);

/**
 * Vraci nejvrchnejsi terminal ze zasobniku.
 * @param[in] stack			-	zasobnik, z ktereho vraci terminal
 * @param[out] terminal		-	Do tohoto nacte nejvrchnejsi terminal.
 * @return	Pokud je chyba (terminal nenalezen) vraci 0, jinak 1.
 */
int stackTopTerminal(tStack* stack, int* terminal);

/**
 * Kontroluje prazdnost zasobniku.
 * @param[in] stack	-	Zasobnik, ktery kontroluje.
 * @return	1 pokud je prazdny, jinak 0.
 */
int stackEmpty(tStack * stack);
#endif /* STACK_H_ */
/*** End of file: stack.h ***/
