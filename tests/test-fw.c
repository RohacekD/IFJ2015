#include "test-fw.h"
#include "../token.h"
/*
 *Pro hezci vypis hlavicek testu
 *
 */
void printDescription(const char* text) {
	int l = strlen(text);
	int num = (60 - l) / 2;
	for (int i = 0; i < num; i++) {
		printf("-");
	}
	printf("%s", text);
	(l % 2) ? num++:num;
	for (int i = 0; i < num; i++) {
		printf("-");
	}
	printf("\n");
}

void printTestData(const char* filename) {
	int c, line = 1;
	FILE *file;
	file = fopen(filename, "r");
	printDescription("Testovana data:");
	printf("%d\t|", line);
	if (file) {
		while ((c = getc(file)) != EOF) {
			putchar(c);
			if (c == '\n') {
				line++;
				printf("%d\t|", line);
			} 
		}
		fclose(file);
	}
	printf("\n");
	printDescription("Konec dat");
}

void printToken(tToken *token) {
	const char *tokenNames[] =
	{
		"Datovy typ integer",
		"Datovy typ double",
		"Datovy typ string",
		"Datovy typ bool",
		"Identifikator",
		"Deleni",
		"Nasobeni",
		"Scitani",
		"Odecitani",
		"Vetsi nez",
		"Vetsi nez rovno",
		"Mensi nez",
		"Mensi nez rovno",
		"Rovno",
		"Nerovno",
		"Pøiøazení",
		"Inkrementace",
		"Dekrementace",
		"Logicky and",
		"Logicky or",
		"Logicky not",
		"minus",
		"plus",
		"keyword int",
		"Oteviraci zavorka",
		"Uzaviraci zavorka",
		"Oteviraci slozena zavorka",
		"Uzaviraci slozena zavorka",
		"Strednik",
		"konec souboru"
	};
	printf("***Token => ");
	switch (token->typ)
	{
	case TYPE_INTEGER:
		printf("%s = %i\n", tokenNames[token->typ], token->value.intVal);
		break;
	case TYPE_DOUBLE:
		printf("%s = %f\n", tokenNames[token->typ], token->value.doubleVal);
		break;
	case TYPE_STRING:
		printf("%s = %s\n", tokenNames[token->typ], token->value.stringVal.str);
		break;
	case TYPE_BOOL:
		printf("%s = %s\n", tokenNames[token->typ], token->value.boolVal ? "true" : "false");
		break;
	case TYPE_IDENTIFICATOR:
		printf("%s = %s\n", tokenNames[token->typ], token->value.stringVal.str);
		break;
	default:
		printf("%s\n", tokenNames[token->typ]);
		break;
	}
}