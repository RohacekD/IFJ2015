#include "scanner.h"
#include "token.h"
#include "test-fw.h"
#include <stdio.h>

/*
 * Kdyz kazdy token po sobe smazu vzdy smazu 
 */

FILE* test(char* filename) {
	printTestData(filename);
	printDescription("");
	FILE *file;
	file = fopen(filename, "r");
	if (!file) {
		printf("neexistujici soubor");
		return file;
	}
	return file;
}

int test_1() {
	printDescription("TEST 01");
	printDescription("Timto testem se nesnazim nic rozbit jen na kazdem radku");
	printDescription("prijmout jeden lexem");
	FILE* file = test("test-1.txt");
	int ret;
	tToken token;
	do {
		if ((ret = getToken(&token, file)) != 1) {
		}
		else {
			printf("%d\t", line);
			printToken(&token);
			freeTokenMem(&token);
		}

	} while (token->typ != END_OF_FILE);
	fclose(file);
	return 1;
}

int test_2() {
	printDescription("TEST 02");
	printDescription("Test komentaru");
	FILE* file = test("test-2.txt");
	int ret;
	tToken token;
	do {
		if ((ret = getToken(&token, file)) != 1) {
			freeTokenMem(token);
		}
		else {
			printf("%d\t", line);
			printToken(&token);
			freeTokenMem(token);
		}

	} while (token->typ != END_OF_FILE);
	fclose(file);
	return 1;
}

int main() {
	printDescription("TESTY SCANNERU");
	if (!test_1()) {

	}
	if (!test_2()) {

	}
	getchar();
}