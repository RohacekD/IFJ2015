//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include "str.h"
#include "error.h"

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla 

#define STR_ERROR   1
#define STR_SUCCESS 0

int strInit(string *s)
// funkce vytvori novy retezec
{
	if ((s->str = (char*)malloc(STR_LEN_INC)) == NULL)
		return STR_ERROR;
	s->str[0] = '\0';
	s->length = 0;
	s->allocSize = STR_LEN_INC;
	return STR_SUCCESS;
}

void strFree(string *s)
// funkce uvolni retezec z pameti
{
    if (s != NULL)
	free(s->str);
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
	s->str[0] = '\0';
	s->length = 0;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
	if (s1->length + 1 >= s1->allocSize)
	{
		// pamet nestaci, je potreba provest realokaci
		if ((s1->str = (char*)realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
			return STR_ERROR;
		s1->allocSize = s1->length + STR_LEN_INC;
	}
	s1->str[s1->length] = c;
	s1->length++;
	s1->str[s1->length] = '\0';
	return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2)
// prekopiruje retezec s2 do s1
{
	int newLength = s2->length;
	if (newLength >= s1->allocSize)
	{
		// pamet nestaci, je potreba provest realokaci
		if ((s1->str = (char*)realloc(s1->str, newLength + 1)) == NULL)
			return STR_ERROR;
		s1->allocSize = newLength + 1;
	}
	strcpy(s1->str, s2->str);
	s1->length = newLength;
	return STR_SUCCESS;
}

int strConConstString(string *s1, char *s2){
//provede konkatenaci s1s2 a ulozi ji v s1
	int i=0;
	while(s2[i]!=0){
		if(strAddChar(s1,s2[i])==STR_ERROR){
			return STR_ERROR;
		}
		i++;
	}
	s1->length=s1->length+i;
	return STR_SUCCESS;
}

int strCmpString(string *s1, string *s2)
// porovna oba retezce a vrati vysledek
{
	return strcmp(s1->str, s2->str);
}

int strCmpConstStr(string *s1, char* s2)
// porovna nas retezec s konstantnim retezcem
{
	return strcmp(s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
	return s->str;
}

int strGetLength(string *s)
// vrati delku daneho retezce
{
	s->length = 0;
	while (s->str[s->length] != 0)
		s->length++;
	return s->length;
}
/**
 * Prijma NEincializovany string a vlozi do nej obsah druheho parametru
 * @param s[out]
 * @param c[in]
 * @return 1 pokud konverze probehne v poradku jinak 0
 */
int strCharToString(string* s, char* c)
{
    if(s==NULL || c==NULL) return 0;
    strInit(s);
    int i = 0;
    
    while(c[i]!='\0'){
		if (strAddChar(s, c[i])) {
			return 0;
		}
        i++;
    }
	return 1;
}

string substr(string s, int i, int n, ERR_CODES *err){
    *err = STR_SUCCESS;
    string new;
    strInit(&new);
    
    if(i<0 || i>= length(s)){
    	strFree(&new);
    	*err = STR_ERROR;
    	return new; //zde by mel nastat pravdepodobne runtime error
    }
    
    if(n<0){
        while(s.str[i] != 0){
            strAddChar(&new,s.str[i]);
            i++;
        }
    }
    else{
        for(int k = 0; k<n; k++){
            if(s.str[i+k] == 0) return new;
            strAddChar(&new,s.str[i+k]);
        }
    }
    return new;
}

string concat(string s1, string s2){
    string new;
    strInit(&new);
    strCopyString(&new,&s1);
    for(int i = 0; i<= s2.length; i++){
        strAddChar(&new,s2.str[i]);
    }
    return new;
}

int length(string s){
	return strGetLength(&s);
}
