/*
 * precTab.h
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: Hlavickovy soubor (pro precTab.c).
 */
/**
 * @file precTab.h
 * @author xdocek09
 * @brief Hlavickovy soubor (pro precTab.c).
 */

#ifndef PRECTAB_H_
#define PRECTAB_H_

#include "token.h"


/**
 * Vraci pravidlo pro dany terminal a token.
 * @param[in] terminal	-	kod terminalu (nejblize vrcholu zasobniku)
 * @param[in] token		-	kod tokenu (prichoziho)
 * @return Vraci nasledujici pravidla:
 * 	0 - chyba
 * 	'<'
 * 	'>'
 * 	'='
 */
int precGetRule(int terminal, unsigned int token);

#endif /* PRECTAB_H_ */
/*** End of file: precTab.h ***/
