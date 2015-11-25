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
	int element;
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

Tree *find(int elem, Tree *t) {
	if (t == NULL) {
		return NULL;
	}

	if (elem < t->element) {
		return find(elem, t->left);
	} else if (elem > t->element) {
		return find(elem, t->right);
	} else {
		return t;
	}
}


//Insert i into the tree t, duplicate will be discarded
//Return a pointer to the resulting tree.
Tree * insert(int value, Tree * t) {
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

	if (value < t->element) {
		t->left = insert(value, t->left);
	} else if (value > t->element) {
		t->right = insert(value, t->right);
	} else {
		//duplicate, ignore it
		return t;
	}
	return t;
}

Tree * delete(int value, Tree * t) {
	//Deletes node from the tree
	// Return a pointer to the resulting tree
	//Tree * x;
	Tree *tmp_cell;

	if (t == NULL)
		return NULL;

	if (value < t->element) {
		t->left = delete(value, t->left);
	} else if (value > t->element) {
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

	sprintf(node->label, "%d", t->element);
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
	root->element=atoi(originalRoot->key->str);

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
    tTabSym *symbolTable, *globalSymbolTable;
    //lokalni tabulka symbolu - pro promenne a konstanty
    tTabSymTypes context = TAB_SYM_LOC;
    int errCount = 0;

    printf("Vytvoreni tabulky symbolu\n");
    if((symbolTable = tabSymCreate(context)) == NULL)
        printf("Alokace tabulky se nezdarila\n");
    //alokace se zdarila
    else {
        printf("Test 0: Ocekavana hodnota korene: NULL\t | Ocekavany typ tabulky symbolu: TAB_SYM_LOC\n");
        tBSTNodePtr tmp = symbolTable->root;
        if (tmp == NULL)
            printf("\tVracena hodnota korene: NULL\t | Vracena hodnota typu tabulky %s:\n", symbolTable->tabType == 1?"TAB_SYM_LOC":"CHYBA");
        else {
            printf("\tVracena hodnota korene neni null -> chyba\n");
            errCount++;
        }
        printf("----------------------------------\n");

        //mam vytvorenou globalni tabulku symbolu
        printf("1. Do tabulky symbolu vlozim promennou 'p' typu integer\n");

        //informace o dane promenne
        //tTabSymVarDataType variableP = TAB_SYM_VAR_INTEGER;
        //bool pIsDefined = false;
        tVariableInfo pInfo = {TAB_SYM_VAR_INTEGER};

        //do string p ulozim hodnotu 'pom'
        string p;
        strInit(&p);
        strAddChar(&p, 'p');
        strAddChar(&p, 'o');
        strAddChar(&p, 'm');

        //vlozeni promenne do tabulky symbolu
        int returnValue = tabSymInsertVar(symbolTable, &p, &pInfo);

        //nastala chyba pri vkladani zaznamu o promenne
        if(returnValue == 0)
            printf("Chyba pri vkladani informaci o promenne\n");
        //uspesne vlozeny zaznam
        else {
            printf("Vypis struktury tabulky symbolu\n");
            printBST(symbolTable->root);

            
            //zkusim v tabulce vyhledat  pridany prvek
            tTabSymElemData *foundData;
            printf("----------------------------------\n");
            printf("2.Vyhledani pridaneho prvku\n");
            if ((foundData = tabSymSearch(symbolTable, &p)) == NULL) {
                printf("Vlozeny prvek nenalezen -> chyba\n");
                errCount++;
            }
            else {
                printf("Ocekavany vystup: %s, %s\n", "TAB_SYM_VARIABLE", "TAB_SYM_VAR_INTEGER");
                printf("Skutecny vystup: %s, %s\n", foundData->type == 0?"TAB_SYM_VARIABLE":"CHYBA",
                                                    foundData->info.var->dataType == 0 ? "TAB_SYM_VAR_INTEGER" : "CHYBA\n");
            }
            
            
            //zkusim najit v tabulce element, ktery se v ni nenachazi
            string tmp2;
            strInit(&tmp2);
            strAddChar(&tmp2, 'k');
            
            printf("----------------------------------\n");
            printf("3.Vyhledani neexistujiciho prvku\n");
            if ((foundData = tabSymSearch(symbolTable, &tmp2)) == NULL){
                printf("Prvek nenalezen -> spravne\n");
            }
            else {
                printf("Prvek nalezen -> chyba\n");
                errCount++;
            }
            
            //pridam do tabulky symbolu dalsi elementy
            string doubleV, stringV, boolV, autoV; //promenne
            string konstanta; //konstanta
               
            //vytvoreni novych klicu
            strInit(&doubleV);
            strAddChar(&doubleV, 'a');
            strInit(&stringV);
            strAddChar(&stringV, 'x');
            strAddChar(&stringV, 'a');
            strAddChar(&stringV, 'v');
            strAddChar(&stringV, 'i');
            strInit(&boolV);
            strAddChar(&boolV, 'p');
            strAddChar(&boolV, 'o');
            strAddChar(&boolV, 'p');
            strInit(&autoV);
            strAddChar(&autoV, 'p');
            strAddChar(&autoV, '7');
            strInit(&konstanta);
            strAddChar(&konstanta, '$');
            strAddChar(&konstanta, 'c');
            strAddChar(&konstanta, 'n');
            strAddChar(&konstanta, 't');
            
            
            //vlozim promenne
            tVariableInfo doubleVInfo = {TAB_SYM_VAR_DOUBLE};
            tVariableInfo stringVInfo = {TAB_SYM_VAR_STRING};
            tVariableInfo boolVInfo = {TAB_SYM_VAR_STRING};
            tVariableInfo autoVInfo = {TAB_SYM_VAR_AUTO};
            tConstantInfo konstantaInfo = {TAB_SYM_VAR_INTEGER, {3}};
            
            int retValue1, retValue2, retValue3, retValue4, retValue5;
            
            printf("----------------------------------\n");
            printf("Vlozeni novych prvku\n");
            if(((retValue1 = tabSymInsertVar(symbolTable, &doubleV, &doubleVInfo)) == 0) ||
              ((retValue2 = tabSymInsertVar(symbolTable, &stringV, &stringVInfo)) == 0) ||
              ((retValue3 = tabSymInsertVar(symbolTable, &boolV, &boolVInfo)) == 0) ||
              ((retValue5 = tabSymInsertConst(symbolTable, &konstanta, &konstantaInfo)) == 0) ||
              ((retValue4 =tabSymInsertVar(symbolTable, &autoV, &autoVInfo)) == 0)) {
                  printf("Vlozeni promennych se nepovedlo\n");
              }
            else {
                printf("Vypis struktury tabulky symbolu\n");
                printBST(symbolTable->root);
            }
            
            
            printf("----------------------------------\n");
            printf("4. Vyhledani promenne typu double\n");
            //vyhledani promenne, ktera je double
            if ((foundData = tabSymSearch(symbolTable, &doubleV)) == NULL) {
                printf("Vlozeny prvek nenalezen -> chyba\n");
                errCount++;
            }
            else {
                printf("Ocekavany vystup: %d, %d\n", TAB_SYM_VARIABLE, TAB_SYM_VAR_DOUBLE);
                printf("Skutecny vystup: %d, %d\n", foundData->type, foundData->info.var->dataType);
            }
            
            
            printf("----------------------------------\n");
            printf("5. Vyhledani promenne typu bool\n");
            //vyhledani promenne, ktera je double
            if ((foundData = tabSymSearch(symbolTable, &boolV)) == NULL) {
                printf("Vlozeny prvek nenalezen -> chyba\n");
                errCount++;
            }
            else {
                printf("Ocekavany vystup: %d, %d\n", TAB_SYM_VARIABLE, TAB_SYM_VAR_BOOLEAN);
                printf("Skutecny vystup: %d, %d\n", foundData->type, foundData->info.var->dataType);
            }
            
            
            printf("----------------------------------\n");
            printf("6. Vyhledani promenne typu string\n");
            //vyhledani promenne, ktera je double
            if ((foundData = tabSymSearch(symbolTable, &stringV)) == NULL) {
                printf("Vlozeny prvek nenalezen -> chyba\n");
                errCount++;
            }
            else {
                printf("Ocekavany vystup: %d, %d\n", TAB_SYM_VARIABLE, TAB_SYM_VAR_STRING);
                printf("Skutecny vystup: %d, %d\n", foundData->type, foundData->info.var->dataType);
            }
            
            
            printf("----------------------------------\n");
            printf("7. Vyhledani promenne typu auto\n");
            //vyhledani promenne, ktera je double
            if ((foundData = tabSymSearch(symbolTable, &autoV)) == NULL) {
                printf("Vlozeny prvek nenalezen -> chyba\n");
                errCount++;
            }
            else {
                printf("Ocekavany vystup: %d, %d\n", TAB_SYM_VARIABLE, TAB_SYM_VAR_AUTO);
                printf("Skutecny vystup: %d, %d\n", foundData->type, foundData->info.var->dataType);
            }
            
            
            printf("----------------------------------\n");
            printf("8. Vyhledani konstanty\n");
            //vyhledani promenne, ktera je double
            if ((foundData = tabSymSearch(symbolTable, &konstanta)) == NULL) {
                printf("Vlozeny prvek nenalezen -> chyba\n");
                errCount++;
            }
            else {
                printf("Ocekavany vystup: %d, %d, %d\n", TAB_SYM_CONSTANT, TAB_SYM_VAR_INTEGER, 3);
                printf("Skutecny vystup: %d, %d, %d\n", foundData->type, foundData->info.constant->dataType, foundData->info.constant->value.intVal);
            }
            
            
            printf("----------------------------------\n");
            printf("Vytvoreni globalni tabulky symbolu\n");
            context = TAB_SYM_GLOB;
            if((globalSymbolTable = tabSymCreate(context)) == NULL)
                printf("Alokace tabulky se nezdarila\n");
                //alokace se zdarila
            else {
                printf("Test 9: Ocekavana hodnota korene: NULL\n");
                tBSTNodePtr tmp = globalSymbolTable->root;
                if (tmp == NULL)
                    printf("Vracena hodnota korene: NULL\n");
                else {
                    printf("Vracena hodnota korene neni null -> chyba\n");
                    errCount++;
                }
                 
                       
                 
            }
            
            
            //pridani prvku do globalni tabulky symbolu
            
            
            
            printf("----------------------------------\n");
            printf("Zruseni lokalni tabulky symbolu\n");
            tabSymFree(symbolTable);
            
        }
    }
    return 0;
}
