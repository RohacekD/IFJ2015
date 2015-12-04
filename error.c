/*
 * error.c
 *  Project: IFJ2015
 *  Created on: 21. 11. 2015
 *  Author: xdocek09
 *  Description: Funkce pro chybove stavy.
 */
/**
 * @file error.c
 * @author xdocek09
 * @brief Funkce pro chybove stavy.
 */


#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/**
 * Chybove hlasky
 */
const char* ERR_MESSAGES[]={
		[ERR_ALLOC]="Chyba pri alokaci pameti.\n",
		[ERR_LEX]="Error lexikalni analyzy.\n",
		[ERR_SYNTAX] = "Syntaktická chyba.\n",
		[ERR_SEM_DEF] = "Prace s nedefinouvanou promennou/funkcí.\n",
		[ERR_SEM_COM] = "Chyba kompatibilty datoveho typu.\n",
		[ERR_SEM_AUTO] = "chyba v odvozeni datoveho typu.\n",
		[ERR_SEM_REST] = "Ostatni semanticke chyby.\n",
		[ERR_RUNTIME_INPUT] = "Chyba cteni cisla ze vstupu.\n",
		[ERR_RUNTIME_INIT_VAR] = "Prace s neinicializovanou promennou.\n",
		[ERR_RUNTIME_ZERO_DIV] = "Deleni nulou.\n",
		[ERR_RUNTIME_REST] = "Ostatni behove chyby.\n",
		// posledni
		[ERR_OK]="Vse v poradku.\n"
};

void Warning(const char *fmt, ...){
	fprintf(stderr, "Chyba: ");
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

void FatalError(const int status, const char *fmt, ...){
	fprintf(stderr, "Chyba: ");
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	getchar();
	exit(status);
}


/*** End of file: error.c ***/

