/*
*  PrintableEnum.h
*  Project: IFJ2015
*  Created on: 23. 10. 2017
*  Author: xrohac04
*  Description: Hlavickovy soubor
*/

#pragma once
#define DECL_NAMED_ENUM_ROW(val) val,
#define DECL_NAMED_ENUM_NAME(val) #val,
#define DECL_NAMED_ENUM_FUNC_HEADER(type) \
	const char* Convert_##type(type in)

#define DECL_NAMED_ENUM_ENUM(name, declaration) \
	typedef enum{	\
		declaration(DECL_NAMED_ENUM_ROW) \
	}name;

#define DECL_NAMED_ENUM_NAMES(name, declaration) \
	char* NAMES_##name[] = {declaration(DECL_NAMED_ENUM_NAME)}; \
	DECL_NAMED_ENUM_FUNC_HEADER(name) {return NAMES_##name[in];}

#define DECL_NAMED_ENUM(name, declaration) \
	DECL_NAMED_ENUM_ENUM(name, declaration) \
	DECL_NAMED_ENUM_FUNC_HEADER(name);
