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
	I_DBF, //delete block frame
	I_RETURN, //navrat z ramce
	I_IFZERO,
	I_SETRESULT, //navratova hodnota z fce
	I_SORT,
	I_FIND,
	I_CONCAT,
	I_SUBSTR,
	I_LENGTH
}tInstructTypes;


/**
 * 3 adresova instrukce na instrukce na instrukcni pasce
 */
typedef struct tInsTapeIns {
	tInstructTypes type;
    void* adr1;
    void* adr2;
    void* adr3;

    struct tInsTapeIns *lptr;
    struct tInsTapeIns *rptr;
} *tInsTapeInsPtr;

/**
 * Struktura instrukcni pasky
 */
typedef struct {
	tInsTapeInsPtr first;	// ukazatel na prvni prvek seznamu
	tInsTapeInsPtr act;		// ukazatel na aktualni prvek seznamu
	tInsTapeInsPtr last;	// ukazatel na posledni prvek seznamu
} tInsTape;

/**
 * Vytvori a inicializuje instrukcni pasku.
 * @return Ukazatel na nove vytvorenou pasku. Pokud chyba vraci NULL.
 */
tInsTape* insTapeCreate();

/**
 * Uvolni instrukcni pasku z pameti.
 * @param tape[in]	-	instrukcni paska pro uvolneni.
 */
void insTapeFree (tInsTape*tape);

/**
 * Vlozi instrukci na zacatek instrukcni pasky
 * @param tape[in]	-	Paska kam vlozi instrukci.
 * @param type[in]	-	Druh instrukce.
 * @param adr1[in]	-	Prvni adresa.
 * @param adr2[in]	-	Druha adresa.
 * @param adr3[in]	-	Treti adresa.
 * @return	Vraci 0 pokud chyba, jinak 1.
 */
int insTapeInsertFirst(tInsTape* tape, tInstructTypes type, void* adr1, void* adr2, void* adr3);

/**
 * Vlozi instrukci na konec instrukcni pasky
 * @param tape[in]	-	Paska kam vlozi instrukci.
 * @param type[in]	-	Druh instrukce.
 * @param adr1[in]	-	Prvni adresa.
 * @param adr2[in]	-	Druha adresa.
 * @param adr3[in]	-	Treti adresa.
 * @return	Vraci 0 pokud chyba, jinak 1.
 */
int insTapeInsertLast(tInsTape* tape, tInstructTypes type, void* adr1, void* adr2, void* adr3);

/**
 * Nastavi aktivitu na prvni instrukci v seznamu tape.
 * @param tape[in]	-	Paska kde nastavi aktivitu.
 */
void insTapeFirst (tInsTape* tape);

/**
 * Nastavi aktivitu na posledni instrukci v seznamu tape.
 * @param tape[in]	-	Paska kde nastavi aktivitu.
 */
void insTapeLast (tInsTape* tape);

/**
 * Ziska prvni instrukci na pasce.
 * @param tape[in]	-	paska instrukci
 * @return Vraci ukazatel na prvni instrukci na pasce. Vraci NULL pokud je paska prazdna.
 */
tInsTapeInsPtr insTapeGetFirst (tInsTape* tape);

/**
 * Ziska posledni instrukci na pasce.
 * @param tape[in]	-	paska instrukci
 * @return Vraci ukazatel na posledni instrukci na pasce. Vraci NULL pokud je paska prazdna.
 */
tInsTapeInsPtr insTapeGetLast (tInsTape* tape);

/**
 * Maze prvni instrukci na pasce. Pokud je paska prazdna nedela nic.
 * Pokud byla prvni aktivni, aktivita se rusi.
 * @param tape[in]	-	paska instrukci
 */
void insTapeDeleteFirst (tInsTape* tape);

/**
 * Maze posledni instrukci na pasce. Pokud je paska prazdna nedela nic.
 * Pokud byla posledni aktivni, aktivita se rusi.
 * @param tape[in]	-	paska instrukci
 */
void insTapeDeleteLast (tInsTape* tape);

/**
 * Maze instrukci za aktivni instrukci. Nedela nic pokud je paska prazdna nebo aktivni je posledni.
 * Pokud je mazana instrukce aktivni rusi aktivitu.
 * @param tape	-	paska instrukci
 */
void insTapePostDelete (tInsTape* tape);

/**
 * Maze instrukci pred aktivni instrukci. Nedela nic pokud je paska prazdna nebo aktivni je prvni.
 * Pokud je mazana instrukce aktivni rusi aktivitu.
 * @param tape	-	paska instrukci
 */
void insTapePreDelete (tInsTape* tape);

/**
 * Vlozi instrukci za aktivni instrukci. Pokud nebyl seznam aktivni nic se nevklada.
 * @param tape[in]	-	Paska kam vlozi instrukci.
 * @param type[in]	-	Druh instrukce.
 * @param adr1[in]	-	Prvni adresa.
 * @param adr2[in]	-	Druha adresa.
 * @param adr3[in]	-	Treti adresa.
 * @return	Vraci 0 pokud chyba, jinak 1.
 */
int insTapePostInsert (tInsTape* tape, tInstructTypes type, void* adr1, void* adr2, void* adr3);

/**
 * Vlozi instrukci pred aktivni instrukci. Pokud nebyl seznam aktivni nic se nevklada.
 * @param tape[in]	-	Paska kam vlozi instrukci.
 * @param type[in]	-	Druh instrukce.
 * @param adr1[in]	-	Prvni adresa.
 * @param adr2[in]	-	Druha adresa.
 * @param adr3[in]	-	Treti adresa.
 * @return	Vraci 0 pokud chyba, jinak 1.
 */
int insTapePreInsert (tInsTape* tape, tInstructTypes type, void* adr1, void* adr2, void* adr3);

/**
 * Ziska instrukci z pasky, ktera je nastavena jako aktivni.
 * @param tape[in]	-	paska instrukci
 * @return Vraci instrukci, ktera je nastavena jako aktivni. Pokud neni paska aktivni vraci NULL.
 */
tInsTapeInsPtr insTapeGetActive (tInsTape* tape);

/**
 * Prepise aktivni instrukci. Pokud nebyl seznam aktivni nic se neprepise.
 * @param tape[in]	-	Paska kde aktualizuje instrukci.
 * @param type[in]	-	Druh instrukce.
 * @param adr1[in]	-	Prvni adresa.
 * @param adr2[in]	-	Druha adresa.
 * @param adr3[in]	-	Treti adresa.
 */
void insTapeActualize (tInsTape* tape, tInstructTypes type, void* adr1, void* adr2, void* adr3);

/**
 * Presune aktivitu na dalsi instrukci. Pri neaktivite se nestane nic.
 * Pokud je aktivita na poslednim prvku a volam tuto funkci, paska se stava neaktivni.
 * @param tape[in]	-	paska instrukci
 */
void insTapeSucc (tInsTape* tape);

/**
 * Presune aktivitu na predeslou instrukci. Pri neaktivite se nestane nic.
 * Pokud je aktivita na prvnim prvku a volam tuto funkci, paska se stava neaktivni.
 * @param tape[in]	-	paska instrukci
 */
void insTapePred (tInsTape* tape);

/**
 * Zjistuje aktivitu pasky.
 * @param tape[in]	-	paska instrukci
 * @return	1 pri aktivite, jinak 0;
 */
int insTapeActive (tInsTape* tape);

#endif /* INSTAPE_H_ */
/*** End of file: insTape.h ***/
