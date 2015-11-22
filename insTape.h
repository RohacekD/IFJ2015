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
