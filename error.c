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
	exit(status);
}


/*** End of file: error.c ***/

