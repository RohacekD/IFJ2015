// IFJ2015.cpp : Defines the entry point for the console application.
//
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "parser.h"
#include "interpret.h"

int main(int argc, char **argv)
{
	if (argc == 1 || argc >= 3) {
		FatalError(ERR_INTERNAL, "Chyba vstupnich argumentu\n");
	}

	FILE *file;

	if ((file = fopen(argv[1], "r")) == NULL)
	{
		FatalError(ERR_INTERNAL, "Nepodarilo se otevrit vstupni soubor\n");
	}

	setSourceFile(file);

	//zapocneme syntaxi rizeny preklad
	parse();

	fclose(file);
	return 0;
}
