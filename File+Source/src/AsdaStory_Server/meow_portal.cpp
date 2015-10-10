/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains portal class implementation       */
#include "meow_portal.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include "meow_sess.h"
#include "meow_main.h"
#include <stdlib.h>
mw_portal* mw_portals[_MW_MAX_PORTALS];

mw_portal::mw_portal()
{

   my_X=100; my_Y=100; my_R=10;   
   map_id.byte0=0x03;  map_id.byte1=0x00;
   map_to.byte0=0x02;  map_to.byte0=0x00;
   X_to=100; Y_to=100; loadsuccess=false;
   mpStrCopy(base_fname,"a_portal.ini");
   int i; for (i=0; i<_MW_MAX_PORTALS; i++) if (!mw_portals[i]) {mw_portals[i]=this; break;}
}
mw_portal::~mw_portal()
{
  int i; for (i=0; i<_MW_MAX_PORTALS; i++) if (mw_portals[i])
    if (mw_portals[i]==this) mw_portals[i]=0;
}
void mw_portal::ReadParamFromString(char* tknleft, char* tknright)
{
   char tkn[63];
   if (mpStrCompare(tknleft,"X")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_X=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"Y")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_Y=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"X_to")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); X_to=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"Y_to")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); Y_to=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"R")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_R=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"map"))
   { 
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); map_id.byte0=strtoul(tkn,0,16);
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); map_id.byte1=strtoul(tkn,0,16);
   }
   if (mpStrCompare(tknleft,"map_to"))
   { 
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); map_to.byte0=strtoul(tkn,0,16);
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); map_to.byte1=strtoul(tkn,0,16);
   }
}
void mw_portal::LoadFromFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/");
   mpStrAddStr(filename,base_fname);
   mpText* prtfile=ReadSytxTextFromFile(filename); if (!prtfile) return;
   if (prtfile->nstrings<=0) {mpDestroyText(&prtfile); return;}
   char s1[256]; char s2[256]; int i;
   for (i=0; i<prtfile->nstrings; i++)
   {
      mpStrCopy(s2,prtfile->strings[i]);  
      mpStrTrimLeft(s2, ' '); mpGetAndCutLeftField(s1, s2, ' ');
      ReadParamFromString(s1,s2);
   }
   mpDestroyText(&prtfile);
   loadsuccess=true; 
}
void mw_portal::SaveToFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/");
   mpStrAddStr(filename,base_fname);
   mpText* prtfile=CreateEmptyText(); if (!prtfile) return;
   char str[255]; char str2[255];
   mpStrCopy(str, "map "); itoa(map_id.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(map_id.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(prtfile, str);
   mpStrCopy(str, "map_to "); itoa(map_to.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(map_to.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(prtfile, str);
   mpStrCopy(str, "X "); mpIntToStr(str2, my_X); mpStrAddStr(str, str2); 
   mpAppendStrToText(prtfile, str);
   mpStrCopy(str, "Y "); mpIntToStr(str2, my_Y); mpStrAddStr(str, str2); 
   mpAppendStrToText(prtfile, str);
   mpStrCopy(str, "R "); mpIntToStr(str2, my_R); mpStrAddStr(str, str2); 
   mpAppendStrToText(prtfile, str);
   mpStrCopy(str, "X_to "); mpIntToStr(str2, X_to); mpStrAddStr(str, str2); 
   mpAppendStrToText(prtfile, str);
   mpStrCopy(str, "Y_to "); mpIntToStr(str2, Y_to); mpStrAddStr(str, str2); 
   mpAppendStrToText(prtfile, str);
   AddInfoToSave(prtfile);
   mpStrCopy(str, "end");
   mpAppendStrToText(prtfile, str);
   WriteTextToFile (filename, prtfile);
   mpDestroyText(&prtfile); 
}
int mw_portal::getMyNumber()
{
   int i;
   for (i=0; i<_MW_MAX_PORTALS; i++) if (mw_portals[i]) if (mw_portals[i]==this) return i;
   return -1;
}
void mw_portal::do_Warp(int sess_N)
{
  if (sess_N<0) return; if (sess_N>=_MW_MAX_SESSIONS) return;
  if (!mw_sessions[sess_N]) return; if (mw_sessions[sess_N]->number_at_map<0) return;
  if (map_id.byte0!=mw_sessions[sess_N]->chr.map_id.byte0) return;
  if (map_id.byte1!=mw_sessions[sess_N]->chr.map_id.byte1) return;
  if (mpmabs(mw_sessions[sess_N]->chr.my_X-my_X)>my_R) return;
  if (mpmabs(mw_sessions[sess_N]->chr.my_Y-my_Y)>my_R) return;
  //ok warp then
  mw_sessions[sess_N]->chr.map_id.byte0=map_to.byte0;
  mw_sessions[sess_N]->chr.map_id.byte1=map_to.byte1;
  mw_sessions[sess_N]->chr.my_X=X_to; mw_sessions[sess_N]->chr.my_Y=Y_to;
  char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
  //assembling map change packet:
  out_buf[3]=0; out_buf[4]=0;
  out_buf[5]=mw_sessions[sess_N]->chr.iid.byte0; out_buf[6]=mw_sessions[sess_N]->chr.iid.byte1;
  out_buf[7]=0xBF; out_buf[8]=0x13; out_buf[9]=0x01;
  char *ip=&(out_buf[10]); mpStrCopy(ip, my_ip); out_buf[30]=0xC8; out_buf[31]=0x0F; //port
  out_buf[32]=mw_sessions[sess_N]->chr.map_id.byte0; out_buf[33]=mw_sessions[sess_N]->chr.map_id.byte1;
  out_buf[34]=(div(mw_sessions[sess_N]->chr.my_X,256)).rem; out_buf[35]=(div(mw_sessions[sess_N]->chr.my_X,256)).quot;
  out_buf[36]=(div(mw_sessions[sess_N]->chr.my_Y,256)).rem; out_buf[37]=(div(mw_sessions[sess_N]->chr.my_Y,256)).quot;
  out_buf[72]=0xFE; out_buf[0]=0xFB; out_buf[1]=0x49; out_buf[2]=0;
  dll_write_n(mw_sessions[sess_N]->number_at_map, 1, 0x4A, &(out_buf[0]));
}
void load_portals()
{
   char portal_num[63]; int i; for (i=0; i<_MW_MAX_PORTALS; i++)
   {
       mw_portal* newportal= new mw_portal(); mpStrCopy(newportal->base_fname, "portal");
       mpIntToStr(portal_num, i); mpStrAddStr(newportal->base_fname, portal_num);
       mpStrAddStr(newportal->base_fname, ".ini");
       newportal->LoadFromFile("_portals");
       if (!newportal->loadsuccess) { delete newportal; continue;}
   }
}
void portals_save()
{
   int i; for (i=0; i<_MW_MAX_PORTALS; i++) if (mw_portals[i]) mw_portals[i]->SaveToFile("_portals");
}
void set_portal(int X, int Y, struct mw_id map_id, struct mw_id map_to_id)
{
   mw_portal* newportal= new mw_portal(); char portal_num[63]; int i;
   for (i=0; i<_MW_MAX_PORTALS; i++) if (mw_portals[i]==newportal)
   {
       mpStrCopy(newportal->base_fname, "portal"); mpIntToStr(portal_num, i); 
       mpStrAddStr(newportal->base_fname, portal_num); mpStrAddStr(newportal->base_fname, ".ini");
       newportal->map_id.byte0=map_id.byte0; newportal->map_id.byte1=map_id.byte1;
       newportal->my_X=X; newportal->my_Y=Y;  
       newportal->map_to.byte0=map_to_id.byte0; newportal->map_to.byte1=map_to_id.byte1;
       break;
   }
}

