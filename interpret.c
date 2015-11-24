#include "interpret.h"

int executeTape(tInsTapeInsPtr ins) {
	/* Prottoze tato fce je volana rekurzivne je nutno si frameStack drzet
	 * v static
	 */
	static tDLList* frameStack = NULL;
	/*Prvotni inicializace stacku*/
	if (!frameStack) {
		if ((tDLList *)malloc(sizeof(tDLList))) {
			FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
		}
		DLInitList(frameStack);
	}
}