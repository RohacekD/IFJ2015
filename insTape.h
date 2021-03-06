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

#include <stdlib.h>
#include <stdio.h>
#include "str.h"
#include "PrintableEnum.h"

 //todo: vsechny instrukce musi mit variantu 
 //pokud jde o operaci vyuzivajici 3 adresy tak oper1,oper2,vysledek
#define T_INSTRUCT_TYPES(XX) \
	XX(I_CIN)			/* adr3 = kam to mam ulozit												*/ \
	XX(I_COUT)			/* adr1 = co mam vypsat													*/ \
	XX(I_PLUS)			/* bezna konvence														*/ \
	XX(I_MINUS)			/* bezna konvence														*/ \
	XX(I_MUL)			/* bezna konvence														*/ \
	XX(I_DIV)			/* bezna konvence														*/ \
	XX(I_EQUAL)			/* bezna konvence - 3. operand je bool									*/ \
	XX(I_NOTEQUAL)		/* bezna konvence - 3. operand je bool									*/ \
	XX(I_GREATER)		/* bezna konvence - 3. operand je bool adr1>adr2						*/ \
	XX(I_LESSER)		/* bezna konvence - 3. operand je bool adr1<adr2						*/ \
	XX(I_GEQUAL)		/* bezna konvence - 3. operand je bool adr1>=adr2						*/ \
	XX(I_LEQUAL)		/* bezna konvence - 3. operand je bool adr1<=adr2						*/ \
	XX(I_UMINUS)		/* adr1 operand adr3 vysledek											*/ \
	XX(I_INC)			/* adr1 operand adr3 dest												*/ \
	XX(I_DEC)			/* adr1 operand adr3 dest												*/ \
	XX(I_LOG_NOT)		/* adr1 operand adr3 vysledek											*/ \
	XX(I_LOG_AND)		/* bezna konvence - 3. operand je bool									*/ \
	XX(I_LOG_OR)		/* bezna konvence - 3. operand je bool									*/ \
	XX(I_CBF)			/* create block frame - adr1 - locTab									*/ \
	/*    Ve volane fci se vytvori promenna $ret kam bude ulozena vracena hodnota. Na adr3 je dana adresa	*/ \
	/*   promenne ve volajici fci kam ulozit vysledek fce													*/ \
	XX(I_CF)			/* call function adr1 - locTab adr2 - ins adr3 - adresa kam ulozit vysledek fce(u main NULL)	*/ \
	XX(I_ASSIGN)		/* prirazeni = adr1(source) => adr3(dest)														*/ \
	XX(I_SP)			/* adr1 -z volajici funkce, , adr3-z volane | set parametr - musis se podivat o jeden niz		*/ \
	XX(I_DBF)			/* delete block frame													*/ \
	XX(I_RETURN)		/* nepotrebuje nic														*/ \
	XX(I_IFZERO)		/* jmp if adr1 == 0 to adr2 (ins)										*/ \
	XX(I_IFNZERO)		/* jmp if adr1 != 0 to adr2 (ins)										*/ \
	XX(I_GOTO)			/* adr1 - ins															*/ \
	XX(I_SORT)			/* adr1 string(argument 1), adr3 string kam ma byt ulozen				*/ \
	XX(I_FIND)			/* adr1 string s adr2 search adr3 int									*/ \
	XX(I_CONCAT)		/* adr1 argument 1, adr2 argument 2 adr3 destination					*/ \
	XX(I_SUBSTR)		/* adr1 string(argument 1), adr2,adr3 variable INT (argument 2 a 3)		*/ \
	XX(I_SUBSTR_DEST)	/* adr3 kam ulozit vysledek												*/ \
	XX(I_LENGTH)		/* adr1 string adr3 int													*/ \
	XX(I_LABEL)			/* adr1 == adr2 == adr3 == NULL cil skoku pri skokovych instr.			*/ \
	XX(I_DECLARE)		/* adr1 meno nove promenne												*/ 

//DECL_NAMED_ENUM_ENUM(tInstructTypes, T_INSTRUCT_TYPES)
DECL_NAMED_ENUM(tInstructTypes, T_INSTRUCT_TYPES)

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

/**
 * nastavi aktivitu na prvek dany ukazatelem gotoInstr
 * zdroj: jednoduchy interpret - ilist.c
 * @param tape[in]          -   ukazatel na instrukcni pasku
 * @param gotoInstr[in]     -   instrukce, na kterou se ma nastavit aktivita
 */
void insTapeGoto(tInsTape *tape, tInsTapeInsPtr gotoInstr);

int insTapeLength(tInsTape*tape);
void printTape(tInsTapeInsPtr ins);
#endif /* INSTAPE_H_ */
/*** End of file: insTape.h ***/
