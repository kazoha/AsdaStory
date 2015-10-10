/*THIS FILE IS A PART OF A NOT YET PUBLISHED PROJECT BY A FRIEND OF MINE      */
/*IT CONTAINS A GENERAL PURPOSE LIBRARY FOR COMMON SYSTEM DEPENDENT PROCEDURES*/
/*THE LIBRARY IS CONSIDERED AS LGPL LICENSED WHEN USED SEPARATELY             */
/*OR EITHER IT MAY BE CONSIDERED AS A PART OF MEOW AND SOME OTHER PROJECT     */
/*(My friend has allowed to use the library in the MEOW project but has not   */
/*yet allowed to tell his project's name. Sorry about this.                   */


/* system dependencies and replacements for incomon functions */

#include "mp_sys.h"
#include <stdlib.h>
//#include <string.h>
MP_SYS_API long mpfsize (MPFILE* fil)
{
#ifdef MP_USES_ZIP
   ZZIP_STAT fst;
   zzip_fstat(fil, &fst); 
   return fst.st_size;
#else /* not MP_USES_ZIP*/
   long fSize; char trash;
   mprewind(fil); fSize=0;
   while (!feof(fil)) 
   {
      fread(&trash, sizeof(char), 1, fil);
      fSize++;
   }; 
   mprewind(fil); 
   return fSize;
#endif /*MP_USES_ZIP*/
}
           
MP_SYS_API void mpmemcpy8 (void* dest, void* origin, int size)
{
    char* pdest=(char*)dest;   
    char* porig=(char*)origin;   
    register int i;
    for (i=0; i<size; i++)  *(pdest+i)=*(porig+i);
}

MP_SYS_API void mpmemcpy32 (void* dest, void* origin, int size)
{
    register int i;
    register int* pdest=(int*)dest;
    register int* porig=(int*)origin;
    int size2;
    size2=size>>2;
    for (i=0; i<size2; i++)  *(pdest+i)=*(porig+i);
    size2=size2<<2;
    mpmemcpy8(pdest+i,porig+i,size-size2);
}

MP_SYS_API void* mpmalloc(int size)
{
    void* result;
    result=malloc(size);
    memset(result, 0, size);
    return result;
}

MP_SYS_API void mpfree(void* ptr)
{
  free(ptr);
}

MP_SYS_API void* mprealloc(void* ptr, int newsize, int oldsize)
{
    void* result; int size;
    if (newsize<=0) 
    { 
      if (oldsize>0) if (ptr) mpfree(ptr);
      return 0;
    }
    result=malloc(newsize);
    size=mpmin(newsize,oldsize);
    if (size<=0) 
    { 
      if (oldsize>0) if (ptr) mpfree(ptr);
      return result;
    }
    mpmemcpy(result, ptr, size);
    if (oldsize>0) if (ptr) mpfree(ptr);
    return result;
}
MP_SYS_API int mpCheckBitInWord(unsigned long word32, unsigned long mask32)
{
	return (word32 & mask32)!=0;
}
MP_SYS_API void mpSetBitInWord(unsigned long *pword32, unsigned long mask32, int enabled)
{
	unsigned long enb=0; if (enabled) enb=!0;
	*pword32 = (*pword32 & (~mask32)) |	(((!enb)-1) & mask32);
}
MP_SYS_API mpText* CreateEmptyText()
{
   mpText* ptxt;
   ptxt=(mpText*) mpmalloc(sizeof(mpText));
   ptxt->host=0; ptxt->nstrings=0;
   ptxt->strings=0; ptxt->cntr=0;
   return ptxt;
}

MP_SYS_API void mpDestroyText(mpText** txt)
{
  int i;
  mpText* ttxt;
  if (!txt) return;  ttxt=*txt; if (!ttxt) return;
  if (ttxt->nstrings>0) 
  {
    for (i=0; i<ttxt->nstrings; i++) mpfree(ttxt->strings[i]);
    mpfree (ttxt->strings);
  }
  mpfree(*txt); *txt=0; return;
}

MP_SYS_API void mpTextCopyRef(mpText* tto, mpText* tfrom)
{
  mpmemcpy (tto, tfrom, sizeof(mpText));
}

MP_SYS_API mpText* ReadSytxTextFromFile (char* fName)
{
     MPFILE* file;
     mpText* ptxt;
     long sz, i;
     int cmt, quot, nStr, nChar, j;
     char old_char, new_char;
     char strbuf[256];
     file=0; file = mpfopen(fName, "rb"); if (!file) return 0;
     ptxt=CreateEmptyText();
     sz=mpfsize(file); mprewind(file); new_char=MP_BLOCK_DELIMETER; cmt=0; quot=0;
     nStr=0; nChar=0;
     for (i=0; i<sz; i++)
     {
        old_char=new_char; mpfread(&new_char, sizeof(char), 1, file);
        if (nChar>=250) new_char=MP_BLOCK_DELIMETER;
        if (new_char==MP_TAB) new_char=MP_WHITESPACE;
        if (new_char<=MP_UNKNOWN_CHAR) new_char=MP_BLOCK_DELIMETER;
        if ((new_char==MP_QUOT1)&&(!cmt)) quot=!quot;
        if ((old_char==MP_REM_OPEN1)&&(new_char==MP_REM_OPEN2)&&(!quot)&&(!cmt)) {cmt=1; nChar--;}
        if ((old_char==MP_REM_CLOSE1)&&(new_char==MP_REM_CLOSE2)&&(!quot)&&(cmt)) {cmt=0; continue;}
        if (cmt) continue;
        if ((new_char==MP_BLOCK_DELIMETER)&&(!quot)&&(nChar<=0)) continue;
        if ((new_char==MP_WHITESPACE)&&(!quot)&&(nChar<=0)) continue;
        if ((new_char==MP_WHITESPACE)&&(!quot)&&(old_char==MP_WHITESPACE)) continue;
        if ((new_char==MP_BLOCK_DELIMETER)&&(!quot)&&(nChar>0))
        {
           if (nStr==0) ptxt->strings=(char**)mpmalloc(sizeof(char*));
           else ptxt->strings=(char**)mprealloc(ptxt->strings,(nStr+1)*sizeof(char*),nStr*sizeof(char*)); 
           ptxt->strings[nStr]=(char*)mpmalloc((nChar+1)*sizeof(char)); 
           mpmemcpy(ptxt->strings[nStr], strbuf, nChar*sizeof(char));
           nStr++; nChar=0; continue;
        }
        if ((new_char==MP_FB_OPEN||new_char==MP_FB_CLOSE)&&(!quot)&&(nChar==0))
        {
           if (nStr==0) ptxt->strings=(char**)mpmalloc(sizeof(char*));
           else ptxt->strings=(char**)mprealloc(ptxt->strings,(nStr+1)*sizeof(char*),nStr*sizeof(char*)); 
           ptxt->strings[nStr]=(char*)mpmalloc(2*sizeof(char)); 
           ptxt->strings[nStr][0]=new_char; ptxt->strings[nStr][1]=0;
           nStr++; nChar=0; continue;
        }
        if ((new_char==MP_FB_OPEN||new_char==MP_FB_CLOSE)&&(!quot)&&(nChar>0))
        {
           if (nStr==0) ptxt->strings=(char**)mpmalloc(2*sizeof(char*));
           else ptxt->strings=(char**)mprealloc(ptxt->strings,(nStr+2)*sizeof(char*),nStr*sizeof(char*)); 
           ptxt->strings[nStr]=(char*)mpmalloc((nChar+1)*sizeof(char)); 
           mpmemcpy(ptxt->strings[nStr], strbuf, nChar*sizeof(char));
           ptxt->strings[nStr+1]=(char*)mpmalloc(2*sizeof(char));
           ptxt->strings[nStr+1][0]=new_char;
           ptxt->strings[nStr+1][1]=0;
           nStr++; nStr++; nChar=0; continue;
        }
        strbuf[nChar]=new_char; nChar++;
     }
  mpfclose(file);
  ptxt->nstrings=nStr;
  return ptxt;
}

#ifdef USES_CONSOLE
#include <stdio.h>
#endif /*USES_CONSOLE*/

MP_SYS_API void WriteTextToFile (char* fName, mpText* txt)
{
  if (!txt) return;
  FILE* file=0; file = fopen(fName, "wb"); if (!file) return;
  if (txt->nstrings<=0) {fprintf(file, "\r\n"); fclose(file); return; }
  int i; for (i=0; i<txt->nstrings; i++) if (txt->strings[i])
  {
    fprintf(file, "%s\r\n", txt->strings[i]);
  }
  fclose(file); return;
}
#ifdef USES_CONSOLE
#include <stdio.h>	/* vsprintf */ 
#include <stdarg.h>	/* valist */ 
#endif /*USES_CONSOLE*/

MP_SYS_API int mpDebugPrint(char *fmt, ...)
{
#ifdef USES_CONSOLE
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args); 
#endif /*USES_CONSOLE*/
return 0;
} 

//MP_SYS_API int mpDebugPrint  (const char* format, ...)
//{
//#ifdef USES_CONSOLE
//    __VALIST va;
//    int len;
//    va=(__VALIST)(&format+1);
//    vprintf (format, va); 
//#endif /*USES_CONSOLE*/
//return 0;
//}
MP_SYS_API int mpGets(char* sdest)
{
 int rslt;
 rslt=1;
#ifdef USES_CONSOLE
  rslt=(int)gets(sdest);
#endif /*USES_CONSOLE*/
return rslt;
}

MP_SYS_API int mpLoad(void* ptr, int lmt, char* fName)
{
  MPFILE* file;
  long sz;
  file=0; file = mpfopen(fName, "rb"); if (!file) return 0;
  sz=mpfsize(file); 
  mprewind(file);
  if(lmt) if (sz>=lmt) {mpfread(ptr, sizeof(char), lmt, file); return -1;}
  mpfread(ptr, sizeof(char), sz, file);
  return (int) sz;
}

MP_SYS_API int mpReserveAndLoad(void** ptr, char* fName)
{
  MPFILE* file;
  long sz;
  file=0; file = mpfopen(fName, "rb"); if (!file) return 0;
  sz=mpfsize(file); if (!sz) return 0;
  mprewind(file); *ptr=mpmalloc(sz);
  mpfread(*ptr, sizeof(char), sz, file);
  return (int) sz;
}

MP_SYS_API void mpWrite(void* ptr, int Nb, char* fName)
{
  FILE* file;
  file = fopen(fName, "ab"); if (!file) return;
  fwrite(ptr, sizeof(char), Nb, file); 
  fclose (file);
}

MP_SYS_API void mpReWrite(void* ptr, int Nb, char* fName)
{
  FILE* file;
  file = fopen(fName, "wb"); if (!file) return;
  fwrite(ptr, sizeof(char), Nb, file); 
  fclose (file);
}

MP_SYS_API int mpfCopy(char* fNameTo, char* fNameFrom)
{
  FILE* f1; FILE* f2; char cbuf;
  f1=fopen(fNameFrom, "rb"); if (!f1) return -1; rewind(f1);
  f2=fopen(fNameTo, "wb"); if (!f2) {fclose (f1); return -1;}
  while (!feof(f1)) {fread(&cbuf, 1, 1, f1); fwrite(&cbuf, 1, 1, f2);}
  fclose (f1); fclose (f2);
  return 0;
}


#include <windows.h>
#if !defined(WIN32) && defined(_WIN32)
#define WIN32
#endif /* Windows */

#include <sys/timeb.h>
 
MP_SYS_API long mpGettime()
{
   struct _timeb  tme;
   _ftime(&tme);
   return tme.time*1000+tme.millitm;
}


MP_SYS_API void mpLoadLib (void* phandle, char* libFname)
{
    MP_HANDLE *h;
    h=(MP_HANDLE *) phandle;
    *h= (MP_HANDLE)LoadLibrary(libFname);
}

MP_SYS_API void mpFreeLib (void* phandle)
{
    MP_HANDLE *h;
    h=(MP_HANDLE *) phandle;
    FreeLibrary((HANDLE)*h);
}


MP_SYS_API void mpFindProcInLib (MP_PTR pfunc, void* phandle, char* fName)
{
    MP_HANDLE h; 
    h=*(MP_HANDLE *) phandle;
    MP_PTR *p;
    p=(MP_PTR *)pfunc;
    *p = GetProcAddress((HANDLE)h, fName);
}

typedef void (_stdcall XternFunc) (void *a);
typedef XternFunc* pXternFunc;

MP_SYS_API void mpCallXternFunc (MP_PTR pfunc, MP_PTR data)
{
  pXternFunc f;
  f=*(pXternFunc*)pfunc;
  f(data);
}

MP_SYS_API void mpSleep(int t)
{
  Sleep(t);
}

