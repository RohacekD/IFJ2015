// IFJ2015.cpp : Defines the entry point for the console application.
//
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "parser.h"

int main(int argc, char **argv)
{
	if (argc == 1 || argc >= 3) {
		FatalError(99, "Chyba vstupnich argumentu\n");
	}

	FILE *file;

	if ((file = fopen(argv[1], "r")) == NULL)
	{
		FatalError(99, "Nepodarilo se otevrit vstupni soubor\n");
	}

	setSourceFile(file);

	parse();

	return 0;
}