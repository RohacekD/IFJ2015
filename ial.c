/* IAL.c dle RohacekD implementace*/



typedef struct tNode {                 /* Uzel bin�rn�ho stromu*/
	int data;                                            /* u�ite�n� data */
	struct tNode *lptr;          /* ukazatel na lev� list uzlu */
	struct tNode *rptr;        /* ukazatel na prav� list uzlu */
} *tNodePtr;

typedef struct {                                  /* bin�rn� strom*/
	tNodePtr root;
} tDLList;


