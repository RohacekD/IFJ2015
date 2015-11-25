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
	//nejak implementovany pointer do tabulky symbolu
	int intVal;
	double doubleVal;
	string stringVal;
	bool boolVal;
}tVariableData;

typedef struct {
	tVariableType type;
	tVariableData data;
} tVariable;

/*
* Utvori strukturu promenne, inicializuje string.
* @param tVariable* struktura promenne
* @param tVariableType urcuje typ
* @void* (int*|bool*|double*|string*)
*/
void variableCreate(tVariable*, tVariableType, void*);
/*
* Spravne smaze promennou. Preda se ukazatelem pri mazani stromu
* @param tVariable* struktura promenne
*/
void variableDelete(tVariable*);


#endif //VARIABLE_H_