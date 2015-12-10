/* 
 * File:   parser.h
 * Author: xpavlu08
 *
 * Created on 25. listopadu 2015, 18:35
 */

#ifndef PARSER_H
#define	PARSER_H
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"
#include "tabSym.h"
#include "error.h"
#include "token.h"
#include "parserExpr.h"

//promenna se nachazi v ial.c a nastavuje se, pokud po pruchodu zdrojovym souborem
//zustane nejaka funkce nedefinovana
extern int notDefined;

/**
 * do globalni promenne ulozim odkaz na soubor
 * @param file[in]      prekladany soubor
 */
void setSourceFile(FILE *file);

/**
 * prevede typ tokenu na typ promenne
 * @param ttype[in]         typ tokenu k prevodu
 * @return                  typ promenne
 */
tTabSymVarDataType tokenTypeToVarType(TokenTypes ttype);

/**
 * funkce rozpozna, zda je dany token term
 * @param ttype         -       typ token
 * @return  1, pokud je token terminal, jinak 0
 */
int isTerm(TokenTypes ttype);

/**funkce naplni tabulku bloku a vraci jeji element
 * @param blockLocalTable[out]       -   nova lokalni tabulka pro bloky
 * @param blockList[in]              -   ukazatel seznam bloku
 * @param blockListElem[in]          -   ukazatel na aktualni rodicovsky element
 * @param localTable[in]             -   existujici lokalni tabulka funkce
 * @return          funkce vraci ukazatel na novy element v pripade uspechu, jinak NULL
 */
tTabSymListElemPtr prepareBlockList(tTabSym** blockLocalTable, tTabSymList* blockList, tTabSymListElemPtr blockListElem, tTabSym *localTable);

/**
 * vlozi do globalni tabulky predpis pro vnorene funkce
 * @return      funkce vraci 1, pokud probehla bez chyby, jinak 0
 */
int prepareGlobalTable();

/**
 * funkce vytvori na halde misto pro novy string a skopiruje do nej hodnotu
 * predaneho stringu
 * @param identificator         -   string ke zkopirovani
 * @return      funkce vraci ukazatel na novy string, pokud probehlo vse v poradku, jinak NULL
 */
string* copyIdName(string *identificator);

/**
 * funkce dealokuje pamet alokovanou pro vytvoreny string
 * @param identificator         -   string, jez je uvolnovan
 */
void freeIdName(string *identificator);

/**
 * Uvodni funkce parseru
 * zpracovava chyby a uvolnuje pamet
 */
void parse();

/**
 * pravidlo ve tvaru:
 *          <function> -> <Kdata_types> fID(<arguments>)<body><function>
 * @return SYNTAX_OK(1) - pokud je vse v poradku
 *         SYNTAX_FAILED(0) - pokud nastala chyba
 */
int parseFunction();

/**
 * pravidlo ve tvaru:
 *          <function> -> <Kdata_types> fID(<arguments>)<body><function>
 * @return          funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseFunction();

/**
 * zpracovava nasledujici pravidla:
 * 4. <Kdata_types> -> keyw_bool
 * 5. <Kdata_types> -> keyw_int
 * 6. <Kdata_types> -> keyw_string
 * 7. <Kdata_types> -> keyw_double
 * @param variableType[out]     -   datovy typ tokenu
 * @param tokenType[in]         -   typ prijimaneho tokenu
 * @return      funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int kDataTypes(tTabSymVarDataType *variableType, TokenTypes tokenType);

/**
 * * zpracovava pravidla:
 * 8. <arguments> -> epsilon
 * 9. <arguments> -> <argument>
 * @param paramList[out]     -   seznam argumentu k naplneni
 * @param data[in]           -   NULL, jestli se identifikator funkce v globalni tabulce nenachazi
 *                               jinak odkaz na dany prvek
 * @param localTabel[out]    -  odkaz na lokalni tabulku, do ktere ukladame parametry
 * @return          funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseArguments(tParamListPtr paramList, tTabSymElemData *data, tTabSym *localTable);

/**
 * pravidlo 10: <argument> -> <Kdata_types>ID<argumentNext>
 * @param paramList[out]        -   seznam parametru k doplneni 
 * @param data[in]              -   NULL, jestli se identifikator funkce v globalni tabulce nenachazi 
 *                                  jinak odkaz na dany prvek
 * @param paramType[in]         -   datovy typ promenne
 * @param localTabel[out]       -  odkaz na lokalni tabulku, do ktere ukladame parametry
 * @return      funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseArgument(tParamListPtr paramList, tTabSymElemData *data, tTabSymVarDataType paramType, tTabSym *localTable);


/**
 * funkce zpracovava providla:
 * 11. <argumentNext> -> epsilon
 * 12. <argumentNext> -> , <argument>
 * @return      funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int argumentNext(tParamListPtr paramList, tTabSymElemData *data, tTabSym *localTable);

//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
/**
 * zpracovava nasledujici pravidla:
 * 15. <st_list> -> epsilon
 * 16. <st_list> -> <statement><st_list>
 * 17. <st_list> -> <declaration><st_list>
 * 18. <st_list> -> {<st_list>}<st_list>
 * @param localTable        -   ukazatel na lokalni tabulku funkce
 * @param blockList         -   ukazatel na list tabulek bloku
 * @param parent            -   ukazatel na rodicovsky prvek 
 * @param instructionTape   -   ukazatel na pasku instrukci
 * @return      funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseStatementList(tTabSym *localTable, tTabSymList *blockList,
                        tTabSymListElemPtr parent, tInsTape *instructionTape);

//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
//-----------   DODELAT GENEROVANI INSTRUKCI ---------
/**
 * zpracovava nasledujici pravidla:
 * 19. <statement> -> <assignment>;
 * 20. <statement> -> cin >> ID <cin>;
 * 21. <statement> -> cout << <term><cout>;
 * 22. <statement> -> return expression;
 * 23. <statement> -> if(expression)<block><else>
 * 24. <statement> -> for(<Kdata_types>ID=expression; expression; <assignment>)<block>
 * 25. <statement> -> while(expression)<block>
 * 26. <statement> -> do <block>while(expression);
 * @param localTable        -   lokalni tabulka symbolu
 * @param token             -   typ tokenu
 * @param instructionTape   -   ukazatel na instrukcni pasku
 * @param blockList         -   ukazatel na list tabulek bloku
 * @param blockListElem     -   aktualni element v listu tabulek symbolu pro bloky
 * @return      funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseStatement(tTabSym *localTable, tToken tokenOrig, tInsTape *instructionTape,
                    tTabSymList *blockList,tTabSymListElemPtr blockListElem);

//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
/**
 * zpracovava nasledujici pravidla:
 * 36. <declaration> -> <Kdata_types> ID<dec_init>
 * 37. <declaration> -> auto ID = expression;
 * @param dataType          -   datovy typ promenne
 * @param localTable        -   ukazatel na lokalni tabulku symbolu
 * @param instructionTape   -   ukazatel na pasku instrukci
 * @param blockListElem     -   aktualni element v listu tabulek bloku
 * @return      funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseDeclaration(tTabSymVarDataType dataType, tTabSym *localTable,
                    tInsTape *instructionTape, tTabSymListElemPtr blockListElem);

//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
/**
 * zpracovava nasledujici pravidla:
 * 27.  <block> -> {<st_list>}
 * 28.  <block> -> <statement>
 * @param localTable[in]            -   lokalni tabulka symbolu
 * @param instructionTape           -   ukazatel na pasku instrukci
 * @param blockList                 -   list tabulek symbolu pro bloky
 * @param blockListElem             -   aktualni element v listu tabulek bloku
 * @return          funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseBlock(tTabSym *localTable, tTabSymList *blockList,
                tTabSymListElemPtr blockListElem, tInsTape *instructionTape);

//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
//nejakym zpusobem musim prohledavat rozsah platnosti (lokalni tabulky bloku...)
/**
 * zpracovava pravidla:
 * 40. <assignment> = ID<specID>
 * 41. <assignment> = <specID>ID
 * @param tokenType
 * @param localTable
 * @return          funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */

int parseAssignment(tToken tokenOrig, tTabSym *localTable, tInsTape *instructionTape,
                tTabSymListElemPtr blockListElem);


//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
//-----------   DODELAT GENEROVANI INSTRUKCI ---------
/**
 * * zpracovava nasledujici pravidla:
 * 32.  <cin> -> epsilon
 * 33.  <cin> -> >>ID<cin>
 * @param instructionTape           -   odkaz na instrukcni pasku
 * @param localTable                -   odkaz na lokalni tabulku symbolu
 * @param blockListElem             -   aktualni element v tabulce symbolu pro bloky
 * @return              funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseCin(tInsTape *instructionTape, tTabSym *localTable, tTabSymListElemPtr blockListElem);


//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
/**
 * zpracovava nasledujici pravidla
 *34. <cout> -> epsilon
 *35. <cout> -> << <term> <cout>
 * @return      funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseCout(tInsTape *instructionTape, tTabSymListElemPtr blockListElem, tTabSym *localTable);

/**
 * zpracovava pravidla:
 * 30: <else> -> epsilon
 * 31. <else> -> else <block>
 * @param localTable[in]            -   lokalni tabulka symbolu
 * @param instructionTape           -   ukazatel na pasku instrukci
 * @param blockList                 -   list tabulek symbolu pro bloky
 * @param blockListElem             -   aktualni element v listu tabulek bloku
 * @return      funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseElse(tTabSym *localTable, tTabSymList *blockList,
                tTabSymListElemPtr blockListElem, tInsTape *instructionTape);

#endif	/* PARSER_H */

