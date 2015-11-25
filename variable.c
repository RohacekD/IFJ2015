#include "variable.h"

void variableCreate(tVariable* var, tVariableType type, void* data) {
	var->type = type;
	switch (type) {
	case VAR_TYPE_INT:
		var->data.intVal = *(int*)data;
		break;
	case VAR_TYPE_BOOL:
		var->data.boolVal = *(bool*)data;
		break;
	case VAR_TYPE_DOUBLE:
		var->data.doubleVal = *(double*)data;
		break;
	case VAR_TYPE_STRING:
		if (strInit(&var->data.stringVal)) {
			FatalError(99, ERR_MESSAGES[ERR_ALLOC]);
		}
		if (strCopyString(&var->data.stringVal, data)) {
			//error
		}
		break;
	default:
		break;
	}
}
void variableDelete(tVariable* var) {
	if (var->type == VAR_TYPE_STRING) {
		strFree(&var->data.stringVal);
	}
}