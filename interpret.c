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
	switch (ins->type)
	{
	case I_CBF:
		pushNewFrame(stack, true);
		break;
	case I_CFF:
		pushNewFrame(stack, false);
		break;
	case I_DBF:
		deleteTopFrame(stack);
		break;
	default:
		break;
	}
	return 1;
}