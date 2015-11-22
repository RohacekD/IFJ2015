/*
 * insTape.h
 *  Project: IFJ2015
 *  Created on: 21. 11. 2015
 *  Author: xdocek09
 *  Description: Hlavickovy soubor (pro insTape.c).
 */
/**
 * @file insTape.h
 * @author xdocek09
 * @brief Hlavickovy soubor (pro insTape.c).
 */
#ifndef INSTAPE_H_
#define INSTAPE_H_

typedef enum{
	I_CIN,
	I_COUT,
	I_PLUS,
	I_MINUS,
	I_MUL,
	I_DIV,
	I_EQUAL,
	I_NOTEQUAL,
	I_GREATER,
	I_LESSER,
	I_GEQUAL,
	I_LEQUAL,
	I_UMINUS,
	I_POSTPP,
	I_POSTMM,
	I_PREPP,
	I_PREMM,
	I_NOT,
	I_AND,
	I_OR,
	I_CBF, //create block frame
	I_CFF, //create function frame
	I_ASSIGN, //prirazeni =
	I_SP, //set parametr - musis se podivat o jeden niz
	I_RETURN, //navrat z ramce
	I_IFZERO,
	I_SETRESULT, //navratova hodnota z fce
	I_SORT,
	I_FIND,
	I_CONCAT,
	I_SUBSTR,
	I_LENGTH
}tInstructTypes;



typedef struct tInsTapeElem {
	tInstructTypes type;
    void* adr1;
    void* adr2;
    void* adr3;

    struct tInsTapeElem *lptr;
    struct tInsTapeElem *rptr;
} *tInsTapeElemPtr;

typedef struct {
	tInsTapeElemPtr first;
	tInsTapeElemPtr last;
} tInsTape;



#endif /* INSTAPE_H_ */
/*** End of file: insTape.h ***/
