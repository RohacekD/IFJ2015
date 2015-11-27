#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"
#include "tabSym.h"
#include "error.h"
#include "parser.h"
#include "token.h"
#include <stdbool.h>
#include <string.h>

//ve funkcich muze dojit k ruznym typum chyb
//chyba v lexikalni analyze(kdy ma vratit 1), nemuzu
//zakodovat jako 1, protoze to u me znamena uspech
#define LEXICAL_ERR 0
#define SYNTAX_ERR 2
#define SEMANTIC_ERROR 3

// vytvorim strukturu pro globalni tabulku symbolu
tTabSym *globalTable;
FILE *f;
//pomocna promenna, slouzi k rozpoznani, zda mam v parametrech carku za identifikatorem
bool isComma;

void setSourceFile(FILE *file)
{
    f = file;
}

/**
 * prevede typ tokenu na typ promenne
 * @param ttype[in]         typ tokenu k prevodu
 * @return                  typ promenne
 */
tTabSymVarNoAutoDataType tokenTypeToVarType(TokenTypes ttype) {
    switch(ttype) {
        case KEYW_INT:
            return TAB_SYM_VAR_NO_AUTO_INTEGER;
            break;
        case KEYW_DOUBLE:
            return TAB_SYM_VAR_NO_AUTO_DOUBLE;
            break;
        case KEYW_STRING:
            return TAB_SYM_VAR_NO_AUTO_STRING;
            break;
        case KEYW_BOOL:
            return TAB_SYM_VAR_NO_AUTO_BOOLEAN;
            break;
        default:
            return 42;
    }
}


/**
 * Uvodni funkce parseru, vubec nevim, co ma delat
 * U nas je syntakticky validni i prazdny program, je to semanticka chyba?
 */
void parse() {
    
    int result;
    
    //vytvorim globalni tabulku symbolu
    if ((globalTable = tabSymCreate(TAB_SYM_GLOB)) == NULL) {
        FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
    }
    
    
    result = parseFunction();
    
    
}

/**
 * pravidlo ve tvaru:
 *          <function> -> <Kdata_types> fID(<arguments>)<body><function>
 * @return 
 */
int parseFunction() {
    int tokenVal, result;
    tToken *token;
    tTabSymVarNoAutoDataType returnType;
    tTabSymElemData *wasDeclared;
    //pomoci teto promenne, predam funkci pro zpracovani
        // parametru informaci, zda ma provadet kontrolu, nebo
        //plnit seznam parametru
        //0 - vkladat, 1 - provadet kontrolu
    bool compareOrAdd = 0;
    //uchovavam nazev identifikatoru
    string idName;
    //pro kazdou funkci tvorim novy seznam parametru
    tParamListPtr paramList;
    initList(paramList);
    
    
    
    
    //volani funkce pro zpracovani <Kdata_types>
    if ((result = kDataTypes(&returnType)) != 1) {
        //uvolnim token
        freeTokenMem(token);
        return result;
    }
    //uvolnim token
    freeTokenMem(token);
    
    if((tokenVal = getToken(token, f)) != 1)
        return LEXICAL_ERR;
    
    //dalsi token je ID - <function> -> <Kdata_types> fID
    if (token->typ == TYPE_IDENTIFICATOR) {
        //do idName ulozim identifikator
        //identifikator potrebuji ulozit do globalni tabulky symbolu
        idName = token->value.stringVal;

        //funkce jeste nebyla deklarovana
        if((wasDeclared = tabSymSearch(globalTable, &idName)) == NULL) {
            //zkontroluji, zda je ID pouzitelne
            if((strcmp(idName->str, "length") == 0) || (strcmp(idName->str, "substr") == 0) ||
                    (strcmp(idName->str, "concat") == 0) || (strcmp(idName->str, "find") == 0) ||
                    (strcmp(idName->str, "sort") == 0)) {
                freeTokenMem(token);
                return SEMANTIC_ERROR; //pokus o redefinici funkce
            }
        }
        //funkce uz byla deklarovana
        else {
            //zkontroluji, zda je dany identifikator identifikatorem funkce
            if (wasDeclared->type != TAB_SYM_FUNCTION) {
               //TODO - jaka je to chyba?
                freeTokenMem(token);
                return 42;
            }
            //v tuto chvili uz muzu zkontrolovat, zda sedi navratovy typ funkce
            if(wasDeclared->info.func->retType != returnType) {
                freeTokenMem(token);
                return SEMANTIC_ERROR; //nesouhlasi navratovy typ
                //TODO: jak dam vedet funkci parseParam, jestli ma kontrolovat, ci vkladat?
            }
            compareOrAdd = 1;
        }
        //uvolneni tokenu
        freeTokenMem(token);
        
        //dalsi token by mel byt '('
        tokenVal = getToken(token, f);
        //token byl '('
        if(token->typ == PARENTHESIS_OPENING) {
            //volani funkce pro zpracovani <arguments>
            result = parseArguments();
            //behem funkce arguments nastala chyba
            if(result != 1)
                return result;

            //pokracovani pravidla...
            //<function> -> <Kdata_types> fID(<arguments>)<body>
            //jsme u <body> -> bud ';'(deklarace), nebo '{' (definice)
            tokenVal = getToken(token, f);
            if(token->typ == SEMICOLON) {
                return parseFunction();
            }

        }
        else {
            freeTokenMem(token);
            return SYNTAX_ERR;
        }
    }
    //dalsi token neni identifikator
    else {
        freeTokenMem(token);
        return SYNTAX_ERR;
    }
}

/**
 * pro pravidla 8 a 9:
 *          <arguments> -> epsilon
 *          <arguments> - > argument
 * @return SYNTAX_OK(1) - pokud je vse v poradku
 *         SYNTAX_FAILED(0) - pokud nastala chyba
 */
int parseArguments() {
    int tokenVal, result;
    tToken *token;
    string idName;
    
    tokenVal = getToken(token, f);
    
    //mel bych plnit seznam parametru
    switch(token->typ) {
        case KEYW_INT:
        case KEYW_DOUBLE:
        case KEYW_STRING:
        case KEYW_BOOL:
            
            isComma = false;
            tokenVal = getToken(token, f);
            
            // <Kdata_types>ID
            if(token->typ == TYPE_IDENTIFICATOR) {
                //ulozeni identifikatoru, ktery musim jeste ulozit do seznamu parametru
                idName = token->value.stringVal;
                
                //pozadam si o dalsi token, ktery muze byt carka, nebo uzaviraci zavorka
                tokenVal = getToken(token, f);
                //<Kdata_types>ID,
                if(token->typ == COMMA) {
                    //poznamenam si, ze  mi prisla carka
                    //z duvodu, ze po carce nemuze prijit ')'
                    isComma = true;
                    return parseArguments();
                }
                //<Kdata_types>ID)
                else if(token->typ == PARENTHESIS_CLOSING) {
                    return SYNTAX_OK;
                }
            }
            else {
                return SYNTAX_FAILED;
            }
        break;
        
        case PARENTHESIS_CLOSING:
            if (isComma = false) {
                return SYNTAX_OK;
            }
            else {
                return SYNTAX_FAILED;
            }
            break;
        
        default:
            return SYNTAX_FAILED;
    }
}

/**
 * 4. <Kdata_types> -> keyw_bool
 * 5. <Kdata_types> -> keyw_int
 * 6. <Kdata_types> -> keyw_string
 * 7. <Kdata_types> -> keyw_double
 * @return          vraci 1
 */
int kDataTypes(tTabSymVarNoAutoDataType *variableType) {
    tToken *token;
    int result;
    //prisel chybny token
    if((result = getToken(token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    //ocekavana klicova slova
    switch(token->typ) {
        case KEYW_INT:
        case KEYW_DOUBLE:
        case KEYW_STRING:
        case KEYW_BOOL:
            
            //prevedu typ tokenu na typ promenne
            *variableType = tokenTypeToVarType(token->typ);
            return 1;
        //jina hodnota - chyba
        default:
            return SYNTAX_ERR;
    }
}