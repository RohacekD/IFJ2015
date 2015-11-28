#include "interpret.h"



int executeTape(tInsTapeInsPtr ins) {
	/* Prottoze tato fce je volana rekurzivne je nutno si frameStack drzet
	 * v static
	 */
	static tStack* frameStack = NULL;
	/*Prvotni inicializace stacku*/
	if (!frameStack) {
		if ((tStack *)malloc(sizeof(tStack))) {
			FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
		}
		SInit(frameStack);
	}
}


int executeIns(tInsTapeInsPtr ins, tStack* stack) {
	tVariablePtr oper1;
	tVariablePtr oper2;
	tVariablePtr dest;

	switch (ins->type)
	{
	case I_PLUS:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		if (dest->type == VAR_TYPE_INT) {
			dest->data.intVal = getVarVal(oper1) + getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_DOUBLE) {
			dest->data.doubleVal = getVarVal(oper1) + getVarVal(oper2);
		}
		else if (dest->type == VAR_TYPE_BOOL) {
			dest->data.boolVal = getVarVal(oper1) + getVarVal(oper2);
		}
		break;
	case I_CBF:
		pushNewFrame(stack, true);
		break;
		//volani fce
	case I_CF:
		/*pushNewFrame(stack, false);
		tTabSym* tab = (tTabSym*)ins->adr1;*/
		break;
	case I_DBF:
		deleteTopFrame(stack);
		break;
	default:
		break;
	}
	return 1;
}