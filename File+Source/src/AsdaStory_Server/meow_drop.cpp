/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains drop class implementation         */
#include "meow_drop.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include "meow_sess.h"
#include "meow_main.h"
#include <stdlib.h>
mw_drop* mw_drops[_MW_MAX_DROPS];
mw_drop::mw_drop()
{
  my_X=100; my_Y=100; my_amount=1; my_class=2; age=0;
  map_id.byte0=0x03; map_id.byte1=0x00;
  type_id.byte0=0x37; type_id.byte1=0x29; //default gold
  int i; 
  for (i=_MW_MAX_DROPS-10; i<_MW_MAX_DROPS; i++) if (mw_drops[i]) delete mw_drops[i];
  for (i=0; i<_MW_MAX_DROPS; i++) if (!mw_drops[i]) {mw_drops[i]=this; break;}
  iid.byte0 =(div(i,256)).rem; //lower byte
  iid.byte1=(div(i,256)).quot; //elder byte
  mpStrCopy(base_fname,"a_drop.ini");
  loadsuccess=false;
  for (i=0; i<_MW_MAX_SESSIONS; i++) remembers_sess[i]=false;
}
mw_drop::~mw_drop()
{
  int i; for (i=0; i<_MW_MAX_DROPS; i++) if (mw_drops[i])
    if (mw_drops[i]==this) mw_drops[i]=0;
}
int mw_drop::getMyNumber()
{
   int i;
   for (i=0; i<_MW_MAX_DROPS; i++) if (mw_drops[i]) if (mw_drops[i]==this) return i;
   return -1;
}
void mw_drop::ReadParamFromString(char* tknleft, char* tknright)
{
   char tkn[63];
   if (mpStrCompare(tknleft,"X")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_X=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"Y")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_Y=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"map"))
   { 
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); map_id.byte0=strtoul(tkn,0,16);
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); map_id.byte1=strtoul(tkn,0,16);
   }
   if (mpStrCompare(tknleft,"dropid"))
   { 
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); type_id.byte0=strtoul(tkn,0,16);
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); type_id.byte1=strtoul(tkn,0,16);
   }
   if (mpStrCompare(tknleft,"amount")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_amount=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"class")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_class=mpStrToInt(tkn);  return; }
}
void mw_drop::LoadFromFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/");
   mpStrAddStr(filename,base_fname);
   mpText* dropfile=ReadSytxTextFromFile(filename); if (!dropfile) return;
   if (dropfile->nstrings<=0) {mpDestroyText(&dropfile); return;}
   char s1[256]; char s2[256]; int i;
   for (i=0; i<dropfile->nstrings; i++)
   {
      mpStrCopy(s2,dropfile->strings[i]);  
      mpStrTrimLeft(s2, ' '); mpGetAndCutLeftField(s1, s2, ' ');
      ReadParamFromString(s1,s2);
   }
   mpDestroyText(&dropfile);
   loadsuccess=true; 
}
void mw_drop::SaveToFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/");
   mpStrAddStr(filename,base_fname);
   mpText* dropfile=CreateEmptyText(); if (!dropfile) return;
   char str[255]; char str2[255];
   mpStrCopy(str, "map "); itoa(map_id.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(map_id.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(dropfile, str);
   mpStrCopy(str, "dropid "); itoa(type_id.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(type_id.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(dropfile, str);
   mpStrCopy(str, "X "); mpIntToStr(str2, my_X); mpStrAddStr(str, str2); 
   mpAppendStrToText(dropfile, str);
   mpStrCopy(str, "Y "); mpIntToStr(str2, my_Y); mpStrAddStr(str, str2); 
   mpAppendStrToText(dropfile, str);
   mpStrCopy(str, "amount "); mpIntToStr(str2, my_amount); mpStrAddStr(str, str2); 
   mpAppendStrToText(dropfile, str);
   mpStrCopy(str, "class "); mpIntToStr(str2, my_class); mpStrAddStr(str, str2); 
   mpAppendStrToText(dropfile, str);
   AddInfoToSave(dropfile);
   mpStrCopy(str, "end");
   mpAppendStrToText(dropfile, str);
   WriteTextToFile (filename, dropfile);
   mpDestroyText(&dropfile); 
}
void mw_drop::send_my_info_to_sess_N(int N_sess)
{
   if (!drop_is_visible_to_sess_N(N_sess, getMyNumber())) return;
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from server
   out_buf[5]=mw_sessions[N_sess]->chr.iid.byte0; out_buf[6]=mw_sessions[N_sess]->chr.iid.byte1; //to sess chr
   out_buf[7]=0x8D; out_buf[8]=0x13; //pkt
   out_buf[9]=type_id.byte0; out_buf[10]=type_id.byte1;
   out_buf[13]=(div(my_X,256)).rem; out_buf[14]=(div(my_X,256)).quot; 
   out_buf[15]=(div(my_Y,256)).rem; out_buf[16]=(div(my_Y,256)).quot; 
   out_buf[17]=(div(my_amount,256)).rem; out_buf[18]=(div(my_amount,256)).quot; 
   out_buf[21]=0x38; out_buf[24]=0xFE; out_buf[0]=0xFB; out_buf[1]=0x19;
   if (!remembers_sess[N_sess]) 
      dll_write_n(mw_sessions[N_sess]->number_at_map, 1, 0x19+1, &(out_buf[0]));
   remembers_sess[N_sess]=true;
}

void mw_drop::progress(unsigned long sv_time)
{
   if (age==0) age=sv_time;
   if ((sv_time-age)>30000) {remove(); return;}
   int i; for (i=0; i<_MW_MAX_SESSIONS; i++)
     if (!drop_is_visible_to_sess_N(i, getMyNumber())) remembers_sess[i]=false;
   for (i=0; i<_MW_MAX_SESSIONS; i++)
     if (drop_is_visible_to_sess_N(i, getMyNumber())) if(!remembers_sess[i])
       send_my_info_to_sess_N(i);
}
void mw_drop::remove()
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   int N_sess; for (N_sess=0; N_sess<_MW_MAX_SESSIONS; N_sess++)
     if (drop_is_visible_to_sess_N(N_sess, getMyNumber()))
     {
        out_buf[3]=0; out_buf[4]=0; //from server
        out_buf[5]=mw_sessions[N_sess]->chr.iid.byte0; out_buf[6]=mw_sessions[N_sess]->chr.iid.byte1; //to sess chr
        out_buf[7]=0x97; out_buf[8]=0x13; //pkt
        out_buf[9]=(div(my_X,256)).rem; out_buf[10]=(div(my_X,256)).quot; 
        out_buf[11]=(div(my_Y,256)).rem; out_buf[12]=(div(my_Y,256)).quot; 
        out_buf[13]=type_id.byte0; out_buf[14]=type_id.byte1;
        out_buf[15]=0xFE; out_buf[0]=0xFB; out_buf[1]=0x10;
        dll_write_n(mw_sessions[N_sess]->number_at_map, 1, 0x10+1, &(out_buf[0]));
     }
   delete this;
}

bool drop_is_visible_to_sess_N(int N_sess, int N_drop)
{
   if ((N_sess<0)||(N_sess>=_MW_MAX_SESSIONS)) return false;
   if ((N_drop<0)||(N_drop>=_MW_MAX_DROPS)) return false;
   if (!mw_sessions[N_sess]) return false;
   if (!mw_drops[N_drop]) return false;
   if (mw_sessions[N_sess]->number_at_map<0) return false;
   if (!mw_sessions[N_sess]->is_online) return false;
   if ((mw_sessions[N_sess]->chr.map_id.byte0!=mw_drops[N_drop]->map_id.byte0)||
       (mw_sessions[N_sess]->chr.map_id.byte1!=mw_drops[N_drop]->map_id.byte1)) return false;
   if (mpmabs(mw_sessions[N_sess]->chr.my_X-mw_drops[N_drop]->my_X)>=_MW_CLIENT_VIEW_DIST) return false;
   if (mpmabs(mw_sessions[N_sess]->chr.my_Y-mw_drops[N_drop]->my_Y)>=_MW_CLIENT_VIEW_DIST) return false;
   return true;
}

void load_drops()
{
   char dropnum[63]; int i; for (i=0; i<_MW_MAX_DROPS; i++)
   {
       mw_drop* newdrop= new mw_drop(); mpStrCopy(newdrop->base_fname, "drop");
       mpIntToStr(dropnum, i); mpStrAddStr(newdrop->base_fname, dropnum);
       mpStrAddStr(newdrop->base_fname, ".ini");
       newdrop->LoadFromFile("_drop");
       if (!newdrop->loadsuccess) { delete newdrop; continue;}
   }
}
void drops_save()
{
   int i; for (i=0; i<_MW_MAX_DROPS; i++) if (mw_drops[i]) mw_drops[i]->SaveToFile("_drop");
}
void put_drop(int X, int Y, int amount, struct mw_id map_id, struct mw_id type_id, int class_id)
{
   mw_drop* old_drop=get_drop_from_coords(X, Y);
   if (old_drop) old_drop->remove();
   mw_drop* newdrop= new mw_drop(); char dropnum[63]; int i;
   for (i=0; i<_MW_MAX_DROPS; i++) if (mw_drops[i]==newdrop)
   {
       mpStrCopy(newdrop->base_fname, "drop"); mpIntToStr(dropnum, i); 
       mpStrAddStr(newdrop->base_fname, dropnum); mpStrAddStr(newdrop->base_fname, ".ini");
       newdrop->map_id.byte0=map_id.byte0; newdrop->map_id.byte1=map_id.byte1;
       newdrop->type_id.byte0=type_id.byte0; newdrop->type_id.byte1=type_id.byte1;
       newdrop->my_X=X; newdrop->my_Y=Y; newdrop->my_amount=amount; newdrop->my_class=class_id;
       break;
   }
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]->number_at_map>=0)
      newdrop->send_my_info_to_sess_N(i);
}
void drop_progress(unsigned long sv_time)
{
   int i; for (i=0; i<_MW_MAX_DROPS; i++) if (mw_drops[i]) mw_drops[i]->progress(sv_time);
}
mw_drop* get_drop_from_coords(int x, int y)
{
   int i; for (i=0; i<_MW_MAX_DROPS; i++) if (mw_drops[i]) 
    if ((mw_drops[i]->my_X==x)&&(mw_drops[i]->my_Y==y)) return mw_drops[i];
   return 0;
}

