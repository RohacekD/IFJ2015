#ifndef VARIABLE_H_
#define VARIABLE_H_
#include "str.h"
#include "error.h"
#include "tabSym.h"
#include <stdbool.h>

#define getVarVal(var)		\
((var->type == VAR_TYPE_INT) ? (var->data.intVal) : (var->type == VAR_TYPE_BOOL) ? (var->data.boolVal) : (var->data.doubleVal))


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
* @param tVariable[out] struktura promenne
* @param tVariableType[in] urcuje typ
*/
void variableCreate(tVariablePtr*, tVariableType);
/*
* Spravne smaze promennou. Preda se ukazatelem pri mazani stromu
* @param tVariable[in] struktura promenne
*/
void variableDelete(void*);

/*
 * Prevede notaci dat. typu tabulky symbolu do notace promenne
 * @param tTabSymVarDataType[in] notace v tabulce symbolu
 * @return tVariableType[out] notece ve variable
 */
tVariableType tTabSymToVarNotatation(tTabSymVarDataType);

#endif //VARIABLE_H_