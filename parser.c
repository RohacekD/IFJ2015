#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"
#include "tabSym.h"
#include "error.h"
#include "parser.h"
#include "token.h"
#include "parserExpr.h"
#include <stdbool.h>
#include <string.h>
#include <limits.h>

//ve funkcich muze dojit k ruznym typum chyb
//chyba v lexikalni analyze(kdy ma vratit 1), nemuzu
//zakodovat jako 1, protoze to u me znamena uspech
#define LEXICAL_ERR 0
#define SYNTAX_ERR 2
#define SEMANTIC_ERROR_DEFINITION 3
#define SEMANTIC_ERROR_PARAM 4
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
tTabSymVarDataType tokenTypeToVarType(TokenTypes ttype) {
    switch(ttype) {
        case KEYW_INT:
            return TAB_SYM_VAR_INTEGER;
            break;
        case KEYW_DOUBLE:
            return TAB_SYM_VAR_DOUBLE;
            break;
        case KEYW_STRING:
            return TAB_SYM_VAR_STRING;
            break;
        case KEYW_BOOL:
            return TAB_SYM_VAR_BOOLEAN;
            break;
        case KEYW_AUTO:
            return TAB_SYM_VAR_AUTO;
        default:
            return INTERNAL_ERROR;
    }
}

/**
 * funkce rozpozna, zda je dany token term
 * @param ttype         -       typ token
 * @return  1, pokud je token terminal, jinak 0
 */
int isTerm(TokenTypes ttype) {
    switch(ttype) {
        case TYPE_BOOL:
        case TYPE_DOUBLE:
        case TYPE_STRING:
        case TYPE_INTEGER:
        case TYPE_IDENTIFICATOR:
            return 1;
            break;
        default:
            return 0;
    }
}


//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
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
    if ((result = parseFunction()) != 1) {
        //TODO
        //uvolneni pameti - globalni tabulky symbolu
        tabSymFree(globalTable);
        //vraceni chyboveho kodu na zaklade chyby v parseru
        switch(result) {
            case LEXICAL_ERR:
                //TODO
            case SYNTAX_ERR:
                //TODO
            case SEMANTIC_ERROR_DEFINITION:
                //TODO
            case SEMANTIC_ERROR_PARAM:
                //TODO
            case MEM_ALLOC_ERROR:
                //TODO
            case INTERNAL_ERROR:
                //TODO
            default:
                break;
        }
    }
    //TODO - syntakticka a semanticka kontrola probehla v poradku
    else {
        //TODO;
    }
    
    
}


//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
/**
 * pravidlo ve tvaru:
 *          <function> -> <Kdata_types> fID(<arguments>)<body><function>
 * @return 
 */
int parseFunction() {
    int result;
    tToken token;
    //oznacuje, zda byla funkce deklarovana, nebo definovana
    bool defined = false;
    //do teto promenne ukladam navratovy typ funkce
    tTabSymVarDataType returnType;
    //promenna, ktera slouzi pro kontrolu, zda uz je dana funkce deklarovana
    tTabSymElemData *funcID_info;
    //promenna do ktere ukladam vytvorene informace o funkci
    tFuncInfo *funcInfo;
    //uchovavam nazev identifikatoru
    string idName;
    //pro kazdou funkci tvorim novy seznam parametru
    tParamListPtr paramList;
    initList(paramList);
    
    
    //nactu prvni token, prisel chybny token
    if((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    //program muze byt i prazdny
    if (token->typ == EOF) {
        freeTokenMem(token);
        return 1;
    }
       
    //volani funkce pro zpracovani <Kdata_types> - kontrola, zda mi prisel datovy typ
    if ((result = kDataTypes(&returnType, token->typ)) != 1) {
        //uvolnim token
        freeTokenMem(token);
        return result;
    }
    //uvolnim token
    freeTokenMem(token);
    
    if((result = getToken(&token, f)) != 1)
        return LEXICAL_ERR;
    
    //dalsi token je ID - <function> -> <Kdata_types> fID
    if (token->typ == TYPE_FUNCTION_IDENTIFICATOR) {
        //do idName ulozim identifikator
        //identifikator potrebuji ulozit do globalni tabulky symbolu
        idName = token->value.stringVal;

        //funkce jeste nebyla deklarovana
        if((funcID_info = tabSymSearch(globalTable, &idName)) == NULL) {
            //zkontroluji, zda je ID pouzitelne
            if((strcmp(idName.str, "length") == 0) || (strcmp(idName.str, "substr") == 0) ||
                    (strcmp(idName.str, "concat") == 0) || (strcmp(idName.str, "find") == 0) ||
                    (strcmp(idName.str, "sort") == 0)) {
                freeTokenMem(token);
                return SEMANTIC_ERROR_DEFINITION; //pokus o redefinici funkce
            }
        }
        //funkce uz byla deklarovana
        else {
            //zkontroluji, zda je dany identifikator identifikatorem funkce
            if (funcID_info->type != TAB_SYM_FUNCTION) {
                freeTokenMem(token);
                return INTERNAL_ERROR;
            }
            //v tuto chvili uz muzu zkontrolovat, zda sedi navratovy typ funkce
            if(funcID_info->info.func->retType != (tTabSymVarNoAutoDataType)returnType) {
                freeTokenMem(token);
                return SEMANTIC_ERROR_PARAM; //nesouhlasi navratovy typ
            }
        }
        //uvolneni tokenu
        freeTokenMem(token);
        
        //dalsi token by mel byt '('
        if ((result = getToken(&token, f) != 1)) {
            return LEXICAL_ERR;
        }
        //token byl '('
        if(token->typ == PARENTHESIS_OPENING) {
            //uvolnim token
            freeTokenMem(token);
            
            //vytvorim si lokalni tabulku symbolu 
            //do ni uz budu ukladat parametry funkce
            tTabSym *localTabSym = tabSymCreate(TAB_SYM_LOC);
            
            //volani funkce pro zpracovani <arguments>
            if((result = parseArguments(paramList, funcID_info, localTabSym)) != 1) {
                //navratim chybovy kod
                return result;
            }

            //<function> -> <Kdata_types> fID(<arguments>)<body>
            //jsme u <body> -> bud ';'(deklarace), nebo '{' (definice)
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //jde pouze o deklaraci funkce
            if(token->typ == SEMICOLON) {
                //uvolnim token
                freeTokenMem(token);
                
                //TODO - vytvoreni informaci o globalni tabulce symbolu
                if((funcInfo = tabSymCreateFuncInfo(&paramList, (tTabSymVarNoAutoDataType)returnType, 
                        localTabSym, NULL, NULL, false)) == NULL) {
                    return INTERNAL_ERROR;
                }
                
                //TODO - vlozeni id funkce do globalni tabulky symbolu
                if((result = tabSymInsertFunc(globalTable, &idName, funcInfo)) == 0) {
                    return INTERNAL_ERROR;
                }
                
                //muzeme zpracovavat  dalsi funkci 
                return parseFunction();
            }
            
            //jedna se o definici funkce
            if(token->typ == BRACES_OPENING) {
                //zkontroluji, zda se uz dana funkce nebyla definovana
                if (funcID_info->info.func->defined == true) {
                    freeTokenMem(token);
                    return SEMANTIC_ERROR_DEFINITION;
                }
                
                freeTokenMem(token);
                
                //funkce nyni uz je definovana
                defined = true;
                
                //vytvorim prazdny seznam tabulek bloku
                //TODO - nesmim zapomenout na uvolneni pameti
                tTabSymList *blockList;
                if ((blockList = tabSymListCreate()) == NULL) {
                    return INTERNAL_ERROR;
                } 
                
                //vytvoreni instrukcni pasky
                tInsTape *instructionTape;
                if ((instructionTape = insTapeCreate()) == NULL) {
                    return INTERNAL_ERROR;
                }
                //<function> -> <Kdata_types> fID (<arguments>)<body>
                if ((result = parseStatementList(localTabSym, blockList, NULL, instructionTape)) != 1) {
                    return result;
                }
                
                //muzu zpracovavat dalsi funkci
                return parseFunction();
                
            }
            
            //token neni ; ani {
            freeTokenMem(token);
            return SYNTAX_ERR;

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
    //neco je spatne
    return INTERNAL_ERROR;
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
int kDataTypes(tTabSymVarDataType *variableType, TokenTypes tokenType) {
    
    //ocekavana klicova slova
    switch(tokenType) {
        case KEYW_INT:
        case KEYW_DOUBLE:
        case KEYW_STRING:
        case KEYW_BOOL:
            
            //prevedu typ tokenu na typ promenne
            if ((*variableType = tokenTypeToVarType(tokenType)) == INTERNAL_ERROR) {
                return INTERNAL_ERROR;
            }
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
    tToken token;
    //typ parametru
    tTabSymVarDataType paramType;
    
    //nacteme token
    if((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    //zadne argumenty: pravidlo 8
    if(token->typ == PARENTHESIS_CLOSING) {
        //pokud je dany ID v globalni tabulce, provadime semantickou kontrolu hlavicky
        if(data != NULL) {
            //list argumentu neni prazdny, chyba
            if((*data->info.func->params)->first != NULL) {
                freeTokenMem(token);
                return SEMANTIC_ERROR_PARAM;
            }
        }
        freeTokenMem(token);
        return 1;
    }
    
    //upravene pravidlo 9: <arguments> -> <argument>
    //kontroluji, zda je token datoveho typu
    if ((result = kDataTypes(&paramType, token->typ)) != 1) {
        //uvolnim token
        freeTokenMem(token);
        return result;
    }
    
    //token je klicove slovo pro datovy typ
    freeTokenMem(token);
    //seznam parametru neni prazdny, nastavime aktivitu prvni prvek seznamu parametru
    first(*(data->info.func->params));
    
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
int parseArgument(tParamListPtr paramList, tTabSymElemData *data, tTabSymVarDataType paramType, tTabSym *localTable) {
    tToken token;
    int result;
    //uchovavam nazev identifikatoru
    string idName;
    
    //pozadam o novy token
    if((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    //token neni ID
    if(token->typ != TYPE_IDENTIFICATOR) {
        freeTokenMem(token);
        return SYNTAX_ERR;
    }
    
    //token je identifikator
    //ulozim si nazev identifikatoru
    idName = token->value.stringVal;
    freeTokenMem(token);
    
    //vkladam prvek do seznamu parametru a lokalni tabulky symbolu
    if(data == NULL) {
        
        //kontroluji, zda uz neni promenna definovana a vkladam do lokalni tabulky symbolu
        tTabSymElemData *localTableInfo = tabSymSearch(localTable, &idName);
        tTabSymElemData *globalTableInfo = tabSymSearch(globalTable, &idName);
        //promenna uz byla definovana
        if(localTableInfo != NULL || globalTableInfo != NULL) {
            return SEMANTIC_ERROR_DEFINITION;
        }
         //vkladam do seznamu parametru
        if (insertEl(paramList, &idName, paramType) == 0) {
            return MEM_ALLOC_ERROR;
        }
        //podarilo se mi vlozit parametr do seznamu parametru
        else {
            //TODO - vlozeni promenne do lokalni tabulky symbolu
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
        if (((tTabSymVarNoAutoDataType)paramType != (*data->info.func->params)->act->dataType) ||
                (strcmp(idName.str, (*data->info.func->params)->act->idName->str) != 0)) {
            return SEMANTIC_ERROR_PARAM;
        }
    }
    
    //volam dalsi cast pro zpracovani parametru
    return argumentNext(paramList, data, localTable);
}

/**
 * funkce zpracovava providla:
 * 11. <argumentNext> -> epsilon
 * 12. <argumentNext> -> , <argument>
 * @return      pokud probehlo vse v poradku, tak 1
 */
int argumentNext(tParamListPtr paramList, tTabSymElemData *data, tTabSym *localTable) {
    tToken token;
    int result;
    //typ parametru
    tTabSymVarDataType paramType;
    
    //nactu dalsi token
    if((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    //token je ')'
    if (token->typ == PARENTHESIS_CLOSING) {
        freeTokenMem(token);
        //zkontroluji, zda neni dana funkce uz deklarovana s vice parametry
        if ((*data->info.func->params)->act->next != NULL) {
            return SEMANTIC_ERROR_PARAM;
        }
        return 1;
    }
    //token je ,
    else if(token->typ == COMMA) {
        //uvolnim token
        freeTokenMem(token);
        //prectu jeste jeden token, jelikoz funkce parse Argument
        //ocekava jako prvni token v me implementaci az ID
         if((result = getToken(&token, f)) != 1) {
            return LEXICAL_ERR;
        }
        if ((result = kDataTypes(&paramType, token->typ)) != 1) {
            //uvolnim token
            freeTokenMem(token);
             return result;
        }
        freeTokenMem(token);
        //posunu se v seznamu argumentu na dalsi prvek
        succ((*data->info.func->params));
        return parseArgument(paramList, data, paramType, localTable);
    }
    //neocekavany token
    else {
        freeTokenMem(token);
        return SYNTAX_ERR;
    }
}


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
 * @return      pokud probehlo vse v poradku, tak 1
 */
int parseStatementList(tTabSym *localTable, tTabSymList *blockList,
                        tTabSymListElemPtr parent, tInsTape *instructionTape) {
    tToken token;
    int result;
    tTabSymVarDataType dataType;
    
    //pozadam o token
    if((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    switch(token->typ) {
        //pravidlo 17 - <st_list> -> <declaration><st_list>
        case KEYW_INT:
        case KEYW_DOUBLE:
        case KEYW_STRING:
        case KEYW_BOOL:
        case KEYW_AUTO:
            
            //ulozim si datovy typ promenne
            if ((dataType = tokenTypeToVarType(token->typ)) == INTERNAL_ERROR) {
                freeTokenMem(token);
                return INTERNAL_ERROR;
            }
            
            //uvolnim token
            freeTokenMem(token);
            
            //volam funkci pro zpracovani deklarace
            // parent je v tomto pripade aktualni element z listu tabulek symbolu bloku
            if ((result = parseDeclaration(dataType, localTable, instructionTape, parent)) != 1) {
                return result;
            }
            
            //opet volam funkci pro zpracovani statement-listu
            return parseStatementList(localTable, blockList, parent, instructionTape);
            
            break;
            
        //pravidlo 15 - <st_list> -> epsilon (konec funkce)
        //TODO jak a kde kontrolovat, zda dana funkce obsahovala return
        //ukonceni bloku funkce, nebo normalniho bloku
        case BRACES_CLOSING:
            freeTokenMem(token);
            return 1;
            break;
        
        //pravidlo 18 - <st_list> -> {<st_list>}<st_list>
        //TODO - zanoreny blok, musim vytvorit novou lokalni tabulku symbolu...
        //TODO - mel bych vytvorit instrukci, pomoci niz dam interpretu vedet
                //ze si ma vytvorit novy ramec
        case BRACES_OPENING:
            freeTokenMem(token);
            
            // list tabulek symbolu pro bloky
            tTabSym *blockLocalTable;
            // polozka v seznamu tabulek pro bloky
            tTabSymListElemPtr newElement;
            
            //vytvoreni nove lokalni tabulky symbolu
            if ((blockLocalTable = tabSymCreate(TAB_SYM_LOC)) == NULL){
                return INTERNAL_ERROR;
            }
            
            //vlozeni nove lokalni tabulky symbolu do listu bloku
            if((newElement = tabSymListInsertTable(blockList, blockLocalTable, parent)) == NULL) {
                return INTERNAL_ERROR;
            }
            
            
            //rodicem se stane novy element, budeme pracovat s novou lokalni tabulkou
            //a novym listem, insrtrukcni paska je stejna
            if ((result = parseStatementList(blockLocalTable, newElement->childList, newElement, instructionTape)) != 1) {
                return result;
            }
            
            //TODO - toto asi nechci
            /*
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //dalsi token by mel byt uzaviraci slozena zavorka
            if(token->typ != BRACES_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
             */
            
            return parseStatementList(localTable, blockList, parent, instructionTape);
            break;
            
        //pravidlo 16 - <st_list> -> <statement><st_list>
        case KEYW_IF:
        case KEYW_FOR:
        case KEYW_WHILE:
        case KEYW_RETURN:
        case KEYW_CIN:
        case KEYW_COUT:
        case KEYW_DO:
        case TYPE_IDENTIFICATOR:
        case INCREMENTATION:
        case DECREMENTATION:
            
            
            //TODO - jeste nevim, co vse musim predat funkci parseStatement
            if((result = parseStatement(localTable, token, instructionTape, blockList, parent)) != 1) {
                return result;
            }
            
            return parseStatementList(localTable, blockList, parent, instructionTape);
            
            break;
        default:
            return SYNTAX_ERR;
    }
    //nikdy bych se sem nemel dostat
    return INTERNAL_ERROR;
}



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
 * @return      pokud probehlo vse v poradku, tak 1
 */
int parseStatement(tTabSym *localTable, tToken tokenOrig, tInsTape *instructionTape,
                    tTabSymList *blockList,tTabSymListElemPtr blockListElem) {
    tToken token;
    int result;
    tTabSymVarNoAutoDataType expressionType;
    tTabSymVarDataType dataType;
    
    switch(tokenOrig->typ) {
        //pravidlo 23 - if(expression)<block><else>
        case KEYW_IF:
            //uvolnim pamet tokenu
            freeTokenMem(tokenOrig);
            
            //nactu dalsi token - mel by byt '('
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            if (token->typ != PARENTHESIS_OPENING){
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            //TODO - volat parser pro zpracovani vyrazu
            if((result = parseExpression(globalTable, blockListElem ,localTable, instructionTape , &expressionType, f)) != 1) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            //ocekavam uzaviraci zavorku
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - doplnit parseBlock
            if((result = parseBlock(localTable, blockList, blockListElem, instructionTape)) != 1) {
                return result;
            }
            
            //TODO - zpracovani else vetve
            if((result = getToken(&token, f)) != 1) {
                return SYNTAX_ERR;
            }
            
            switch(token->typ) {
                //pravidlo 30 - <else> -> epsilon
                case KEYW_IF:
                case KEYW_FOR:
                case KEYW_WHILE:
                case KEYW_RETURN:
                case KEYW_CIN:
                case KEYW_COUT:
                case KEYW_DO:
                case TYPE_IDENTIFICATOR:
                case INCREMENTATION:
                case DECREMENTATION:
                case KEYW_INT:
                case KEYW_DOUBLE:
                case KEYW_STRING:
                case KEYW_BOOL:
                case KEYW_AUTO:
                case BRACES_CLOSING:
                case BRACES_OPENING:
                    //navratim token, dalsi funkce pocitaji s tim, ze ho prectou
                    ungetToken(&token);
                    freeTokenMem(token);
                    return 1;
                    break;
                    
                //pravidlo 31 <else> -> else <block>
                case KEYW_ELSE:
                    //TODO - co poslu bloku
                    freeTokenMem(token);
                    return parseBlock(localTable, blockList, blockListElem, instructionTape);
                    break;
                    
                default:
                    return SYNTAX_ERR;
            }
            break;
            
            
            
        //pravidlo 24 - for(<Kdata_types>ID=expression; expression; <assignment>)<block>
        //TODO - lokalni tabulka u funkce for?
        case KEYW_FOR:
            //uvolnim pamet tokenu
            freeTokenMem(tokenOrig);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //v promenne dataType je ulozeny datovy typ promenne
            if ((result = kDataTypes(&dataType, token->typ)) != 1) {
                //uvolnim token
                freeTokenMem(token);
                return result;
            }
            freeTokenMem(token);
            
            //dalsi token by mel byt ID
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //TODO - ulozeni identifikatoru do tabulky symbolu
            if(token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //dalsi token by mel byt =
            
            if (token->typ != EQUAL) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - volat parser  pro zpracovani vyrazu
            //TODO - zpracovat vystup parseru
            if ((result = parseExpression(globalTable, blockListElem , localTable, instructionTape, &expressionType, f)) != 1) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovat dalsi cast vyrazu
            //TODO - zpracovat vystup parseru
            if ((result = parseExpression(globalTable, blockListElem , localTable, instructionTape, &expressionType, f)) != 1) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - funkce pro parseAssingment
            //TODO - asi ji budu chtit predavat rovnou typ tokenu
            //for(<Kdata_types>ID=expression; expression; <assignment>
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != INCREMENTATION || token->typ != DECREMENTATION ||
                    token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            
            //TODO - co predavat 
            if((result = parseAssignment(token, localTable, instructionTape, blockListElem)) != 1) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam uzaviraci zavorku - for(<Kdata_types>ID=expression; expression; <assignment>)
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO
            return parseBlock(localTable, blockList, blockListElem, instructionTape);

            break;
            
            
            
        //pravidlo 20 - cin >> ID <cin>;
        case KEYW_CIN:
            //uvolneni predaneho tokenu
            freeTokenMem(tokenOrig);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam prvni >
            if (token->typ != GREATER) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam druhe >
            if (token->typ != GREATER) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam identifikator
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            
            string idName = token->value.stringVal;
            
            freeTokenMem(token);
            
            //semanticka kontrola, zda je ID pouzitelne 
            tTabSymElemData *idUsable;
            if ((idUsable = tabSymListSearch(blockListElem, localTable, &idName)) == NULL) {
                return SEMANTIC_ERROR_DEFINITION; //promenna nedefinovana
            }
            
            //TODO - VLOZENI INSTRUKCE
            if ((result = insTapeInsertLast(instructionTape, I_CIN, NULL, NULL, (void*) &idName)) == 0) {
                return INTERNAL_ERROR;
            }
            
            
            if ((result = parseCin(instructionTape, localTable, blockListElem)) != 1) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //dalsi token by mel byt ;
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //pokud jsem se dostal az sem, tak syntakticka analyza probehla bez chyby
            return 1;
            break;
            
            
            
        //pravidlo 21 - <statement> -> cout << <term><cout>;
        case KEYW_COUT:
            freeTokenMem(tokenOrig);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam prvni <
            if (token->typ != LESS) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam druhe <
            if (token->typ != LESS) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //isTerm
            if((result = isTerm(token->typ)) == 0){
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani dalsich vstupu
            if((result = parseCout()) != 1) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //dalsi token by mel byt ;
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //pokud jsem se dostal az sem, tak syntakticka analyza probehla v poradku
            return 1;
            break;
            
            
            
        //pravidlo 22 - <statement> -> return expression;
        case KEYW_RETURN:
            freeTokenMem(tokenOrig);
            //TODO - zpracovani vyrazu
            if ((result = parseExpression(globalTable, blockListElem, localTable, instructionTape, &expressionType, f)) != 1) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            return 1;
            break;
            
            
            
        //pravidlo 25 - while(expression)<block>
        case KEYW_WHILE:
            freeTokenMem(tokenOrig);
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani vyrazu
            if ((result = parseExpression(globalTable, blockListElem ,localTable, instructionTape , &expressionType, f)) != 1) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani bloku
            return parseBlock(localTable, blockList, blockListElem, instructionTape);
            
            break;
            
            
            
        //pravidlo 26 - <statement> -> do <block>while(expression);
        case KEYW_DO:
            freeTokenMem(tokenOrig);
            
            //TODO - zpracovani bloku
            if ((result = parseBlock(localTable, blockList, blockListElem, instructionTape)) != 1) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani vyrazu
            if ((result = parseExpression(globalTable, blockListElem, localTable, instructionTape , &expressionType, f)) != 1) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //pokud jsem se dostal az sem, tak je vse v poradku
            return 1;
            break;
            
            
        //pravidlo 19
        case INCREMENTATION:
        case DECREMENTATION:
        case TYPE_IDENTIFICATOR:
            
            //TODO - parse Assignment
            return parseAssignment(tokenOrig, localTable, instructionTape, blockListElem);
            break;
        default:
            return SYNTAX_ERR;
    }
    //nikdy bych se sem nemel dostat
    return INTERNAL_ERROR;
}


//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
//-----------   DODELAT GENEROVANI INSTRUKCI ---------
/**
 * zpracovava nasledujici pravidla:
 * 36. <declaration> -> <Kdata_types> ID<dec_init>
 * 37. <declaration> -> auto ID = expression;
 * @param dataType          -   datovy typ promenne
 * @param localTable        -   ukazatel na lokalni tabulku symbolu
 * @param instructionTape   -   ukazatel na pasku instrukci
 * @param blockListElem     -   aktualni element v listu tabulek bloku
 * @return      pokud probehlo vse v poradku, tak 1
 */
int parseDeclaration(tTabSymVarDataType dataType, tTabSym *localTable,
                    tInsTape *instructionTape, tTabSymListElemPtr blockListElem) {
    int result;
    tToken token;
    tTabSymElemData *varIdentifikator, *funcIdentifikator;
    tTabSymVarNoAutoDataType expressionType;
    tVariableInfo *variableInfo;
    string idName;
    
    switch(dataType) {
        //pravidlo 36 - <declaration> -> <Kdata_types> ID<dec_init>
        case TAB_SYM_VAR_BOOLEAN:
        case TAB_SYM_VAR_DOUBLE:
        case TAB_SYM_VAR_INTEGER:
        case TAB_SYM_VAR_STRING:
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            
            idName = token->value.stringVal;
            freeTokenMem(token);
            
            //kontroluji, zda uz promenna byla definovana
            varIdentifikator = tabSymSearch(localTable, &idName);
            funcIdentifikator = tabSymSearch(globalTable, &idName);
            if (varIdentifikator != NULL || funcIdentifikator != NULL) {
                return SEMANTIC_ERROR_DEFINITION;
            }
            
            //vytvoreni informaci o promenne
            if ((variableInfo = tabSymCreateVariableInfo(dataType)) == NULL) {
                return MEM_ALLOC_ERROR;
            }
            
            //vlozeni promenne do lokalni tabulky symbolu
            if ((tabSymInsertVar(localTable, &idName, variableInfo)) == 0) {
                return INTERNAL_ERROR;
            }
            
            //cast <decInit>
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //pouze definice promenne
            if(token->typ == SEMICOLON) {
                freeTokenMem(token);
                return 1;
            }
            
            //inicializace
            if(token->typ == EQUAL) {
                freeTokenMem(token);

                 if ((result = parseExpression(globalTable, blockListElem, localTable, instructionTape, &expressionType, f)) != 1) {
                     return result;
                 }
                 
                string *lastGeneratedTMP;
                
                if((lastGeneratedTMP = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                    return INTERNAL_ERROR;
                }
                
                //TODO GENEROVANI INSTRUKCE
                if ((result = insTapeInsertLast(instructionTape, I_ASSIGN, (void*) lastGeneratedTMP, NULL, (void*) &idName)) == 0) {
                    return INTERNAL_ERROR;
                }
                
                 if((result = getToken(&token, f)) != 1) {
                     return LEXICAL_ERR;
                 }
                 
                 //dalsi token by mel byt ;
                 if(token->typ != SEMICOLON){
                     freeTokenMem(token);
                     return SYNTAX_ERR;
                 }
                 freeTokenMem(token);
                 //pokud jsem se dostal az sem, vse je v poradku
                 return 1;
            }
            
            freeTokenMem(token);
            //chybna syntaxe
            return SYNTAX_ERR;
            
            break;
            
            
        // pravidlo 37 - <declaration> -> auto ID = expression;
        case TAB_SYM_VAR_AUTO:
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            
            idName = token->value.stringVal;
            freeTokenMem(token);
            
            //kontroluji, zda uz promenna byla definovana
            varIdentifikator = tabSymSearch(localTable, &idName);
            funcIdentifikator = tabSymSearch(globalTable, &idName);
            if (varIdentifikator != NULL || funcIdentifikator != NULL) {
                return SEMANTIC_ERROR_DEFINITION;
            }
            
            //TODO - zpracovani vyrazu
            if((result = parseExpression(globalTable, blockListElem ,localTable, instructionTape, &expressionType, f)) != 1) {
                return result;
            }
            
            //vytvoreni informaci o promenne
            //TODO - vkladam jeji datovy typ az na zaklade datoveho typu
                    // po vyhodnoceni vyrazu
            if ((variableInfo = tabSymCreateVariableInfo((tTabSymVarDataType)expressionType)) == NULL) {
                return MEM_ALLOC_ERROR;
            }
            
            //vlozeni promenne do lokalni tabulky symbolu
            if ((tabSymInsertVar(localTable, &idName, variableInfo)) == 0) {
                return INTERNAL_ERROR;
            }
            
            //TODO - GENEROVANI INSTRUKCE
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON){
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            //pokud jsem se dostal az sem, tak je vse v poradku
            return 1;
            
            break;
        default:
            return SYNTAX_ERR;
    }
    //sem bych se nikdy nemel dostat
    return INTERNAL_ERROR;
}



//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
//-----------   DODELAT GENEROVANI INSTRUKCI ---------
/**
 * zpracovava nasledujici pravidla:
 * 27.  <block> -> {<st_list>}
 * 28.  <block> -> <statement>
 * @param localTable[in]            -   lokalni tabulka symbolu
 * @param instructionTape           -   ukazatel na pasku instrukci
 * @param blockList                 -   list tabulek symbolu pro bloky
 * @param blockListElem             -   aktualni element v listu tabulek bloku
 * @return          pokud probehlo vse v poradku, tak 1
 */
int parseBlock(tTabSym *localTable, tTabSymList *blockList,
                tTabSymListElemPtr blockListElem, tInsTape *instructionTape) {
    tToken token;
    int result;
    
    if((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    switch(token->typ) {
        // pravidlo 27 - <block> -> {<st_list>}
        //TODO - navratit precteny token, aby funkce parseStatementList
        //vytvorila novy rozsah platnosti pro blok if?
        case BRACES_OPENING:
            freeTokenMem(token);
            ungetToken(&token);
            
            if((result = parseStatementList(localTable, blockList, blockListElem, instructionTape)) != 1) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if (token->typ != BRACES_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            //pokud jsem dosel az sem, tak je vse v poradku
            return 1;
            break;
            
        // pravidlo 28 - <block> -> <statement>
        case KEYW_IF:
        case KEYW_FOR:
        case KEYW_WHILE:
        case KEYW_RETURN:
        case KEYW_CIN:
        case KEYW_COUT:
        case KEYW_DO:
        case TYPE_IDENTIFICATOR:
        case INCREMENTATION:
        case DECREMENTATION:
            
            return parseStatement(localTable, token, instructionTape, blockList, blockListElem);
            
        default:
            return SYNTAX_ERR;
    }
    return SYNTAX_ERR;
}

//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
//nejakym zpusobem musim prohledavat rozsah platnosti (lokalni tabulky bloku...)
/**
 * zpracovava pravidla:
 * 40. <assignment> = ID<specID>
 * 41. <assignment> = <specID>ID
 * @param tokenType
 * @param localTable
 * @return          pokud probehlo vse v poradku, tak 1
 */

int parseAssignment(tToken tokenOrig, tTabSym *localTable, tInsTape *instructionTape,
                tTabSymListElemPtr blockListElem) {
    
    int result; 
    tToken token;
    string idName;
    tTabSymElemData *idUsable;
    
    switch(tokenOrig->typ) {
        case TYPE_IDENTIFICATOR:
            
            idName = tokenOrig->value.stringVal;
            
            //semanticka kontrala, zda je promenna definovana v danem rozsahu platnosti
            if ((idUsable = tabSymListSearch(blockListElem, localTable, &idName)) == NULL) {
                freeTokenMem(tokenOrig);
                return SEMANTIC_ERROR_DEFINITION;
            }
            
            freeTokenMem(tokenOrig);
            
            if((result = getToken(&token, f))) {
                return LEXICAL_ERR;
            }
            
            //TODO semanticky akce a generovani vnitrniho kodu
            if(token->typ == INCREMENTATION) {
                freeTokenMem(token);
                return 1;
            }
            else if(token->typ == DECREMENTATION) {
                freeTokenMem(token);
                return 1;
            }
            else {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            
            break;
            
        case INCREMENTATION:
            
            freeTokenMem(tokenOrig);
            //TODO - semanticke akce a generovani kodu
            if((result = getToken(&token, f))) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            
            idName = token->value.stringVal;
            
            //semanticka kontrala, zda je promenna definovana v danem rozsahu platnosti
            if ((idUsable = tabSymListSearch(blockListElem, localTable, &idName)) == NULL) {
                freeTokenMem(token);
                return SEMANTIC_ERROR_DEFINITION;
            }
            
            freeTokenMem(token);
            //pokud jsem se dostal az sem, vse je v poradku
            return 1;
            break;
            
        case DECREMENTATION:
            
            freeTokenMem(tokenOrig);
            //TODO - semanticke akce a generovani kodu
            if((result = getToken(&token, f))) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            
            idName = token->value.stringVal;
            
            //semanticka kontrala, zda je promenna definovana v danem rozsahu platnosti
            if ((idUsable = tabSymListSearch(blockListElem, localTable, &idName)) == NULL) {
                freeTokenMem(token);
                return SEMANTIC_ERROR_DEFINITION;
            }
            
            freeTokenMem(token);
            //pokud jsem se dostal az sem, vse je v poradku
            return 1;
            break;
            
        default:
            freeTokenMem(tokenOrig);
            return SYNTAX_ERR;
    }
}

//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!
//-----------   DODELAT GENEROVANI INSTRUKCI ---------
/**
 * * zpracovava nasledujici pravidla:
 * 32.  <cin> -> epsilon
 * 33.  <cin> -> >>ID<cin>
 * @param instructionTape           -   odkaz na instrukcni pasku
 * @param localTable                -   odkaz na lokalni tabulku symbolu
 * @param blockListElem             -   aktualni element v tabulce symbolu pro bloky
 * @return              pokud probehlo vse v poradku, tak 1
 */
int parseCin(tInsTape *instructionTape, tTabSym *localTable, tTabSymListElemPtr blockListElem) {
    int result;
    tToken token;
    
    if((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    if(token->typ == SEMICOLON) {
        //navratime token, protoze s nim pocita volajici funkce
        ungetToken(&token);
        return 1;
    }
    
    if(token->typ == GREATER) {
        freeTokenMem(token);
        
        if((result = getToken(&token, f)) != 1) {
            return LEXICAL_ERR;
        }
        //uz mame >>
        if(token->typ == GREATER) {
            freeTokenMem(token);
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam identifikator
            if(token->typ == TYPE_IDENTIFICATOR) {
                string *idName; 
                idName = token->value.stringVal;
                tTabSymElemData* idUsable;
                freeTokenMem(token);
                
                //semanticka kontrola
                if((idUsable = tabSymListSearch(blockListElem, localTable, &idName)) == NULL) {
                    return SEMANTIC_ERROR_DEFINITION;
                }
                
                //TODO - vlozeni instrukce 
                if ((result = insTapeInsertLast(instructionTape, I_CIN, NULL, NULL, (void *) &idName)) == 0) {
                    return INTERNAL_ERROR;
                }
                //muzeme funkci provadet znovu
                return parseCin(instructionTape, localTable, blockListElem);
            }
            
            freeTokenMem(token);
            return SYNTAX_ERR;
        }
        
        freeTokenMem(token);
        return SYNTAX_ERR;
    }
    
    freeTokenMem(token);
    return SYNTAX_ERR;
}


//!!!!!!!!!!!   UNCOMPLETE  !!!!!!!!!!!!

/**
 * zpracovava nasledujici pravidla
 *34. <cout> -> epsilon
 *35. <cout> -> << <term> <cout>
 * @return      pokud probehlo vse v poradku, tak 1
 */
int parseCout() {
    tToken token;
    int result;
    TokenTypes ttype;
    
    if ((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    //pravidlo 34
    if(token->typ == SEMICOLON) {
        freeTokenMem(token);
        return 1;
    }
    
    //pravidlo 35
    if(token->typ == LESS) {
        freeTokenMem(token);
        
        if((result = getToken(&token, f)) != 1) {
            return LEXICAL_ERR;
        }
        
        if(token->typ == LESS) {
            freeTokenMem(token);
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            ttype = token->typ;
            freeTokenMem(token);
            
            if((result = isTerm(ttype)) == 1) {
                return parseCout();
            }
            
            return SYNTAX_ERR;
        }
        
        freeTokenMem(token);
        return SYNTAX_ERR;
    }
    
    freeTokenMem(token);
    return SYNTAX_ERR;
}