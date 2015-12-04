#include "parser.h"


//ve funkcich muze dojit k ruznym typum chyb
//chyba v lexikalni analyze(kdy ma vratit 1), nemuzu
//zakodovat jako 1, protoze to u me znamena uspech
#define LEXICAL_ERR 0

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
            return ERR_INTERNAL;
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
 * vlozi do globalni tabulky predpis pro vnorene funkce
 * @return      funkce vraci 1, pokud probehla bez chyby, jinak 0
 */
int prepareGlobalTable() {
    tParamListPtr lengthParam, subsParam, concatParam, findParam, sortParam;
    tFuncInfo *lengthInfo, *subsInfo, *concatInfo, *findInfo, *sortInfo;
    
    //inicializace seznamu parametru
    initList(lengthParam); initList(subsParam); initList(concatParam);
    initList(findParam); initList(sortParam);
    
    //vlozeni predpisu pro funkci:  int length (string s)
    char *lengthID = "length";
    string *length;
    if (strConConstString(length, lengthID) == 1) {
        return 0;
    }
    if ((insertEl(lengthParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0) {
        return 0;
    }
    if ((lengthInfo = tabSymCreateFuncInfo(&lengthParam, TAB_SYM_VAR_NO_AUTO_INTEGER, NULL, NULL, NULL, true)) == NULL) {
        return 0;
    }
    if (tabSymInsertFunc(globalTable, length, lengthInfo) == 0)
        return 0;
    
    //-------------------------------------------------------------------
    //vlozeni predpisu pro funkci: string substr(string s, int i, int n)
    char *substrID = "substr";
    string *substr;
    if (strConConstString(substr, substrID) == 1) {
        return 0;
    }
    if ((insertEl(subsParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 || 
         (insertEl(subsParam, NULL, TAB_SYM_VAR_NO_AUTO_INTEGER)) == 0  ||
             (insertEl(subsParam, NULL, TAB_SYM_VAR_NO_AUTO_INTEGER)) == 0) {
        return 0;
    }
    if ((subsInfo = tabSymCreateFuncInfo(&subsParam, TAB_SYM_VAR_NO_AUTO_STRING, NULL, NULL, NULL, true)) == NULL) {
        return 0;
    }
    if (tabSymInsertFunc(globalTable, substr, subsInfo) == 0)
        return 0;
    
    //-------------------------------------------------------------------
    //vlozeni predpisu pro funkci: string concat(string s1, string s2)
    char *concatID = "concat";
    string *concat;
    if (strConConstString(concat, concatID) == 1) {
        return 0;
    }
    if ((insertEl(concatParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 || 
         (insertEl(concatParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 ) {
        return 0;
    }
    if ((concatInfo = tabSymCreateFuncInfo(&concatParam, TAB_SYM_VAR_NO_AUTO_STRING, NULL, NULL, NULL, true)) == NULL) {
        return 0;
    }
    if (tabSymInsertFunc(globalTable, concat, concatInfo) == 0)
        return 0;
    
    //-------------------------------------------------------------------
    //vlozeni predpisu pro funkci: string find(string s, string search)
    char *findID = "find";
    string *find;
    if (strConConstString(find, findID) == 1) {
        return 0;
    }
    if ((insertEl(findParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 || 
         (insertEl(findParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 ) {
        return 0;
    }
    if ((findInfo = tabSymCreateFuncInfo(&findParam, TAB_SYM_VAR_NO_AUTO_STRING, NULL, NULL, NULL, true)) == NULL) {
        return 0;
    }
    if (tabSymInsertFunc(globalTable, find, findInfo) == 0)
        return 0;
    
     //-------------------------------------------------------------------
    //vlozeni predpisu pro funkci: string sort(string s)
    char *sortID = "sort";
    string *sort;
    if (strConConstString(sort, sortID) == 1) {
        return 0;
    }
    if ((insertEl(sortParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0) {
        return 0;
    }
    if ((sortInfo = tabSymCreateFuncInfo(&sortParam, TAB_SYM_VAR_NO_AUTO_STRING, NULL, NULL, NULL, true)) == NULL) {
        return 0;
    }
    if (tabSymInsertFunc(globalTable, sort, sortInfo) == 0)
        return 0;
    //cela funkce probehla uspesne
    return 1;
}

/**
 * funkce vytvori na halde misto pro novy string a skopiruje do nej hodnotu
 * predaneho stringu
 * @param identificator         -   string ke zkopirovani
 * @return      funkce vraci ukazatel na novy string, pokud probehlo vse v poradku, jinak NULL
 */
string* copyIdName(string *identificator) {
    string *idName = malloc(sizeof(string));
    if (idName == NULL) return 0;

    if (strInit(idName) == 1) {
        free(idName);
        return NULL;
    }
    if (strCopyString(idName, identificator) == 1) {
        strFree(idName);
        free(idName);
        return NULL;
    }
    return idName;
}

/**
 * funkce dealokuje pamet alokovanou pro vytvoreny string
 * @param identificator         -   string, jez je uvolnovan
 */
void freeIdName(string *identificator) {
    if (identificator != NULL) {
        strFree(identificator);
        free(identificator);
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

    if(prepareGlobalTable() == 0) {
        tabSymFree(globalTable);
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
            case ERR_SYNTAX:
                //TODO
            case ERR_SEM_DEF:
                //TODO
            case ERR_SEM_COM:
                //TODO
            case ERR_INTERNAL:
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
    
    //dalsi token je fID - <function> -> <Kdata_types> fID
    if (token->typ == TYPE_FUNCTION_IDENTIFICATOR) {
         
        //uchovavam nazev identifikatoru
        string *idName;
        if((idName = copyIdName(&(token->value.stringVal))) == NULL) {
            freeTokenMem(token);
            return ERR_INTERNAL;
        }
        
        //uvolnim token
        freeTokenMem(token);
        
        //funkce jeste nebyla deklarovana
        if((funcID_info = tabSymSearch(globalTable, idName)) == NULL) {
            //zkontroluji, zda je ID pouzitelne
            if((strcmp(idName->str, "length") == 0) || (strcmp(idName->str, "substr") == 0) ||
                    (strcmp(idName->str, "concat") == 0) || (strcmp(idName->str, "find") == 0) ||
                    (strcmp(idName->str, "sort") == 0)) {
                //uvolneni naalokovanych struktur
                freeIdName(idName);         
                return ERR_SEM_DEF; //pokus o redefinici funkce
            }
        }
        //funkce uz byla deklarovana
        else {
            //zkontroluji, zda je dany identifikator identifikatorem funkce
            if (funcID_info->type != TAB_SYM_FUNCTION) {
                freeIdName(idName);
                return ERR_INTERNAL;
            }
            //v tuto chvili uz muzu zkontrolovat, zda sedi navratovy typ funkce
            if(funcID_info->info.func->retType != (tTabSymVarNoAutoDataType)returnType) {
                freeIdName(idName);
                return ERR_SEM_COM; //nesouhlasi navratovy typ
            }
        }
        
        //dalsi token by mel byt '('
        if ((result = getToken(&token, f) != 1)) {
            freeIdName(idName);
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
                freeIdName(idName);
                //navratim chybovy kod
                return result;
            }

            //<function> -> <Kdata_types> fID(<arguments>)<body>
            //jsme u <body> -> bud ';'(deklarace), nebo '{' (definice)
            if ((result = getToken(&token, f)) != 1) {
                freeIdName(idName);
                return LEXICAL_ERR;
            }
            
            //jde pouze o deklaraci funkce
            if(token->typ == SEMICOLON) {
                //uvolnim token
                freeTokenMem(token);
                
                //vytvoreni informaci o funkci pro globalni tabulku symbolu
                if((funcInfo = tabSymCreateFuncInfo(&paramList, (tTabSymVarNoAutoDataType)returnType, 
                        localTabSym, NULL, NULL, false)) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                if((result = tabSymInsertFunc(globalTable, idName, funcInfo)) == 0) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                //nyni uz muzu uvolnit string, ktery nese informace o id funkce
                //tento identifikator funkce uz je ulozen v globalni tabulce symbolu
                freeIdName(idName);
                
                //muzeme zpracovavat  dalsi funkci 
                return parseFunction();
            }
            
            //jedna se o definici funkce
            if(token->typ == BRACES_OPENING) {
                 freeTokenMem(token);
                //zkontroluji, zda se uz dana funkce nebyla definovana
                if (funcID_info->info.func->defined == true) {
                    freeIdName(idName);
                    return ERR_SEM_DEF;
                }
                
                //nyni uz je funkce definovana
                defined = true;
                
                //vytvorim prazdny seznam tabulek bloku
                //TODO - nesmim zapomenout na uvolneni pameti
                tTabSymList *blockList;
                if ((blockList = tabSymListCreate()) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                } 
                
                //vytvoreni instrukcni pasky
                tInsTape *instructionTape;
                if ((instructionTape = insTapeCreate()) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                //pripravu jsem dokoncil, muzu provadet telo funkce
                if ((result = parseStatementList(localTabSym, blockList, NULL, instructionTape)) != 1) {
                    freeIdName(idName);
                    return result;
                }
                
                //telo funkce probehlo v poradku, doplnim informace do globalni tabulky symbolu
                if ((funcInfo = tabSymCreateFuncInfo(&paramList, (tTabSymVarNoAutoDataType)returnType,
                        localTabSym, blockList, instructionTape, defined)) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                if ((result = tabSymInsertFunc(globalTable, idName, funcInfo)) == 0) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                //muzu zpracovavat dalsi funkci
                freeIdName(idName);
                return parseFunction();
                
            }
            
            //token neni ; ani {
            freeTokenMem(token);
            freeIdName(idName);
            return ERR_SYNTAX;

        }
        //token neni oteviraci zavorka
        else {
            freeIdName(idName);
            freeTokenMem(token);
            return ERR_SYNTAX;
        }
    }
    //dalsi token neni identifikator
    else {
        freeTokenMem(token);
        return ERR_SYNTAX;
    }
    //neco je spatne
    return ERR_INTERNAL;
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
            if ((*variableType = tokenTypeToVarType(tokenType)) == 99) {
                return ERR_INTERNAL;
            }
            return 1;
        //jina hodnota - chyba
        default:
            return ERR_SYNTAX;
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
                return ERR_SEM_COM;
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
    
    //pozadam o novy token
    if((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    //token neni ID
    if(token->typ != TYPE_IDENTIFICATOR) {
        freeTokenMem(token);
        return ERR_SYNTAX;
    }
    
    //token je identifikator
    string *idName;
    
    if((idName = copyIdName(&(token->value.stringVal))) == NULL) {
        freeTokenMem(token);
        return ERR_INTERNAL;
    }
    freeTokenMem(token);
    
    //vkladam prvek do seznamu parametru a lokalni tabulky symbolu
    if(data == NULL) {
        //kontroluji, zda uz neni promenna definovana a vkladam do lokalni tabulky symbolu
        tTabSymElemData *localTableInfo = tabSymSearch(localTable, idName);
        tTabSymElemData *globalTableInfo = tabSymSearch(globalTable, idName);
        //promenna uz byla definovana
        if(localTableInfo != NULL || globalTableInfo != NULL) {
            freeIdName(idName);
            return ERR_SEM_DEF;
        }
         //vkladam do seznamu parametru
        if (insertEl(paramList, idName, paramType) == 0) {
            freeIdName(idName);
            return ERR_INTERNAL;
        }
        //podarilo se mi vlozit parametr do seznamu parametru
        tVariableInfo* varInfo = tabSymCreateVariableInfo((tTabSymVarDataType)paramType);
        if (varInfo == NULL) {
            freeIdName(idName);
            return ERR_INTERNAL;
        }
        //vlozim promennou do lokalni tabulky symbolu
        if (tabSymInsertVar(localTable, idName, varInfo) == 0) {
            freeIdName(idName);
            return ERR_INTERNAL;
        }
    }
    //porovnavam parametry
    else {
        if (((tTabSymVarNoAutoDataType)paramType != (*data->info.func->params)->act->dataType) ||
                (strcmp(idName->str, (*data->info.func->params)->act->idName->str) != 0)) {
            freeIdName(idName);
            return ERR_SEM_COM;
        }
    }
    
    freeIdName(idName);
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
            return ERR_SEM_COM;
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
        return ERR_SYNTAX;
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
            if ((dataType = tokenTypeToVarType(token->typ)) == 99) {
                freeTokenMem(token);
                return ERR_INTERNAL;
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
        //ukonceni bloku funkce, nebo normalniho bloku
        case BRACES_CLOSING:
            //TODO - GENEROVANI INSTRUKCE PRO ODSTRANENI BLOCK FRAME
            freeTokenMem(token);
            //instrukci pro ukonceni bloku vkladam jenom kdyz jsem v zanorenem bloku
            if (parent != NULL) {
                if (insTapeInsertLast(instructionTape, I_DBF, NULL, NULL, NULL) == 0) {
                    return ERR_INTERNAL;
                }
            }
            return 1;
            break;
        
        //pravidlo 18 - <st_list> -> {<st_list>}<st_list>
        //TODO - GENEROVANI INSTRUKCE
        case BRACES_OPENING:
            freeTokenMem(token);
            
            // nova lokalni tabulka
            tTabSym *blockLocalTable;
            // polozka v seznamu tabulek pro bloky
            tTabSymListElemPtr newElement;
            
            //vytvoreni nove lokalni tabulky symbolu
            if ((blockLocalTable = tabSymCreate(TAB_SYM_LOC)) == NULL){
                return ERR_INTERNAL;
            }
            
            //vlozeni nove lokalni tabulky symbolu do listu bloku
            if((newElement = tabSymListInsertTable(blockList, blockLocalTable, parent)) == NULL) {
                return ERR_INTERNAL;
            }
            
            //TODO - GENEROVANI INSTRUKCE
            if(insTapeInsertLast(instructionTape, I_CBF, (void*) blockLocalTable, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //rodicem se stane novy element, budeme pracovat s novou lokalni tabulkou
            //a novym listem, instrukcni paska je stejna
            if ((result = parseStatementList(blockLocalTable, newElement->childList, newElement, instructionTape)) != 1) {
                return result;
            }
            
            //TODO - nasledujici blok je nejspis spatne
            // BRACES_CLOSING uz jsem totiz musel precist, abych mohl pokracovat az sem
            /*
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //dalsi token by mel byt uzaviraci slozena zavorka
            if(token->typ != BRACES_CLOSING) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
             */
            
            //volam rekurzivne parseStatementList po vynoreni
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
            return ERR_SYNTAX;
    }
    //nikdy bych se sem nemel dostat
    return ERR_INTERNAL;
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
    string *key;
    tTabSymElemData *idUsable;
    string *idName;
    
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
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            //TODO - volat parser pro zpracovani vyrazu
            if((result = parseExpression( blockListElem ,localTable, instructionTape , &expressionType, f)) != ERR_OK) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            //ocekavam uzaviraci zavorku
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            //TODO - doplnit parseBlock
            if((result = parseBlock(localTable, blockList, blockListElem, instructionTape)) != 1) {
                return result;
            }
            
            //TODO - zpracovani else vetve
            if((result = getToken(&token, f)) != 1) {
                return ERR_SYNTAX;
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
                    return 1;
                    break;
                    
                //pravidlo 31 <else> -> else <block>
                case KEYW_ELSE:
                    //TODO - co poslu bloku
                    freeTokenMem(token);
                    return parseBlock(localTable, blockList, blockListElem, instructionTape);
                    break;
                    
                default:
                    return ERR_SYNTAX;
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
                return ERR_SYNTAX;
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
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //dalsi token by mel byt =
            
            if (token->typ != EQUAL) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            //TODO - volat parser  pro zpracovani vyrazu
            //TODO - zpracovat vystup parseru
            if ((result = parseExpression( blockListElem , localTable, instructionTape, &expressionType, f)) != ERR_OK) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            //TODO - zpracovat dalsi cast vyrazu
            //TODO - zpracovat vystup parseru
            if ((result = parseExpression( blockListElem , localTable, instructionTape, &expressionType, f)) != ERR_OK) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return ERR_SYNTAX;
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
                return ERR_SYNTAX;
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
                return ERR_SYNTAX;
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
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam druhe >
            if (token->typ != GREATER) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam identifikator
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            
            //zkopiruju si nazev identifikatoru
            if ((idName = copyIdName(&(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(token);
            
            //semanticka kontrola, zda je ID pouzitelne 
            if ((idUsable = tabSymListSearch(blockListElem, localTable, idName)) == NULL) {
                freeIdName(idName);
                return ERR_SEM_DEF; //promenna nedefinovana
            }
            
            
            //vyhledame klic v existujici tabulce symbolu
            if((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                freeIdName(idName);
                return ERR_INTERNAL;
            }
            
            freeIdName(idName);
            
            //TODO - VLOZENI INSTRUKCE
            if ((result = insTapeInsertLast(instructionTape, I_CIN, NULL, NULL, (void*) key)) == 0) {
                return ERR_INTERNAL;
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
                return ERR_SYNTAX;
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
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //ocekavam druhe <
            if (token->typ != LESS) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //isTerm
            if((result = isTerm(token->typ)) == 0){
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            
            //udrzuje informace o konstante
            tConstantInfo *constInfo;
            //udzuje hodnoty, kterych muze konstanta nabyvat
            unionValue uval;
            // vygenerovany
            string *tmp;
            
            switch (token->typ) {
                case TYPE_BOOL:
                case TYPE_INTEGER:
                case TYPE_DOUBLE:
                case TYPE_STRING:

                    //funkci createConstantInfo musim predat union typu unionValue
                    //tudiz si musim tento union vyrobit, tento string neuvolnim
                   //TODO
                    uval.boolVal = token->value.boolVal;
                    uval.doubleVal = token->value.doubleVal;
                    uval.intVal = token->value.intVal;
                    uval.stringVal = copyIdName(&(token->value.stringVal));

                    freeTokenMem(token);

                    //vytvorim informace o konstante
                    if ((constInfo = tabSymCreateConstantInfo(token->typ, uval)) == NULL) {
                        return ERR_INTERNAL;
                    }
                    //vytvorim si novy identifikator, ktery priradim konstante
                    if ((tmp = tabSymListCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                        return ERR_INTERNAL;
                    }
                    //vlozim konstantu do tabulky symbolu
                    if (tabSymInsertConst(localTable, tmp, constInfo) == 0) {
                        return ERR_INTERNAL;
                    }

                    //vzdy bych mel dany klic najit, jelikoz jsem ho prave vlozil
                    key = tabSymListGetPointerToKey(blockListElem, localTable, tmp);

                    //vlozim instrukci do instrukcni pasky
                    if (insTapeInsertLast(instructionTape, I_COUT, (void *) key, NULL, NULL) == 0) {
                        return ERR_INTERNAL;
                    }
                    break;

                //vypisujeme identifikator
                case TYPE_IDENTIFICATOR:

                    //zkusim dany identifikator najit
                    if ((idUsable = tabSymListSearch(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                        freeTokenMem(token);
                        return ERR_SEM_DEF;
                    }

                    //identifikator se nasel, chci ziskat klic do tabulky symbolu
                    if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                        freeTokenMem(token);
                        return ERR_INTERNAL;
                    }

                    freeTokenMem(token);

                    //vlozim instrukci do instrukcni pasky
                    if (insTapeInsertLast(instructionTape, I_COUT, (void *) key, NULL, NULL) == 0) {
                        return ERR_INTERNAL;
                    }
                    break;
                    
                //nikdy bych se sem nemel dostat
                default:
                    freeTokenMem(token);
                    return ERR_INTERNAL;
            }
            
            
            //TODO - zpracovani dalsich vstupu
            if((result = parseCout(instructionTape, blockListElem, localTable)) != 1) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            //dalsi token by mel byt ;
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            //pokud jsem se dostal az sem, tak syntakticka analyza probehla v poradku
            return 1;
            break;
            
            
            
        //pravidlo 22 - <statement> -> return expression;
        case KEYW_RETURN:
            freeTokenMem(tokenOrig);
            //TODO - zpracovani vyrazu
            if ((result = parseExpression( blockListElem, localTable, instructionTape, &expressionType, f)) != ERR_OK) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return ERR_SYNTAX;
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
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani vyrazu
            if ((result = parseExpression( blockListElem ,localTable, instructionTape , &expressionType, f)) != ERR_OK) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return ERR_SYNTAX;
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
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            //TODO - zpracovani vyrazu
            if ((result = parseExpression( blockListElem, localTable, instructionTape , &expressionType, f)) != ERR_OK) {
                return result;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(token);
                return ERR_SYNTAX;
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
            return ERR_SYNTAX;
    }
    //nikdy bych se sem nemel dostat
    return ERR_INTERNAL;
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
                return ERR_SYNTAX;
            }
            
            string *idName;
            if ((idName = copyIdName(&(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(token);
            
            //kontroluji, zda uz promenna byla definovana
            varIdentifikator = tabSymSearch(localTable, idName);
            funcIdentifikator = tabSymSearch(globalTable, idName);
            if (varIdentifikator != NULL || funcIdentifikator != NULL) {
                freeIdName(idName);
                return ERR_SEM_DEF;
            }
            
            //vytvoreni informaci o promenne
            if ((variableInfo = tabSymCreateVariableInfo(dataType)) == NULL) {
                freeIdName(idName);
                return ERR_INTERNAL;
            }
            
            //vlozeni promenne do lokalni tabulky symbolu
            if ((tabSymInsertVar(localTable, idName, variableInfo)) == 0) {
                freeIdName(idName);
                return ERR_INTERNAL;
            }
            
            //cast <decInit>
            
            if((result = getToken(&token, f)) != 1) {
                freeIdName(idName);
                return LEXICAL_ERR;
            }
            
            //pouze definice promenne
            if(token->typ == SEMICOLON) {
                freeIdName(idName);
                freeTokenMem(token);
                return 1;
            }
            
            //inicializace
            if(token->typ == EQUAL) {
                freeTokenMem(token);

                 if ((result = parseExpression( blockListElem, localTable, instructionTape, &expressionType, f)) != ERR_OK) {
                     freeIdName(idName);
                     return result;
                 }
                 
                string *lastGeneratedTMP;
                string *key;
                //vyhledame klic v existujici tabulce symbolu
                if((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                freeIdName(idName);
                
                //chci ziskat nazev posledn idocasne vygenerovane promenne
                if((lastGeneratedTMP = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                    return ERR_INTERNAL;
                }
                
                //TODO GENEROVANI INSTRUKCE
                if ((result = insTapeInsertLast(instructionTape, I_ASSIGN, (void*) lastGeneratedTMP, NULL, (void*) key)) == 0) {
                    return ERR_INTERNAL;
                }
                
                 if((result = getToken(&token, f)) != 1) {
                     return LEXICAL_ERR;
                 }
                 
                 //dalsi token by mel byt ;
                 if(token->typ != SEMICOLON){
                     freeTokenMem(token);
                     return ERR_SYNTAX;
                 }
                 freeTokenMem(token);
                 //pokud jsem se dostal az sem, vse je v poradku
                 return 1;
            }
            
            freeTokenMem(token);
            //chybna syntaxe
            return ERR_SYNTAX;
            
            break;
            
            
        // pravidlo 37 - <declaration> -> auto ID = expression;
        case TAB_SYM_VAR_AUTO:
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            
            string *idNameAuto;
            if ((idNameAuto = copyIdName(&(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(token);
            
            //kontroluji, zda uz promenna byla definovana
            varIdentifikator = tabSymSearch(localTable, idNameAuto);
            funcIdentifikator = tabSymSearch(globalTable, idNameAuto);
            if (varIdentifikator != NULL || funcIdentifikator != NULL) {
                freeIdName(idNameAuto);
                return ERR_SEM_DEF;
            }
            
            //nactu dalsi token, ktery by mel by =
            if((result = getToken(&token, f)) != 1) {
                freeIdName(idNameAuto);
                return LEXICAL_ERR;
            }
            
            if(token->typ != EQUAL) {
                freeIdName(idNameAuto);
                freeTokenMem(token);
                return ERR_SEM_AUTO;
            }
            
            freeTokenMem(token);
            
            //TODO - zpracovani vyrazu
            if((result = parseExpression( blockListElem ,localTable, instructionTape, &expressionType, f)) != ERR_OK) {
                freeIdName(idNameAuto);
                return result;
            }
            
            //vytvoreni informaci o promenne
            //TODO - vkladam jeji datovy typ az na zaklade datoveho typu
                    // po vyhodnoceni vyrazu
            if ((variableInfo = tabSymCreateVariableInfo((tTabSymVarDataType)expressionType)) == NULL) {
                freeIdName(idNameAuto);
                return ERR_INTERNAL;
            }
            
            //vlozeni promenne do lokalni tabulky symbolu
            if ((tabSymInsertVar(localTable, idName, variableInfo)) == 0) {
                freeIdName(idNameAuto);
                return ERR_INTERNAL;
            }
            
            //TODO - GENEROVANI INSTRUKCE
            string *lastGeneratedTMP;
            string *key;
            //vyhledame klic v existujici tabulce symbolu
            if((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                freeIdName(idNameAuto);
                return ERR_INTERNAL;
            }

            freeIdName(idNameAuto);

            //chci ziskat nazev posledni docasne vygenerovane promenne
            if((lastGeneratedTMP = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                return ERR_INTERNAL;
            }

            //TODO GENEROVANI INSTRUKCE
            if ((result = insTapeInsertLast(instructionTape, I_ASSIGN, (void*) lastGeneratedTMP, NULL, (void*) key)) == 0) {
                return ERR_INTERNAL;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if(token->typ != SEMICOLON){
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);
            //pokud jsem se dostal az sem, tak je vse v poradku
            return 1;
            
            break;
        default:
            return ERR_SYNTAX;
    }
    //sem bych se nikdy nemel dostat
    return ERR_INTERNAL;
}



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
        //vytvorila novy rozsah platnosti pro blok?
        case BRACES_OPENING:
            ungetToken(&token);
            
            if((result = parseStatementList(localTable, blockList, blockListElem, instructionTape)) != 1) {
                return result;
            }
            
            //nasledujici prikazy asi nechci, protoze parseStatement list zpracuje uzaviraci zavorku
            /*
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            if (token->typ != BRACES_CLOSING) {
                freeTokenMem(token);
                return ERR_SYNTAX;
            }
            freeTokenMem(token);*/
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
            return ERR_SYNTAX;
    }
    //nikdy bych se sem nemel dostat
    return ERR_INTERNAL;
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
    string *idName;
    string *key;
    tTabSymElemData *idUsable;
    tTabSymVarDataType idType;
    
    switch(tokenOrig->typ) {
        case TYPE_IDENTIFICATOR:
            //TODO
            if ((idName = copyIdName(&(tokenOrig->value.stringVal))) == NULL ) {
                freeTokenMem(tokenOrig);
                return ERR_INTERNAL;
            }
            freeTokenMem(tokenOrig);
            
            //semanticka kontrala, zda je promenna definovana v danem rozsahu platnosti
            if ((idUsable = tabSymListSearch(blockListElem, localTable, idName)) == NULL) {
                freeIdName(idName);
                return ERR_SEM_DEF;
            }
            
            //zkontroluji, zda jsem dostal info o promenne
            //pokud ne, hlasim chybu (ale nikdy by k ni dojit nemelo)
            if(idUsable->type != TAB_SYM_VARIABLE) {
                freeIdName(idName);
                return ERR_SEM_DEF;
            }
            
            //ziskam datovy typ promenne, abych mohl provadet semantickou kontrolu
            idType = idUsable->info.var->dataType;
            
            if((result = getToken(&token, f))) {
                return LEXICAL_ERR;
                freeIdName(idName);
            }
            
            //TODO semanticky akce a generovani vnitrniho kodu
            if(token->typ == INCREMENTATION) {
                //u inkrementace nesmi byt operator string
                if (idType == TAB_SYM_VAR_STRING) {
                    freeTokenMem(token);
                    freeIdName(idName);
                    return ERR_SEM_COM;
                }
                
                //ziskam odkaz do tabulky symbolu
                if ((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeTokenMem(token);
                    freeIdName(idName);
                }
                
                //id uz muzu uvolnit
                freeIdName(idName);
                
                if(insTapeInsertLast(instructionTape, I_INC, (void *) key, NULL, (void *) key) == 0) {
                    freeTokenMem(token);
                    return ERR_INTERNAL;
                }
               
                freeTokenMem(token);
                return 1;
            }
            else if(token->typ == DECREMENTATION) {
                 //u dekrementace nesmi byt operator string, ani bool
                if (idType == TAB_SYM_VAR_STRING || idType == TAB_SYM_VAR_BOOLEAN) {
                    freeTokenMem(token);
                    freeIdName(idName);
                    return ERR_SEM_COM;
                }
                
                //ziskam odkaz do tabulky symbolu
                if ((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeTokenMem(token);
                    freeIdName(idName);
                }
                
                //id uz muzu uvolnit
                freeIdName(idName);
                
                if(insTapeInsertLast(instructionTape, I_DEC, (void *)key, NULL, (void *)key) == 0) {
                    freeTokenMem(token);
                    return ERR_INTERNAL;
                }
   
                freeTokenMem(token);
                return 1;
            }
            else {
                freeTokenMem(token);
                return ERR_SYNTAX;
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
                return ERR_SYNTAX;
            }
            
            if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
            }
            
            //semanticka kontrala, zda je promenna definovana v danem rozsahu platnosti
            if ((idUsable = tabSymListSearch(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
                return ERR_SEM_DEF;
            }
            
            //zkontroluji, zda jsem dostal info o promenne
            //pokud ne, hlasim chybu (ale nikdy by k ni dojit nemelo)
            if(idUsable->type != TAB_SYM_VARIABLE) {
                freeTokenMem(token);
                return ERR_SEM_DEF;
            }
            
            idType = idUsable->info.var->dataType;
            
            if (idType == TAB_SYM_VAR_STRING) {
                freeTokenMem(token);
                return ERR_SEM_COM;
            }
                
            //ziskam odkaz do tabulky symbolu
            if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
            }

            if(insTapeInsertLast(instructionTape, I_INC, (void *) key, NULL, (void *) key) == 0) {
                freeTokenMem(token);
                return ERR_INTERNAL;
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
                return ERR_SYNTAX;
            }
            
            if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
            }
            
            //semanticka kontrala, zda je promenna definovana v danem rozsahu platnosti
            if ((idUsable = tabSymListSearch(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
                return ERR_SEM_DEF;
            }
            
            //zkontroluji, zda jsem dostal info o promenne
            //pokud ne, hlasim chybu (ale nikdy by k ni dojit nemelo)
            if(idUsable->type != TAB_SYM_VARIABLE) {
                freeTokenMem(token);
                return ERR_SEM_DEF;
            }
            
            idType = idUsable->info.var->dataType;
            
            if (idType == TAB_SYM_VAR_STRING || idType == TAB_SYM_VAR_BOOLEAN) {
                freeTokenMem(token);
                return ERR_SEM_COM;
            }
                
            //ziskam odkaz do tabulky symbolu
            if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(token);
            }

            if(insTapeInsertLast(instructionTape, I_DEC, (void *) key, NULL, (void *) key) == 0) {
                freeTokenMem(token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(token);
            //pokud jsem se dostal az sem, vse je v poradku
            return 1;
            break;
            
        default:
            freeTokenMem(tokenOrig);
            return ERR_SYNTAX;
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
    tTabSymElemData *idUsable;
    
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
                
                if((idName = copyIdName(&(token->value.stringVal))) == NULL) {
                    freeTokenMem(token);
                    return ERR_INTERNAL;
                }
                freeTokenMem(token);
                
                //semanticka kontrola
                if((idUsable = tabSymListSearch(blockListElem, localTable, idName)) == NULL) {
                    freeIdName(idName);
                    return ERR_SEM_DEF;
                }
                
                string *key;
                if((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeIdName(idName);
                }
                
                freeIdName(idName);
                
                //TODO - vlozeni instrukce 
                if ((result = insTapeInsertLast(instructionTape, I_CIN, NULL, NULL, (void *) key)) == 0) {
                    return ERR_INTERNAL;
                }
                //muzeme funkci provadet znovu
                return parseCin(instructionTape, localTable, blockListElem);
            }
            
            freeTokenMem(token);
            return ERR_SYNTAX;
        }
        
        freeTokenMem(token);
        return ERR_SYNTAX;
    }
    
    freeTokenMem(token);
    return ERR_SYNTAX;
}


//!!!!!!!!!!!   TO CHECK  !!!!!!!!!!!!

/**
 * zpracovava nasledujici pravidla
 *34. <cout> -> epsilon
 *35. <cout> -> << <term> <cout>
 * @param instructionTape       -   ukazatel na instrukcni pasku
 * @param blockListElem         -   aktualni element z tabulky symbolu pro bloky
 * @param localTable            -   ukazatel a lokalni tabulku
 * @return      pokud probehlo vse v poradku, tak 1
 */
int parseCout(tInsTape *instructionTape, tTabSymListElemPtr blockListElem, tTabSym *localTable) {
    tToken token;
    int result;
    TokenTypes ttype;
    //udrzuje informace o konstante
    tConstantInfo *constInfo;
    //udzuje hodnoty, kterych muze konstanta nabyvat
    unionValue uval;
    //tmp je string pro novy identifikator, key je string, ktery ukazuje na prvek v
    //tabulce symbolu
    string *tmp, *key;
    //informace o promenne
    tTabSymElemData *idData;
    
    if ((result = getToken(&token, f)) != 1) {
        return LEXICAL_ERR;
    }
    
    //pravidlo 34 - <cout> -> epsilon
    if(token->typ == SEMICOLON) {
        //volajici funkce s timto tokenem pocita
        ungetToken(&token);
        return 1;
    }
    
    //pravidlo 35 - <cout> -> << <term> <cout>
    if(token->typ == LESS) {
        freeTokenMem(token);
        
        if((result = getToken(&token, f)) != 1) {
            return LEXICAL_ERR;
        }
        
        if(token->typ == LESS) {
            freeTokenMem(token);
            
            //dalsi token by mel byt term
            if((result = getToken(&token, f)) != 1) {
                return LEXICAL_ERR;
            }
            
            ttype = token->typ;
            //freeTokenMem(token);
            
            //jedna se o term: hodnota int, string, bool, double, nebo ID
            if((result = isTerm(ttype)) == 1) {
                
                switch(ttype){
                    case TYPE_BOOL:
                    case TYPE_INTEGER:
                    case TYPE_DOUBLE:
                    case TYPE_STRING:

                        //funkci createConstantInfo musim predat union typu unionValue
                        //tudiz si musim tento union vyrobit, tento string neuvolnim
                       // s = copyIdName(&(token->value.stringVal));
                        uval.boolVal = token->value.boolVal;
                        uval.doubleVal = token->value.doubleVal;
                        uval.intVal = token->value.intVal;
                        uval.stringVal = copyIdName(&(token->value.stringVal));
                        
                        freeTokenMem(token);
                        
                        //vytvorim informace o konstante
                        if ((constInfo = tabSymCreateConstantInfo(ttype, uval)) == NULL) {
                            return ERR_INTERNAL;
                        }
                        //vytvorim si novy identifikator, ktery priradim konstante
                        if ((tmp = tabSymListCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                            return ERR_INTERNAL;
                        }
                        //vlozim konstantu do tabulky symbolu
                        if (tabSymInsertConst(localTable, tmp, constInfo) == 0) {
                            return ERR_INTERNAL;
                        }
                        
                        //vzdy bych mel dany klic najit, jelikoz jsem ho prave vlozil
                        key = tabSymListGetPointerToKey(blockListElem, localTable, tmp);
                        
                        //vlozim instrukci do instrukcni pasky
                        if (insTapeInsertLast(instructionTape, I_COUT, (void *) key, NULL, NULL) == 0) {
                            return ERR_INTERNAL;
                        }
                        
                        //muzu opet volat tuto funkci
                        return parseCout(instructionTape, blockListElem, localTable);
                    
                    //vypisujeme identifikator
                    case TYPE_IDENTIFICATOR:
                        
                        //zkusim dany identifikator najit
                        if ((idData = tabSymListSearch(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                            freeTokenMem(token);
                            return ERR_SEM_DEF;
                        }
                        
                        //identifikator se nasel, chci ziskat klic do tabulky symbolu
                        if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                            freeTokenMem(token);
                            return ERR_INTERNAL;
                        }
                        
                        freeTokenMem(token);
                        
                        //vlozim instrukci do instrukcni pasky
                        if (insTapeInsertLast(instructionTape, I_COUT, (void *) key, NULL, NULL) == 0) {
                            return ERR_INTERNAL;
                        }
                        
                        //muzu opet volat tuto funkci
                        return parseCout(instructionTape, blockListElem, localTable);
                        
                    //nikdy bych se sem nemel dostat
                    default:
                        freeTokenMem(token);
                        return ERR_INTERNAL;
                }
   
            }
            freeTokenMem(token);
            return ERR_SYNTAX;
        }
        
        freeTokenMem(token);
        return ERR_SYNTAX;
    }
    
    freeTokenMem(token);
    return ERR_SYNTAX;
}
