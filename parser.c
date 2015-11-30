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
    tToken token;
    //oznacuje, zda byla funkce deklarovana, nebo definovana
    bool defined = false;
    tTabSymVarDataType returnType;
    //promenna, ktera slouzi pro kontrolu, zda uz je dana funkce deklarovana
    tTabSymElemData *funcID_info;
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
       
    //volani funkce pro zpracovani <Kdata_types>
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
            if(funcID_info->info.func->retType != (tTabSymVarNoAutoDataType)returnType) {
                freeTokenMem(token);
                return SEMANTIC_ERROR; //nesouhlasi navratovy typ
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
            
            //vytvorim si lokalni tabulku symbolu
            tTabSym *localTabSym = tabSymCreate(TAB_SYM_LOC);
            
            //volani funkce pro zpracovani <arguments>
            result = parseArguments(paramList, funcID_info, localTabSym);
            //behem funkce parseArguments nastala chyba
            if(result != 1)
                return result;

            //<function> -> <Kdata_types> fID(<arguments>)<body>
            //jsme u <body> -> bud ';'(deklarace), nebo '{' (definice)
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //jde pouze o deklaraci funkce
            if(token->typ == SEMICOLON) {
                //zpracovavame nasledujici funkci
                freeTokenMem(token);
                //TODO - musim do globalni tabulky symbolu ulozit informace o funkci
                return parseFunction();
            }
            
            //jedna se o definici funkce
            if(token->typ == BRACES_OPENING) {
                //zkontroluji, zda se uz dana funkce nebyla definovana
                if (funcID_info->info.func->defined == true) {
                    freeTokenMem(token);
                    return SEMANTIC_ERROR;
                }
                
                //funkce je definovana
                defined = true;
                freeTokenMem(token);
                
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
    //neco je spatne
    return SYNTAX_ERR;
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
        //TODO - muze byt ID funkce a ID promenne stejne
        tTabSymElemData *globalTableInfo = tabSymSearch(globalTable, &idName);
        //promenna uz byla definovana
        if(localTableInfo != NULL || globalTableInfo != NULL) {
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
        if (((tTabSymVarNoAutoDataType)paramType != (*data->info.func->params)->act->dataType) ||
                (strcmp(idName.str, (*data->info.func->params)->act->idName->str) != 0)) {
            return SEMANTIC_ERROR;
        }
    }
    
    freeTokenMem(token);
    
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

/**
 * zpracovava nasledujici pravidla:
 * 15. <st_list> -> epsilon
 * 16. <st_list> -> <statement><st_list>
 * 17. <st_list> -> <declaration><st_list>
 * 18. <st_list> -> {<st_list>}<st_list>
 * @param localTable        -   lokalni tabulka funkce
 * @return      pokud probehlo vse v poradku, tak 1
 */
int parseStatementList(tTabSym *localTable) {
    tToken token;
    int result;
    tTabSymVarDataType dataType;
    TokenTypes tokenType;
    
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
                return INTERNAL_ERROR;
            }
            
            //uvolnim token
            freeTokenMem(token);
            
            //volam funkci pro zpracovani deklarace
            if ((result = parseDeclaration(localTable)) != 1) {
                return result;
            }
            
            //opet volam funkci pro zpracovani statement-listu
            return parseStatementList(localTable);
            
            break;
            
        //pravidlo 15 - <st_list> -> epsilon (konec funkce)
        //TO-DO jak a kde kontrolovat, zda dana funkce obsahovala return
        case BRACES_CLOSING:
            freeTokenMem(token);
            return 1;
            break;
        
        //pravidlo 18 - <st_list> -> {<st_list>}<st_list>
        //TODO - zanoreny blok, musim vytvorit novou lokalni tabulku symbolu...
        //TODO - mel bych vytvorit instrukci, pomoci niz dam interpretu vedet
                //ze si ma vytvorit novy ramec
        //TODO - vsechno to nastavovani veci okolo bloku
        case BRACES_OPENING:
            freeTokenMem(token);
            
            if ((result = parseStatementList(localTable)) != 1) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //dalsi token by mel byt uzaviraci slozena zavorka
            if(token->typ != BRACES_CLOSING) {
                freeTokenMem(token);
                return SEMANTIC_ERROR;
            }
            freeTokenMem(token);
            
            return parseStatementList(localTable);
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
            
            //ulozim si typ tokenu
            tokenType = token->typ;
            freeTokenMem(token);
            
            //TODO - jeste nevim, co vse musim predat funkci parseStatement
            if((result = parseStatement(localTable, tokenType)) != 1) {
                return result;
            }
            
            return parseStatementList(localTable);
            
            break;
        default:
            return SYNTAX_ERR;
    }
}


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
 * @param localTable
 * @param token
 * @return 
 */
int parseStatement(tTabSym *localTable, tTokenTypes tokenType) {
    tToken token;
    int result;
    tTabSymVarNoAutoDataType expressionType;
    tTabSymVarDataType dataType;
    
    switch(tokenType) {
        //pravidlo 23
        case KEYW_IF:
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
            parseExpression(globalTable, localTable, , &expressionType, f);
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            //ocekavam uzaviraci zavorku
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO
            if((result = parseBlock()) != 1) {
                return result;
            }
            
            //TODO
            if((result = parseElse()) != 1) {
                return result;
            }
            
            break;
            
            
            
        //pravidlo 24 - for(<Kdata_types>ID=expression; expression; <assignment>)<block>
        //TODO - lokalni tabulka u funkce for?
        case KEYW_FOR:
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(token, f)) != 1) {
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
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(token, f)) != 1) {
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
            parseExpression(globalTable, localTable, , &expressionType, f);
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovat dalsi cast vyrazu
            //TODO - zpracovat vystup parseru
            parseExpression(globalTable, localTable, , &expressionType, f);
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - funkce pro parseAssingment
            //TODO - asi ji budu chtit predavat rovnou typ tokenu
            if((result = parseAssingnment()) != 1) {
                return result;
            }
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam uzaviraci zavorku - for(<Kdata_types>ID=expression; expression; <assignment>)
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            return parseBlock();

            break;
            
            
            
        //pravidlo 20 - cin >> ID <cin>;
        case KEYW_CIN:
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam prvni >
            if (token->typ != GREATER) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam druhe >
            if (token->typ != GREATER) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam identifikator
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani dalsich vstupu
            if ((result = parseCin()) != 1) {
                return result;
            }
            
            if ((result = getToken(token, f)) != 1) {
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
            
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam prvni <
            if (token->typ != LESS) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam druhe <
            if (token->typ != LESS) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if ((result = getToken(token, f)) != 1) {
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
            
            if ((result = getToken(token, f)) != 1) {
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
            //TODO - zpracovani vyrazu
            parseExpression(globalTable, localTable, , &expressionType, f);
            
            if((result = getToken(token, f)) != 1) {
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
            if((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani vyrazu
            parseExpression(globalTable, localTable, , &expressionType, f);
            
            if((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani bloku
            return parseBlock();
            
            break;
            
            
            
        //pravidlo 26 - <statement> -> do <block>while(expression);
        case KEYW_DO:
            
            //TODO - zpracovani bloku
            if ((result = parseBlock()) != 1) {
                return result;
            }
            
            if((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani vyrazu
            parseExpression(globalTable, localTable, , &expressionType, f);
            
            if((result = getToken(token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return SYNTAX_ERR;
            }
            freeTokenMem(token);
            
            if((result = getToken(token, f)) != 1) {
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
            return parseAssignment();
            break;
        default:
            return SYNTAX_ERR;
    }
}


int parseDeclaration(tTabSym *localTable) {
    
}