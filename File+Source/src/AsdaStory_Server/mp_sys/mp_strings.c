/*THIS FILE IS A PART OF A NOT YET PUBLISHED PROJECT BY A FRIEND OF MINE      */
/*IT CONTAINS A GENERAL PURPOSE LIBRARY FOR COMMON SYSTEM DEPENDENT PROCEDURES*/
/*THE LIBRARY IS CONSIDERED AS LGPL LICENSED WHEN USED SEPARATELY             */
/*OR EITHER IT MAY BE CONSIDERED AS A PART OF MEOW AND SOME OTHER PROJECT     */
/*(My friend has allowed to use the library in the MEOW project but has not   */
/*yet allowed to tell his project's name. Sorry about this.                   */

/* useful string functions */
#define __MP_STRINGS_BUILD__
#include "mp_strings.h"
/* dependencies */

const char mpDIGITS[11]="0123456789";
const char mpINTDIGITS[12]="0123456789-";
const char mpDELIMS[30]=" '()[]{}:;,=+-*/#%^&\\/|!";
const char mpOPERATORS[10]=",+-*/";
const char mpLETTERS[64]="ABCDEFGHIJKLMNOPQRSTUWVXYZabcdefghijklmnopqrstuwvxyz_";
const char mpALFANUM[64]="ABCDEFGHIJKLMNOPQRSTUWVXYZabcdefghijklmnopqrstuwvxyz_0123456789";
const char mpLETTERS_LOW[64]="abcdefghijklmnopqrstuwvxyz_ ";
const char mpLETTERS_UP[64]= "ABCDEFGHIJKLMNOPQRSTUWVXYZ_ ";
/*******************general string functions***********************************/
MP_STRINGS_API int mpStrLen(char* in)
{ /*length of str*/
 int i;
 for (i=0; i<255; i++) if (in[i]==0) return i;
 return 255;
} /* mpStrLen - check passed*/

MP_STRINGS_API int mpStrCompare(char* str1, char* str2)
{ /*compare two strings zero when NOT equal */
 int l1, l2, i;
 l1=mpStrLen(str1);  l2=mpStrLen(str2);
 if (l1!=l2) return 0;
 for (i=0; i<=l1-1; i++) if (str1[i]!=str2[i]) return 0;
 return 1;
} /* mpStrCompare */

MP_STRINGS_API int mpStrFromNowOnIs(char* str, char* check, int idx)
{ 
  int i, l1, l2;
  l1=mpStrLen(str);  l2=mpStrLen(check);
  if (!l1) return 0; if (!l2) return 0;
  if (l2+idx>l1) return 0;
  for (i=0; i<l2; i++) if (str[idx+i]!=check[i]) return 0;
  return 1;
}

MP_STRINGS_API int mpStrToUpper(char* in)
{
  int i, j, l1, l2, quot; char* cp;
  l1=mpStrLen(in); cp=(char*)&mpLETTERS_LOW[0];
  l2=mpStrLen(cp); quot=0;
  for (i=0; i<l1; i++) 
    if (in[i]==MP_QUOT1) quot=!quot;
    else 
      for (j=0; j<l2; j++) 
        if ((in[i]==mpLETTERS_LOW[j])&&(!quot)) in[i]=mpLETTERS_UP[j];
  return 0;
}

MP_STRINGS_API void mpStrCopy(char* dest, char* orign) /*copy str2 into str1*/
{
  int l1, l2, i;
  l2=mpStrLen(orign); if (!l2) {dest[0]=0; return;}
  mpmemcpy(dest,orign,(l2)*sizeof(char));
  dest[l2]=0;
}/* mpStrCopy - check passed*/

MP_STRINGS_API void mpStrAddStr(char* str1, char* str2)
{
 int l1, l2, i;
 l1=mpStrLen(str1);  l2=mpStrLen(str2);
 if ((l1+l2)>=254) return;
 for (i=0; i<l2; i++) str1[l1+i]=str2[i];
 str1[l1+l2]=0; /*EOL append*/
 return;
} /* strAddStr - check passed */

MP_STRINGS_API char* mpStrReplace(char* swhere, char* sto, char* sfrom) 
{/*replaces all occurences of $from in $where by $to*/
 /*creates new string with this replacement and returns its pointer*/
 int i, j, k, l, l1, l2, eq;
 char buf[255]; char* result;

 if (!swhere) {result=mpmalloc(2*sizeof(char)); mpStrCopy(result,""); return result;};
 l=mpStrLen(swhere); 
 if (l==0) {result=mpmalloc(2*sizeof(char)); mpStrCopy(result,""); return result;};
 if ((!sto)||(!sfrom)) {result=mpmalloc((l+1)*sizeof(char)); mpStrCopy(result,swhere); return result;};
 l1=mpStrLen(sfrom); l2=mpStrLen(sto);
 if (l2==0) {result=mpmalloc((l+1)*sizeof(char)); mpStrCopy(result,swhere); return result;};

 k=0; for (i=0; i<l; i++)
 {
   eq=1;
   for (j=0; j<l1; j++) if (i+j<l) if (swhere[i+j]!=sfrom[j]) eq=0;
   if (eq)
   {
     //if (i+l1>l) break;
     for (j=0; j<l2; j++) if (k<254) buf[k++]=sto[j]; 
     i+=l1-1;
   }
   else if (k<254) buf[k++]=swhere[i];
 }  buf[k]=0;
 l=mpStrLen(buf); result=mpmalloc((l+1)*sizeof(char));
 mpmemcpy(result, buf, l*sizeof(char));
 return result;
}

MP_STRINGS_API void mpStrTrimLeft(char* str, char delim)
{
  int i, j;
  i=0; while (str[i++]==delim); i--;
  j=0; while (str[j++]=str[i++]);
}

/*******************end of general string functions****************************/


/*******************token parssing related*************************************/
MP_STRINGS_API int mpGetLastFieldIndex(char* str, char delim)
{
 int i, l, n, quot;
 l=mpStrLen(str); n=0; quot=0;
 for (i=0; i<l; i++) 
 {
   if ((str[i]==MP_QUOT1)&&(delim!=MP_QUOT1)) quot=!quot;
   if ((str[i]==delim)&&(!quot)) n++;
 }
 return n;
}

MP_STRINGS_API void mpGetNthField(char* fld, char* str, int N, char delim)
{
 int i,j,l, n, quot;
 if (!fld) return; fld[0]=0; if (!str) return;
 l=mpStrLen(str); n=0; quot=0; j=0;
 for (i=0; i<l; i++) 
 {
   if ((str[i]==MP_QUOT1)&&(delim!=MP_QUOT1)) quot=!quot;
   if ((str[i]==delim)&&(!quot)) {n++; continue;}
   if (n==N) fld[j++]=str[i];
 }
 fld[j]=0;
 return;
}

MP_STRINGS_API int mpChrIsDelim(char chr, char* delims)
{
   int i, l; char* lcldelims;
   if (!chr) return 0; 
   if (!delims) lcldelims=(char*)mpDELIMS; 
   if (mpStrLen(delims)==0) lcldelims=(char*)mpDELIMS; else lcldelims=delims;
   l=mpStrLen(lcldelims); for (i=0; i<l; i++) if (chr==lcldelims[i]) return i+1;
   return 0;
}


MP_STRINGS_API void mpGetAndCutLeftField(char* fld, char* str, char delim)
{
 int i,j,l, quot;
 if (!fld) return; fld[0]=0; if (!str) return;
 l=mpStrLen(str); quot=0; j=0;
 for (i=0; i<l; i++) 
 {
   if ((str[i]==MP_QUOT1)&&(delim!=MP_QUOT1)) quot=!quot;
   if ((str[i]==delim)&&(!quot)) break;
   fld[j++]=str[i]; str[i]=delim;
 }
 fld[j]=0; mpStrTrimLeft(str, delim);
 return;
}

MP_STRINGS_API int mpGetAndCutLeftToken(char* fld, char* str, char* delims)
{
 int i,j,k,l,l1,quot,gotcha;
 if (!fld) return -1; fld[0]=0; if (!str) return -1;
 l=mpStrLen(str); l1=mpStrLen(delims); quot=0; j=0;
 for (i=0; i<l; i++) 
 {
   if ((str[i]==MP_QUOT1)&&(delims[0]!=MP_QUOT1)) quot=!quot;
   gotcha=0; for (k=0; k<l1; k++) if ((str[i]==delims[k])&&(!quot)) {gotcha=k+1; break;}
   if (gotcha) {str[i]=MP_BLOCK_DELIMETER; break;}
   fld[j++]=str[i]; str[i]=MP_BLOCK_DELIMETER;
 }
 fld[j]=0; mpStrTrimLeft(str, MP_BLOCK_DELIMETER);
 return gotcha-1;
}

/*******************end of token parssing related******************************/


/******************** text related ********************************************/
MP_STRINGS_API mpText* mpCreateCopyOfText(mpText* txt)
{
  mpText* txt2; int i, j, l;
  if (!txt) return 0;
  txt2=(mpText*) mpmalloc(sizeof(mpText));
  if (txt->nstrings==0) {txt2->nstrings=0; return txt2;}
  txt2->nstrings=txt->nstrings;
  txt2->strings=(char**) mpmalloc(txt2->nstrings*sizeof(char*));
  for (i=0; i<txt2->nstrings; i++) 
  {
      l=mpStrLen(txt->strings[i]);
      txt2->strings[i]=(char*) mpmalloc((l+1)*sizeof(char));
      for (j=0; j<l; j++) txt2->strings[i][j]=txt->strings[i][j];
  }
  return txt2;
}

MP_STRINGS_API void mpInsertTextToText(int idx, mpText* tto, mpText* tfrom)
{
  int i, j, l; char** strings;
  if (!tto) return;  if (!tfrom) return;  if (tfrom->nstrings==0) return;
  if (idx<0) idx=0; if (idx>tto->nstrings) idx=tto->nstrings+1;
  strings=(char**) mpmalloc((tto->nstrings+tfrom->nstrings)*sizeof(char*));
  for (i=0; i<idx; i++) strings[i]=tto->strings[i];
  for (i=0; i<tfrom->nstrings; i++)
  {
      l=mpStrLen(tfrom->strings[i]);
      strings[idx+i]=(char*) mpmalloc((l+1)*sizeof(char));
      for (j=0; j<l; j++) strings[idx+i][j]=tfrom->strings[i][j];
  }
  for (i=idx; i<tto->nstrings; i++) strings[i+tfrom->nstrings]=tto->strings[i];
  mpfree(tto->strings);
  tto->strings=strings;
  tto->nstrings=tto->nstrings+tfrom->nstrings;
}

MP_STRINGS_API void mpAppendStrToText(mpText* tto, char* sfrom)
{
  int l;
  if (!tto) return;  if (!sfrom) return;
  l=mpStrLen(sfrom);//if (!l) return;
  tto->nstrings++;
  tto->strings=(char**) mprealloc(tto->strings, (tto->nstrings)*sizeof(char*),(tto->nstrings-1)*sizeof(char*));
  tto->strings[tto->nstrings-1]=mpmalloc((l+2)*sizeof(char));
  mpStrCopy(tto->strings[tto->nstrings-1], sfrom);
}
/******************** end of text related *************************************/

/**************numeric to string and reverse conversion************************/
MP_STRINGS_API void mpIntToStr(char* outStr, int inInt)
{ /*convert integer to string */
  /* rather silly implementation but seems to be compatible and portable */
  /* TODO: rewise and optimize if U've got a free day */
  int i, ibuf, order;
  if (!inInt) {outStr[0]=mpDIGITS[0]; outStr[1]=0; return;}
  ibuf=inInt; order=0;
  while (ibuf/10) {ibuf=ibuf/10; order++;}
  if (inInt<0) outStr[0]='-'; else outStr[0]=MP_WHITESPACE;
  if (inInt<0) inInt=-inInt;
  ibuf=inInt; for (i=0; i<order; i++)
  {
     outStr[order+1-i]=mpDIGITS[ibuf-10*(ibuf/10)];
     ibuf=ibuf/10;
  }
  outStr[order+1-i]=mpDIGITS[ibuf-10*(ibuf/10)];
  outStr[order+2]=0;
  mpStrTrimLeft(outStr, MP_WHITESPACE);
}  /* mpIntToStr - check passed */

MP_STRINGS_API int mpStrToInt(char* inStr)
{ /*convert string to integer */
  /* rather silly implementation but seems to be compatible and portable */
  /* TODO: rewise and optimize if U've got a free day */
  int i, res, len, negate, digit;
  res=0; negate=1; if (!inStr) return 0;
  i=0; if (inStr[i]==MP_WHITESPACE) while (inStr[i]==MP_WHITESPACE) i++;
  if ((inStr[i]=='+')||(inStr[i]=='-'))
   while ((inStr[i]=='+')||(inStr[i]=='-'))
     if (inStr[i++]=='-') negate=-1*negate;
  if (inStr[i]==MP_WHITESPACE) while (inStr[i]==MP_WHITESPACE) i++;
  while (inStr[i])
  {
     digit=mpChrIsDelim(inStr[i],(char*)mpDIGITS)-1;
     if (digit<0) return 0;
     res=10*res+digit; i++;
  }
  return negate*res;
}  /* mpStrToInt - check passed */

MP_STRINGS_API int mpStrIsInteger(char* inStr)
{
  int i, j, l1, l2, isdigit; char* cp;
  l1=mpStrLen(inStr); cp=(char*)&mpINTDIGITS[0]; l2=mpStrLen(cp); 
  for (i=0; i<l1; i++) 
  {
    isdigit=0;  for (j=0; j<l2; j++) if (inStr[i]==mpINTDIGITS[j]) isdigit=1;
    if (!isdigit) return 0;
  }
  return 1;
}

/* string to float conversion */
MP_STRINGS_API float mpStr2Flt(char *s)
{
	float sum,		/* the partial result */
		scale;		/* scale factor for the next digit */
	char *start,	/* copy if input pointer */
		*end,		/* points to end of number */
		c;			/* character from input line */
	int minus,		/* nonzero if number is negative */
		dot,		/* nonzero if *s is decimal point */
		decimal;	/* nonzero if decimal point found */

	if ( *s == '-' ) { minus = 1 ; ++s ; }
	else minus = 0 ;
	start = s ;
	decimal = 0 ;  /* no decimal point seen yet */
	while( (dot=(*s=='.')) || isdigit(*s) ) {
		if ( dot ) ++decimal ;
		++s ;	/* scan to end of string */
	}
	end = s ;
	sum = 0.0 ;		/* initialize answer */
	if ( decimal ) {
		/* handle digits to right of decimal */
		--s ;
		while ( *s != '.' )
			sum = ( sum + (float)( *(s--) - '0' ) ) / 10.0 ;
	}
	scale = 1.0 ;	/* initialize scale factor */
	while ( --s >= start ) {
		/* handle remaining digits */
		sum += scale * (float)( *s-'0' ) ;
		scale *= 10.0 ;
	}
	c = *end++ ;
	if( tolower(c)=='e' ) {	/* interpret exponent */
		int neg ;		/* nonzero if exp negative */
		int expon ;		/* absolute value of exp */
		int k ;			/* mask */

		neg = expon = 0 ;
		if ( *end == '-' ) {
			/* negative exponent */
			neg = 1 ;
			++end ;
		}
		while(1) {	/* read an integer */
			if ( (c=*end++) >= '0' ) {
				if ( c <= '9' ) {
					expon = expon * 10 + c - '0' ;
					continue ;
				}
			}
			break;
		}
		if ( expon > 38 ) {
			expon = 0 ;
		}
		k = 32 ;	/* set one bit in mask */
		scale = 1.0 ;
		while(k) {
			scale *= scale;
			if ( k & expon ) scale *= 10.0 ;
			k >>= 1 ;
		}
		if(neg) sum /= scale;
		else    sum *= scale;
	}
	if (minus) sum = -sum ;
	return sum ;
}

MP_STRINGS_API void mpFlt2Str(float x, char *str)
{
    int f=5;
	float scale;		/* scale factor */
	int i,				/* copy of f, and # digits before decimal point */
		d;				/* a digit */

	if( x < 0.0 ) {
		*str++ = '-' ;
		x = -x ;
	}
	i = f ;
	scale = 2.0 ;
	while ( i-- )
		scale *= 10.0 ;
	x += 1.0 / scale ;
	/* count places before decimal & scale the number */
	i = 0 ;
	scale = 1.0 ;
	while ( x >= scale ) {
		scale *= 10.0 ;
		i++ ;
	}
	while ( i-- ) {
		/* output digits before decimal */
		scale = floor(0.5 + scale * 0.1 ) ;
		d =  x / scale  ;
		*str++ = d + '0' ;
		x -= (float)(d) * scale ;
	}
	if ( f <= 0 ) {
		*str = 0 ;
		return ;
	}
	*str++ = '.' ;
	while ( f-- ) {
		/* output digits after decimal */
		x *= 10.0 ;
		d = x ;
		*str++ = d + '0' ;
		x -= (float)(d) ;
	}
	*str = 0 ;
}


/**************end of numeric to string and reverse conversion*****************/

