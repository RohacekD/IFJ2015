/*
 * precTab.c
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: Obsahuje precedencni tabulku.
 */
/**
 * @file precTab.c
 * @author xdocek09
 * @brief Obsahuje precedencni tabulku.
 */
#ifndef PRECTAB_C_
#define PRECTAB_C_

enum tTokenTypes
{
		INTEGER,
		DOUBLE,
		STRING,
		BOOL,
		DIVISION,
		MULTIPLY,
		ADD,
		SUBTRACTION,
		GREATER,
		GREATER_EQUAL,
		LESS,
		LESS_EQUAL,
		EQUAL,
		NOT_EQUAL,
		INCREMENTATION,
		DECREMENTATION,
		LOG_AND,
		LOG_OR,
		LOG_NOT,
		MINUS,
		KEYW_INT,
		END_OF_FILE
};


int precGetRule(int terminal, unsigned int token){
	/**
	 * Tabulka obsahujici pravidla:
	 * 	0 - chyba
	 * 	'<'
	 * 	'>'
	 * 	'='
	 */
	static int precTab[22][22]={0};


	if(terminal > END_OF_FILE || token > END_OF_FILE){
		return 0;
	}
	return precTab[terminal][token];
}

#endif /* PRECTAB_C_ */
/*** End of file: precTab.c ***/
