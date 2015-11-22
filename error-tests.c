/*
 * error-tests.c
 *  Project: IFJ2015
 *  Created on: 21. 11. 2015
 *  Author: xdocek09
 *  Description: 
 */
/**
 * @file error-tests.c
 * @author xdocek09
 * @brief 
 */

#include "error.h"

int main() {
	//vypiseme chybove hlaseni

	Warning("Vypsane chybove hlaseni.\n");

	FatalError(111,ERR_MESSAGES[ERR_OK]);

	return 0;
}

/*** End of file: error-tests.c ***/

