
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Tomáš
 *
 * Created on 25. listopadu 2015, 19:32
 */

#include <stdio.h>
#include <stdlib.h>

#include "frameStack.h"
#include "variable.h"

/*
 * 
 */
int main(int argc, char** argv) {
         int x;
        //pocet chyb v testu
        unsigned int chyby = 0;
        //vytvoreni zasobniku
	tStack stack;
	SInit(&stack);
        
        //****************************************************************
        //TEST1 - vytvoreni noveho pruchoziho ramce, vlozeni 4 prvku
        pushNewFrame(&stack, true); //vlozim na stack novy pruchozi ramec
        string s,p,r,q,e,f,pru1,pru2,pru3, pru4; //vytvorim 4 stringy (klice)       
        strCharToString(&s,"klic1");
        strCharToString(&p,"klic2");
        strCharToString(&r,"klic3");
        strCharToString(&q,"klic4");
        strCharToString(&e,"klic5");
        strCharToString(&f,"klic6");
        strCharToString(&pru1,"pruchozi1");
        strCharToString(&pru2,"nepruchozi2");
        strCharToString(&pru3,"nepruchozi3");
        strCharToString(&pru4,"blabla");
        
        tVariablePtr s1,s2,s3,s4,spru1,spru2,spru3,spru4,destVar;
        //vytvorim 4 promenné ktere navazu na ukazatele vyse 
        variableCreate(&s1, VAR_TYPE_BOOL);
        variableCreate(&s2, VAR_TYPE_DOUBLE);
        variableCreate(&s3, VAR_TYPE_STRING);
        variableCreate(&s4, VAR_TYPE_INT);
        variableCreate(&spru1, VAR_TYPE_STRING);
        variableCreate(&spru2, VAR_TYPE_DOUBLE);
        variableCreate(&spru3, VAR_TYPE_BOOL);
        variableCreate(&spru4, VAR_TYPE_INT);
        //naplnim je hodnotami
        s1->data.boolVal = true; 
        s2->data.doubleVal = 10.25;
        strCharToString(&(s3->data.stringVal),"pivo");
        s4->data.intVal = 20; 
        strCharToString(&(spru1->data.stringVal),"rum");
        spru2->data.doubleVal = 20.225;
        spru3->data.boolVal = false;
        spru4->data.intVal = 11;
        //hodim tam tyto prvky
        insertNewVariable(&(stack.Top->frameContainer),s1, &s);
        insertNewVariable(&(stack.Top->frameContainer),s2, &p);
        insertNewVariable(&(stack.Top->frameContainer),s3, &r);
        insertNewVariable(&(stack.Top->frameContainer),s4, &q);
        
        printf("::TEST1: BOOL,DOUBLE,STRING, INT - EXISTUJOU::\n");
       x = findVariable(&stack, &s, &destVar);
        //melo by to nalezt hodnotu true
        if(x){
            printf("Nalezena promenna: %d\n", (destVar->data.boolVal));
        } else {
            printf("Chyba - BOOL nenalezen \n"); 
            chyby++;
              }
      x = findVariable(&stack, &p, &destVar); 
        if(x){
            printf("Nalezena promenna: %f\n", (destVar->data.doubleVal));
        } else {
            printf("Chyba - DOUBLE nenalezen \n"); 
            chyby++;
              }
       x = findVariable(&stack, &r, &destVar); //hledam neexistujici
        if(x){
            printf("Nalezena promenna: %s\n", (destVar->data.stringVal.str));
        } else {
            printf("Chyba - STRING nenalezen \n"); 
            chyby++;
              }
        x = findVariable(&stack, &q, &destVar); //hledam neexistujici
        if(x){
            printf("Nalezena promenna: %d\n", (destVar->data.intVal));
        } else {
            printf("Chyba - INT nenalezen \n"); 
            chyby++;
              }
       if(chyby == 0) {
           printf("::TEST01 OK::\n");
       } 
        printf("::TEST2 NEEXISTUJI::\n");
        x = findVariable(&stack, &e, &destVar); //hledam neexistujici
        if(x){
            printf("Nalezena promenna: %d\n", (destVar->data.intVal));
            chyby++;
        } else {
            printf("Nenalezen \n"); 
              }
        x = findVariable(&stack, &f, &destVar); //hledam neexistujici
        if(x){
            printf("Nalezena promenna: %d\n", (destVar->data.intVal));
            chyby++;
        } else {
            printf("Nenalezen \n");
              }
        if(chyby==0) {
            printf("::TEST02 OK::\n");
        }
        printf("::TEST3 - dalsi ramec ktery bude pruchozi.\n");
        pushNewFrame(&stack, true);
        insertNewVariable(&(stack.Top->frameContainer),spru1, &pru1);
        insertNewVariable(&(stack.Top->frameContainer),spru2, &pru2);
        insertNewVariable(&(stack.Top->frameContainer),spru3, &pru3);
        //najde, je z tohoto ramce
        x = findVariable(&stack, &pru1, &destVar); 
        if(x){
            printf("Nalezena promenna v top ramci: %s\n", (destVar->data.stringVal.str));
        } else {
            printf("Nenalezen \n");
            chyby++;
              }
        x = findVariable(&stack, &f, &destVar); 
        if(x){
            printf("Nemel byt nalezen: %f\n", (destVar->data.doubleVal));
            chyby++;
        } else {
            printf("Nenalezeno\n");
        }
        
         x = findVariable(&stack, &r, &destVar); 
        if(x){
            printf("Nalezen ve spodnim ramci: %s\n", destVar->data.stringVal.str);
        } else {
            printf("Nenalezeno\n");
            chyby++;
        }
         if(chyby==0) {
             printf("::TEST03 OK::\n");
         }
         
         //DALSI RAMEC CO NEBUDE PRUCHOZI
         pushNewFrame(&stack, false);
         printf("::TEST4 - dalsi ramec nepruchozi,najde jen v top::\n");
         insertNewVariable(&(stack.Top->frameContainer),spru4, &pru4);
          x = findVariable(&stack, &pru4, &destVar); 
        if(x){
            printf("Nalezen v aktualnim ramci: %d\n", destVar->data.intVal);
        } else {
            printf("Nenalezeno\n");
            chyby++;
        }
           x = findVariable(&stack, &r, &destVar); 
        if(x){
            printf("Nemel by byt nalezen: %s\n", destVar->data.stringVal.str);
            chyby++;
        } else {
            printf("Nenalezeno\n");
        }
           x = findVariable(&stack, &pru1, &destVar); 
        if(x){
            printf("Nalezena promenna: %s\n", (destVar->data.stringVal.str));
            chyby++;
        } else {
            printf("Nenalezeno\n");
              }
           if(chyby==0) {
             printf("::TEST04 OK::\n");
         }
        
       deleteTopFrame(&stack);
        
    return (EXIT_SUCCESS);
}

