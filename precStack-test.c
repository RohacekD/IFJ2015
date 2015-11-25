/*
 * precStack-test.c
 *  Project: IFJ2015
 *  Created on: 24. 11. 2015
 *  Author: xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 *  Description: Testy pro precStack.
 */
/**
 * @file precStack-test.c
 * @author xdocek09, xpavlu08, xbartu03, xjelin42, xrohac04
 * @brief Testy pro precStack.
 */
#include <stdio.h>
#include "precStack.h"

int main() {
	unsigned int errNum = 0;
	tPrecStack stack;
	precStackInit(&stack);


	tPrecStackData endMarkData;
	endMarkData.key = 0;
	endMarkData.type = PREC_STACK_ENDMARK;
	int key;

	printf("Vlozime na vrchol endmark(precStackPush):\n");
	if (!precStackPush(&stack, endMarkData)) {
		printf("\tCHYBA: precStackPush\n");
		errNum++;
	}

	if (precStackTop(&stack)->type == PREC_STACK_ENDMARK) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf(
			"Vlozime na vrchol terminal s klicem 2(precStackPushElementOfKind):\n");

	if (!precStackPushElementOfKind(&stack, PREC_STACK_TERMINAL, 2)) {
		printf("\tCHYBA: precStackPushElementOfKind\n");
		errNum++;
	}
	if (precStackTop(&stack)->type == PREC_STACK_TERMINAL) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf(
			"Vlozime na vrchol neterminal s klicem 1(precStackPushElementOfKind | vyzvednem pomoci precStackTopElement):\n");

	if (!precStackPushElementOfKind(&stack, PREC_STACK_NONTERMINAL, 1)) {
		printf("\tCHYBA: precStackPushElementOfKind\n");
		errNum++;
	}
	if (precStackTopElement(&stack)->data.type == PREC_STACK_NONTERMINAL) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf("Vlozime pred nejvrchnejsi terminal <:\n");
	if (!precStackPushElemBeforeTopTerm(&stack, PREC_STACK_SIGN, '<')) {
		printf("\tCHYBA: precStackPushElementOfKind\n");
		errNum++;
	}
	if (precStackTopElement(&stack)->next->data.type == PREC_STACK_SIGN) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf("Nechame si vratit klic nejvrchnejsi terminalu:\n");

	if (precStackTopTerminal(&stack, &key) && key == 2) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf("Je zasobnik prazdny (nema byt):\n");
	if (!precStackEmpty(&stack)) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf("Pop zasobniku:\n");
	precStackPop(&stack);
	if (precStackTop(&stack)->type == PREC_STACK_SIGN) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf("Dispose zasobniku:\n");
	precStackDispose(&stack);
	if (precStackEmpty(&stack)) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf(
			"Zavolame jednotlive funkce pro ziskavani dat ze zasobniku na prazdnym zasobnikem:\n");
	printf("\tprecStackTop:");
	if (precStackTop(&stack) == NULL) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}

	printf("\tprecStackTopElement:");
	if (precStackTopElement(&stack) == NULL) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}
	printf("\tprecStackTopTerminal:");
	if (!precStackTopTerminal(&stack, &key)) {
		printf("\tOK\n");
	} else {
		printf("\tERROR\n");
		errNum++;
	}
	printf("Chyb: %d \n", errNum);
	return 0;
}

/*** End of file: precStack-test.c ***/

