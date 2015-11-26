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
		DLInitList(frameStack);
	}
}


int executeIns(tInsTapeInsPtr ins, tStack* stack) {
	tVariable oper1;
	tVariable oper2;
	tVariable dest;

	switch (ins->type)
	{
	case I_PLUS:
		findVariable(stack, (string*)ins->adr1, &oper1);
		findVariable(stack, (string*)ins->adr2, &oper2);
		findVariable(stack, (string*)ins->adr3, &dest);
		((oper1.type == VAR_TYPE_INT) ?
			(oper1.data.intVal) :
			(oper1.type == VAR_TYPE_BOOL) ?
			(oper1.data.boolVal) :
			(oper1.data.doubleVal)) = 1;
		break;
	case I_CBF:
		pushNewFrame(stack, true);
		break;
		//volani fce
	case I_CF:
		pushNewFrame(stack, false);
		tTabSym* tab = (tTabSym*)ins->adr1;
		break;
	case I_DBF:
		deleteTopFrame(stack);
		break;
	default:
		break;
	}
	return 1;
}