/*THIS FILE IS A PART OF A NOT YET PUBLISHED PROJECT BY A FRIEND OF MINE      */
/*IT CONTAINS A GENERAL PURPOSE LIBRARY FOR COMMON SYSTEM DEPENDENT PROCEDURES*/
/*THE LIBRARY IS CONSIDERED AS LGPL LICENSED WHEN USED SEPARATELY             */
/*OR EITHER IT MAY BE CONSIDERED AS A PART OF MEOW AND SOME OTHER PROJECT     */
/*(My friend has allowed to use the library in the MEOW project but has not   */
/*yet allowed to tell his project's name. Sorry about this.                   */

/* useful string functions */
/* I intentially declined idea of dynamic allocation and reallocation */
/* in order not to make mess of memory heap (a lot trouble on machines with flash)*/
/* so far before use of functions U must allocate strings (char arrays)*/
/* of needed capacity.*/
/* e.g. char *str1[255] */
/* or mpDeclareStr(str1) - defined in mp_sys.h */
/* or either char* str1; str1=(char*)mpmalloc(NeededSize*sizeof(char))*/
/* Since there's no comon method to know either pointer does point to*/
/* array of suitable capacity U may want to reserve a surplus and always use [255]'s*/
#include "mp_sys.h"


#ifndef __MP_STRINGS_H_INCLUDED__
#define __MP_STRINGS_H_INCLUDED__
#define MP_STRINGS_API _stdcall
#define MP_STRINGS_LOCAL static

#ifdef __cplusplus
extern "C" {
#endif
/*******************general string functions***********************************/
MP_STRINGS_API int mpStrLen(char* in); /*length of str*/
MP_STRINGS_API int mpStrCompare(char* str1, char* str2);
MP_STRINGS_API int mpStrFromNowOnIs(char* str, char* check, int idx);
MP_STRINGS_API void mpStrCopy(char* dest, char* orign); /*copy str2 into str1*/
MP_STRINGS_API void mpStrAddStr(char* str1, char* str2);
MP_STRINGS_API void mpStrTrimLeft(char* str, char delim);
MP_STRINGS_API int mpStrToUpper(char* in);
/*note ALL pointer resulted functions DO CREATE OBJECTS (don't forget to free)*/
MP_STRINGS_API char* mpStrReplace(char* swhere, char* sto, char* sfrom);
/*******************end of general string functions****************************/

/*******************token parssing related*************************************/
MP_STRINGS_API int mpGetLastFieldIndex(char* str, char delim);
/*returns token with index N (indexing begins from 0)*/
/*empty fields are empty fields. they are counted and get but have zero length*/
/*this allows to parse constructs like that: print (0,,,2,"")*/
MP_STRINGS_API void mpGetNthField(char* fld, char* str, int N, char delim);
MP_STRINGS_API void mpGetAndCutLeftField(char* fld, char* str, char delim);
MP_STRINGS_API int mpGetAndCutLeftToken(char* fld, char* str, char* delims);
/*******************end of token parssing related******************************/


/******************** text related ********************************************/
MP_STRINGS_API mpText* mpCreateCopyOfText(mpText* txt);
MP_STRINGS_API void mpInsertTextToText(int idx, mpText* tto, mpText* tfrom);
MP_STRINGS_API void mpAppendStrToText(mpText* tto, char* sfrom);
/******************** end of text related *************************************/

/**************numeric to string and reverse conversion************************/
MP_STRINGS_API void mpIntToStr(char* outStr, int inInt);
MP_STRINGS_API int mpStrIsInteger(char* inStr);
MP_STRINGS_API int mpStrToInt(char* inStr);
MP_STRINGS_API float mpStr2Flt(char *s);
MP_STRINGS_API void mpFlt2Str(float x, char *str);

/**************end of numeric to string and reverse conversion*****************/

#ifdef __cplusplus
}
#endif

#ifndef __MP_STRINGS_BUILD__
extern const char mpDIGITS[11];
extern const char mpDELIMS[30];
extern const char mpOPERATORS[10];
extern const char mpLETTERS[64];
extern const char mpALFANUM[64];
#endif


#endif /*__MP_STRINGS_H_INCLUDED__*/
