
#include <stdio.h>
#include <stdlib.h>
#include "tabSym.h"


void initList(tParamListPtr list) 
{
    list->first = NULL;
    list->act = NULL;
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
            //vkladani informaci 
            UkPomEl->dataType = dataType;
            UkPomEl->idName = idName;
            UkPomEl->next = NULL; 
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
      UkPomEl->dataType = dataType;
      UkPomEl->idName = idName;
      UkPomEl->next = NULL;
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

  //dokud nebude seznam prázdný, odstranuju prvni element
    while(list->first != NULL){
        UkPomEl = list->first;
        //pokud narazim na aktivni element, deaktivuji ho
        if(list->first == list->act) 
          list->act = NULL;
        //preskocim puvodni prvni element
        list->first = list->first->next; 
        free(UkPomEl); //uvolnim pamet
    }
}

