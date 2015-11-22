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
 * Kody pro chybove hlasky
 */
typedef enum{
	ERR_ALLOC,//!< ERR_ALLOC	Chyba alokace

	ERR_LEX,//!< ERR_LEX Chyba lexikalni analyzy

	ERR_SYNTAX,//!< ERR_SYNTAX Syntaktická chyba

	ERR_SEM_DEF,//!< ERR_SEM_DEF Prace s nedefinouvanou promennou/funkcí

	ERR_SEM_COM,//!< ERR_SEM_COM Chyba kompatibilty dat. typu

	ERR_SEM_AUTO,//!< ERR_SEM_AUTO chyba v odvozeni dat. typu

	ERR_SEM_REST,//!< ERR_SEM_REST Ostatni semanticke chyby

	ERR_RUNTIME_INPUT,//!< ERR_RUNTIME_INPUT Chyba cteni cisla ze vstupu

	ERR_RUNTIME_INIT_VAR,//!< ERR_RUNTIME_INIT_VAR Prace s neinicializovanou promennou

	ERR_RUNTIME_ZERO_DIV,//!< ERR_RUNTIME_ZERO_DIV Deleni nulou

	ERR_RUNTIME_REST,//!< ERR_RUNTIME_REST Ostatni behove chyby

	// posledni
	ERR_OK    //!< ERR_OK	Vse v poradku
}ERR_CODES;

/**
 * Chybove hlasky
 */
extern const char* ERR_MESSAGES[];

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
