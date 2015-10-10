/*THIS FILE IS A PART OF A NOT YET PUBLISHED PROJECT BY A FRIEND OF MINE      */
/*IT CONTAINS A GENERAL PURPOSE LIBRARY FOR COMMON SYSTEM DEPENDENT PROCEDURES*/
/*THE LIBRARY IS CONSIDERED AS LGPL LICENSED WHEN USED SEPARATELY             */
/*OR EITHER IT MAY BE CONSIDERED AS A PART OF MEOW AND SOME OTHER PROJECT     */
/*(My friend has allowed to use the library in the MEOW project but has not   */
/*yet allowed to tell his project's name. Sorry about this.                   */

/* system dependencies and replacements for incomon functions */

#ifndef __MP_SYS_H_INCLUDED__
#define __MP_SYS_H_INCLUDED__

/* some system config options */
#define MP_SYS_API _stdcall
#define MP_SYS_LOCAL static
//#define MP_USES_ZIP
#define MP_8
#define USES_CONSOLE
#define MP_DEBUG_ON
#define mpDeclareStr(a) char (a)[256] /*allous to declare strings as mpDeclareStr(a1)*/
#define MP_QUOT1 '"'
#define MP_QUOT2 '\''
#define MP_REM_OPEN1 '/'
#define MP_REM_OPEN2 '*'
#define MP_REM_CLOSE1 '*'
#define MP_REM_CLOSE2 '/'
#define MP_WHITESPACE ' '
#define MP_TAB '\t'
#define MP_BLOCK_DELIMETER ';'
#define MP_UNKNOWN_CHAR 13
#define MP_RB_OPEN '('
#define MP_RB_CLOSE ')'
#define MP_FB_OPEN '{'
#define MP_FB_CLOSE '}'
#define MP_COMMA ','


#ifdef MP_USES_ZIP
/* if using zip make sure to link with zziplib.a and llibz.a*/
  #include "../../external/src/zzip/zzip.h"
  #define  mpfread zzip_fread
  #define  mpfopen zzip_fopen
  #define  mpfclose zzip_fclose
  #define  mprewind zzip_rewind
  #define  mpeof zzip_feof
  #define  MPFILE ZZIP_FILE
#else /* not MP_USES_ZIP*/
  #include <stdio.h>
  #define  mpfread fread
  #define  mpfopen fopen
  #define  mpfclose fclose
  #define  mprewind rewind
  #define  mpeof feof
  #define  MPFILE FILE
#endif /*MP_USES_ZIP*/

#define mpmax(a,b) ((a)>(b)?(a):(b))
#define mpmin(a,b) ((a)<(b)?(a):(b))
#define mpmabs(a) ((a)<(0)?(-(a)):(a))

#ifdef  MP_32
 #define mpmemcpy mpmemcpy32
#endif 
#ifdef  MP_8
 #define mpmemcpy mpmemcpy8
#endif 
#define MP_PTR_SIZE sizeof(void*) 
#define MP_PTR void* 
#define MP_HANDLE_SIZE sizeof(long) 
#define MP_HANDLE long 
/* end of system config options */

typedef struct _mpText{
  int nstrings;
  int cntr;
  void* host; 
  char** strings;
} mpText;

#ifdef __cplusplus
extern "C" {
#endif

MP_SYS_API long mpfsize (MPFILE* fil);
MP_SYS_API void mpmemcpy8 (void* dest, void* origin, int size);
MP_SYS_API void mpmemcpy32 (void* dest, void* origin, int size);
MP_SYS_API void* mpmalloc(int size);
MP_SYS_API void mpfree(void* ptr);
MP_SYS_API void* mprealloc(void* ptr, int newsize, int oldsize);
MP_SYS_API int mpCheckBitInWord(unsigned long word32, unsigned long mask32);
MP_SYS_API void mpSetBitInWord(unsigned long *pword32, unsigned long mask32, int enabled);
//MP_SYS_API int mpDebugPrint  (const char* format, ...);
MP_SYS_API int mpDebugPrint  (char* format, ...);
MP_SYS_API void mpLoadLib (void* phandle, char* libFname);
MP_SYS_API void mpFreeLib (void* phandle);
MP_SYS_API void mpFindProcInLib (MP_PTR pfunc, void* phandle, char* fName);
MP_SYS_API void mpCallXternFunc (MP_PTR pfunc, MP_PTR data);
MP_SYS_API mpText* CreateEmptyText();
MP_SYS_API void mpTextCopyRef(mpText* tto, mpText* tfrom);
MP_SYS_API void mpDestroyText(mpText** txt); /*all other text related should be in mpstrings.h*/
MP_SYS_API mpText* ReadSytxTextFromFile (char* fName);
MP_SYS_API void WriteTextToFile (char* fName, mpText* txt);
MP_SYS_API int mpLoad(void* ptr, int lmt, char* fName);
MP_SYS_API int mpReserveAndLoad(void** ptr, char* fName);
MP_SYS_API void mpWrite(void* ptr, int Nb, char* fName);
MP_SYS_API void mpReWrite(void* ptr, int Nb, char* fName);
MP_SYS_API int mpfCopy(char* fNameTo, char* fNameFrom);
MP_SYS_API int mpGets(char* sdest);
MP_SYS_API long mpGettime();
MP_SYS_API void mpSleep(int t);


#ifdef __cplusplus
}
#endif

#endif /* __MP_SYS_H_INCLUDED__ */
