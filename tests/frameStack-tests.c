/*
 * IFJ 2015 - tym 059
 * tests for framestack
 * author: xbartu03, xrohac, xjelin42
 */


#include <stdio.h>
#include <stdlib.h>
#include "frameStack.h"
#include "variable.h"

int main(int argc, char** argv) {
        //vytvoreni a inicializace stacku
	tStack stack;
	SInit(&stack);
    /*
     * Pridani 4 prvku s ruznymi dat.typy a jejich vyhledani pomoci findVariable
     */
    //promenna pro findVariable
    int x;
    //pocet chyb
    unsigned int chyby = 0;
    //vlozim na stack novy pruchozi ramec
    pushNewFrame(&stack, false); 
     string a,b,c,d;
        //pomoci strCharToString priradim klice
        strCharToString(&a,"klic1");
        strCharToString(&b,"klic2");
        strCharToString(&c,"klic3");
        strCharToString(&d,"klic4");
        tVariablePtr s1,s2,s3,s4,destVar;
        //vytvorim tyto 4 promenne ktere navazu na ukazatele vyse
        variableCreate(&s1, VAR_TYPE_BOOL);
        variableCreate(&s2, VAR_TYPE_DOUBLE);
        variableCreate(&s3, VAR_TYPE_STRING);
        variableCreate(&s4, VAR_TYPE_INT);
        //naplnim hodnoty
        s1->data.boolVal = true; 
        s2->data.doubleVal = 10.25;
        strCharToString(&(s3->data.stringVal),"pivo");
        s4->data.intVal = 20; 
        //hodim na stack do framecontaineru tyto prvky s klicema
        insertNewVariable(&(stack.Top->frameContainer),s1, &a);
        insertNewVariable(&(stack.Top->frameContainer),s2, &b);
        insertNewVariable(&(stack.Top->frameContainer),s3, &c);
        insertNewVariable(&(stack.Top->frameContainer),s4, &d);
        printf(":::TEST01 STARTED:::\n");
        //zkusim najit klic1 - mel by byt nalezen
        x = findVariable(&stack, &a, &destVar);
        if(x){
            printf("Nalezena promenna: %d\n", (destVar->data.boolVal));
        } else {
            printf("Chyba - BOOL nenalezen \n"); 
            chyby++;
              }
        //mel by byt nalezen
      x = findVariable(&stack, &b, &destVar); 
        if(x){
            printf("Nalezena promenna: %f\n", (destVar->data.doubleVal));
        } else {
            printf("Chyba - DOUBLE nenalezen \n"); 
            chyby++;
              }
      //mel by byt nalezen
       x = findVariable(&stack, &c, &destVar); 
        if(x){
            printf("Nalezena promenna: %s\n", (destVar->data.stringVal.str));
        } else {
            printf("Chyba - STRING nenalezen \n"); 
            chyby++;
              }
       //mel by byt nalezen
        x = findVariable(&stack, &d, &destVar); //hledam neexistujici
        if(x){
            printf("Nalezena promenna: %d\n", (destVar->data.intVal));
        } else {
            printf("Chyba - INT nenalezen \n"); 
            chyby++;
              }
       if(chyby == 0) {
           printf("::TEST01 OK::\n");
          
       } else {
           printf("::TEST01 FAILED::\n");
         
       }
        //TEST2 - pridam dalsi ramec se stejnymi klicemi - melo by to brat z top
        pushNewFrame(&stack, true); //pruchozi
        tVariablePtr s5,s6,s7;
        //vytvorim tyto 4 promenne ktere navazu na ukazatele vyse
        variableCreate(&s5, VAR_TYPE_BOOL);
        variableCreate(&s6, VAR_TYPE_DOUBLE);
        variableCreate(&s7, VAR_TYPE_STRING);
       
        //naplnim hodnoty
        s5->data.boolVal = false; 
        s6->data.doubleVal = 25.10;
        strCharToString(&(s7->data.stringVal),"rum");
        //hodim na stack do framecontaineru tyto prvky s klicema
        insertNewVariable(&(stack.Top->frameContainer),s5, &a);
        insertNewVariable(&(stack.Top->frameContainer),s6, &b);
        insertNewVariable(&(stack.Top->frameContainer),s7, &c);
        printf(":::TEST02 STARTED:::\n");
        //zkusim najit klic1 - mel by byt nalezen v hornim framu
        x = findVariable(&stack, &a, &destVar);
        if(destVar->data.boolVal == 0){
            printf("Bere to z horniho framu: %d\n", (destVar->data.boolVal));
        } else {
            printf("Chyba. \n"); 
            chyby++;
              }
        //mel by byt nalezen
      x = findVariable(&stack, &b, &destVar); 
        if(x){
            printf("Nalezeno v hornim framu %f\n", (destVar->data.doubleVal));
        } else {
            printf("Chyba  \n"); 
            chyby++;
              }
      //mel by byt nalezen
       x = findVariable(&stack, &c, &destVar); 
        if(x){
            printf("Nalezena promenna v hornim framu %s\n", (destVar->data.stringVal.str));
        } else {
            printf("Chyba \n"); 
            chyby++;
              }
       //mel by byt nalezen ve spodnim framu
        x = findVariable(&stack, &d, &destVar); //hledam neexistujici
        if(destVar->data.intVal == 20){
            printf("Nalezena promenna ve spodnim framu: %d\n", (destVar->data.intVal));
        } else {
            printf("Chyba\n"); 
            chyby++;
              }
       if(chyby == 0) {
           printf("::TEST02 OK::\n");
          
       } else {
           printf("::TEST02 FAILED::\n");
         
       }
        //TEST03
        //pridam dalsi frame bloku - takze ho pak smazu, a bude nepruchozi
        pushNewFrame(&stack, false);
        tVariablePtr s8,s9,s10,s11;
        //vytvorim tyto 4 promenne ktere navazu na ukazatele vyse
        variableCreate(&s8, VAR_TYPE_BOOL);
        variableCreate(&s9, VAR_TYPE_DOUBLE);
        variableCreate(&s10, VAR_TYPE_STRING);
        variableCreate(&s11, VAR_TYPE_BOOL);
       
        //naplnim hodnoty
        s8->data.boolVal = true; 
        s9->data.doubleVal = 251.05;
        strCharToString(&(s10->data.stringVal),"cokolada");
        s11->data.boolVal = false;
        //hodim na stack do framecontaineru tyto prvky s klicema
        insertNewVariable(&(stack.Top->frameContainer),s8, &a);
        insertNewVariable(&(stack.Top->frameContainer),s9, &b);
        insertNewVariable(&(stack.Top->frameContainer),s10,&c);
        //vlozim dalsi se stejnym klicem - melo by se updatnout
        insertNewVariable(&(stack.Top->frameContainer),s11,&c);
        
        printf(":::TEST03 STARTED:::\n");
        x = findVariable(&stack, &a, &destVar);
        if(destVar->data.boolVal == 1){
            printf("Bere to z aktualniho framu: %d\n", (destVar->data.boolVal));
        } else {
            printf("Chyba. \n"); 
            chyby++;
              }
        x = findVariable(&stack, &b, &destVar); 
        if(x){
            printf("Nalezeno v aktualnim framu %f\n", (destVar->data.doubleVal));
        } else {
            printf("Chyba  \n"); 
            chyby++;
              }
      //mel by byt nalezen
       x = findVariable(&stack, &c, &destVar); 
        if(destVar->data.boolVal == 0){
            printf("Nalezena promenna v hornim framu, mela by to byt aktualizovana hodnota pro tento klic %s\n", (destVar->data.boolVal));
        } else {
            printf("Chyba \n"); 
            chyby++;
              }
       x = findVariable(&stack, &c, &destVar); 
        if(destVar->data.stringVal.str == NULL){
            printf("Nenaslo to tu pred aktualizaci\n");
        } else {
            printf("Chyba \n"); 
            chyby++;
              }
        x = findVariable(&stack, &d, &destVar); //hledam neexistujici
        if(x){
            printf("NEMELO BY TO NAJIT protoze neni pruchozi: %d\n", (destVar->data.intVal));
            chyby++;
        } else {
            printf("Nenaslo - neni pruchozi\n"); 
              }
       if(chyby == 0) {
           printf("::TEST03 OK::\n");
          
       } else {
           printf("::TEST03 FAILED::\n");
         
       }
        deleteFunctionsFrames(&stack);
        printf("Smazan TOPFRAME!\n");
         x = findVariable(&stack, &a, &destVar); 
        if(x){
            printf("Nalezeno v novem top ramci  %d\n", (destVar->data.boolVal)); 
              }
          x = findVariable(&stack, &b, &destVar);
        if(x){
            printf("Naslo se v novem top ramci %f\n", (destVar->data.doubleVal)); 
              }
         x = findVariable(&stack, &c, &destVar); 
        if(x){
            printf("Nalezeno v novem top ramci  %s\n", (destVar->data.stringVal.str)); 
              }
         x = findVariable(&stack, &d, &destVar); 
        if(x){
            printf("Nalezeno ve spodnim ramci  %d\n", (destVar->data.intVal)); 
              }
         deleteFunctionsFrames(&stack);
         printf("Ted se smazou vsechny ramce..\n");
          x = findVariable(&stack, &a, &destVar); 
        if(x){
            printf("CHYBA\n"); 
            chyby++;
              }
          x = findVariable(&stack, &b, &destVar);
        if(x){
            printf("CHYBA"); 
            chyby++;
              }
         x = findVariable(&stack, &c, &destVar); 
        if(x){
            printf("CHYBA"); 
            chyby++;
              }
         x = findVariable(&stack, &d, &destVar); 
        if(x){
            printf("Chyba"); 
            chyby++;
              }
         if(chyby == 0) {
             printf("Smazani probehlo uspesne.\n");
         }
         printf("Disposnu stack..\n");
         SDispose(&stack);
    return (EXIT_SUCCESS);
}

