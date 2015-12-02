#include "interpret.h"

int executeTape(tInsTapeInsPtr ins) {
	/* Prottoze tato fce je volana rekurzivne je nutno si frameStack drzet
	 * v static
	 */
	static tStack* frameStack = NULL;

	tInsTapeInsPtr* instruction = &ins;

	/*Prvotni inicializace stacku*/
	if (!frameStack) {
		if (!(frameStack = (tStack *)malloc(sizeof(tStack)))) {
			FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
		}
		SInit(frameStack);
	}
	while (*instruction) {
		executeIns(instruction, frameStack);

	}
	tFrameContainer frame;
	/* Top vraci 0 pri prazdnem framestacku */
	if (!STop(frameStack, &frame)) {
		/*Pokud je frame stack prazdny tak ho deinicializuji*/
		SDispose(frameStack);//redundantni ale vyhneme se mem. leaku
		free(frameStack);
	}
	return 1;//todo
}


int executeIns(tInsTapeInsPtr* instruction, tStack* stack) {
	if (stack == NULL) return 0;
	tVariablePtr oper1;
	tVariablePtr oper2;
	tVariablePtr dest;

	tTabSym* tab;
	tInsTapeInsPtr ins = *instruction;

	switch (ins->type)
	{
	case I_COUT:
		findVariable(stack, (string*)ins->adr1, &oper1);
		if (oper1->type == VAR_TYPE_INT) {
			printf("%d", oper1->data.intVal);
		}
		else if (oper1->type == VAR_TYPE_DOUBLE) {
			printf("%f", oper1->data.doubleVal);
		}
		else if (oper1->type == VAR_TYPE_BOOL) {
			//todo
		}
		else if (oper1->type == VAR_TYPE_STRING) {
			printf("%s", oper1->data.stringVal.str);
		}
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
			FatalError(9, ERR_MESSAGES[ERR_RUNTIME_ZERO_DIV]);
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
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) == (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) == getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) == getVarVal(oper2);
		}
		break;
	case I_NOTEQUAL:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) != (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) != getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) != getVarVal(oper2);
		}
		break;
	case I_GREATER:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) > (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) > getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) > getVarVal(oper2);
		}
		break;
	case I_GEQUAL:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) >= (int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) >= getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) >= getVarVal(oper2);
		}
		break;
	case I_LESSER:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = (int)getVarVal(oper1) <(int)getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) < getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) < getVarVal(oper2);
		}
		break;
	case I_LEQUAL:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
			if (dest->type == VAR_TYPE_INT) {
				dest->data.intVal = (int)getVarVal(oper1) <= (int)getVarVal(oper2);
			}
			else if (dest->type == VAR_TYPE_DOUBLE) {
				dest->data.doubleVal = getVarVal(oper1) <= getVarVal(oper2);
			}
			else if (dest->type == VAR_TYPE_BOOL) {
				dest->data.boolVal = getVarVal(oper1) <= getVarVal(oper2);
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
		pushNewFrame(stack, false);
		tab = (tTabSym*)ins->adr1;
		tTabSymToFrame(tab->root, &stack->Top->frameContainer);
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
			strCopyString(&oper1->data.stringVal, &dest->data.stringVal);
		}
		break;
	case I_DBF:
		deleteTopFrame(stack);
		break;
	case I_RETURN://todo
		deleteFunctionsFrames(stack);
		break;
	case I_IFZERO:
		//todo
		break;
	default:
		break;
	}
	*instruction = ins->rptr;
	return 1;
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