/* 
 * File:   parser.h
 * Author: xpavlu08
 *
 * Created on 25. listopadu 2015, 18:35
 */

#ifndef PARSER_H
#define	PARSER_H

/*
#define SYNTAX_OK 1
#define SYNTAX_FAILED 0
*/

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
tTabSymVarNoAutoDataType tokenTypeToVarType(TokenTypes ttype);

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
 * pro pravidla 8 a 9:
 *          <arguments> -> epsilon
 *          <arguments> - > argument
 * @return SYNTAX_OK(1) - pokud je vse v poradku
 *         SYNTAX_FAILED(0) - pokud nastala chyba
 */
int parseArguments();

/**
 * zpracovava nasledujici pravidla:
 * 4. <Kdata_types> -> keyw_bool
 * 5. <Kdata_types> -> keyw_int
 * 6. <Kdata_types> -> keyw_string
 * 7. <Kdata_types> -> keyw_double
 * @param variableType[out]     -   datovy typ tokenu
 * @param tokenType[in]         -   typ prijimaneho tokenu
 * @return      funkce vraci 1, pokud je vse v poradku
 */
int kDataTypes(tTabSymVarNoAutoDataType *variableType, TokenTypes tokenType);

/**
 * * zpracovava pravidla:
 * 8. <arguments> -> epsilon
 * 9. <arguments> -> <argument>
 * @param paramList[out]     -   seznam argumentu k naplneni
 * @param data[in]           -   NULL, jestli se identifikator funkce v globalni tabulce nenachazi
 *                               jinak odkaz na dany prvek
 * @return 
 */
int parseArguments(tParamListPtr paramList, tTabSymElemData *data);

/**
 * pravidlo 10: <argument> -> <Kdata_types>ID<argumentNext>
 * @param paramList[out]        -   seznam parametru k doplneni 
 * @param data[in]              -   NULL, jestli se identifikator funkce v globalni tabulce nenachazi 
 *                                  jinak odkaz na dany prvek
 * @param paramType[in]         -   datovy typ promenne
 * @return      pokud probehlo vse v poradku, tak 1
 */
int parseArgument(tParamListPtr paramList, tTabSymElemData *data, tTabSymVarNoAutoDataType paramType);


/**
 * funkce zpracovava providla:
 * 11. <argumentNext> -> epsilon
 * 12. <argumentNext> -> , <argument>
 * @return      pokud probehlo vse v poradku, tak 1
 */
int argumentNext(tParamListPtr paramList, tTabSymElemData *data);

#endif	/* PARSER_H */

