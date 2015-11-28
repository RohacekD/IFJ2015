#include <stdio.h>
#include "token.h"
#include "scanner.h"
#include "test-fw.h"

int main() {
	//obsah souboru nas zajimat nebude
	FILE* f = fopen("test-1.txt", "r");
	tToken t;
	
	
	printDescription("Do fronty tlacim bool = true");
	t = malloc(sizeof(struct stToken));
	t->typ = TYPE_BOOL;
	t->value.boolVal = true;
	ungetToken(&t);

	printDescription("Zkusim zavolat scanner ocekavam bool = true");
	getToken(&t, f);
	if (t->typ == TYPE_BOOL && t->value.boolVal) {
		printDescription("TEST OK");
	}
	else {
		printDescription("TEST FAILED");
	}
	freeTokenMem(t);
	printDescription("Od ted testuji jen pomoci fce void TQDequeue(tToken*);");
	printDescription("protoze jestli se vola skrz scanner jiz vime");

	printDescription("Do fronty tlacim int = 42");
	t = malloc(sizeof(struct stToken));
	t->typ = TYPE_INTEGER;
	t->value.intVal = 42;
	ungetToken(&t);
	printDescription("Do fronty tlacim KEYW_RETURN");
	t = malloc(sizeof(struct stToken));
	t->typ = KEYW_RETURN;
	ungetToken(&t);

	printDescription("Ocekavam int = 42 fronta by nemela byt deinicializovana");
	TQDequeue(&t);
	if (t->typ == TYPE_INTEGER && t->value.intVal == 42 && TQueue) {
		printDescription("TEST OK");
	}
	else {
		printDescription("TEST FAILED");
	}
	freeTokenMem(t);

	printDescription("Ocekavam return keyword fronta by mela byt deinicializovana");
	TQDequeue(&t);
	if (t->typ == KEYW_RETURN && !TQueue) {
		printDescription("TEST OK");
	}
	else {
		printDescription("TEST FAILED");
	}
	freeTokenMem(t);

	printDescription("Do fronty tlacim string = 'HuaHuaString'");
	t = malloc(sizeof(struct stToken));
	t->typ = TYPE_STRING;
	strInit(&t->value.stringVal);
	strConConstString(&t->value.stringVal, "HuaHuaString");
	ungetToken(&t);



	printDescription("Ocekavam string = 'HuaHuaString' fronta by mela byt deinicializovana");
	string s;
	strInit(&s);
	strConConstString(&s, "HuaHuaString");


	TQDequeue(&t);
	if (t->typ == TYPE_STRING && !strCmpString(&t->value.stringVal, &s) && !TQueue) {
		printDescription("TEST OK");
	}
	else {
		printDescription("TEST FAILED");
	}
	strFree(&s);
	freeTokenMem(t);

	printDescription("Zavolam void freeTokenMem(tToken); nad NULL");
	printDescription("Nemela by nastat chyba");
	printDescription("Test zrusen je to blbost");
	//freeTokenMem(t);
	printDescription("Mam deinicializovanou frontu zkusim z ni neco vytahnout");
	printDescription("behem programu nebude volano, ale pro jistotu");
	TQDequeue(&t);
	if (!TQueue/* && !t*/) {
		printDescription("TEST OK");
	}
	else {
		printDescription("TEST FAILED");
	}
	//freeTokenMem(t);//pro jistotu




	getchar();
	fclose(f);
	return 0;
}