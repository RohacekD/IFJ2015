#include "interpret.h"

int executeTape(tInsTapeInsPtr ins) {
	/* Prottoze tato fce je volana rekurzivne je nutno si frameStack drzet
	 * v static
	 */
	static tStack* frameStack = NULL;

	tInsTapeInsPtr instruction;
	instruction = ins;

	//pro navratove kody executeIns
	ERR_CODES ret;

	//toto vytiskne pasku kazde fce na stdout
	printTape(ins);

	/*Prvotni inicializace stacku*/
	if (!frameStack) {
		if (!(frameStack = (tStack *)malloc(sizeof(tStack)))) {
			return ERR_ALLOC;
		}
		SInit(frameStack);
	}
	//pro skokove instrukce
	tVariablePtr oper1;
	while (instruction && instruction->type!=I_RETURN) {
		//zpracovani skokovych instrukci
		if ((instruction)->type == I_IFZERO) {
			findVariable(frameStack, (string*)(instruction)->adr1, &oper1);
			if (getVarVal(oper1) == 0) {
				instruction = (tInsTapeInsPtr)(instruction->adr2);
			}
			else {
				instruction = ((instruction)->rptr);
			}
		}
		else if ((instruction)->type == I_IFNZERO) {
			findVariable(frameStack, (string*)(instruction)->adr1, &oper1);
			if (getVarVal(oper1) != 0) {
				instruction = (tInsTapeInsPtr)(instruction->adr2);
			}
			else {
				instruction = ((instruction)->rptr);
			}
		}
		else if ((instruction)->type == I_GOTO) {
			if ((instruction)->adr1 != NULL) {
				instruction = (tInsTapeInsPtr)(instruction->adr1);
			}
		}
		else {
			//zprcovani ostatnich instrukci
			//kontrola navratoveho kodu
			ret = executeIns(&instruction, frameStack);
			if (ret != ERR_OK) {
				//vypise "callstack" funkci od te kde nastala chyba
				//zpet po main
				//printTape(ins);
				//pokud nastala chyba, prvni uroven rekurze smaze frame stack
				//zde by slo udelat stack dump
				if(frameStack)
					SDispose(frameStack);
				free(frameStack);
				frameStack = NULL;
				//frame stack vycisten
				return ret;
			}
		}
	}

	if (instruction==NULL) {//paska dosla na konec a nenarazil jsem na I_RETURN
		return ERR_RUNTIME_INIT_VAR;
	}


	tFrameContainer frame;
	/* Top vraci 0 pri prazdnem framestacku */
	if (!STop(frameStack, &frame)) {
		/*Pokud je frame stack prazdny tak ho deinicializuji*/
		SDispose(frameStack);//redundantni ale vyhneme se mem. leaku
		free(frameStack);
	}
	return ERR_OK;
}


int executeIns(tInsTapeInsPtr* instruction, tStack* stack) {
	if (stack == NULL) return 0;
	tVariablePtr oper1;
	tVariablePtr oper2;
	tVariablePtr oper3;
	tVariablePtr dest;
	ERR_CODES retErr;//pro pripad volani jine pasky
        
    int tmp1,tmp2;
	tTabSym* tab;
	tInsTapeInsPtr ins = *instruction;
	tInsTapeInsPtr insToCall;
	switch (ins->type)
	{
	//standardni vstup se rozhoduje o typu vstupu dle 
	//typu promenne na adrese ins->adr3
	case I_CIN:
		findVariable(stack, (string*)ins->adr3, &dest);
		string strInput;
		strInit(&strInput);
		int c = getchar();
		if (isspace(c)) {
			while (isspace(c))
                        c = getchar();
		}
                if (c != EOF) {
                    while (!isspace(c) && c != EOF) {                                        
                            if (strAddChar(&strInput, c)) {
                                    strFree(&strInput);
                                    return ERR_ALLOC;
                            }
                                    c = getchar();
				}
		}
		if (dest->type == VAR_TYPE_INT) {
			if (!strToInt(&strInput, &(dest->data.intVal))) {
				strFree(&strInput);
				return ERR_RUNTIME_INPUT;
			}
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {                        
			if (!strToDouble(&strInput, &(dest->data.doubleVal))) {
				strFree(&strInput);
				return ERR_RUNTIME_INPUT;
			}
		}
		else if (dest->type == VAR_TYPE_BOOL) {
                        int tmp;
			if (!strCmpConstStr(&strInput, "true")) {
				dest->data.boolVal = true;
			}
			else if (!strCmpConstStr(&strInput, "false")) {
				dest->data.boolVal = false;
			}
                        else if (strToInt(&strInput, &tmp) && strCmpConstStr(&strInput, "")){
                               dest->data.boolVal = (tmp == 0) ? false : true; 
                        }
			else {
				strFree(&strInput);
				return ERR_RUNTIME_INPUT; 
			}
		}
		else if (dest->type == VAR_TYPE_STRING) {
			strFree(&dest->data.stringVal);
			strInit(&dest->data.stringVal);
			strCopyString(&dest->data.stringVal,&strInput);
		}
		strFree(&strInput);
                dest->init = true;
		break;
	case I_COUT:
		//standardni vystup vytiskne ins->adr1
		findVariable(stack, (string*)ins->adr1, &oper1);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type == VAR_TYPE_INT) {
			printf("%d", oper1->data.intVal);
		}
		else if (oper1->type == VAR_TYPE_DOUBLE) {
			printf("%g", oper1->data.doubleVal);
		}
		else if (oper1->type == VAR_TYPE_BOOL) {
			printf("%d", oper1->data.boolVal ? 1 : 0);//takto se chova cout c++11
		}
		else if (oper1->type == VAR_TYPE_STRING) {
			printf("%s", oper1->data.stringVal.str);
		}
		fflush(stdout);
		break;
	case I_PLUS:
		//scitani dvou promennych
		//string nelze scitat proto nesmi zadna z adres byt tohoto typu
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_STRING ||
			oper1->type == VAR_TYPE_STRING ||
			oper2->type == VAR_TYPE_STRING)
			return ERR_SEM_COM;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) + (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) + getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) + getVarVal(oper2);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_MINUS:
		//odecitani dvou promennych
		//adr3 = adr1-adr2
		//string nelze odcitat proto nesmi zadna z adres byt tohoto typu
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_STRING ||
			oper1->type == VAR_TYPE_STRING ||
			oper2->type == VAR_TYPE_STRING)
			return ERR_SEM_COM;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) - (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) - getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) - getVarVal(oper2);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_DIV:
		//deleni dvou promennych
		//adr3 = adr1/adr2
		//pokud je adr2==0 vrati ERR_RUNTIME_ZERO_DIV
		//string nelze delit proto nesmi zadna z adres byt tohoto typu
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_STRING ||
			oper1->type == VAR_TYPE_STRING ||
			oper2->type == VAR_TYPE_STRING)
			return ERR_SEM_COM;
		if (getVarVal(oper2)==0) {
			return ERR_RUNTIME_ZERO_DIV;
		}
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) / (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) / getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) / getVarVal(oper2);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_MUL:
		//nasobeni dvou promennych
		//adr3 = adr1*adr2
		//string nelze nasobit proto nesmi zadna z adres byt tohoto typu
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_STRING ||
			oper1->type == VAR_TYPE_STRING ||
			oper2->type == VAR_TYPE_STRING)
			return ERR_SEM_COM;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) * (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) * getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) * getVarVal(oper2);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_EQUAL:
		//porovnani dvou promennych
		//adr3 = adr1==adr2
		//lze porovnat int==int, double==double, int==double(vice versa)
		//string==string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = getVarVal(oper1) == getVarVal(oper2);
			}
			/*else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) == getVarVal(oper2);
			}*/
			else if (dest->type == VAR_TYPE_BOOL) {
				dest->data.boolVal = getVarVal(oper1) == getVarVal(oper2);
			}
		}
		else {
			if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			else {
				if (dest->type == VAR_TYPE_INT) {
					dest->data.intVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)==0) ? 1: 0;
				}
				else {
					return ERR_INTERNAL;
				}
			}
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_NOTEQUAL:
		//nonekvivalence dvou promennych
		//adr3 = adr1!=adr2
		//lze porovnat int!=int, double!=double, int!=double(vice versa)
		//string!=string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = getVarVal(oper1) != getVarVal(oper2);
			}
			/*else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) != getVarVal(oper2);
			}*/
			else if (dest->type == VAR_TYPE_BOOL) {
				dest->data.boolVal = getVarVal(oper1) != getVarVal(oper2);
			}
		}
		else {
			if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			else {
				if (dest->type == VAR_TYPE_INT) {
					dest->data.intVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)!=0) ? 1 : 0;
				}
				else {
					return ERR_INTERNAL;
				}
			}
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_GREATER:
		//porovnani dvou promennych
		//adr3 = adr1>adr2
		//lze porovnat int>int, double>double, int>double(vice versa)
		//string>string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = getVarVal(oper1) > getVarVal(oper2);
			}
			/*else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) > getVarVal(oper2);
			}*/
			else if (dest->type == VAR_TYPE_BOOL) {
				dest->data.boolVal = getVarVal(oper1) > getVarVal(oper2);
			}
		}
		else {
			if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			else {
				if (dest->type == VAR_TYPE_INT) {
					dest->data.intVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)>0) ? 1: 0;
				}
				else {
					return ERR_INTERNAL;
				}
			}
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_GEQUAL:
		//porovnani dvou promennych
		//adr3 = adr1>=adr2
		//lze porovnat int>=int, double>=double, int>=double(vice versa)
		//string>=string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = getVarVal(oper1) >= getVarVal(oper2);
			}
			/*else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) >= getVarVal(oper2);
			}*/
			else if (dest->type == VAR_TYPE_BOOL) {
				dest->data.boolVal = getVarVal(oper1) >= getVarVal(oper2);
			}
		}
		else {
			if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			else {
				if (dest->type == VAR_TYPE_INT) {
					dest->data.intVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) >= 0) ? 1 : 0;
				}
				else {
					return ERR_INTERNAL;
				}
			}
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_LESSER:
		//porovnani dvou promennych
		//adr3 = adr1<adr2
		//lze porovnat int<int, double<double, int<double(vice versa)
		//string<string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = getVarVal(oper1) < getVarVal(oper2);
			}
			/*else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) < getVarVal(oper2);
			}*/
			else if (dest->type == VAR_TYPE_BOOL) {
				dest->data.boolVal = getVarVal(oper1) < getVarVal(oper2);
			}
		}
		else {
			if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			else {
				if (dest->type == VAR_TYPE_INT) {
					dest->data.intVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)<0) ? 1 : 0;
				}
				else {
					return ERR_INTERNAL;
				}
			}
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_LEQUAL:
		//porovnani dvou promennych
		//adr3 = adr1<=adr2
		//lze porovnat int<=int, double<=double, int<=double(vice versa)
		//string<=string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = getVarVal(oper1) <= getVarVal(oper2);
			}
			/*else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) <= getVarVal(oper2);
			}*/
			else if (dest->type == VAR_TYPE_BOOL) {
				dest->data.boolVal = getVarVal(oper1) <= getVarVal(oper2);
			}
		}
		else {
			if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			else {
				if (dest->type == VAR_TYPE_INT) {
					dest->data.intVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) <= 0) ? 1 : 0;
				}
				else {
					return ERR_INTERNAL;
				}
			}
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_UMINUS:
		//vynasobi adr1*(-1)
		//adr3 = -1*adr1
		//nelze vyoperovat string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = -(int)getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = -getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = -getVarVal(oper1);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_INC:
		//inkremetntuje adr1
		//adr3 = adr1 +1
		//nelze pouzit string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1)+1;
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1)+1;
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1)+1;
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_DEC:
		//dekrementuje adr1
		//adr3 = adr1 +1
		//nelze pouzit string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_STRING ||
			oper1->type == VAR_TYPE_STRING)
			return ERR_SEM_COM;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1)-1;
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1)-1;
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1)-1;
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_LOG_NOT:
		//aplikuje logicky not na operand adr1
		//melo by byt pouzito na nic jineho nez bool?
		//adr3 = !adr1
		//nelze pouzit string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_STRING ||
			oper1->type == VAR_TYPE_STRING)
			return ERR_SEM_COM;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = !(int)getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = !getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = !getVarVal(oper1);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_LOG_AND:
		//aplikuje logicky and na operand adr1
		//adr3 = adr1 && adr2
		//nelze pouzit string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_STRING ||
			oper1->type == VAR_TYPE_STRING ||
			oper2->type == VAR_TYPE_STRING)
			return ERR_SEM_COM;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) && (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) && getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) && getVarVal(oper2);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_LOG_OR:
		//aplikuje logicky or na operand adr1
		//adr3 = adr1 || adr2
		//nelze pouzit string
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_STRING ||
			oper1->type == VAR_TYPE_STRING ||
			oper2->type == VAR_TYPE_STRING)
			return ERR_SEM_COM;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) || (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) || getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) || getVarVal(oper2);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
		//vytoreni blokoveho ramce
	case I_CBF:
		//vytvori na zasobnik ramcu novy blokovy ramec
		pushNewFrame(stack, true);
		tab = (tTabSym*)ins->adr1;
		tTabSymToFrame(tab->root, &stack->Top->frameContainer, false);
		break;
		//volani fce
	case I_CF:
		//volani fce
        oper1 = NULL;
		dest = NULL;
		//urcuje zda funkce je main
		//diky tomu ze IFJ2015 nevraci navratovy kod fce 
		//neni nutno nekam return main nekam davat
		bool isMain = (ins->adr3 == NULL);
		if (!isMain) {//nejsme v main
			//najdeme kam budeme vracet navratovou hodnotu
			findVariable(stack, (string*)ins->adr3, &dest);
		}
		
		//vytvori novy nepruchozi ramec
		pushNewFrame(stack, false);
		tab = (tTabSym*)ins->adr1;
		insToCall = (tInsTapeInsPtr)ins->adr2;
		//prekopiruje lokalni tabulku symbolu na ramec fce
		tTabSymToFrame(tab->root, &stack->Top->frameContainer, true);
		//do ramce volane fce nastavime parametry a zavolame fci, pokud neporjde tak vratime error
		if ((retErr=setParams(&ins, stack)) != ERR_OK || (retErr=executeTape(insToCall)) != ERR_OK)
			return retErr;
		//string pro nalezeni promenne pro vraceni
		if (!isMain) {//nejsme v main
			//tak najdeme navratovou hodnotu
			string ret;
			strInit(&ret);
			strConConstString(&ret, "$ret");
			findVariable(stack, &ret, &oper1);
			strFree(&ret);
			//a ulozime ji do urcene promenne
			if (dest->type == VAR_TYPE_INT) {
				if (oper1->type == VAR_TYPE_STRING) {
					return ERR_SEM_COM;
				}
				dest->data.intVal = (int)getVarVal(oper1);
			}
			else if (dest->type == VAR_TYPE_DOUBLE) {
				if (oper1->type == VAR_TYPE_STRING) {
					return ERR_SEM_COM;
				}
				dest->data.doubleVal = getVarVal(oper1);
			}
			else if (dest->type == VAR_TYPE_BOOL) {
				if (oper1->type == VAR_TYPE_STRING) {
					return ERR_SEM_COM;
				}
				dest->data.boolVal = getVarVal(oper1);
			}
			else if (dest->type == VAR_TYPE_STRING) {
				if (oper1->type != VAR_TYPE_STRING) {
					return ERR_SEM_COM;
				}
				strCopyString(&dest->data.stringVal, &oper1->data.stringVal);
			}
			dest->init = true;//dest je nyni inicializovan
		}
		deleteFunctionsFrames(stack);
		break;
	case I_ASSIGN:
		//priradi hodnotu adr1 do adr3
		//adr3=adr1
		//dochazi k inicializaci adr3
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_INT) {
			if (oper1->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			dest->data.intVal = (int)getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			if (oper1->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			dest->data.doubleVal = getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			if (oper1->type == VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			dest->data.boolVal = getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_STRING) {
			if (oper1->type != VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			strCopyString(&dest->data.stringVal,&oper1->data.stringVal);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_DBF:
		//smaze ramec bloku ze zasobniku
		deleteTopFrame(stack);
		break;
	case I_SORT:
		//vestavena funkce razeni znaku ve stringu
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING || dest->type != VAR_TYPE_STRING)
			return ERR_SEM_COM;
		//zkopirujeme trideny string do ciloveho
		strCopyString(&dest->data.stringVal, &oper1->data.stringVal);
		//az pote ho seradime
		heapSort(dest->data.stringVal.str);
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_FIND:
		//vestavena fce vyhledavani retezce v retezci
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;

		//prisna typova kontrola u fce
		if (oper1->type != VAR_TYPE_STRING ||
			oper2->type != VAR_TYPE_STRING ||
			dest->type != VAR_TYPE_INT) {
			return ERR_SEM_COM;
		}
		//samotne volani fce
		dest->data.intVal = find(oper1->data.stringVal.str, oper2->data.stringVal.str);
		if (dest->data.intVal == oper1->data.stringVal.length) {
			dest->data.intVal = -1;
		}
        //vyjimka pro situaci, kdy jsou oba retezce prazdne
        if(oper1->data.stringVal.length == 0 && oper2->data.stringVal.length == 0) dest->data.intVal = 0;
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_CONCAT:
		//vestavena fce konkatenace dvou retezcu
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		//prisna typova kontrola u fce
		if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type != VAR_TYPE_STRING){
			return ERR_SEM_COM;
		}
		else {
			//strFree protoze fce concat() provadi init => predchazeni mem. leaku
			strFree(&dest->data.stringVal);
			dest->data.stringVal = concat(oper1->data.stringVal, oper2->data.stringVal);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_SUBSTR:
		//vestavena fce pro vybrani substringu
		//tato instrukce cte i nasledujici ktera musi byt typu I_SUBSTR_DEST
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &oper3);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper3->init) return ERR_RUNTIME_INIT_VAR;
		//posuneme se na dalsi instrukci
		*instruction = ins->rptr;
		ins = *instruction;
		//ktera musi byt I_SUBSTR_DEST
		if (ins->type != I_SUBSTR_DEST) {
			return ERR_RUNTIME_REST;
		}
		findVariable(stack, (string*)ins->adr3, &dest);
		//datova kontrola parametru
		if (dest->type != VAR_TYPE_STRING || oper1->type != VAR_TYPE_STRING ||
			oper2->type == VAR_TYPE_STRING || oper3->type == VAR_TYPE_STRING) {
			return ERR_RUNTIME_REST;
		}
        //ulozeni do tempovych promennych + zaokrouhleni dle podminek zadani
        if(oper2->type == VAR_TYPE_INT) tmp1 = oper2->data.intVal;
        if(oper3->type == VAR_TYPE_INT) tmp2 = oper3->data.intVal;                 
        if(oper2->type == VAR_TYPE_DOUBLE) tmp1 = (int)floor(oper2->data.doubleVal);
        if(oper3->type == VAR_TYPE_DOUBLE) tmp2 = (int)floor(oper3->data.doubleVal);
        if(oper2->type == VAR_TYPE_BOOL) tmp1 = oper2->data.boolVal;
        if(oper3->type == VAR_TYPE_BOOL) tmp2 = oper3->data.boolVal;
        
		//substr alokuje vlastni string proto ten nas dealokujeme
		strFree(&dest->data.stringVal);
		dest->data.stringVal = substr(oper1->data.stringVal, tmp1, tmp2,&retErr);
		dest->init = true;//dest je nyni inicializovan
		if (retErr) {
			return ERR_RUNTIME_REST;
		}

		break;
	case I_LENGTH:
		//vestavena fce pro delku retezce
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		//typova kontrola vstupu vs vystupu
		if (dest->type == VAR_TYPE_INT && oper1->type==VAR_TYPE_STRING) {
			dest->data.intVal = length(oper1->data.stringVal);
		}
		else if (dest->type == VAR_TYPE_DOUBLE && oper1->type == VAR_TYPE_STRING) {
			dest->data.doubleVal = length(oper1->data.stringVal);
		}
		else {
			return ERR_SEM_COM;
		}
		dest->init = true;//dest je nyni inicializovan
		break;
    case I_LABEL://jen navesti nic nedelej
            break;
	case I_DECLARE:
		//probiha deklrarace promenne
		findVariableToDeclare(stack, (string*)ins->adr1, &oper1);
		oper1->deklared = true;
		break;
	default:
		break;
	}
	*instruction = ins->rptr;
	return ERR_OK;
}


int setParams(tInsTapeInsPtr* ins, tStack* stack) {
	tVariablePtr oper1;
	tVariablePtr dest;

	tInsTapeInsPtr istruction = *ins;
	//dokud bude nasledujici instrukce I_SP se posouvej paskou
	while (istruction->rptr->type == I_SP) {
		istruction = istruction->rptr;
		//adr1 je z framu volajici fuknce
		findVariableInSubFrame(stack, (string*)istruction->adr1, &oper1);
		//s destinace je v aktualnim funkcnim ramci
		findVariable(stack, (string*)istruction->adr3, &dest);
		//zdrojova promenna musi byt inicializovana
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_STRING) {
			if (oper1->type != VAR_TYPE_STRING) {
				return ERR_SEM_COM;
			}
			strCopyString(&dest->data.stringVal, &oper1->data.stringVal);
		}
		dest->init = true;//dest je nyni inicializovan
		dest->deklared = true;//tudiz je i deklarovana
	}
	*ins = istruction;
	return ERR_OK;
}


/*
 * Projde cely strom pomoci preorder
 */
void tTabSymToFrame(tBSTNodePtr node, tFrameContainer* frameContainer, bool isFunction) {
	if (node) {
		tVariablePtr var;
		tVariableType type;
		//vetev pro promennou, ukladam jen dat. typ
		if (((tTabSymElemData*)node->data)->type == TAB_SYM_VARIABLE){
			type = tTabSymToVarNotatation(((tTabSymElemData*)node->data)->info.var->dataType);
			variableCreate(&var, type);
		}
		//pro konstantu kopirujeme hodnotu
		else if (((tTabSymElemData*)node->data)->type == TAB_SYM_CONSTANT) {
			type = tTabSymToVarNotatation(((tTabSymElemData*)node->data)->info.constant->dataType);
			variableCreate(&var, type);
			var->init = true;//konstanty jsou inicializovane
			switch (var->type)
			{
			case VAR_TYPE_INT:
				var->data.intVal = ((tTabSymElemData*)node->data)->info.constant->value.intVal;
				break;
			case VAR_TYPE_BOOL:
				var->data.boolVal = ((tTabSymElemData*)node->data)->info.constant->value.boolVal;

				break;
			case VAR_TYPE_DOUBLE:
				var->data.doubleVal = ((tTabSymElemData*)node->data)->info.constant->value.doubleVal;

				break;
			case VAR_TYPE_STRING://string - strInit vola jiz fce variableCreate
				strCopyString(&var->data.stringVal, ((tTabSymElemData*)node->data)->info.constant->value.stringVal);
				break;
			default:
				break;
			}
		}
		else {
			return;
		}
		//jestli bylo deklarovano zalezi jestli jde o funkcni ramec nebo pokud 
		//vytvarime temp. promennou
		var->deklared = isFunction;
		if (isdigit((int)node->key->str[0])) {
			var->deklared = true;
		}
		//vytvorenou promennou vlozime na frame
		insertNewVariable(frameContainer, var,node->key);
		//pruchod preorder
		tTabSymToFrame(node->l, frameContainer, isFunction);
		tTabSymToFrame(node->r, frameContainer, isFunction);
	}
}