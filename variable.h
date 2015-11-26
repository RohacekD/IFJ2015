#ifndef VARIABLE_H_
#define VARIABLE_H_
#include "str.h"
#include "error.h"
#include <stdbool.h>

typedef enum {
	VAR_TYPE_INT,
	VAR_TYPE_BOOL,
	VAR_TYPE_DOUBLE,
	VAR_TYPE_STRING
} tVariableType;

typedef union
{
	int intVal;
	double doubleVal;
	string stringVal;
	bool boolVal;
}tVariableData;

typedef struct {
	tVariableType type;
	tVariableData data;
} *tVariablePtr;

/*
* Utvori strukturu promenne, inicializuje string.
* @param tVariable* struktura promenne
* @param tVariableType urcuje typ
*/
void variableCreate(tVariablePtr*, tVariableType);
/*
* Spravne smaze promennou. Preda se ukazatelem pri mazani stromu
* @param tVariable* struktura promenne
*/
void variableDelete(void*);


#endif //VARIABLE_H_