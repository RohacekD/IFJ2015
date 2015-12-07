#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "tabSym.h"

#define MAX_HEIGHT 1000
#define INFINITY (1<<20)
int lprofile[MAX_HEIGHT];
int rprofile[MAX_HEIGHT];


//adjust gap between left and right nodes
int gap = 3;

//used for printing next node in the same level,
//this is the x coordinate of the next char printed
int print_next;
typedef struct Tree Tree;

struct Tree {
	Tree * left, *right;
	char *element;
};

//printing tree in ascii

typedef struct asciinode_struct asciinode;

struct asciinode_struct {
	asciinode * left, *right;

	//length of the edge from this node to its children
	int edge_length;

	int height;

	int lablen;

	//-1=I am left, 0=I am root, 1=right
	int parent_dir;

	//max supported unit32 in dec, 10 digits max
	char label[11];
};

Tree *make_empty(Tree *t) {
	if (t != NULL) {
		make_empty(t->left);
		make_empty(t->right);
		free(t);
	}
	return NULL;
}

Tree *find_min(Tree *t) {
	if (t == NULL) {
		return NULL;
	} else if (t->left == NULL) {
		return t;
	} else {
		return find_min(t->left);
	}
}

Tree *find_max(Tree *t) {
	if (t == NULL) {
		return NULL;
	} else if (t->right == NULL) {
		return t;
	} else {
		return find_max(t->right);
	}
}

Tree *finder(char* elem, Tree *t) {
	if (t == NULL) {
		return NULL;
	}

	if (strcmp(elem, t->element) < 0) {
		return finder(elem, t->left);
	} else if (strcmp(elem, t->element) > 0) {
		return finder(elem, t->right);
	} else {
		return t;
	}
}


//Insert i into the tree t, duplicate will be discarded
//Return a pointer to the resulting tree.
Tree * insert(char* value, Tree * t) {
	Tree * new_node;

	if (t == NULL) {
		new_node = (Tree *) malloc(sizeof(Tree));
		if (new_node == NULL) {
			return t;
		}

		new_node->element = value;

		new_node->left = new_node->right = NULL;
		return new_node;
	}

	if (strcmp(value, t->element) < 0) {
		t->left = insert(value, t->left);
	} else if (strcmp(value, t->element) > 0) {
		t->right = insert(value, t->right);
	} else {
		//duplicate, ignore it
		return t;
	}
	return t;
}

Tree * delete(char* value, Tree * t) {
	//Deletes node from the tree
	// Return a pointer to the resulting tree
	//Tree * x;
	Tree *tmp_cell;

	if (t == NULL)
		return NULL;

	if (strcmp(value, t->element) < 0) {
		t->left = delete(value, t->left);
	} else if (strcmp(value, t->element) > 0) {
		t->right = delete(value, t->right);
	} else if (t->left && t->right) {
		tmp_cell = find_min(t->right);
		t->element = tmp_cell->element;
		t->right = delete(t->element, t->right);
	} else {
		tmp_cell = t;
		if (t->left == NULL)
			t = t->right;
		else if (t->right == NULL)
			t = t->left;
		free(tmp_cell);
	}

	return t;
}


int MIN(int X, int Y) {
	return ((X) < (Y)) ? (X) : (Y);
}

int MAX(int X, int Y) {
	return ((X) > (Y)) ? (X) : (Y);
}

asciinode * build_ascii_tree_recursive(Tree * t) {
	asciinode * node;

	if (t == NULL)
		return NULL;

	node = malloc(sizeof(asciinode));
	node->left = build_ascii_tree_recursive(t->left);
	node->right = build_ascii_tree_recursive(t->right);

	if (node->left != NULL) {
		node->left->parent_dir = -1;
	}

	if (node->right != NULL) {
		node->right->parent_dir = 1;
	}

	sprintf(node->label, "%s", t->element);
	node->lablen = strlen(node->label);

	return node;
}

//Copy the tree into the ascii node structre
asciinode * build_ascii_tree(Tree * t) {
	asciinode *node;
	if (t == NULL)
		return NULL;
	node = build_ascii_tree_recursive(t);
	node->parent_dir = 0;
	return node;
}

//Free all the nodes of the given tree
void free_ascii_tree(asciinode *node) {
	if (node == NULL)
		return;
	free_ascii_tree(node->left);
	free_ascii_tree(node->right);
	free(node);
}

//The following function fills in the lprofile array for the given tree.
//It assumes that the center of the label of the root of this tree
//is located at a position (x,y).  It assumes that the edge_length
//fields have been computed for this tree.
void compute_lprofile(asciinode *node, int x, int y) {
	int i, isleft;
	if (node == NULL)
		return;
	isleft = (node->parent_dir == -1);
	lprofile[y] = MIN(lprofile[y], x - ((node->lablen - isleft) / 2));
	if (node->left != NULL) {
		for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
			lprofile[y + i] = MIN(lprofile[y + i], x - i);
		}
	}
	compute_lprofile(node->left, x - node->edge_length - 1,
			y + node->edge_length + 1);
	compute_lprofile(node->right, x + node->edge_length + 1,
			y + node->edge_length + 1);
}

void compute_rprofile(asciinode *node, int x, int y) {
	int i, notleft;
	if (node == NULL)
		return;
	notleft = (node->parent_dir != -1);
	rprofile[y] = MAX(rprofile[y], x + ((node->lablen - notleft) / 2));
	if (node->right != NULL) {
		for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
			rprofile[y + i] = MAX(rprofile[y + i], x + i);
		}
	}
	compute_rprofile(node->left, x - node->edge_length - 1,
			y + node->edge_length + 1);
	compute_rprofile(node->right, x + node->edge_length + 1,
			y + node->edge_length + 1);
}

//This function fills in the edge_length and
//height fields of the specified tree
void compute_edge_lengths(asciinode *node) {
	int h, hmin, i, delta;
	if (node == NULL)
		return;
	compute_edge_lengths(node->left);
	compute_edge_lengths(node->right);

	/* first fill in the edge_length of node */
	if (node->right == NULL && node->left == NULL) {
		node->edge_length = 0;
	} else {
		if (node->left != NULL) {
			for (i = 0; i < node->left->height && i < MAX_HEIGHT; i++) {
				rprofile[i] = -INFINITY;
			}
			compute_rprofile(node->left, 0, 0);
			hmin = node->left->height;
		} else {
			hmin = 0;
		}
		if (node->right != NULL) {
			for (i = 0; i < node->right->height && i < MAX_HEIGHT; i++) {
				lprofile[i] = INFINITY;
			}
			compute_lprofile(node->right, 0, 0);
			hmin = MIN(node->right->height, hmin);
		} else {
			hmin = 0;
		}
		delta = 4;
		for (i = 0; i < hmin; i++) {
			delta = MAX(delta, gap + 1 + rprofile[i] - lprofile[i]);
		}

		//If the node has two children of height 1, then we allow the
		//two leaves to be within 1, instead of 2
		if (((node->left != NULL && node->left->height == 1)
				|| (node->right != NULL && node->right->height == 1))
				&& delta > 4) {
			delta--;
		}

		node->edge_length = ((delta + 1) / 2) - 1;
	}

	//now fill in the height of node
	h = 1;
	if (node->left != NULL) {
		h = MAX(node->left->height + node->edge_length + 1, h);
	}
	if (node->right != NULL) {
		h = MAX(node->right->height + node->edge_length + 1, h);
	}
	node->height = h;
}

//This function prints the given level of the given tree, assuming
//that the node has the given x cordinate.
void print_level(asciinode *node, int x, int level) {
	int i, isleft;
	if (node == NULL)
		return;
	isleft = (node->parent_dir == -1);
	if (level == 0) {
		for (i = 0; i < (x - print_next - ((node->lablen - isleft) / 2)); i++) {
			printf(" ");
		}
		print_next += i;
		printf("%s", node->label);
		print_next += node->lablen;
	} else if (node->edge_length >= level) {
		if (node->left != NULL) {
			for (i = 0; i < (x - print_next - (level)); i++) {
				printf(" ");
			}
			print_next += i;
			printf("/");
			print_next++;
		}
		if (node->right != NULL) {
			for (i = 0; i < (x - print_next + (level)); i++) {
				printf(" ");
			}
			print_next += i;
			printf("\\");
			print_next++;
		}
	} else {
		print_level(node->left, x - node->edge_length - 1,
				level - node->edge_length - 1);
		print_level(node->right, x + node->edge_length + 1,
				level - node->edge_length - 1);
	}
}

//prints ascii tree for given Tree structure
void print_ascii_tree(Tree * t) {
	asciinode *proot;
	int xmin, i;
	if (t == NULL)
		return;
	proot = build_ascii_tree(t);
	compute_edge_lengths(proot);
	for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
		lprofile[i] = INFINITY;
	}
	compute_lprofile(proot, 0, 0);
	xmin = 0;
	for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
		xmin = MIN(xmin, lprofile[i]);
	}
	for (i = 0; i < proot->height; i++) {
		print_next = 0;
		print_level(proot, -xmin, i);
		printf("\n");
	}
	if (proot->height >= MAX_HEIGHT) {
		printf("(This tree is taller than %d, and may be drawn incorrectly.)\n",
				MAX_HEIGHT);
	}
	free_ascii_tree(proot);
}

Tree * createTreeFromBST(tBSTNodePtr originalRoot){
	if(originalRoot==NULL){
		return NULL;
	}

	Tree * root= (Tree *) malloc(sizeof(Tree));

	root->left=createTreeFromBST(originalRoot->l);
	root->right=createTreeFromBST(originalRoot->r);
	root->element=originalRoot->key->str;

	return root;
}

void printBST(tBSTNodePtr originalRoot){
	//prevedem tBSTNodePtr na tree
	Tree * root=createTreeFromBST(originalRoot);
	print_ascii_tree(root);
    make_empty(root);
}



/**
 *  ve funkci main provadime testovani funkcnosti tabulky symbolu
 *
 */
int main()
{
    //lokalni tabulka pro funkce
tTabSym *functionTable;
tTabSymTypes context = TAB_SYM_GLOB;
int errCount = 0;

printf("Vytvarim tabulku symbolu\n");
if((functionTable = tabSymCreate(context)) == NULL) {
	printf("alokace tabulky failnula\n");
	tabSymFree(functionTable);
} else {
	printf("Test 0: Ocekavana hodnota korene: NULL\t | ocekavany typ tabulky: TAB_SYMLOC\n");
	tBSTNodePtr pom = functionTable->root;
	if(pom == NULL) {
	printf("Koren je NULL\t | typ tabulky %s: \n", functionTable->tabType == 0?"TAB_SYM_GLOB":"CHYBA");
	} else {
	printf("TEST FAIL\n");
	errCount++;
	}
	printf("---------------------------------");
        }
        printf("1. Do tabulky symbolu vlozim funkci 'pocitej' typu integer\n");
        tTabSymVarNoAutoDataType variableP = TAB_SYM_VAR_NO_AUTO_INTEGER;
               
        bool pIsDefined = true;
        tParamListPtr parametry;
        parametry = initList();
        string param1, param2, param3;
        strInit(&param1);
        strInit(&param2);
        strInit(&param3);
        strAddChar(&param1, 'a');
        strAddChar(&param2, 'b');
        strAddChar(&param3, 'c');
        
       // insertEl(parametry, &param1, variableP);
       // insertEl(parametry, &param2, variableP);
        //insertEl(parametry, &param3, variableP);
        
        //vytvorim si tabsymlist
        tTabSymList* tabslist;
        tabslist = tabSymListCreate();
        //vytvorim novou pasku pro danou funkci
        tInsTape* sextape;
        sextape = insTapeCreate();
        //vlozim funcinfo        
        tFuncInfo *pInfo = tabSymCreateFuncInfo(parametry,TAB_SYM_VAR_NO_AUTO_INTEGER, functionTable, tabslist, sextape, pIsDefined );

        //do string p ulozim hodnotu 'pocitej'
        string p;
        strInit(&p);
        strAddChar(&p, 'p');
        strAddChar(&p, 'o');
        strAddChar(&p, 'c');
        strAddChar(&p, 'i');
        strAddChar(&p, 't');
        strAddChar(&p, 'e');
        strAddChar(&p, 'j');
        //vlozeni funkce pocitej s funcinfo do tabulky funkci 
        int returnValue = tabSymInsertFunc(functionTable, &p, pInfo);

        //nastala chyba pri vkladani zaznamu o promenne
        if(returnValue == 0)
            printf("Chyba pri vkladani informaci\n");
        //uspesne vlozeny zaznam
        else {
            printf("Vypis struktury tabulky funkci\n");
            printBST(functionTable->root);
            
            //zkusim v tabulce vyhledat  pridany prvek
            tTabSymElemData *foundData;
            printf("----------------------------------\n");
            printf("2.Vyhledani pridaneho prvku\n");
            if ((foundData = tabSymSearch(functionTable, &p)) == NULL) {
                printf("Vlozeny prvek nenalezen -> chyba\n");
                errCount++;
            }
            else {
                printf("Ocekavany vystup: %s, %s\n", "TAB_SYM_FUNCTION", "TAB_SYM_VAR_NO_AUTO_INTEGER");
                printf("Skutecny vystup: %s, %s\n", foundData->type == 2?"TAB_SYM_FUNCTION":"CHYBA",
                                                    foundData->info.func->retType == 0? "TAB_SYM_VAR_NO_AUTO_INTEGER" : "CHYBA\n");
            }
            
            //zkusim najit v tabulce element, ktery se v ni nenachazi
            string tmp2;
            strInit(&tmp2);
            strAddChar(&tmp2, 'k');
            
            printf("----------------------------------\n");
            printf("3.Vyhledani neexistujiciho prvku\n");
            if ((foundData = tabSymSearch(functionTable, &tmp2)) == NULL){
                printf("Prvek nenalezen -> spravne\n");
            } else {
                printf("Prvek nalezen -> chyba\n");
                errCount++;
            }
            printf("----------------------------------\n");
            printf("Zruseni globalni tabulky funkci\n");
            //sigseg -> tabSymFree->BSTFREE -> strFree..
            tabSymFree(functionTable);
            
            //uvolneni vsech stringu
            strFree(&p);
            strFree(&param1);
            strFree(&param2);
            strFree(&param3);
            strFree(&tmp2);        
        }
        return 0;
}
