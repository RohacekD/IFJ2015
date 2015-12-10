#include "interpret.h"

int executeTape(tInsTapeInsPtr ins) {
	/* Prottoze tato fce je volana rekurzivne je nutno si frameStack drzet
	 * v static
	 */
	static tStack* frameStack = NULL;

	tInsTapeInsPtr instruction;
	instruction = ins;

	ERR_CODES ret;

	//printTape(ins);

	/*Prvotni inicializace stacku*/
	if (!frameStack) {
		if (!(frameStack = (tStack *)malloc(sizeof(tStack)))) {
			return ERR_ALLOC;
		}
		SInit(frameStack);
	}
	tVariablePtr oper1;
	while (instruction && instruction->type!=I_RETURN) {
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
			ret = executeIns(&instruction, frameStack);
			if (ret != ERR_OK) {
				if(frameStack)
					SDispose(frameStack);
				free(frameStack);
				frameStack = NULL;
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
		pushNewFrame(stack, true);
		tab = (tTabSym*)ins->adr1;
		tTabSymToFrame(tab->root, &stack->Top->frameContainer, false);
		break;
		//volani fce
	case I_CF:
        oper1 = NULL;
		dest = NULL;
		bool isMain = (ins->adr3 == NULL);
		if (!isMain) {//nejsme v main
			//najdeme kam budeme vracet navratovou hodnotu
			findVariable(stack, (string*)ins->adr3, &dest);
		}
		
		pushNewFrame(stack, false);
		tab = (tTabSym*)ins->adr1;
		insToCall = (tInsTapeInsPtr)ins->adr2;
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
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariableToDeclare(stack, (string*)ins->adr3, &dest);
		dest->deklared = true;
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
		deleteTopFrame(stack);
		break;
	case I_SORT:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING || dest->type != VAR_TYPE_STRING)
			return ERR_SEM_COM;
		strCopyString(&dest->data.stringVal, &oper1->data.stringVal);
		heapSort(dest->data.stringVal.str);
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_FIND:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;

		if (oper1->type != VAR_TYPE_STRING ||
			oper2->type != VAR_TYPE_STRING ||
			dest->type != VAR_TYPE_INT) {
			return ERR_SEM_COM;
		}
		dest->data.intVal = find(oper1->data.stringVal.str, oper2->data.stringVal.str);
		if (dest->data.intVal == oper1->data.stringVal.length) {
			dest->data.intVal = -1;
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_CONCAT:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type != VAR_TYPE_STRING){
			return ERR_SEM_COM;
		}
		else {
			strFree(&dest->data.stringVal);
			dest->data.stringVal = concat(oper1->data.stringVal, oper2->data.stringVal);
		}
		dest->init = true;//dest je nyni inicializovan
		break;
	case I_SUBSTR://tato instrukce cte i nasledujici ktera musi byt typu I_SUBSTR_DEST
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &oper3);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper2->init) return ERR_RUNTIME_INIT_VAR;
		if (!oper3->init) return ERR_RUNTIME_INIT_VAR;
		*instruction = ins->rptr;
		ins = *instruction;
		if (ins->type != I_SUBSTR_DEST) {
			return ERR_RUNTIME_REST;
		}
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type != VAR_TYPE_STRING || oper1->type != VAR_TYPE_STRING ||
			oper2->type == VAR_TYPE_STRING || oper3->type == VAR_TYPE_STRING) {
			return ERR_RUNTIME_REST;
		}
                
                if(oper2->type == VAR_TYPE_INT) tmp1 = oper2->data.intVal;
                if(oper3->type == VAR_TYPE_INT) tmp2 = oper3->data.intVal;                 
                if(oper2->type == VAR_TYPE_DOUBLE) tmp1 = (int)floor(oper2->data.doubleVal);
                if(oper3->type == VAR_TYPE_DOUBLE) tmp2 = (int)floor(oper3->data.doubleVal);
                if(oper2->type == VAR_TYPE_BOOL) tmp1 = oper2->data.doubleVal;
                if(oper3->type == VAR_TYPE_BOOL) tmp2 = oper3->data.doubleVal;
                
		strFree(&dest->data.stringVal);
		dest->data.stringVal = substr(oper1->data.stringVal, tmp1, tmp2,&retErr);
		dest->init = true;//dest je nyni inicializovan
                
                if(retErr) return ERR_RUNTIME_REST;

		break;
	case I_LENGTH:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (!oper1->init) return ERR_RUNTIME_INIT_VAR;
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
	while (istruction->rptr->type == I_SP) {
		istruction = istruction->rptr;
		findVariableInSubFrame(stack, (string*)istruction->adr1, &oper1);
		findVariable(stack, (string*)istruction->adr3, &dest);
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
		if (((tTabSymElemData*)node->data)->type == TAB_SYM_VARIABLE){
			type = tTabSymToVarNotatation(((tTabSymElemData*)node->data)->info.var->dataType);
			variableCreate(&var, type);
		}
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
		var->deklared = isFunction;
		if (isdigit((int)node->key->str[0])) {
			var->deklared = true;
		}
		insertNewVariable(frameContainer, var,node->key);
		tTabSymToFrame(node->l, frameContainer, isFunction);
		tTabSymToFrame(node->r, frameContainer, isFunction);
	}
}