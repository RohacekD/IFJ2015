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

#endif	/* PARSER_H */

