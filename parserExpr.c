/*
 * parserExpr.c
 *  Project: IFJ2015
 *  Created on: 15. 11. 2015
 *  Author: xdocek09
 *  Description: 
 */
/**
 * @file parserExpr.c
 * @author xdocek09
 * @brief 
 */

#include "parserExpr.h"

#include "stack.h"
#include <stdlib.h>
#include "token.h"


int parseExpression(){
	//vytvorime zasobnik
	tStack stack;
	stackInit(stack);

	//pridame ukoncujici znak
	stackPushElementOfKind(stack, ENDMARK,0);

	int a;
	int b=getToken();
	do{
		a=stackTopTerminal(&stack, &a);
		switch (precGetRule(a,b)) {
			case '=':
				stackPushElementOfKind(stack, TERMINAL,b);
				b=getToken();
				break;
			case '<':
				//zaměň a za a< na zásobníku
				stackPushElemBeforeTopTerm(stack,PRECSIGN,'<');

				stackPushElementOfKind(stack, TERMINAL,b);
				b=getToken();
				break;
			case '>':

				break;
			default:
				//chyba

				break;
		}
	}while();

	return 1;
}


/*** End of file: parserExpr.c ***/
