#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"
#include "tabSym.h"
#include "error.h"
#include "parser.h"
#include "token.h"
#include <stdbool.h>

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

/*
int allocMemForToken(tToken *token) {
    //alokace pameti pro strukturu 
    if ((token = malloc(sizeof(struct stToken))) == NULL) {
        return 0;
    }
    else {
        
    }
}
*/
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
    tTabSymVarNoAutoDataType dataType;
    
    string idName;
    //pro kazdou funkci tvorim novy seznam parametru
    tParamListPtr paramList;
    initList(paramList);
    
    //tFuncInfo functionInfo;
    
    tokenVal = getToken(token, f);
    
    //navratova hodnota funkce
    switch(token->typ) {
        case KEYW_INT:
        case KEYW_DOUBLE:
        case KEYW_STRING:
        case KEYW_BOOL:
        
            dataType = tokenTypeToVarType(token->typ);
            tokenVal = getToken(token, f);
            
            //nasledujici token je identifikator
            if (token->typ == TYPE_IDENTIFICATOR) {
                //do idName ulozim identifikator
                //abych mohl doplnit globalni tabulku symbolu
                    //musim znat vice informaci
                //chci ulozit identifikator do globalni tabulky identifikatoru
                idName = token->value.stringVal;
                
                //dalsi token by mel byt '('
                tokenVal = getToken(token, f);
                //token byl '('
                if(token->typ == PARENTHESIS_OPENING) {
                    //volani funkce pro zpracovani <arguments>
                    result = parseArguments();
                    //behem funkce arguments nastala chyba
                    if(!result)
                        return SYNTAX_FAILED;
                    
                    //pokracovani pravidla...
                    
                }
                else {
                    //musim uvolnit token
                    return SYNTAX_FAILED;
                }
            }
            //dalsi token neni identifikator
            else {
                return SYNTAX_FAILED;
            }
            
        
        break;
        
        default:
            //musim uvolnit token
            return SYNTAX_FAILED;
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

