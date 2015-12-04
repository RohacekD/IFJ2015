#include "interpret.h"

int executeTape(tInsTapeInsPtr ins) {
	/* Prottoze tato fce je volana rekurzivne je nutno si frameStack drzet
	 * v static
	 */
	static tStack* frameStack = NULL;

	tInsTapeInsPtr* instruction = &ins;

	ERR_CODES ret;

	/*Prvotni inicializace stacku*/
	if (!frameStack) {
		if (!(frameStack = (tStack *)malloc(sizeof(tStack)))) {
			return ERR_ALLOC;
		}
		SInit(frameStack);
	}
	tVariablePtr oper1;
	while (*instruction && (*instruction)->type!=I_RETURN) {
		if ((*instruction)->type == I_IFZERO) {
			findVariable(frameStack, (string*)ins->adr1, &oper1);
			if (getVarVal(oper1) == 0) {
				instruction = (tInsTapeInsPtr*)ins->adr2;
			}
			else {
				instruction = (&(*instruction)->rptr);
			}
		}
		else if ((*instruction)->type == I_IFNZERO) {
			findVariable(frameStack, (string*)ins->adr1, &oper1);
			if (getVarVal(oper1) != 0) {
				instruction = (tInsTapeInsPtr*)ins->adr2;
			}
			else {
				instruction = (&(*instruction)->rptr);
			}
		}
		else if ((*instruction)->type == I_GOTO) {
			if (ins->adr1 == NULL) {
				instruction = (tInsTapeInsPtr*)ins->adr1;
			}
		}
		else {
			ret = executeIns(instruction, frameStack);
			if (ret != ERR_OK) {
				if(frameStack)
					SDispose(frameStack);
				free(frameStack);
				frameStack = NULL;
				return ret;
			}
		}
	}

	if (*instruction==NULL) {//paska dosla na konec a nenarazil jsem na I_RETURN
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

	tTabSym* tab;
	tInsTapeInsPtr ins = *instruction;
	tInsTapeInsPtr insToCall;
	switch (ins->type)
	{
	case I_CIN:
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			
		}
		else if (dest->type == VAR_TYPE_STRING) {
			strFree(&dest->data.stringVal);
			strInit(&dest->data.stringVal);
			int c = getchar();
			if (isspace(c)) {
				while (isspace(c))
					c = getchar();
			}
			if (c != EOF) {
				while (!isspace(c) && c != EOF) {
					if (strAddChar(&dest->data.stringVal, c)) {
						return ERR_ALLOC;
					}
				}
			}
		}
		break;
	case I_COUT:
		findVariable(stack, (string*)ins->adr1, &oper1);
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
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) + (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) + getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) + getVarVal(oper2);
		}
		break;
	case I_MINUS:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) - (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) - getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) - getVarVal(oper2);
		}
		break;
	case I_DIV:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
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
		break;
	case I_MUL:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) * (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) * getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) * getVarVal(oper2);
		}
		break;
	case I_EQUAL:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = (int)getVarVal(oper1) == (int)getVarVal(oper2);
			}
			else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) == getVarVal(oper2);
			}
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
				else if (dest->type == VAR_TYPE_DOUBLE) {
					dest->data.doubleVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) == 0) ? 1: 0;
				}
				else if (dest->type == VAR_TYPE_BOOL) {
					dest->data.boolVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) == 0) ? true : false;
				}
			}
		}
		break;
	case I_NOTEQUAL:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = (int)getVarVal(oper1) == (int)getVarVal(oper2);
			}
			else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) == getVarVal(oper2);
			}
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
					dest->data.intVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)!=0) ? 1 : 0;
				}
				else if (dest->type == VAR_TYPE_DOUBLE) {
					dest->data.doubleVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) != 0) ? 1 : 0;
				}
				else if (dest->type == VAR_TYPE_BOOL) {
					dest->data.boolVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) != 0) ? true : false;
				}
			}
		}
		break;
	case I_GREATER:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = (int)getVarVal(oper1) > (int)getVarVal(oper2);
			}
			else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) > getVarVal(oper2);
			}
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
				else if (dest->type == VAR_TYPE_DOUBLE) {
					dest->data.doubleVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)>0) ? 1: 0;
				}
				else if (dest->type == VAR_TYPE_BOOL) {
					dest->data.boolVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)>0) ? true: false;
				}
			}
		}
		break;
	case I_GEQUAL:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = (int)getVarVal(oper1) >= (int)getVarVal(oper2);
			}
			else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) >= getVarVal(oper2);
			}
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
				else if (dest->type == VAR_TYPE_DOUBLE) {
					dest->data.doubleVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) >= 0) ? 1 : 0;
				}
				else if (dest->type == VAR_TYPE_BOOL) {
					dest->data.boolVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) >= 0) ? true : false;
				}
			}
		}
		break;
	case I_LESSER:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = (int)getVarVal(oper1) < (int)getVarVal(oper2);
			}
			else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) < getVarVal(oper2);
			}
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
				else if (dest->type == VAR_TYPE_DOUBLE) {
					dest->data.doubleVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)<0) ? 1 : 0;
				}
				else if (dest->type == VAR_TYPE_BOOL) {
					dest->data.boolVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal)<0) ? true : false;
				}
			}
		}
		break;
	case I_LEQUAL:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (oper1->type != VAR_TYPE_STRING && oper2->type != VAR_TYPE_STRING) {
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = (int)getVarVal(oper1) <= (int)getVarVal(oper2);
			}
			else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) <= getVarVal(oper2);
			}
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
				else if (dest->type == VAR_TYPE_DOUBLE) {
					dest->data.doubleVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) <= 0) ? 1 : 0;
				}
				else if (dest->type == VAR_TYPE_BOOL) {
					dest->data.boolVal = (strCmpString(&oper1->data.stringVal, &oper2->data.stringVal) <= 0) ? true : false;
				}
			}
		}
		break;
	case I_UMINUS:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = -(int)getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = -getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = -getVarVal(oper1);
		}
		break;
	case I_INC:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1)+1;
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1)+1;
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1)+1;
		}
		break;
	case I_DEC:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1)-1;
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1)-1;
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1)-1;
		}
		break;
	case I_LOG_NOT:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = !(int)getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = !getVarVal(oper1);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = !getVarVal(oper1);
		}
		break;
	case I_LOG_AND:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) && (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) && getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) && getVarVal(oper2);
		}
		break;
	case I_LOG_OR:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) || (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) || getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) || getVarVal(oper2);
		}
		break;
		//vytoreni blokoveho ramce
	case I_CBF:
		pushNewFrame(stack, true);
		tab = (tTabSym*)ins->adr1;
		tTabSymToFrame(tab->root, &stack->Top->frameContainer);
		break;
		//volani fce
	case I_CF:
		if (ins->adr3 != NULL) {//nejsme v main
			findVariable(stack, (string*)ins->adr3, &dest);
		}
		oper1 = NULL;
		dest = NULL;
		pushNewFrame(stack, false);
		tab = (tTabSym*)ins->adr1;
		insToCall = (tInsTapeInsPtr)ins->adr2;
		tTabSymToFrame(tab->root, &stack->Top->frameContainer);
		retErr = executeTape(insToCall);
		if (retErr != ERR_OK)
			return retErr;
		//string pro nalezeni promenne pro vraceni
		if (ins->adr3 != NULL) {//nejsme v main
			string ret;
			strInit(&ret);
			strConConstString(&ret, "$ret");
			findVariable(stack, &ret, &oper1);
			strFree(&ret);
		}

		deleteFunctionsFrames(stack);

		if (ins->adr3 != NULL) {//nejsme v main
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
				strCopyString(&oper1->data.stringVal, &dest->data.stringVal);
			}
		}
		break;
	case I_ASSIGN:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
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
			strCopyString(&dest->data.stringVal,&oper1->data.stringVal);
		}
		break;
	case I_SP:

		break;
	case I_DBF:
		deleteTopFrame(stack);
		break;
	case I_SORT:
		findVariable(stack, (string*)ins->adr1, &oper1);
		if (oper1->type != VAR_TYPE_STRING)
			return ERR_SEM_COM;
		heapSort(oper1->data.stringVal.str);
		break;
	case I_FIND:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);

		if (oper1->type != VAR_TYPE_STRING ||
			oper2->type != VAR_TYPE_STRING ||
			dest->type != VAR_TYPE_INT) {
			return ERR_SEM_COM;
		}
		dest->data.intVal = find(oper1->data.stringVal.str, oper2->data.stringVal.str);
		if (dest->data.intVal > oper1->data.stringVal.length) {
			dest->data.intVal = -1;
		}
		break;
	case I_CONCAT:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (oper1->type != VAR_TYPE_STRING || oper2->type != VAR_TYPE_STRING || dest->type != VAR_TYPE_STRING){
			return ERR_SEM_COM;
		}
		else {
			strFree(&dest->data.stringVal);
			dest->data.stringVal = concat(oper1->data.stringVal, oper2->data.stringVal);
		}
		break;
	case I_SUBSTR://tato instrukce cte i nasledujici ktera musi byt typu I_SUBSTR_DEST
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &oper3);
		*instruction = ins->rptr;
		ins = *instruction;
		if (ins->type != I_SUBSTR_DEST) {
			return ERR_RUNTIME_REST;
		}
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type != VAR_TYPE_STRING || oper1->type != VAR_TYPE_STRING ||
			oper2->type != VAR_TYPE_INT || oper3->type != VAR_TYPE_INT) {
			return ERR_RUNTIME_REST;
		}
		string resultSubstr;
		resultSubstr = substr(oper1->data.stringVal, oper2->data.intVal, oper3->data.intVal);
		strFree(&dest->data.stringVal);
		dest->data.stringVal = resultSubstr;
		strFree(&resultSubstr);
		break;
	case I_LENGTH:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT && oper1->type==VAR_TYPE_STRING) {
			dest->data.intVal = length(oper1->data.stringVal);
		}
		else if (dest->type == VAR_TYPE_DOUBLE && oper1->type == VAR_TYPE_STRING) {
			dest->data.doubleVal = length(oper1->data.stringVal);
		}
		else {
			return ERR_SEM_COM;
		}
		break;
        case I_LABEL://jen navesti nic nedelej
            break;
	default:
		break;
	}
	*instruction = ins->rptr;
	return ERR_OK;
}

/*
 * Projde cely strom pomoci preorder
 */
void tTabSymToFrame(tBSTNodePtr node, tFrameContainer* frameContainer) {
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
		insertNewVariable(frameContainer, var,node->key);
		tTabSymToFrame(node->l, frameContainer);
		tTabSymToFrame(node->r, frameContainer);
	}
}