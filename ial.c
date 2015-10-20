/* IAL.c dle RohacekD implementace*/



typedef struct tNode {                 /* Uzel binárního stromu*/
	int data;                                            /* u¾iteèná data */
	struct tNode *lptr;          /* ukazatel na levý list uzlu */
	struct tNode *rptr;        /* ukazatel na pravý list uzlu */
} *tNodePtr;

typedef struct {                                  /* binární strom*/
	tNodePtr root;
} tDLList;


