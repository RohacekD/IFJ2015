/*
 * error.h
 *  Project: IFJ2015
 *  Created on: 21. 11. 2015
 *  Author: xdocek09
 *  Description: Hlavickovy soubor pro error.c.
 */
/**
 * @file error.h
 * @author xdocek09
 * @brief Hlavickovy soubor pro error.c.
 */

#ifndef ERROR_H_
#define ERROR_H_

/**
 * Tiskne warning na stderr. S formatem *fmt.
 * @param fmt[in]	-	format hlaseni
 */
void Warning(const char *fmt, ...);

/**
 * Tiskne error na stderr (s formatem *fmt) a ukonci program exit(status).
 * @param status[in]	-	chybovy status
 * @param fmt[in]		-	format hlaseni
 */
void FatalError(const int status, const char *fmt, ...);

#endif /* ERROR_H_ */
/*** End of file: error.h ***/
