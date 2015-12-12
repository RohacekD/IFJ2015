#include "parser.h"
#include "interpret.h"


// vytvorim strukturu pro globalni tabulku symbolu
tTabSym *globalTable;
FILE *f;

string *retGlobal;

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
 * funkce naplni tabulku bloku a vraci jeji element
 * @param blockLocalTable[out]       -   nova lokalni tabulka pro bloky
 * @param blockList[in]              -   ukazatel seznam bloku
 * @param blockListElem[in]          -   ukazatel na aktualni rodicovsky element
 * @param localTable[in]             -   existujici lokalni tabulka funkce
 * @return          funkce vraci ukazatel na novy element v pripade uspechu, jinak NULL
 */
tTabSymListElemPtr prepareBlockList(tTabSym** blockLocalTable, tTabSymList* blockList, tTabSymListElemPtr blockListElem, tTabSym *localTable) {
    // polozka v seznamu tabulek symbolu pro bloky
    tTabSymListElemPtr newElement;
    
    //vytvoreni nove lokalni tabulky symbolu
    if ((*blockLocalTable = tabSymCreate(TAB_SYM_LOC)) == NULL){
        return NULL;
    }   
    
    //vlozeni nove lokalni tabulky symbolu do listu tabulek symbolu pro bloky
    if((newElement = tabSymListInsertTable(blockList, *blockLocalTable, blockListElem)) == NULL) {
        tabSymFree(*blockLocalTable);
        return NULL;
    }

    //pokud nema nove vytvoreny element rodice, bude jeho rodicem lokalni tabulka
    //symbolu dane funkce
    if(newElement->parentElement == NULL) {
        tTabSymList *newList;
        tTabSymListElemPtr localTableElem;
        if ((newList = tabSymListCreate()) == NULL) {
            return NULL;
        }
        if ((localTableElem = tabSymListInsertTable(newList, localTable, NULL)) == NULL) {
            tabSymListFree(newList);
            return NULL;
        }
        newElement->parentElement = localTableElem;
    }
    
    return newElement;
}

/**
 * vlozi do globalni tabulky predpis pro vnorene funkce
 * @return      funkce vraci 1, pokud probehla bez chyby, jinak 0
 */
int prepareGlobalTable() {
    tParamListPtr lengthParam = NULL, subsParam = NULL, concatParam = NULL, findParam = NULL, sortParam = NULL;
    tFuncInfo *lengthInfo, *subsInfo, *concatInfo, *findInfo, *sortInfo;
    
    //inicializace seznamu parametru
    if ((lengthParam = initList()) == NULL )
        return 0;
    if ((subsParam = initList()) == NULL)  {
        paramListFree(lengthParam);
        return 0;
    } 
    if ((concatParam =initList()) == NULL) {
        paramListFree(lengthParam); paramListFree(subsParam);
        return 0;
    }
    if ((findParam = initList()) == NULL) {
        paramListFree(lengthParam); paramListFree(subsParam);
        paramListFree(concatParam);
        return 0;
    }
    if ((sortParam = initList()) == NULL) {
        paramListFree(lengthParam); paramListFree(subsParam);
        paramListFree(concatParam); paramListFree(findParam);
        return 0;
    }
    
    //-----------------------------------------------------------------------------------------------------------------
    //vlozeni predpisu pro funkci:  int length (string s)
    char *lengthID = "length";
    string *length = NULL;
    if ((length = (string*)malloc(sizeof(string))) == NULL) {
        paramListFree(lengthParam); paramListFree(subsParam);
        paramListFree(concatParam); paramListFree(findParam); paramListFree(sortParam);
        return 0;
    }
        
    if (strInit(length) == 1) {
        paramListFree(lengthParam); paramListFree(subsParam);
        paramListFree(concatParam); paramListFree(findParam); paramListFree(sortParam);
        free(length);
        return 0;
    }
    if (strConConstString(length, lengthID) == 1) {
        paramListFree(lengthParam); paramListFree(subsParam);
        paramListFree(concatParam); paramListFree(findParam); paramListFree(sortParam);
        free(length);
        return 0;
    }
    if ((insertEl(lengthParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0) {
        paramListFree(lengthParam); paramListFree(subsParam);
        paramListFree(concatParam); paramListFree(findParam); paramListFree(sortParam);
        freeIdName(length);
        return 0;
    }
    if ((lengthInfo = tabSymCreateFuncInfo(lengthParam, TAB_SYM_VAR_NO_AUTO_INTEGER, NULL, NULL, NULL, true)) == NULL) {
        paramListFree(lengthParam); paramListFree(subsParam);
        paramListFree(concatParam); paramListFree(findParam); paramListFree(sortParam);
        freeIdName(length);
        return 0;
    }
    if (tabSymInsertFunc(globalTable, length, lengthInfo) == 0) {
        paramListFree(lengthParam); paramListFree(subsParam);
        paramListFree(concatParam); paramListFree(findParam); paramListFree(sortParam);
        freeIdName(length); free(lengthInfo);
        return 0;
    }
    freeIdName(length);
    //-------------------------------------------------------------------
    //vlozeni predpisu pro funkci: string substr(string s, int i, int n)
    char *substrID = "substr";
    string *substr = NULL;
    if ((substr = (string*)malloc(sizeof(string))) == NULL)  {
        paramListFree(subsParam);  paramListFree(sortParam);
        paramListFree(concatParam); paramListFree(findParam);
        return 0;
    }
    if (strInit(substr) == 1) {
        paramListFree(subsParam);  paramListFree(sortParam);
        paramListFree(concatParam); paramListFree(findParam);
        free(substr);
        return 0;
    }
    
    if (strConConstString(substr, substrID) == 1) {
        paramListFree(subsParam);  paramListFree(sortParam);
        paramListFree(concatParam); paramListFree(findParam);
        free(substr);
        return 0;
    }
    if ((insertEl(subsParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 || 
         (insertEl(subsParam, NULL, TAB_SYM_VAR_NO_AUTO_INTEGER)) == 0  ||
             (insertEl(subsParam, NULL, TAB_SYM_VAR_NO_AUTO_INTEGER)) == 0) {
        paramListFree(subsParam);  paramListFree(sortParam);
        paramListFree(concatParam); paramListFree(findParam);
        freeIdName(substr);
        return 0;
    }
    if ((subsInfo = tabSymCreateFuncInfo(subsParam, TAB_SYM_VAR_NO_AUTO_STRING, NULL, NULL, NULL, true)) == NULL) {
        paramListFree(subsParam);  paramListFree(sortParam);
        paramListFree(concatParam); paramListFree(findParam);
        freeIdName(substr);
        return 0;
    }
    if (tabSymInsertFunc(globalTable, substr, subsInfo) == 0) {
        paramListFree(subsParam);  paramListFree(sortParam);
        paramListFree(concatParam); paramListFree(findParam);
        freeIdName(substr); free(subsInfo);
        return 0;
    }
    freeIdName(substr);
    //-------------------------------------------------------------------
    //vlozeni predpisu pro funkci: string concat(string s1, string s2)
    char *concatID = "concat";
    string *concat;
    if ((concat = (string*)malloc(sizeof(string))) == NULL) {
        paramListFree(sortParam); paramListFree(concatParam); paramListFree(findParam);
        return 0;
    }
    if (strInit(concat) == 1) {
        paramListFree(sortParam); paramListFree(concatParam); paramListFree(findParam);
        free(concat);
        return 0;
    }
    
    if (strConConstString(concat, concatID) == 1) {
        paramListFree(sortParam); paramListFree(concatParam); paramListFree(findParam);
        return 0;
    }
    if ((insertEl(concatParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 || 
         (insertEl(concatParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 ) {
        paramListFree(sortParam); paramListFree(concatParam); paramListFree(findParam);
        freeIdName(concat);
        return 0;
    }
    if ((concatInfo = tabSymCreateFuncInfo(concatParam, TAB_SYM_VAR_NO_AUTO_STRING, NULL, NULL, NULL, true)) == NULL) {
        paramListFree(sortParam); paramListFree(concatParam); paramListFree(findParam);
        freeIdName(concat);
        return 0;
    }
    if (tabSymInsertFunc(globalTable, concat, concatInfo) == 0) {
        paramListFree(sortParam); paramListFree(concatParam); paramListFree(findParam);
        freeIdName(concat); free(concatInfo);
        return 0;
    }
    freeIdName(concat);
    //-------------------------------------------------------------------
    //vlozeni predpisu pro funkci: string find(string s, string search)
    char *findID = "find";
    string *find;
    if ((find = (string*)malloc(sizeof(string))) == NULL) {
        paramListFree(sortParam); paramListFree(findParam);
        return 0;
    }
    if (strInit(find) == 1) {
        paramListFree(sortParam); paramListFree(findParam);
        free(find);
        return 0;
    }
    
    if (strConConstString(find, findID) == 1) {
        paramListFree(sortParam); paramListFree(findParam);
        free(find);
        return 0;
    }
    if ((insertEl(findParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 || 
         (insertEl(findParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0 ) {
        paramListFree(sortParam); paramListFree(findParam);
        freeIdName(find);
        return 0;
    }
    if ((findInfo = tabSymCreateFuncInfo(findParam, TAB_SYM_VAR_NO_AUTO_INTEGER, NULL, NULL, NULL, true)) == NULL) {
        paramListFree(sortParam); paramListFree(findParam);
        freeIdName(find);
        return 0;
    }
    if (tabSymInsertFunc(globalTable, find, findInfo) == 0) {
        paramListFree(sortParam); paramListFree(findParam);
        freeIdName(find); free(findInfo);
        return 0;
    }
    freeIdName(find);
     //-------------------------------------------------------------------
    //vlozeni predpisu pro funkci: string sort(string s)
    char *sortID = "sort";
    string *sort;
    if ((sort = (string*)malloc(sizeof(string))) == NULL) {
        paramListFree(sortParam);
        return 0;
    }
    if (strInit(sort) == 1) {
        paramListFree(sortParam);
        free(sort);
        return 0;
    }
    
    if (strConConstString(sort, sortID) == 1) {
        paramListFree(sortParam);
        free(sort);
        return 0;
    }
    if ((insertEl(sortParam, NULL, TAB_SYM_VAR_NO_AUTO_STRING)) == 0) {
        paramListFree(sortParam);
        freeIdName(sort);
        return 0;
    }
    if ((sortInfo = tabSymCreateFuncInfo(sortParam, TAB_SYM_VAR_NO_AUTO_STRING, NULL, NULL, NULL, true)) == NULL) {
        paramListFree(sortParam);
        freeIdName(sort);
        return 0;
    }
    if (tabSymInsertFunc(globalTable, sort, sortInfo) == 0) {
        paramListFree(sortParam);
        freeIdName(sort); free(sortInfo);
        return 0;
    }
    freeIdName(sort);
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
    string *idName = (string*)malloc(sizeof(string));
    if (idName == NULL) return NULL;

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
    identificator = NULL;
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
    
    //pridam do globalni tabulky informace o vnorenych funkcich 
    if(prepareGlobalTable() == 0) {
        tabSymFree(globalTable);
        FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
    }
    
        
    if ((result = parseFunction()) != ERR_OK) {
        //uvolneni pameti - globalni tabulky symbolu
        tabSymFree(globalTable);
        //uvolnim frontu, pokud v ni zustaly nejake prvku
        freeTokenQueue();
        //uzavreni souboru
        fclose(f);
        //uvolneni globalni promenne
        freeIdName(retGlobal);
        //vraceni chyboveho kodu na zaklade chyby v parseru, nebo interpretu
        
        ERROR:
		if (errorFlag != 0) {
			result = ERR_LEX;
		}
        switch(result) {
            case ERR_LEX:
                FatalError(ERR_LEX, ERR_MESSAGES[ERR_LEX]);
            case ERR_SYNTAX:
                FatalError(ERR_SYNTAX, ERR_MESSAGES[ERR_SYNTAX]);
            case ERR_SEM_DEF:
                FatalError(ERR_SEM_DEF, ERR_MESSAGES[ERR_SEM_DEF]);
            case ERR_SEM_COM:
                FatalError(ERR_SEM_COM, ERR_MESSAGES[ERR_SEM_COM]);
            case ERR_SEM_AUTO:
                FatalError(ERR_SEM_AUTO, ERR_MESSAGES[ERR_SEM_AUTO]);
            case ERR_SEM_REST:
                FatalError(ERR_SEM_REST, ERR_MESSAGES[ERR_SEM_REST]);
            case ERR_RUNTIME_INPUT:
                FatalError(ERR_RUNTIME_INPUT, ERR_MESSAGES[ERR_RUNTIME_INPUT]);
            case ERR_RUNTIME_INIT_VAR:
                FatalError(ERR_RUNTIME_INIT_VAR, ERR_MESSAGES[ERR_RUNTIME_INIT_VAR]);
            case ERR_RUNTIME_ZERO_DIV:
                FatalError(ERR_RUNTIME_ZERO_DIV, ERR_MESSAGES[ERR_RUNTIME_ZERO_DIV]);
            case ERR_RUNTIME_REST:
                FatalError(ERR_RUNTIME_REST, ERR_MESSAGES[ERR_RUNTIME_REST]);
            case ERR_INTERNAL:
                FatalError(ERR_INTERNAL, ERR_MESSAGES[ERR_ALLOC]);
            default:
                break;
        }
    }
    //TODO - syntakticka a semanticka kontrola probehla v poradku
    else {
        //musim jeste zkontrolovat, zda byly vsechny funkce definovany
        BSTcheckFuncDef(&(globalTable->root));
        if (notDefined == 1) {
            tabSymFree(globalTable);
            fclose(f);
            FatalError(ERR_SEM_DEF, ERR_MESSAGES[ERR_SEM_DEF]);
        }
        
        tTabSymElemData *findMain; //promenna slouzi pro ulozeni informac pro funkci main
        char *mainId = "main"; //nazev funkce main
        string *mainString; //string, do ktereho si chci ulozit nazev funkce
        
        //ulozwni nazvu funkce do promenne mainString
        if (((mainString = malloc(sizeof(string))) == NULL) || (strInit(mainString) == 1) ||
            (strConConstString(mainString, mainId)) == 1) {
            tabSymFree(globalTable);
            fclose(f);
            FatalError(ERR_INTERNAL, ERR_MESSAGES[ERR_ALLOC]);
        }
        
        //vyhledani funkce main
	if ((findMain = tabSymSearch(globalTable, mainString)) == NULL) {
            tabSymFree(globalTable);
            //uvolneni stringu fce main
            freeIdName(mainString);
            fclose(f);
            FatalError(ERR_SEM_DEF, ERR_MESSAGES[ERR_SEM_DEF]);
        }

        //uvolneni stringu fce main
        freeIdName(mainString);
        
        //kontrola, jestli je funkce main definovana se spravnym seznamem parametru
        if (findMain->info.func->params->first != NULL || findMain->info.func->retType != TAB_SYM_VAR_NO_AUTO_INTEGER) {
            tabSymFree(globalTable);
            fclose(f);
            FatalError(ERR_SEM_DEF, ERR_MESSAGES[ERR_SEM_DEF]);
        }
        
        tTabSym *localTable = NULL; //ukazatel na lokalni tabulku symbolu
        tInsTapeInsPtr firstIns = NULL; //ukazatel na prvni instrukci
        tInsTape *insTape = NULL; //ukazatel na instrukcni pasku
                
        insTape = findMain->info.func->instTape; //ukazatel ukazuje na instrukcni pasku mainu
        localTable = findMain->info.func->locTab; //ukazatel ukazuje na lokalni tabulku mainu
        firstIns = insTape->first; //ukazatelna prvni instrukci mainu
        
        //vlozeni instrukci, ktery po mne chtel Dominik
        if (insTapeInsertFirst(insTape, I_CF ,(void *) localTable, (void *) firstIns, NULL) == 0) {
            tabSymFree(globalTable);
            fclose(f);
            FatalError(ERR_INTERNAL, ERR_MESSAGES[ERR_ALLOC]);
        }
        
        //aktivni instrukci se stane prvni
        insTapeFirst(insTape);
        
        //vlozeni druhe instrukce
        if (insTapePostInsert(insTape, I_RETURN, NULL, NULL, NULL) == 0) {
            tabSymFree(globalTable);
            fclose(f);
            FatalError(ERR_INTERNAL, ERR_MESSAGES[ERR_ALLOC]);
        }

        //provedeni interpretace
        if ((result = executeTape(findMain->info.func->instTape->first)) != ERR_OK) {
           //uvolneni globalni tabuljy
            tabSymFree(globalTable);
            fclose(f);
            goto ERROR;
        }
        
        //uvolneni globalni tabulky po vygenerovani vsech instrukci a uzavreni souboru
        tabSymFree(globalTable);
        freeIdName(retGlobal);
        
        return;
    }
}


//!!!!! PRIPRAVENO K TESTOVANI !!!!!
/**
 * pravidlo ve tvaru:
 *          <function> -> <Kdata_types> fID(<arguments>)<body><function>
 * @return          funkce vraci ERR_OK, pokud je v poradku, jinak kod chyby
 */
int parseFunction() {
    int result;
    tToken token;
    //do teto promenne ukladam navratovy typ funkce
    tTabSymVarDataType returnType;
    //promenna, ktera slouzi pro kontrolu, zda uz je dana funkce deklarovana
    tTabSymElemData *funcID_info = NULL;
    //promenna do ktere ukladam vytvorene informace o funkci
    tFuncInfo *funcInfo = NULL;
    //pro kazdou funkci tvorim novy seznam parametru
    tParamListPtr paramList = NULL;
    tTabSym *localTabSym = NULL;
    tInsTape *instructionTape;
    
    //nactu prvni token, prisel chybny token
    if((result = getToken(&token, f)) != 1) {
        return ERR_LEX;
    }
    
    //program muze byt i prazdny
    if (token->typ == END_OF_FILE) {
        freeTokenMem(&token);
        return ERR_OK;
    }
       
    //volani funkce pro zpracovani <Kdata_types> - kontrola, zda mi prisel datovy typ
    if ((result = kDataTypes(&returnType, token->typ)) != 1 || returnType == TAB_SYM_VAR_AUTO) {
        freeTokenMem(&token);
        if (returnType == TAB_SYM_VAR_AUTO) {
            return ERR_SYNTAX;
        }
        else
            return result;
    }
    //uvolnim token
    freeTokenMem(&token);
    
    if((result = getToken(&token, f)) != 1)
        return ERR_LEX;
    
    //dalsi token je fID - <function> -> <Kdata_types> fID
    if (token->typ == TYPE_IDENTIFICATOR) {
         
        //uchovavam nazev identifikatoru
        string *idName;
        if((idName = copyIdName(&(token->value.stringVal))) == NULL) {
            freeTokenMem(&token);
            return ERR_INTERNAL;
        }
        
        //uvolnim token
        freeTokenMem(&token);
        
        //ziskam informace o funkci
        funcID_info = tabSymSearch(globalTable, idName);
            
        //uzivatel nemuze definovat funkce se shodnym nazvem, jaky maji funkce preddefinovane
        if((strcmp(idName->str, "length") == 0) || (strcmp(idName->str, "substr") == 0) ||
                (strcmp(idName->str, "concat") == 0) || (strcmp(idName->str, "find") == 0) ||
                (strcmp(idName->str, "sort") == 0)) {
            //uvolneni naalokovanych struktur
            freeIdName(idName);         
            return ERR_SEM_DEF; //pokus o redefinici funkce
        }
        
        //funkce uz byla deklarovana
        if (funcID_info != NULL) {
            //zkontroluji, zda je dany identifikator identifikatorem funkce
            if (funcID_info->type != TAB_SYM_FUNCTION) {
                freeIdName(idName);
                return ERR_INTERNAL;
            }
            //v tuto chvili uz muzu zkontrolovat, zda sedi navratovy typ funkce
            if(funcID_info->info.func->retType != (tTabSymVarNoAutoDataType)returnType) {
                freeIdName(idName);
                return ERR_SEM_DEF; //nesouhlasi navratovy typ
            }
        }
        
        //dalsi token by mel byt '('
        if ((result = getToken(&token, f) != 1)) {
            freeIdName(idName);
            return ERR_LEX;
        }
        
        //token byl '('
        if(token->typ == PARENTHESIS_OPENING) {
            //uvolnim token
            freeTokenMem(&token);
            
            if (funcID_info == NULL){
                //vytvorim si lokalni tabulku symbolu 
                //do ni uz budu ukladat parametry funkce
                localTabSym = tabSymCreate(TAB_SYM_LOC);

                //vytvorim promennou $ret, po ktere Dominik z celeho srdce touzi
                char *retId = "$ret";
                string *ret;
                if ((ret = (string*)malloc(sizeof(string))) == NULL){
                    freeIdName(idName); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }
                if (strInit(ret) == 1){
                    freeIdName(idName); free(ret); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }

                if (strConConstString(ret, retId) == 1) {
                    freeIdName(idName); freeIdName(ret); tabSymFree(localTabSym);              
                    return ERR_INTERNAL;
                }

                if ((retGlobal = copyIdName(ret)) == NULL) {
                    freeIdName(idName); freeIdName(ret); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }

                tVariableInfo *retInfo;

                if ((retInfo = tabSymCreateVariableInfo(returnType)) == NULL) {
                    freeIdName(idName); freeIdName(ret); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }

                if (tabSymInsertVar(localTabSym, ret, retInfo) == 0) {
                    freeIdName(idName); freeIdName(ret); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }
                freeIdName(ret);

                
                if ((paramList = initList(paramList)) == 0) {
                    freeIdName(idName); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }
                
                //vytvoreni instrukcni pasky
                if ((instructionTape = insTapeCreate()) == NULL) {
                    freeIdName(idName); 
                    paramListFree(paramList); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }
                //vlozim zbytecnou instrukci z duvodu rekurze, kdy funkce vola sebe sama
                insTapeInsertFirst(instructionTape, I_LABEL, NULL, NULL, NULL);
                
                //vytvoreni informaci o funkci pro globalni tabulku symbolu, kvuli kontrole, 
                //zda nema parametr stejny nazev jako funkce
                if((funcInfo = tabSymCreateFuncInfo(paramList, (tTabSymVarNoAutoDataType)returnType, 
                        localTabSym, NULL, instructionTape, false)) == NULL) {
                    freeIdName(idName); insTapeFree(instructionTape);
                    paramListFree(paramList); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }

                if((result = tabSymInsertFunc(globalTable, idName, funcInfo)) == 0) {
                    freeIdName(idName); free(funcInfo);
                    paramListFree(paramList); tabSymFree(localTabSym);
                    return ERR_INTERNAL;
                }
            }
            else {
                localTabSym = funcID_info->info.func->locTab;
                paramList = funcID_info->info.func->params;
                instructionTape = funcID_info->info.func->instTape;
            }
            
            //-------------------------------------------------------------
            //volani funkce pro zpracovani <arguments>
            if((result = parseArguments(paramList, funcID_info, localTabSym)) != ERR_OK) {
                freeIdName(idName);
                //navratim chybovy kod
                return result;
            }
            //-------------------------------------------------------------

            //<function> -> <Kdata_types> fID(<arguments>)<body>
            //jsme u <body> -> bud ';'(deklarace), nebo '{' (definice)
            if ((result = getToken(&token, f)) != 1) {
                freeIdName(idName);
                return ERR_LEX;
            }
            
            // DEKLARACE FUNKCE
            if(token->typ == SEMICOLON) {
                //uvolnim token
                freeTokenMem(&token);
                
                
                //nyni uz muzu uvolnit string, ktery nese informace o id funkce
                //tento identifikator funkce uz je ulozen v globalni tabulce symbolu
                freeIdName(idName);
                
                //muzeme zpracovavat  dalsi funkci 
                return parseFunction();
            }
            
            //DEFINICE FUNKCE
            if(token->typ == BRACES_OPENING) {
                 freeTokenMem(&token);
                //zkontroluji, zda se uz dana funkce nebyla definovana
                if (funcID_info != NULL) {
                    if (funcID_info->info.func->defined == true) {
                        freeIdName(idName); 
                        return ERR_SEM_DEF;
                    }
                }
                
                //vytvorim prazdny seznam tabulek bloku
                tTabSymList *blockList;
                if ((blockList = tabSymListCreate()) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                } 
                
                //--------------------------------------------------------
                //pripravu jsem dokoncil, muzu provadet telo funkce
                if ((result = parseStatementList(localTabSym, blockList, NULL, instructionTape)) != ERR_OK) {
                    return result;
                }
                //--------------------------------------------------------
                
                //vytvorim informace o funkci
                if ((funcInfo = tabSymCreateFuncInfo(paramList, (tTabSymVarNoAutoDataType)returnType,
                        localTabSym, blockList, instructionTape, true)) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                //vlozim informace o funkci do globalni tabulky symbolu
                if ((result = tabSymInsertFunc(globalTable, idName, funcInfo)) == 0) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                freeIdName(idName);
                //muzu zpracovavat dalsi funkci
                return parseFunction();  
            }
            
            //token neni ; ani {
            freeTokenMem(&token);
            freeIdName(idName);
            return ERR_SYNTAX;
        }
        //token neni oteviraci zavorka
        else {
            freeIdName(idName);
            freeTokenMem(&token);
            return ERR_SYNTAX;
        }
    }
    //dalsi token neni identifikator
    else {
        freeTokenMem(&token);
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
 * @return      funkce vraci 1, pokud je vse v poradku, jinak kod chyby
 */
int kDataTypes(tTabSymVarDataType *variableType, TokenTypes tokenType) {
    
    //ocekavana klicova slova
    switch(tokenType) {
        case KEYW_INT:
        case KEYW_DOUBLE:
        case KEYW_STRING:
        case KEYW_BOOL:
        case KEYW_AUTO:    
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
 * @return          pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
 */
int parseArguments(tParamListPtr paramList, tTabSymElemData *data, tTabSym *localTable) {
    int result;
    tToken token;
    //typ parametru
    tTabSymVarDataType paramType;
    
    //nacteme token
    if((result = getToken(&token, f)) != 1) {
        return ERR_LEX;
    }
    
    //zadne argumenty: pravidlo 8
    if(token->typ == PARENTHESIS_CLOSING) {
        //pokud je dany ID v globalni tabulce, provadime semantickou kontrolu hlavicky
        if(data != NULL) {
            //list argumentu neni prazdny, chyba
            if(data->info.func->params->first != NULL) {
                freeTokenMem(&token);
                return ERR_SEM_DEF;
            }
        }
        freeTokenMem(&token);
        return ERR_OK;
    }
    
    //upravene pravidlo 9: <arguments> -> <argument>
    //-----------------------------------------------------------
    if ((result = kDataTypes(&paramType, token->typ)) != 1 || paramType == TAB_SYM_VAR_AUTO) {
        freeTokenMem(&token);
        if (paramType == TAB_SYM_VAR_AUTO) {
            return ERR_SYNTAX;
        }
        else
            return result;
    }
    //----------------------------------------------------------
    
    freeTokenMem(&token);
    
    //pukud kontrolujeme parametry (to je v pripade, kdy data != NULL)
    //tak nastavime aktivitu prvni prvek seznamu parametru
     if (data != NULL)  first(data->info.func->params);
    
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
 * @return      pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
 */
int parseArgument(tParamListPtr paramList, tTabSymElemData *data, tTabSymVarDataType paramType, tTabSym *localTable) {
    tToken token;
    int result;
    
    //pozadam o novy token
    if((result = getToken(&token, f)) != 1) {
        return ERR_LEX;
    }
    //token neni ID
    if(token->typ != TYPE_IDENTIFICATOR) {
        freeTokenMem(&token);
        return ERR_SYNTAX;
    }
    
    //token je identifikator
    string *idName;
    
    //ulozim si nazev identifikatoru
    if((idName = copyIdName(&(token->value.stringVal))) == NULL) {
        freeTokenMem(&token);
        return ERR_INTERNAL;
    }
    freeTokenMem(&token);
    
    //pripad, kdy vkladam prvek do seznamu parametru a lokalni tabulky symbolu
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
        //vytvorim informace o promenne
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
        if (data->info.func->params->act == NULL) {
            freeIdName(idName);
            return ERR_SEM_DEF;
        }
        if (((tTabSymVarNoAutoDataType)paramType != data->info.func->params->act->dataType) ||
                (strcmp(idName->str, data->info.func->params->act->idName->str) != 0)) {
            freeIdName(idName);
            return ERR_SEM_DEF;
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
 * @return      pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
 */
int argumentNext(tParamListPtr paramList, tTabSymElemData *data, tTabSym *localTable) {
    tToken token;
    int result;
    //typ parametru
    tTabSymVarDataType paramType;
    
    //nactu dalsi token
    if((result = getToken(&token, f)) != 1) {
        return ERR_LEX;
    }
    
    //token je ')'
    if (token->typ == PARENTHESIS_CLOSING) {
        freeTokenMem(&token);
        //zkontroluji, zda neni dana funkce uz deklarovana s vice parametry
        if (data != NULL && data->info.func->params->act->next != NULL) {
            return ERR_SEM_DEF;
        }
        return ERR_OK;
    }
    //token je ,
    else if(token->typ == COMMA) {
        //uvolnim token
        freeTokenMem(&token);
        //prectu jeste jeden token, jelikoz funkce parse Argument
        //ocekava jako prvni token v me implementaci az ID
         if((result = getToken(&token, f)) != 1) {
            return ERR_LEX;
        }
        
        //--------------------------------------------------------
        if ((result = kDataTypes(&paramType, token->typ)) != 1 || paramType == TAB_SYM_VAR_AUTO) {
            freeTokenMem(&token);
            if (paramType == TAB_SYM_VAR_AUTO) {
                return ERR_SYNTAX;
            }
            else
                return result;
        }
        //--------------------------------------------------------
        
        freeTokenMem(&token);
        //v pripade, kdy provadim kontrolu, tak se posunu se v seznamu argumentu na dalsi prvek
        if (data != NULL)  succ(data->info.func->params);
        return parseArgument(paramList, data, paramType, localTable);
    }
    //neocekavany token
    else {
        freeTokenMem(&token);
        return ERR_SYNTAX;
    }
}


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
 * @return      pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
 */
int parseStatementList(tTabSym *localTable, tTabSymList *blockList,
                        tTabSymListElemPtr parent, tInsTape *instructionTape) {
    tToken token;
    int result;
    tTabSymVarDataType dataType;

    
    //pozadam o token
    if((result = getToken(&token, f)) != 1) {
        return ERR_LEX;
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
                freeTokenMem(&token);
                return ERR_INTERNAL;
            }
            
            //uvolnim token
            freeTokenMem(&token);
            
            //----------------------------------------------------
            //volam funkci pro zpracovani deklarace
            if ((result = parseDeclaration(dataType, localTable, instructionTape, parent)) != ERR_OK) {
                return result;
            }
            //----------------------------------------------------
            
            //opet volam funkci pro zpracovani statement-listu
            return parseStatementList(localTable, blockList, parent, instructionTape);
            
            break;
            
        //pravidlo 15 - <st_list> -> epsilon (konec funkce)
        //ukonceni bloku funkce, nebo normalniho bloku
        case BRACES_CLOSING:
            freeTokenMem(&token);
            //instrukci pro ukonceni bloku vkladam jenom kdyz jsem v zanorenem bloku
            if (parent != NULL) {
                if (insTapeInsertLast(instructionTape, I_DBF, NULL, NULL, NULL) == 0) {
                    return ERR_INTERNAL;
                }
            }
            return ERR_OK;
            break;
        
        //pravidlo 18 - <st_list> -> {<st_list>}<st_list>
        //TODO - GENEROVANI INSTRUKCE
        case BRACES_OPENING:
            freeTokenMem(&token);
            
            // nova lokalni tabulka
            tTabSym *blockLocalTable;
            // polozka v seznamu tabulek pro bloky
            tTabSymListElemPtr newElement;
            
            if ((newElement = prepareBlockList(&blockLocalTable, blockList, parent, localTable)) == 0){
                return ERR_INTERNAL;
            }
            
            //GENEROVANI INSTRUKCE
            if(insTapeInsertLast(instructionTape, I_CBF, (void*) blockLocalTable, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //rodicem se stane novy element, budeme pracovat s novou lokalni tabulkou
            //a novym listem, instrukcni paska je stejna
            //--------------------------------------------------------------
            if ((result = parseStatementList(blockLocalTable, newElement->childList, newElement, instructionTape)) != ERR_OK) {
                return result;
            }
            //--------------------------------------------------------------
            
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
            
            
            //-----------------------------------------------------
            if((result = parseStatement(localTable, token, instructionTape, blockList, parent)) != ERR_OK) {
                return result;
            }
            //-----------------------------------------------------
            
            return parseStatementList(localTable, blockList, parent, instructionTape);
            
            break;
        default:
            return ERR_SYNTAX;
    }
    //nikdy bych se sem nemel dostat
    return ERR_INTERNAL;
}



/**
 * zpracovava nasledujici pravidla:
 * 19. <statement> -> <assignment>;
 * 20. <statement> -> cin >> ID <cin>;
 * 21. <statement> -> cout << <term><cout>;
 * 22. <statement> -> return expression;
 * 23. <statement> -> if(expression)<block><else>
 * 24. <statement> -> for(<declaration>; expression; <assignment>)<block>
 * 25. <statement> -> while(expression)<block>
 * 26. <statement> -> do <block>while(expression);
 * @param localTable        -   lokalni tabulka symbolu
 * @param token             -   typ tokenu
 * @param instructionTape   -   ukazatel na instrukcni pasku
 * @param blockList         -   ukazatel na list tabulek bloku
 * @param blockListElem     -   aktualni element v listu tabulek symbolu pro bloky
 * @return      pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
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
            freeTokenMem(&tokenOrig);
            
            //nactu dalsi token - mel by byt '('
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            if (token->typ != PARENTHESIS_OPENING){
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //******************************************
            if((result = parseExpression( blockListElem ,localTable, instructionTape , &expressionType, f)) != ERR_OK) {
                return result;
            }
            //******************************************
            
            string *lastGeneratedTMPif, *keyIF;
            
            //ziskam posledni vygenerovany identifikator
            if((lastGeneratedTMPif = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                return ERR_INTERNAL;
            }
            
            //vyhledam ho v tabulce symbolu
            if ((keyIF = tabSymListGetPointerToKey(blockListElem, localTable, lastGeneratedTMPif)) == NULL) {
                freeIdName(lastGeneratedTMPif);
                return ERR_INTERNAL;
            }
            
            freeIdName(lastGeneratedTMPif);
            
            //vygeneruji instrukci pro znegovani hodnoty
            if((insTapeInsertLast(instructionTape, I_LOG_NOT, (void *) keyIF, NULL, (void *)keyIF)) == 0) {
                return ERR_INTERNAL;
            }
            
            //vygeneruji instrukci podmineneho skoku - zatim nevim, kam budu skakat
            if(insTapeInsertLast(instructionTape, I_IFNZERO, (void *) keyIF, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //zapamatuji si instrukci
            tInsTapeInsPtr ifSkip1 = insTapeGetLast(instructionTape);
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            //ocekavam uzaviraci zavorku
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //------------------------------------------
            if((result = parseBlock(localTable, blockList, blockListElem, instructionTape)) != ERR_OK) {
                return result;
            }
            //------------------------------------------
            
            //vygenerji instrukci pro nepodmineny skok - budu preskakovat else vetev
            //zatim neznam adresu instrukce, na kterou budu skakat
            if (insTapeInsertLast(instructionTape, I_GOTO, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //zapamatuji si instrukci
            tInsTapeInsPtr skipElse1 = insTapeGetLast(instructionTape);
            
            //vytvorim navesti
            if (insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //ulozim si odkaz na navesti
            tInsTapeInsPtr labElse = insTapeGetLast(instructionTape);
            
            //nastavim aktivitu na ifskip1
            insTapeGoto(instructionTape, ifSkip1);
            
            //aktualizuji instrukci, jiz vim, kam budu skakat
            insTapeActualize(instructionTape, I_IFNZERO, (void *) keyIF, (void *) labElse, NULL);
            
            //zpracovavam else vetev
            //----------------------------------------------
            if ((result = parseElse(localTable, blockList, blockListElem, instructionTape)) != ERR_OK){
                return result;
            }
            //----------------------------------------------
            
            //vytvorim instrukci navesti
            if(insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //ulozim si odkaz na tuto instrukci
            tInsTapeInsPtr labSkipElse = insTapeGetLast(instructionTape);
            
            //aktualizuji instrukci skipElse1
            insTapeGoto(instructionTape, skipElse1);
            
            //aktualizuji informace
            insTapeActualize(instructionTape, I_GOTO, (void *) labSkipElse, NULL, NULL);
            
            //vse probehlo v poradku
            return ERR_OK;
            break;
            
            
            
        //pravidlo 24 - for(<declaration>; expression; <assignment>)<block>
        //TODO - lokalni tabulka u funkce for?
        case KEYW_FOR:
            //uvolnim pamet tokenu
            freeTokenMem(&tokenOrig);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //vytorim novy blok, cimz vytvorim novy rozsah platnosti
            // nova lokalni tabulka
            tTabSym *blockLocalTable;
            tTabSymListElemPtr newElementFor = NULL;
            
            if((newElementFor = prepareBlockList(&blockLocalTable, blockList, blockListElem, localTable)) == NULL) {
                return ERR_INTERNAL;
            }
            
            if(insTapeInsertLast(instructionTape, I_CBF, (void*) blockLocalTable, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //v promenne dataType je ulozeny datovy typ promenne
            if ((result = kDataTypes(&dataType, token->typ)) != 1) {
                //uvolnim token
                freeTokenMem(&token);
                return result;
            }
            freeTokenMem(&token);
            
            //jsem ve stavu -  for(<declaration>
            //-----------------------------------
            if ((result = parseDeclaration(dataType, blockLocalTable, instructionTape, newElementFor)) != ERR_OK) {
                return result;
            }
            //---------------------------------
            
            //vygeneruji instrukci pro LABEL
            if (insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //zaznamenam si instrukci
            tInsTapeInsPtr labBegin = insTapeGetLast(instructionTape);
            
            //jsem ve stavu - for(<declaration>;expr
            //***********************************
            if ((result = parseExpression(newElementFor, blockLocalTable, instructionTape, &expressionType, f)) != ERR_OK) {
                return result;
            }
            //***********************************
            
            string *lastGeneratedTMPfor;
            if ((lastGeneratedTMPfor = tabSymListLastCreateTmpSymbol(newElementFor, blockLocalTable)) == NULL) {
                return ERR_INTERNAL;
            }
            
            string *keyFor1;
            if ((keyFor1 = tabSymListGetPointerToKey(newElementFor, blockLocalTable, lastGeneratedTMPfor)) == NULL) {
                freeIdName(lastGeneratedTMPfor); 
                return ERR_INTERNAL;
            }
            freeIdName(lastGeneratedTMPfor);
            
            //zneguji si vysledek vyrazu
            if (insTapeInsertLast(instructionTape, I_LOG_NOT, (void *)keyFor1, NULL, (void *) keyFor1) == 0) {
                return ERR_INTERNAL;
            }
            
            //vygeneruji podmineny skok, kterym preskocim provadeni cyklu
            if (insTapeInsertLast(instructionTape, I_IFNZERO, (void *)keyFor1, NULL, NULL) == 0){
                return ERR_INTERNAL;
            }
            
            //zapamatuji si instrukci
            tInsTapeInsPtr stepOver = insTapeGetLast(instructionTape);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //jsem ve stavu - for(<declaration>;expr;
            if(token->typ != SEMICOLON) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //vygeneruji si instrukci pro skok do bloku
            if(insTapeInsertLast(instructionTape, I_GOTO, NULL, NULL, NULL) == 0) {
                freeTokenMem(&token);
                return ERR_INTERNAL;
            }
            
            //zapamatuji si instrukci 
            tInsTapeInsPtr goToBlock = insTapeGetLast(instructionTape);
            
            //vygeneruji si navesti pro assignment
            if (insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                freeTokenMem(&token);
                return ERR_INTERNAL;
            }
            
            //zapamatuji si adresu navesti
            tInsTapeInsPtr labAssignment = insTapeGetLast(instructionTape);
            
            //jsem ve stavu - for(<declaration>;expr;<assignment>
            if(token->typ != INCREMENTATION && token->typ != DECREMENTATION &&
                    token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            //---------------------------------------------
            if((result = parseAssignment(token, blockLocalTable, instructionTape, newElementFor)) != ERR_OK) {
                return result;
            }
            //---------------------------------------------
            
            //vytvorim instrukci skoku zpet k vyhodnoceni podminky
            if (insTapeInsertLast(instructionTape, I_GOTO, (void *) labBegin, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //ocekavam uzaviraci zavorku - for(<Kdata_types>ID=expression; expression; <assignment>)
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //vygeneuji instrukci pro navesti
            if(insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //poznamenam si instrukci navesti
            tInsTapeInsPtr labBlock = insTapeGetLast(instructionTape);
            
            //upravim instrukci pro skok do bloku
            insTapeGoto(instructionTape, goToBlock);
            insTapeActualize(instructionTape, I_GOTO, (void *) labBlock, NULL, NULL);
            
            //-----------------------------------------------
            if ((result = parseBlock(blockLocalTable, newElementFor->childList, newElementFor, instructionTape)) != ERR_OK) {
                return result;
            }
            //-----------------------------------------------
            
            //vytvorim instrukci pro skok na assignment
            if (insTapeInsertLast(instructionTape, I_GOTO, (void *)labAssignment, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //vytvorim si navesti pro preskoceni cyklu
            if (insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //poznamenam si adresu navesti
            tInsTapeInsPtr labEndCycle = insTapeGetLast(instructionTape);
            
            //aktualizuji instrukce, ktera preskakuje telo cyklu
            insTapeGoto(instructionTape, stepOver);
            insTapeActualize(instructionTape, I_IFNZERO, (void *)keyFor1, (void *) labEndCycle, NULL);
            
            if (insTapeInsertLast(instructionTape, I_DBF, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            return ERR_OK;
            
            break;
            
            
            
        //pravidlo 20 - cin >> ID <cin>;
        case KEYW_CIN:
            //uvolneni predaneho tokenu
            freeTokenMem(&tokenOrig);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //ocekavam prvni >
            if (token->typ != GREATER) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //ocekavam druhe >
            if (token->typ != GREATER) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //ocekavam identifikator
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            //zkopiruju si nazev identifikatoru
            if ((idName = copyIdName(&(token->value.stringVal))) == NULL) {
                freeTokenMem(&token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(&token);
            
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
            
            
            if ((result = parseCin(instructionTape, localTable, blockListElem)) != ERR_OK) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //dalsi token by mel byt ;
            if(token->typ != SEMICOLON) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //pokud jsem se dostal az sem, tak syntakticka analyza probehla bez chyby
            return ERR_OK;
            break;
            
            
            
        //pravidlo 21 - <statement> -> cout << <term><cout>;
        case KEYW_COUT:
            freeTokenMem(&tokenOrig);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //ocekavam prvni <
            if (token->typ != LESS) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //ocekavam druhe <
            if (token->typ != LESS) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //isTerm
            if((result = isTerm(token->typ)) == 0){
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            //udrzuje informace o konstante
            tConstantInfo *constInfo;
            //udzuje hodnoty, kterych muze konstanta nabyvat
            unionValue uval;
            // vygenerovany identifikator pro konstantu
            string *tmp;
            
            switch (token->typ) {
                case TYPE_BOOL:
                case TYPE_INTEGER:
                case TYPE_DOUBLE:
                case TYPE_STRING:

                    //funkci createConstantInfo musim predat union typu unionValue
                    //tudiz si musim tento union vyrobit
                   //TODO - jak je to s uvolnovanim stringu?
                    if(token->typ == TYPE_BOOL)
                        uval.boolVal = token->value.boolVal;
                    if(token->typ == TYPE_DOUBLE)
                        uval.doubleVal = token->value.doubleVal;
                    if(token->typ == TYPE_INTEGER)
                        uval.intVal = token->value.intVal;
                    if(token->typ == TYPE_STRING)
                        uval.stringVal = copyIdName(&(token->value.stringVal));


                    //vytvorim informace o konstante
                    if ((constInfo = tabSymCreateConstantInfo(token->typ, uval)) == NULL) {
                        freeTokenMem(&token);
                        return ERR_INTERNAL;
                    }
                    
                    freeTokenMem(&token);
                    
                    //vytvorim si novy identifikator, ktery priradim konstante
                    if ((tmp = tabSymListCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                        return ERR_INTERNAL;
                    }
                    //vlozim konstantu do tabulky symbolu
                    if (tabSymInsertConst(localTable, tmp, constInfo) == 0) {
                        freeIdName(tmp);
                        return ERR_INTERNAL;
                    }

                    //vzdy bych mel dany klic najit, jelikoz jsem ho prave vlozil
                    key = tabSymListGetPointerToKey(blockListElem, localTable, tmp);
                    freeIdName(tmp);
                    
                    //vlozim instrukci do instrukcni pasky
                    if (insTapeInsertLast(instructionTape, I_COUT, (void *) key, NULL, NULL) == 0) {
                        return ERR_INTERNAL;
                    }
                    break;

                //vypisujeme identifikator
                case TYPE_IDENTIFICATOR:

                    //zkusim dany identifikator najit
                    if ((idUsable = tabSymListSearch(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                        freeTokenMem(&token);
                        return ERR_SEM_DEF;
                    }

                    //identifikator se nasel, chci ziskat klic do tabulky symbolu
                    if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                        freeTokenMem(&token);
                        return ERR_INTERNAL;
                    }

                    freeTokenMem(&token);

                    //vlozim instrukci do instrukcni pasky
                    if (insTapeInsertLast(instructionTape, I_COUT, (void *) key, NULL, NULL) == 0) {
                        return ERR_INTERNAL;
                    }
                    break;
                    
                //nikdy bych se sem nemel dostat
                default:
                    freeTokenMem(&token);
                    return ERR_INTERNAL;
            }
            
            
            //zpracovani dalsich vstupu
            if((result = parseCout(instructionTape, blockListElem, localTable)) != ERR_OK) {
                return result;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //dalsi token by mel byt ;
            if(token->typ != SEMICOLON) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //pokud jsem se dostal az sem, tak syntakticka analyza probehla v poradku
            return ERR_OK;
            break;
            
            
            
        //pravidlo 22 - <statement> -> return expression;
        case KEYW_RETURN:
            freeTokenMem(&tokenOrig);
            
            //*****************************************
            if ((result = parseExpression( blockListElem, localTable, instructionTape, &expressionType, f)) != ERR_OK) {
                return result;
            }
            //*****************************************
            
            string *lastGeneratedTMPreturn, *keyReturn, *keyRet;
            //zjistim ID posledniho vygenerovaneho identifikatoru
            if((lastGeneratedTMPreturn = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                return ERR_INTERNAL;
            }
            
            //vyhledam v tabulce symbolu klic korespondujici k danemu ID
            if((keyReturn = tabSymListGetPointerToKey(blockListElem, localTable, lastGeneratedTMPreturn)) == NULL) {
                freeIdName(lastGeneratedTMPreturn);
                return ERR_INTERNAL;
            }
            
            if((keyRet = tabSymListGetPointerToKey(blockListElem, localTable, retGlobal)) == NULL) {
                freeIdName(lastGeneratedTMPreturn);
                return ERR_INTERNAL;
            }
            
            freeIdName(lastGeneratedTMPreturn);
            
            if (insTapeInsertLast(instructionTape, I_ASSIGN, (void *) keyReturn, NULL, (void *) keyRet) == 0) {
                return ERR_INTERNAL;
            }
            
            //generuji instrukci pro navrat z funkce
            if(insTapeInsertLast(instructionTape, I_RETURN, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
			freeTokenMem(&token);
            return ERR_OK;
            break;
            
            
        //TODO - generovani instrukci
        //pravidlo 25 - while(expression)<block>
        case KEYW_WHILE:
            freeTokenMem(&tokenOrig);
            
            //vygeneruji si instrukci noveho navesti a uchovam si na ni ukazatel
            if (insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            //uchovam si ukazatel na tuto instrukci
            tInsTapeInsPtr whileBegin = insTapeGetLast(instructionTape);
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //***********************************
            if ((result = parseExpression(blockListElem, localTable, instructionTape , &expressionType, f)) != ERR_OK) {
                return result;
            }
            //***********************************
            
            string *lastGeneratedTMP, *keyOfLastTMP;
            //zjistim ID posledniho vygenerovaneho identifikatoru
            if((lastGeneratedTMP = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                return ERR_INTERNAL;
            }
            
            //vyhledam v tabulce symbolu klic korespondujici k danemu ID
            if((keyOfLastTMP = tabSymListGetPointerToKey(blockListElem, localTable, lastGeneratedTMP)) == NULL) {
                freeIdName(lastGeneratedTMP);
                return ERR_INTERNAL;
            }
            
            freeIdName(lastGeneratedTMP);
            
            //vygeneruji instrukci, ktera zneguje vyslednou promennou vyrazu
            if (insTapeInsertLast(instructionTape, I_LOG_NOT, (void*)keyOfLastTMP , NULL, (void *) keyOfLastTMP) == 0) {
                return ERR_INTERNAL;
            }
            
            //vygeneruji instrukci podmineneho skoku a ulozim si jeji adresu
            if (insTapeInsertLast(instructionTape, I_IFNZERO, (void *) keyOfLastTMP, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            tInsTapeInsPtr whileGoToEnd = insTapeGetLast(instructionTape);
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //------------------------------------------
            if ((result = parseBlock(localTable, blockList, blockListElem, instructionTape)) != ERR_OK) {
                return result;
            }
            //------------------------------------------
            
            //vytvorim instrukci skoku, ktera skoci vzdy na zacatek whilu
            if (insTapeInsertLast(instructionTape, I_GOTO, (void *) whileBegin, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //vygeneruji instrukci druheho navesti
            if (insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            //ulozim si adresu navesti
            tInsTapeInsPtr whileEnd = insTapeGetLast(instructionTape);
            
            //nastavim aktivitu na adresu, kterou chci prepsat
            insTapeGoto(instructionTape, whileGoToEnd);
            //prepisu danou instrukci
            insTapeActualize(instructionTape, I_IFNZERO, (void *) keyOfLastTMP, (void *) whileEnd, NULL);
            
            return ERR_OK;
            break;
            
            
         //TODO - generovani instrukci
        //pravidlo 26 - <statement> -> do <block>while(expression);
        case KEYW_DO:
            freeTokenMem(&tokenOrig);
            
            //vygeneruji si navesti pro zacatek cyklu
            if (insTapeInsertLast(instructionTape, I_LABEL, NULL, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //zapamatuji si adresu navesti
            tInsTapeInsPtr labDoBegin = insTapeGetLast(instructionTape);
            
            //---------------------------------------
            if ((result = parseBlock(localTable, blockList, blockListElem, instructionTape)) != ERR_OK) {
                return result;
            }
            //---------------------------------------
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != KEYW_WHILE) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != PARENTHESIS_OPENING) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //********************************
            if ((result = parseExpression( blockListElem, localTable, instructionTape , &expressionType, f)) != ERR_OK) {
                return result;
            }
            //********************************
            
            string* lastGeneratedTMPdo, *keyDo;
            
            //ziskam identifikator posledniho vygenerovaneho prvku
            if ((lastGeneratedTMPdo = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                return ERR_INTERNAL;
            }
            
            //ziskam odkaz na dany identifikator do tabulky symbolu
            if ((keyDo = tabSymListGetPointerToKey(blockListElem, localTable, lastGeneratedTMPdo)) == NULL) {
                freeIdName(lastGeneratedTMPdo);
                return ERR_INTERNAL;
            }
            
            freeIdName(lastGeneratedTMPdo);
            
            //provedu instrukci skoku, na navesti, pokud hodnota neni 0
            if(insTapeInsertLast(instructionTape, I_IFNZERO, (void *) keyDo, (void *) labDoBegin, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != PARENTHESIS_CLOSING) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            
            //pokud jsem se dostal az sem, tak je vse v poradku
            return ERR_OK;
            break;
            
            
        //pravidlo 19
        case INCREMENTATION:
        case DECREMENTATION:
        case TYPE_IDENTIFICATOR:
            
            //------------------------------------
            if ((result = parseAssignment(tokenOrig, localTable, instructionTape, blockListElem)) != ERR_OK) {
                return result;
            } 
            //------------------------------------
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != SEMICOLON) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            return ERR_OK;
                
            break;
        default:
            return ERR_SYNTAX;
    }
    //nikdy bych se sem nemel dostat
    return ERR_INTERNAL;
}


//!!!!! PRIPRAVENO K TESTOVANI !!!!!
/**
 * zpracovava nasledujici pravidla:
 * 36. <declaration> -> <Kdata_types> ID<dec_init>
 * 37. <declaration> -> auto ID = expression;
 * @param dataType          -   datovy typ promenne
 * @param localTable        -   ukazatel na lokalni tabulku symbolu
 * @param instructionTape   -   ukazatel na pasku instrukci
 * @param blockListElem     -   aktualni element v listu tabulek bloku
 * @return      pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
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
                return ERR_LEX;
            }
            
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            string *idName;
            if ((idName = copyIdName(&(token->value.stringVal))) == NULL) {
                freeTokenMem(&token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(&token);
            
            
            //kontroluji, zda uz promenna byla definovana
            varIdentifikator = tabSymSearch(localTable, idName);
            funcIdentifikator = tabSymSearch(globalTable, idName);
            if (varIdentifikator != NULL || funcIdentifikator != NULL) {
                freeIdName(idName); 
                return ERR_SEM_DEF;
            }
            
            
            //cast <decInit>
            
            if((result = getToken(&token, f)) != 1) {
                freeIdName(idName); 
                return ERR_LEX;
            }
            
            //pouze definice promenne
            if(token->typ == SEMICOLON) {
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
                
                string *declarationID;
                
                if ((declarationID = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                freeIdName(idName);
                
                //generovani instrukce pro definici 
                if (insTapeInsertLast(instructionTape, I_DECLARE, (void *) declarationID, NULL, NULL) == 0) {
                    return ERR_INTERNAL;
                }
                
                freeTokenMem(&token);
                return ERR_OK;
            }
            
            //inicializace
            if(token->typ == SET_OPER) {
                freeTokenMem(&token);
                 
                //****************************************************
                 if ((result = parseExpression( blockListElem, localTable, instructionTape, &expressionType, f)) != ERR_OK) {
                     freeIdName(idName); 
                     return result;
                 }
                //****************************************************
                
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
                
                string *lastGeneratedTMP;
                string *key, *key2;
                //vyhledame klic korespondujici k danemu identifikatoru tabulce symbolu
                if((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeIdName(idName); 
                    return ERR_INTERNAL;
                }
                
                freeIdName(idName);
                
                if (insTapeInsertLast(instructionTape, I_DECLARE, (void *) key, NULL, NULL) == 0) {
                    return ERR_INTERNAL;
                }
                
                //chci ziskat nazev posledn id docasne vygenerovane promenne
                if((lastGeneratedTMP = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                    return ERR_INTERNAL;
                }
                
                //vyhledam klic korespondujici k posledni vygenerovane promenne
                if ((key2 = tabSymListGetPointerToKey(blockListElem, localTable, lastGeneratedTMP)) == NULL) {
                    freeIdName(lastGeneratedTMP); 
                    return ERR_INTERNAL;
                }
                
                freeIdName(lastGeneratedTMP);
                
                //TODO GENEROVANI INSTRUKCE
                if ((result = insTapeInsertLast(instructionTape, I_ASSIGN, (void*) key2, NULL, (void*) key)) == 0) {
                    return ERR_INTERNAL;
                }
                
                 if((result = getToken(&token, f)) != 1) {
                     return ERR_LEX;
                 }
                 
                 //dalsi token by mel byt ;
                 if(token->typ != SEMICOLON){
                     freeTokenMem(&token);
                     return ERR_SYNTAX;
                 }
                 freeTokenMem(&token);
                 //pokud jsem se dostal az sem, vse je v poradku
                 return ERR_OK;
            }
            
            freeTokenMem(&token);
            //chybna syntaxe
            return ERR_SYNTAX;
            
            break;
            
            
        // pravidlo 37 - <declaration> -> auto ID = expression;
        case TAB_SYM_VAR_AUTO:
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if (token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            string *idNameAuto;
            if ((idNameAuto = copyIdName(&(token->value.stringVal))) == NULL) {
                freeTokenMem(&token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(&token);
            
            
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
                return ERR_LEX;
            }
            
            if(token->typ != SET_OPER) {
                freeIdName(idNameAuto);
                if (token->typ == SEMICOLON){
                    freeTokenMem(&token);
                    return ERR_SEM_AUTO;
                }
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            freeTokenMem(&token);
            
            //***************************************
            if((result = parseExpression( blockListElem ,localTable, instructionTape, &expressionType, f)) != ERR_OK) {
                freeIdName(idNameAuto); 
                return result;
            }
            //***************************************
            
            //vytvoreni informaci o promenne
            if ((variableInfo = tabSymCreateVariableInfo((tTabSymVarDataType)expressionType)) == NULL) {
                freeIdName(idNameAuto);
                return ERR_INTERNAL;
            }
            
            //vlozeni promenne do lokalni tabulky symbolu
            if ((tabSymInsertVar(localTable, idNameAuto, variableInfo)) == 0) {
                freeIdName(idNameAuto);
                return ERR_INTERNAL;
            }
            
            //TODO - GENEROVANI INSTRUKCE
            string *lastGeneratedTMP;
            string *key, *key2;
            //vyhledame klic v existujici tabulce symbolu
            if((key = tabSymListGetPointerToKey(blockListElem, localTable, idNameAuto)) == NULL) {
                freeIdName(idNameAuto);
                return ERR_INTERNAL;
            }

            freeIdName(idNameAuto);
            
            if (insTapeInsertLast(instructionTape, I_DECLARE, (void *)key, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //chci ziskat nazev posledni docasne vygenerovane promenne
            if((lastGeneratedTMP = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                return ERR_INTERNAL;
            }
            
            //vyhledam klic korespondujici k posledni vygenerovane promenne
            if((key2 = tabSymListGetPointerToKey(blockListElem, localTable, lastGeneratedTMP)) == NULL) {
                freeIdName(lastGeneratedTMP); 
                return ERR_INTERNAL;
            }
            freeIdName(lastGeneratedTMP);
            
            //GENEROVANI INSTRUKCE
            if ((result = insTapeInsertLast(instructionTape, I_ASSIGN, (void*) key2, NULL, (void*) key)) == 0) {
                return ERR_INTERNAL;
            }
            
            if ((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != SEMICOLON){
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            freeTokenMem(&token);
            //pokud jsem se dostal az sem, tak je vse v poradku
            return ERR_OK;
            
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
 * @return          pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
 */
int parseBlock(tTabSym *localTable, tTabSymList *blockList,
                tTabSymListElemPtr blockListElem, tInsTape *instructionTape) {
    tToken token;
    int result;
    
    if((result = getToken(&token, f)) != 1) {
        return ERR_LEX;
    }
    
    switch(token->typ) {
        // pravidlo 27 - <block> -> {<st_list>}
        case BRACES_OPENING:
            freeTokenMem(&token);
            
            // nova lokalni tabulka
            tTabSym *blockLocalTable;
            tTabSymListElemPtr newElement = NULL;
            
            if((newElement = prepareBlockList(&blockLocalTable, blockList, blockListElem, localTable)) == NULL) {
                return ERR_INTERNAL;
            }
            
            if(insTapeInsertLast(instructionTape, I_CBF, (void*) blockLocalTable, NULL, NULL) == 0) {
                return ERR_INTERNAL;
            }
            
            //---------------------------------------------------
            if((result = parseStatementList(blockLocalTable, newElement->childList, newElement, instructionTape)) != ERR_OK) {
                return result;
            }
            //---------------------------------------------------
            
            return ERR_OK;
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
            
            //--------------------------------------------
            return parseStatement(localTable, token, instructionTape, blockList, blockListElem);
            //--------------------------------------------
            
        default:
            return ERR_SYNTAX;
    }
    //nikdy bych se sem nemel dostat
    return ERR_INTERNAL;
}


/**
 * zpracovava pravidla:
 * 40. <assignment> = ID<specID>
 * 41. <assignment> = <inc_dec>ID
 * @param tokenType
 * @param localTable
 * @return          pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
 */

int parseAssignment(tToken tokenOrig, tTabSym *localTable, tInsTape *instructionTape,
                tTabSymListElemPtr blockListElem) {
    
    int result; 
    tToken token;
    string *idName;
    string *key, *key2;
    tTabSymElemData *idUsable;
    tTabSymVarDataType idType;
    tTabSymVarNoAutoDataType expType;
    
    switch(tokenOrig->typ) {
        case TYPE_IDENTIFICATOR:
            //TODO
            if ((idName = copyIdName(&(tokenOrig->value.stringVal))) == NULL ) {
                freeTokenMem(&tokenOrig);
                return ERR_INTERNAL;
            }
            freeTokenMem(&tokenOrig);
            
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
            
            if((result = getToken(&token, f)) != 1) {
                freeIdName(idName);
                return ERR_LEX;
            }
            
            //zpracovavam vetev ID++
            if(token->typ == INCREMENTATION) {
                //u inkrementace nesmi byt operator string
                if (idType == TAB_SYM_VAR_STRING) {
                    freeTokenMem(&token);
                    freeIdName(idName);
                    return ERR_SEM_COM;
                }
                
                //ziskam odkaz do tabulky symbolu
                if ((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeTokenMem(&token);
                    freeIdName(idName);
                }
                
                //id uz muzu uvolnit
                freeIdName(idName);
                
                if(insTapeInsertLast(instructionTape, I_INC, (void *) key, NULL, (void *) key) == 0) {
                    freeTokenMem(&token);
                    return ERR_INTERNAL;
                }
               
                freeTokenMem(&token);
                return ERR_OK;
            }
            //zpracovavam vetev ID--
            else if(token->typ == DECREMENTATION) {
                 //u dekrementace nesmi byt operator string, ani bool
                if (idType == TAB_SYM_VAR_STRING || idType == TAB_SYM_VAR_BOOLEAN) {
                    freeTokenMem(&token);
                    freeIdName(idName);
                    return ERR_SEM_COM;
                }
                
                //ziskam odkaz do tabulky symbolu
                if ((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeTokenMem(&token);
                    freeIdName(idName);
                }
                
                //id uz muzu uvolnit
                freeIdName(idName);
                
                if(insTapeInsertLast(instructionTape, I_DEC, (void *)key, NULL, (void *)key) == 0) {
                    freeTokenMem(&token);
                    return ERR_INTERNAL;
                }
   
                freeTokenMem(&token);
                return ERR_OK;
            }
            //zpracovavam vetev ID = expression;
            else if (token->typ == SET_OPER) {
                
                freeTokenMem(&token);
                
                //*****************************************
                if((result = parseExpression(blockListElem, localTable, instructionTape, &expType, f)) != ERR_OK) {
                    freeIdName(idName);
                    return result;
                }
                //*****************************************
                
                //potrebuji ziskat vyhodnoceni vyrazu
                string *lastCreatedTMP;
                if ((lastCreatedTMP = tabSymListLastCreateTmpSymbol(blockListElem, localTable)) == NULL) {
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                //chci ziskat klic, ktery koresponduje k danemu identifikatoru
                if((key = tabSymListGetPointerToKey(blockListElem, localTable, idName)) == NULL) {
                    freeIdName(lastCreatedTMP);
                    freeIdName(idName);
                    return ERR_INTERNAL;
                }
                
                freeIdName(idName);
                
                //chci ziskat klic, ktery koresponduje k posledni vygenerovane promenne
                if((key2 = tabSymListGetPointerToKey(blockListElem, localTable, lastCreatedTMP)) == NULL) {
                    freeIdName(lastCreatedTMP);
                    return ERR_INTERNAL;
                }
                
                freeIdName(lastCreatedTMP);
                
                //vlozeni instrukce
                if(insTapeInsertLast(instructionTape, I_ASSIGN, (void *) key2, NULL, (void *) key) == 0) {
                    return ERR_INTERNAL;
                }
                
                return ERR_OK;
            }
            else {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            break;
            
        //zpracovavam vetev ++ID
        case INCREMENTATION:
            
            freeTokenMem(&tokenOrig);
            //TODO - semanticke akce a generovani kodu
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            //semanticka kontrala, zda je promenna definovana v danem rozsahu platnosti
            if ((idUsable = tabSymListSearch(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(&token);
                return ERR_SEM_DEF;
            }
            
            //zkontroluji, zda jsem dostal info o promenne
            //pokud ne, hlasim chybu (ale nikdy by k ni dojit nemelo)
            if(idUsable->type != TAB_SYM_VARIABLE) {
                freeTokenMem(&token);
                return ERR_SEM_DEF;
            }
            
            idType = idUsable->info.var->dataType;
            
            if (idType == TAB_SYM_VAR_STRING) {
                freeTokenMem(&token);
                return ERR_SEM_COM;
            }
                
            //ziskam odkaz do tabulky symbolu
            if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(&token);
            }

            if(insTapeInsertLast(instructionTape, I_INC, (void *) key, NULL, (void *) key) == 0) {
                freeTokenMem(&token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(&token);
            //pokud jsem se dostal az sem, vse je v poradku
            return ERR_OK;
            break;
            
            
        //zpracovavam vetev --ID
        case DECREMENTATION:
            
            freeTokenMem(&tokenOrig);
            //TODO - semanticke akce a generovani kodu
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            if(token->typ != TYPE_IDENTIFICATOR) {
                freeTokenMem(&token);
                return ERR_SYNTAX;
            }
            
            //semanticka kontrala, zda je promenna definovana v danem rozsahu platnosti
            if ((idUsable = tabSymListSearch(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(&token);
                return ERR_SEM_DEF;
            }
            
            //zkontroluji, zda jsem dostal info o promenne
            //pokud ne, hlasim chybu (ale nikdy by k ni dojit nemelo)
            if(idUsable->type != TAB_SYM_VARIABLE) {
                freeTokenMem(&token);
                return ERR_SEM_DEF;
            }
            
            idType = idUsable->info.var->dataType;
            
            if (idType == TAB_SYM_VAR_STRING || idType == TAB_SYM_VAR_BOOLEAN) {
                freeTokenMem(&token);
                return ERR_SEM_COM;
            }
                
            //ziskam odkaz do tabulky symbolu
            if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                freeTokenMem(&token);
            }

            if(insTapeInsertLast(instructionTape, I_DEC, (void *) key, NULL, (void *) key) == 0) {
                freeTokenMem(&token);
                return ERR_INTERNAL;
            }
            
            freeTokenMem(&token);
            //pokud jsem se dostal az sem, vse je v poradku
            return ERR_OK;
            break;
            
        default:
            freeTokenMem(&tokenOrig);
            return ERR_SYNTAX;
    }
}

/**
 * * zpracovava nasledujici pravidla:
 * 32.  <cin> -> epsilon
 * 33.  <cin> -> >>ID<cin>
 * @param instructionTape           -   odkaz na instrukcni pasku
 * @param localTable                -   odkaz na lokalni tabulku symbolu
 * @param blockListElem             -   aktualni element v tabulce symbolu pro bloky
 * @return              pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
 */
int parseCin(tInsTape *instructionTape, tTabSym *localTable, tTabSymListElemPtr blockListElem) {
    int result;
    tToken token;
    tTabSymElemData *idUsable;
    
    if((result = getToken(&token, f)) != 1) {
        return ERR_LEX;
    }
    
    if(token->typ == SEMICOLON) {
        //navratime token, protoze s nim pocita volajici funkce
        ungetToken(&token);
        return ERR_OK;
    }
    
    if(token->typ == GREATER) {
        freeTokenMem(&token);
        
        if((result = getToken(&token, f)) != 1) {
            return ERR_LEX;
        }
        //uz mame >>
        if(token->typ == GREATER) {
            freeTokenMem(&token);
            
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            //ocekavam identifikator
            if(token->typ == TYPE_IDENTIFICATOR) {
                string *idName; 
                
                if((idName = copyIdName(&(token->value.stringVal))) == NULL) {
                    freeTokenMem(&token);
                    return ERR_INTERNAL;
                }
                freeTokenMem(&token);
                
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
            
            freeTokenMem(&token);
            return ERR_SYNTAX;
        }
        
        freeTokenMem(&token);
        return ERR_SYNTAX;
    }
    
    freeTokenMem(&token);
    return ERR_SYNTAX;
}


/**
 * zpracovava nasledujici pravidla
 *34. <cout> -> epsilon
 *35. <cout> -> << <term> <cout>
 * @param instructionTape       -   ukazatel na instrukcni pasku
 * @param blockListElem         -   aktualni element z tabulky symbolu pro bloky
 * @param localTable            -   ukazatel a lokalni tabulku
 * @return      pokud probehlo vse v poradku, tak ERR_OK, jinak kod chyby
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
        return ERR_LEX;
    }
    
    //pravidlo 34 - <cout> -> epsilon
    if(token->typ == SEMICOLON) {
        //volajici funkce s timto tokenem pocita
        ungetToken(&token);
        return ERR_OK;
    }
    
    //pravidlo 35 - <cout> -> << <term> <cout>
    if(token->typ == LESS) {
        freeTokenMem(&token);
        
        if((result = getToken(&token, f)) != 1) {
            return ERR_LEX;
        }
        
        if(token->typ == LESS) {
            freeTokenMem(&token);
            
            //dalsi token by mel byt term
            if((result = getToken(&token, f)) != 1) {
                return ERR_LEX;
            }
            
            ttype = token->typ;
            //freeTokenMem(&token);
            
            //jedna se o term: hodnota int, string, bool, double, nebo ID
            if((result = isTerm(ttype)) == 1) {
                
                switch(ttype){
                    case TYPE_BOOL:
                    case TYPE_INTEGER:
                    case TYPE_DOUBLE:
                    case TYPE_STRING:

                        //funkci createConstantInfo musim predat union typu unionValue
                        //tudiz si musim tento union vyrobit, tento string neuvolnim
                       // TODO - predelat do funkce
                        if(token->typ == TYPE_BOOL)
                            uval.boolVal = token->value.boolVal;
                        if(token->typ == TYPE_DOUBLE)
                            uval.doubleVal = token->value.doubleVal;
                        if(token->typ == TYPE_INTEGER)
                            uval.intVal = token->value.intVal;
                        if(token->typ == TYPE_STRING)
                            uval.stringVal = copyIdName(&(token->value.stringVal));
                        
                        freeTokenMem(&token);
                        
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
                            freeIdName(tmp);
                            return ERR_INTERNAL;
                        }
                        
                        //vzdy bych mel dany klic najit, jelikoz jsem ho prave vlozil
                        key = tabSymListGetPointerToKey(blockListElem, localTable, tmp);
                        freeIdName(tmp);
                        
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
                            freeTokenMem(&token);
                            return ERR_SEM_DEF;
                        }
                        
                        //identifikator se nasel, chci ziskat klic do tabulky symbolu
                        if ((key = tabSymListGetPointerToKey(blockListElem, localTable, &(token->value.stringVal))) == NULL) {
                            freeTokenMem(&token);
                            return ERR_INTERNAL;
                        }
                        
                        freeTokenMem(&token);
                        
                        //vlozim instrukci do instrukcni pasky
                        if (insTapeInsertLast(instructionTape, I_COUT, (void *) key, NULL, NULL) == 0) {
                            return ERR_INTERNAL;
                        }
                        
                        //muzu opet volat tuto funkci
                        return parseCout(instructionTape, blockListElem, localTable);
                        
                    //nikdy bych se sem nemel dostat
                    default:
                        freeTokenMem(&token);
                        return ERR_INTERNAL;
                }
   
            }
            freeTokenMem(&token);
            return ERR_SYNTAX;
        }
        
        freeTokenMem(&token);
        return ERR_SYNTAX;
    }
    
    freeTokenMem(&token);
    return ERR_SYNTAX;
}

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
                tTabSymListElemPtr blockListElem, tInsTape *instructionTape) {
    tToken token;
    int result;
    
    if((result = getToken(&token, f)) != 1) {
        return ERR_SYNTAX;
    }
            
    switch(token->typ) {
        //pravidlo 31 <else> -> else <block>
        case KEYW_ELSE:
            freeTokenMem(&token);
            //------------------------------------
            return parseBlock(localTable, blockList, blockListElem, instructionTape);
            //------------------------------------
            break;


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

            //navratim token - dalsi instrukce s nim pocitaji
            ungetToken(&token);

            return ERR_OK;
            break;
        default:
            return ERR_SYNTAX;
    }
}

