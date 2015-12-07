
#include <stdio.h>
#include <stdlib.h>
#include "tabSym.h"


tParamListPtr initList() 
{
    tParamListPtr list;
    if ((list = malloc(sizeof(struct tParamList))) == NULL) {
        return NULL;
    }
    list->first = NULL;
    list->act = NULL;
    return list;
}

int createParamListElem(string *idName, tParamListElemPtr elem,
                        tTabSymVarNoAutoDataType dataType) {
    
    if (idName != NULL) {
        string *newString = malloc(sizeof(string));

        if (newString == NULL) {
            return 0;
        }

        if (strInit(newString) || strCopyString(newString, idName)) {
            //pokus o alokaci se nazdaril
            return 0;
        }
        
        elem->idName = newString; 
    }
    else {
        elem->idName = NULL;
    }
    
    elem->dataType = dataType;
    elem->next = NULL;
    
    return 1;
}

int insertEl(tParamListPtr list, string *idName, tTabSymVarNoAutoDataType dataType)
{
    //vkladam do prazdneho seznamu
    if(list->first == NULL) {
        tParamListElemPtr UkPomEl;
        //alokace pameti
        if((UkPomEl = (struct tParamListElem *) malloc(sizeof(struct tParamListElem))) == NULL) 
            return 0;
        else{
            //vytvoreni prostoru pro identifikator a jeho vlozeni
            //vkladani informaci 
            if(createParamListElem(idName, UkPomEl,dataType) == 0) {
                return 0;
            }
            //prvek je prvni a zaroven i aktivni
            list->first = UkPomEl;
            list->act = UkPomEl;
        }
    }
    else {
        //vlozeni prvku za aktivni prvek
        if ((postInsert(list, idName, dataType)) == 0) {
            return 0;
        }
        //posunuti aktivity na posledni prvek
        succ(list);
    }
    return 1;
}


int postInsert(tParamListPtr list, string *idName, tTabSymVarNoAutoDataType dataType)
{
  if(list->act != NULL){ //seznam je aktivni
    tParamListElemPtr UkPomEl;
    if((UkPomEl = (struct tParamListElem *) malloc(sizeof(struct tParamListElem))) == NULL) //alokace se nezdarila
      return 0;
        //Todo volani chyby
    else{  //alokace se zdarila
        if(createParamListElem(idName, UkPomEl,dataType) == 0) {
            return 0;
        }
      list->act->next = UkPomEl;
    }
  }
  return 1;
}
 

tParamListElemPtr getActElement(tParamListPtr list)
{
    if (list->act != NULL)
        return list->act;
    return NULL;
}


void first(tParamListPtr list) 
{
    list->act = list->first;
}

void succ(tParamListPtr list) 
{
    if(list->act != NULL) //seznam je aktivni
        list->act = list->act->next;
}
 

void paramListFree(tParamListPtr list)
{
    tParamListElemPtr UkPomEl; //pomocny ukazatel na strukturu element
    
    if(list == NULL)
        return;
    
  //dokud nebude seznam prázdný, odstranuju prvni element
    while(list->first != NULL){
        UkPomEl = list->first;
        //pokud narazim na aktivni element, deaktivuji ho
        if(list->first == list->act) 
          list->act = NULL;
        //preskocim puvodni prvni element
        list->first = list->first->next;
        //uvolneni identifikatoru
        strFree(UkPomEl->idName);
        free(UkPomEl); //uvolnim pamet
    }
    free(list);
    list = NULL;
}

