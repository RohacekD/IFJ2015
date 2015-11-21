#include "../scanner.h"
#include "../token.h"
#include "test-fw.h"

FILE* test(char* filename) {
	printTestData(filename);
	printDescription("");
	FILE *file;
	file = fopen("tests/scanner/test-1.txt", "r");
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
	FILE* file = test("tests/scanner/test-1.txt");
	int ret;
	tToken token;
	do {
		if ((ret = getToken(&token, file)) != 1) {
			printf("getToken: %d\n", ret);
		}
		else {
			printToken(&token);
		}

	} while (token.typ!=END_OF_FILE);
	fclose(file);
	return 1;
}

int test_2() {
	printDescription("TEST 02");
	printDescription("Test komentaru");
	FILE* file = test("tests/scanner/test-2.txt");
	int ret;
	tToken token;
	do {
		if ((ret = getToken(&token, file)) != 1) {
			printf("getToken: %d\n", ret);
		}
		else {
			printToken(&token);
		}

	} while (token.typ != END_OF_FILE);
	fclose(file);
	return 1;
}

int main() {
	printDescription("TESTY SCANNERU");
	if (!test_1()) {

	}
	getchar();
}