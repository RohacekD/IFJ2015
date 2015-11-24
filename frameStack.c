#include "frameStack.h"


void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
 L->First = NULL; //první prvek zatím neexistuje
 L->Last = NULL; //poslední prvek zatím neexistuje
 L->Act = NULL; //aktuální prvek zatím neexistuje
}

void DLDisposeList (tDLList *L) {
/*
     ** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
     ** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
     ** uvolněny voláním operace free. 
     **/
    L->Act=NULL; //zruší aktivitu seznamu
    L->Last=NULL; //ukazatel na poslední již není potřeba
    tDLElemPtr tmp = NULL; 
    while (L->First!=NULL) {  //dokud existují prvky jsou postupně mazány
       tmp = L->First->rptr;
       free(L->First);
       L->First = tmp;       
    }
}

void DLPush (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
tDLElemPtr tmp =NULL;    
tmp = malloc(sizeof(struct tDLElem)); //alokace prostoru pro nový prvek

    if (tmp==NULL) { //nezdarilá alokace
        DLError();
        return;
    }

tmp->data = val; //naplní novy prvek daty
tmp->lptr = NULL; 
tmp->rptr = NULL; 

    if ((L->First ==NULL) && (L->Last == NULL)) { //pokud je prvek jediný
        L->First = tmp; //stavá se jak prvním
        L->Last = tmp; //tak i posledním prvkem
    }
    else{
        tmp->rptr = L->First;  //v opačném připadě je prvek navázán na nasledující
        L->First->lptr = tmp; 
        L->First = tmp;
        
    }
}



void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
L->Act = L->First;	
}


void DLTop (tDLList *L, int *val) {
/*
     ** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
     ** Pokud je seznam L prázdný, volá funkci DLError().
     **/

    if (L->First == NULL) { //pokud je seznam prádný
        DLError();
        return;
    }
    *val = L->First->data; //na adresu val uloží data prvního prvku
}


void DLPop (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
tDLElemPtr tmp = NULL;
    if (L->First == NULL) return; //seznam je prázdný 
    if (L->Act == L->First) L->Act = NULL;
    if (L->First == L->Last)L->Last = NULL; 


tmp = L->First->rptr; //zálohuje ukazatel na pravý prvek
free(L->First); //uvolní prostor
L->First = tmp; //aktalizuje ukazatel na první prvek
if(L->First != NULL) L->First->lptr = NULL; 

}	



void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
    if(L->Act ==NULL){
        DLError();
        return;
    }
    
    *val = L->Act->data;
}


void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
if(L->Act == NULL) return;
L->Act = L->Act->rptr;

}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
if(L->Act == NULL) return;
L->Act = L->Act->lptr;	
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
return L->Act!= NULL;    
}