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
#define MEM_ALLOC_ERROR 99
#define INTERNAL_ERROR 42

// vytvorim strukturu pro globalni tabulku symbolu
tTabSym *globalTable;
FILE *f;

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
 * Uvodni funkce parseru
 * zpracovava chyby a uvolnuje pamet
 */
void parse() {
    
    int result;
    
    //vytvorim globalni tabulku symbolu
    if ((globalTable = tabSymCreate(TAB_SYM_GLOB)) == NULL) {
        FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
    }
    
    //TODO - osetreni chyb a uvolneni pameti
    result = parseFunction();
    
    
}

/**
 * pravidlo ve tvaru:
 *          <function> -> <Kdata_types> fID(<arguments>)<body><function>
 * @return 
 */
int parseFunction() {
    int result;
    tToken *token;
    tTabSymVarNoAutoDataType returnType;
    //promenna, ktera slouzi pro kontrolu, zda uz je dana funkce deklarovana
    tTabSymElemData *funcID_info;
    //uchovavam nazev identifikatoru
    string idName;
    //pro kazdou funkci tvorim novy seznam parametru
    tParamListPtr paramList;
    initList(paramList);
    
    
    //nactu prvni token, prisel chybny token
    if((result = getToken(token, f)) != 1) {
        return LEXICAL_ERR;
    }
    //program muze byt i prazdny
    if (token->typ == EOF) {
        freeTokenMem(token);
        return 1;
    }
       
    //volani funkce pro zpracovani <Kdata_types>
    if ((result = kDataTypes(&returnType, token->typ)) != 1) {
        //uvolnim token
        freeTokenMem(token);
        return result;
    }
    //uvolnim token
    freeTokenMem(token);
    
    if((result = getToken(token, f)) != 1)
        return LEXICAL_ERR;
    
    //dalsi token je ID - <function> -> <Kdata_types> fID
    if (token->typ == TYPE_IDENTIFICATOR) {
        //do idName ulozim identifikator
        //identifikator potrebuji ulozit do globalni tabulky symbolu
        idName = token->value.stringVal;

        //funkce jeste nebyla deklarovana
        if((funcID_info = tabSymSearch(globalTable, &idName)) == NULL) {
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
            if (funcID_info->type != TAB_SYM_FUNCTION) {
               //TODO - jaka je to chyba?
                freeTokenMem(token);
                return INTERNAL_ERROR;
            }
            //v tuto chvili uz muzu zkontrolovat, zda sedi navratovy typ funkce
            if(funcID_info->info.func->retType != returnType) {
                freeTokenMem(token);
                return SEMANTIC_ERROR; //nesouhlasi navratovy typ
            }
        }
        //uvolneni tokenu
        freeTokenMem(token);
        
        //dalsi token by mel byt '('
        if ((result = getToken(token, f) != 1)) {
            return LEXICAL_ERR;
        }
        //token byl '('
        if(token->typ == PARENTHESIS_OPENING) {
            
            //vytvorim si lokalni tabulku symbolu
            tTabSym *localTabSym = tabSymCreate(TAB_SYM_LOC);
            
            //volani funkce pro zpracovani <arguments>
            result = parseArguments(paramList, funcID_info, localTabSym);
            //behem funkce parseArguments nastala chyba
            if(result != 1)
                return result;

            //<function> -> <Kdata_types> fID(<arguments>)<body>
            //jsme u <body> -> bud ';'(deklarace), nebo '{' (definice)
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //jde pouze o deklaraci funkce
            if(token->typ == SEMICOLON) {
                //zpracovavame nasledujici funkci
                return parseFunction();
            }
            
            //jedna se o definici funkce
            if(token->typ == BRACES_OPENING) {
                //zkontroluji, zda se uz dana funkce nebyla definovana
                if (funcID_info->info.func->defined == true) {
                    freeTokenMem(token);
                    return SEMANTIC_ERROR;
                }
                //funkce jeste nebyla definovana
                //<function> -> <Kdata_types> fID (<arguments>)<body>
                //TODO - doplnit funkci zpracovavajici <statementList>
                result = parseStatementList(localTabSym);
                
            }

        }
        //token neni oteviraci zavorka
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
 * zpracovava nasledujici pravidla:
 * 4. <Kdata_types> -> keyw_bool
 * 5. <Kdata_types> -> keyw_int
 * 6. <Kdata_types> -> keyw_string
 * 7. <Kdata_types> -> keyw_double
 * @param variableType[out]     -   datovy typ tokenu
 * @param tokenType[in]         -   typ prijimaneho tokenu
 * @return      funkce vraci 1, pokud je vse v poradku
 */
int kDataTypes(tTabSymVarNoAutoDataType *variableType, TokenTypes tokenType) {
    
    //ocekavana klicova slova
    switch(tokenType) {
        case KEYW_INT:
        case KEYW_DOUBLE:
        case KEYW_STRING:
        case KEYW_BOOL:
            
            //prevedu typ tokenu na typ promenne
            *variableType = tokenTypeToVarType(tokenType);
            return 1;
        //jina hodnota - chyba
        default:
            return SYNTAX_ERR;
    }
}

/**
 * * zpracovava pravidla:
 * 8. <arguments> -> epsilon
 * 9. <arguments> -> <argument>
 * @param paramList[out]     -   seznam argumentu k naplneni
 * @param data[in]           -   NULL, jestli se identifikator funkce v globalni tabulce nenachazi
 *                               jinak odkaz na dany prvek
 * @param localTabel[out]    -  odkaz na lokalni tabulku, do ktere ukladame parametry
 * @return 
 */
int parseArguments(tParamListPtr paramList, tTabSymElemData *data, tTabSym *localTable) {
    int result;
    tToken *token;
    //typ parametru
    tTabSymVarNoAutoDataType paramType;
    
    //nacteme token
    if((result = getToken(token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    //zadne argumenty: pravidlo 8
    if(token->typ == PARENTHESIS_CLOSING) {
        //pokud je dany ID v globalni tabulce, provadime semantickou kontrolu hlavicky
        if(data != NULL) {
            //list argumentu neni prazdny, chyba
            if(data->info.func->params->first != NULL) {
                freeTokenMem(token);
                return SEMANTIC_ERROR;
            }
        }
        freeTokenMem(token);
        return 1;
    }
    
    //upravene pravidlo 9: <arguments> -> <argument>
    if ((result = kDataTypes(&paramType, token->typ)) != 1) {
        //uvolnim token
        freeTokenMem(token);
        return result;
    }
    //seznam parametru neni prazdny, nastavime aktivitu prvni prvek seynamu parametru
    first(data->info.func->params);
    
    //volam funkci pro zpracovani argumentu
    return parseArgument(paramList, data, paramType, localTable);
}

/**
 * pravidlo 10: <argument> -> <Kdata_types>ID<argumentNext>
 * @param paramList[out]        -   seznam parametru k doplneni 
 * @param data[in]              -   NULL, jestli se identifikator funkce v globalni tabulce nenachazi 
 *                                  jinak odkaz na dany prvek
 * @param paramType[in]         -   datovy typ promenne
 * @param localTabel[out]       -  odkaz na lokalni tabulku, do ktere ukladame parametry
 * @return      pokud probehlo vse v poradku, tak 1
 */
int parseArgument(tParamListPtr paramList, tTabSymElemData *data,
                    tTabSymVarNoAutoDataType paramType, tTabSym *localTable) {
    tToken *token;
    int result;
    //uchovavam nazev identifikatoru
    string idName;
    
    //pozadam o novy token
    if((result = getToken(token, f)) != 1) {
        freeTokenMem(token);
        return LEXICAL_ERR;
    }
    //token neni ID
    if(token->typ != IDENTIFICATOR) {
        freeTokenMem(token);
        return SYNTAX_ERR;
    }
    //ulozim si nazev identifikatoru
    idName = token->value.stringVal;
    
    //vkladam prvek do seznamu parametru a lokalni tabulky symbolu
    if(data == NULL) {
        //vkladam do seznamu
        if (insertEl(paramList, &idName, paramType) == 0) {
            return MEM_ALLOC_ERROR;
        }
        //kontroluji, zda uz neni promenna definovana a vkladam do lokalni tabulky symbolu
        tTabSymElemData *localTableInfo = tabSymSearch(localTable, &idName);
        //promenna uz byla definovana
        if(localTableInfo != NULL) {
            return SEMANTIC_ERROR;
        }
        else {
            //vytvorim informace o promenne - nejsem si jisty, jestli musim/muzu pretypovavat
            tVariableInfo* varInfo = tabSymCreateVariableInfo((tTabSymVarDataType)paramType);
            if (varInfo == NULL) {
                return INTERNAL_ERROR;
            }
            //vlozim promennou do lokalni tabulky symbolu
            if (tabSymInsertVar(localTable, &idName, varInfo) == 0) {
                return INTERNAL_ERROR;
            }
        }
    }
    //porovnavam parametry
    else {
        if ((paramType != data->info.func->params->act->dataType) ||
                (strcmp(idName->str, data->info.func->params->act->idName->str) != 0)) {
            return SEMANTIC_ERROR;
        }
    }
    
    //volam dalsi cast pro zpracovani parametru
    return argumentNext(paramList, data);
}

/**
 * funkce zpracovava providla:
 * 11. <argumentNext> -> epsilon
 * 12. <argumentNext> -> , <argument>
 * @return      pokud probehlo vse v poradku, tak 1
 */
int argumentNext(tParamListPtr paramList, tTabSymElemData *data) {
    tToken *token;
    int result;
    //typ parametru
    tTabSymVarNoAutoDataType paramType;
    
    //nactu dalsi token
    if((result = getToken(token, f)) != 1) {
        freeTokenMem(token);
        return LEXICAL_ERR;
    }
    
    //token je ')'
    if (token->typ == PARENTHESIS_CLOSING) {
        freeTokenMem(token);
        //zkontroluji, zda neni dana funkce uz deklarovana s vice parametry
        if (data->info.func->params->act->next != NULL) {
            return SEMANTIC_ERROR;
        }
        return 1;
    }
    //token je ,
    else if(token->typ == COMMA) {
        //uvolnim token
        freeTokenMem(token);
        //prectu jeste jeden token, jelikoz funkce parse Argument
        //ocekava jako prvni token v me implementaci az ID
         if((result = getToken(token, f)) != 1) {
            freeTokenMem(token);
            return LEXICAL_ERR;
        }
        if ((result = kDataTypes(&paramType, token->typ)) != 1) {
            //uvolnim token
            freeTokenMem(token);
             return result;
        }
        //posunu se v seznamu argumentu na dalsi prvek
        succ(data->info.func->params);
        return parseArgument(paramList, data, paramType);
    }
    //neocekavany token
    else {
        freeTokenMem(token);
        return SYNTAX_ERR;
    }
}


int parseStatementList(tTabSym *localTable) {
    tToken *token;
    int result;
    
    //pozadam o token
    if((result = getToken(token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    switch(token) {
        case KEYW_INT:
        case KEYW_DOUBLE:
        case KEYW_STRING:
        case KEYW_BOOL:
        
            break;
        case KEYW_AUTO:
       
            break;
        case BRACES_CLOSING:
            
            break;
            
        case BRACES_OPENING:
            
            break;
        case KEYW_IF:
            
            break;
        case KEYW_FOR:
            
            break;
        case KEYW_WHILE:
            
            break;
        case KEYW_RETURN:
            
            break;
        case KEYW_CIN:
            
            break;
        case KEYW_COUT:
            
            break;
        default:
            return SYNTAX_ERR;
    }
}