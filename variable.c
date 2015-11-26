#include "variable.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>

void variableCreate(tVariablePtr* var, tVariableType type) {
    tVariablePtr v = (tVariablePtr) malloc(sizeof(tVariablePtr));
    if(!v){
        FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
    }
    v->type = type;
    if(type==VAR_TYPE_STRING) {
        if(strInit(&v->data.stringVal)) //str init vraci 0 pri uspechu protoze str.c psal idiot
            FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
    }
    *var = v;
}
void variableDelete(void* var) {
	if (((tVariablePtr)var)->type == VAR_TYPE_STRING) {
		strFree(&((tVariablePtr)var)->data.stringVal);
	}
       // printf("Mazu promennou %d\n",((tVariablePtr)var)->data.intVal);
}