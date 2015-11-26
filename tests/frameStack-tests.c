
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
        unsigned int chyby = 0;
	tStack stack;
	SInit(&stack);
        
        pushNewFrame(&stack, true); //vlozim na stack novy pruchozi ramec
        
        string s,p,q,r,h; //vytvorim 4 stringy (klice)       
        strCharToString(&s,"test");
        strCharToString(&p,"tom");
        strCharToString(&q,"pivo");
        strCharToString(&r,"rum");
        strCharToString(&h, "honzik");
        

        tVariablePtr s1,s2,s3,destVar,s4;   //vytvorím tri ukazatele na struktury pro promene
     
        
        variableCreate(&s1, VAR_TYPE_INT); //vytovrim 3 promenné ktere navazu na kazatele vyse
        variableCreate(&s2, VAR_TYPE_INT);
        variableCreate(&s3, VAR_TYPE_INT);
        variableCreate(&s4, VAR_TYPE_STRING);
        
        s1->data.intVal = 5; //naplnim je hodnotami
        s2->data.intVal = 10;
        s3->data.intVal = 15;
        strCharToString(&(s4->data.stringVal),"pivo");
         
        
        insertNewVariable(&(stack.Top->frameContainer),s1, &s);
        insertNewVariable(&(stack.Top->frameContainer),s2, &p);
        insertNewVariable(&(stack.Top->frameContainer),s3, &q);
        insertNewVariable(&(stack.Top->frameContainer),s4, &h);
        
       x = findVariable(&stack, &p, &destVar); //hledam neexistujici
        if(x)printf("Nalezena promenna: %d\n", (destVar->data.intVal));
        else printf("neexistuje \n");
       x = findVariable(&stack, &q, &destVar); //hledam neexistujici
        if(x)printf("Nalezena promenna: %d\n", (destVar->data.intVal));
        else printf("neexistuje \n");
       x = findVariable(&stack, &r, &destVar); //hledam neexistujici
        if(x)printf("Nalezena promenna: %d\n", (destVar->data.intVal));
        else printf("neexistuje \n");
       x = findVariable(&stack, &h, &destVar); //hledam neexistujici
        if(x)printf("Nalezena promenna: %s\n", (destVar->data.stringVal.str));
        else printf("neexistuje \n");
        
       deleteTopFrame(&stack);
        
    return (EXIT_SUCCESS);
}

