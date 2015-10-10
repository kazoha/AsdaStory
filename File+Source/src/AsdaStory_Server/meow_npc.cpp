/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains npc class implementation          */
#include "meow_npc.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include "meow_sess.h"
#include "meow_main.h"
#include <stdlib.h>
mw_npc* mw_npcs[_MW_MAX_NPCS];


mw_npc::mw_npc()
{
   my_X=100; my_Y=100;
   map_id.byte0=3;  map_id.byte1=0; //default - alpen
   type_id.byte0=0x84; type_id.byte1=0; //default Yumi
   int i; for (i=0; i<_MW_MAX_NPCS; i++) if (!mw_npcs[i]) {mw_npcs[i]=this; break;}
   iid.byte0 =(div(i,256)).rem; //lower byte
   iid.byte1=(div(i,256)).quot; //elder byte
   mpStrCopy(base_fname,"an_npc.ini");
   loadsuccess=false; is_mob=false;
   for (i=0; i<_MW_MAX_SESSIONS; i++) remembers_sess[i]=false;
}
mw_npc::~mw_npc()
{
  int i; for (i=0; i<_MW_MAX_NPCS; i++) if (mw_npcs[i])
    if (mw_npcs[i]==this) mw_npcs[i]=0;
}
int mw_npc::getMyNumber()
{
   int i;
   for (i=0; i<_MW_MAX_NPCS; i++) if (mw_npcs[i]) if (mw_npcs[i]==this) return i;
   return -1;
}

void mw_npc::ReadParamFromString(char* tknleft, char* tknright)
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
   if (mpStrCompare(tknleft,"npcid"))
   { 
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); type_id.byte0=strtoul(tkn,0,16);
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); type_id.byte1=strtoul(tkn,0,16);
   }
}
void mw_npc::LoadFromFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/");
   mpStrAddStr(filename,base_fname);
   mpText* npcfile=ReadSytxTextFromFile(filename); if (!npcfile) return;
   if (npcfile->nstrings<=0) {mpDestroyText(&npcfile); return;}
   char s1[256]; char s2[256]; int i;
   for (i=0; i<npcfile->nstrings; i++)
   {
      mpStrCopy(s2,npcfile->strings[i]);  
      mpStrTrimLeft(s2, ' '); mpGetAndCutLeftField(s1, s2, ' ');
      ReadParamFromString(s1,s2);
   }
   mpDestroyText(&npcfile);
   loadsuccess=true; 
}
void mw_npc::SaveToFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/");
   mpStrAddStr(filename,base_fname);
   mpText* npcfile=CreateEmptyText(); if (!npcfile) return;
   char str[255]; char str2[255];
   mpStrCopy(str, "map "); itoa(map_id.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(map_id.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(npcfile, str);
   mpStrCopy(str, "npcid "); itoa(type_id.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(type_id.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(npcfile, str);
   mpStrCopy(str, "X "); mpIntToStr(str2, my_X); mpStrAddStr(str, str2); 
   mpAppendStrToText(npcfile, str);
   mpStrCopy(str, "Y "); mpIntToStr(str2, my_Y); mpStrAddStr(str, str2); 
   mpAppendStrToText(npcfile, str);
   AddInfoToSave(npcfile);
   mpStrCopy(str, "end");
   mpAppendStrToText(npcfile, str);
   WriteTextToFile (filename, npcfile);
   mpDestroyText(&npcfile); 
}

void mw_npc::send_my_info_to_sess_N(int N_sess)
{
   if (!npc_is_visible_to_sess_N(N_sess, getMyNumber())) return;
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from server
   out_buf[5]=0; out_buf[6]=0; //to any
   out_buf[7]=0xE3; out_buf[8]=0x0F; //pkt
   int my_N=getMyNumber();
   out_buf[9]=iid.byte0; out_buf[10]=iid.byte1; //individual id on location
   out_buf[11]=type_id.byte0; out_buf[12]=type_id.byte1; //npc type
   out_buf[13]=0xB9; out_buf[14]=0x00; out_buf[15]=0x00; out_buf[16]=0x00;//??
   out_buf[17]=(div(my_X,256)).rem; //lower byte
   out_buf[18]=(div(my_X,256)).quot; //elder byte
   out_buf[19]=(div(my_Y,256)).rem; //lower byte
   out_buf[20]=(div(my_Y,256)).quot; //elder byte
   out_buf[21]=0x00; out_buf[22]=0x00; out_buf[23]=0x00; out_buf[24]=0x00;
   out_buf[25]=0xFE;
   out_buf[0]=0xFB;  out_buf[1]=26;
   if (!remembers_sess[N_sess]) 
      dll_write_n(mw_sessions[N_sess]->number_at_map, 1, 27, &(out_buf[0]));
   remembers_sess[N_sess]=true;
}

void mw_npc::progress(unsigned long sv_time)
{
   int i; for (i=0; i<_MW_MAX_SESSIONS; i++)
     if (!npc_is_visible_to_sess_N(i, getMyNumber())) remembers_sess[i]=false;
   for (i=0; i<_MW_MAX_SESSIONS; i++)
     if (npc_is_visible_to_sess_N(i, getMyNumber())) if(!remembers_sess[i])
       send_my_info_to_sess_N(i);
}

bool npc_is_visible_to_sess_N(int N_sess, int N_npc)
{
   if ((N_sess<0)||(N_sess>=_MW_MAX_SESSIONS)) return false;
   if ((N_npc<0)||(N_npc>=_MW_MAX_NPCS)) return false;
   if (!mw_sessions[N_sess]) return false;
   if (!mw_npcs[N_npc]) return false;
   if (mw_sessions[N_sess]->number_at_map<0) return false;
   if (!mw_sessions[N_sess]->is_online) return false;
   if ((mw_sessions[N_sess]->chr.map_id.byte0!=mw_npcs[N_npc]->map_id.byte0)||
       (mw_sessions[N_sess]->chr.map_id.byte1!=mw_npcs[N_npc]->map_id.byte1)) return false;
   if (mpmabs(mw_sessions[N_sess]->chr.my_X-mw_npcs[N_npc]->my_X)>=_MW_CLIENT_VIEW_DIST) return false;
   if (mpmabs(mw_sessions[N_sess]->chr.my_Y-mw_npcs[N_npc]->my_Y)>=_MW_CLIENT_VIEW_DIST) return false;
   return true;
}

void load_npcs()
{
   char npcnum[63]; int i; for (i=0; i<_MW_MAX_NPCS; i++)
   {
       mw_npc* newnpc= new mw_npc(); mpStrCopy(newnpc->base_fname, "npc");
       mpIntToStr(npcnum, i); mpStrAddStr(newnpc->base_fname, npcnum);
       mpStrAddStr(newnpc->base_fname, ".ini");
       newnpc->LoadFromFile("_npc");
       if (!newnpc->loadsuccess) { delete newnpc; continue;}
   }
}

void spawn_npc(int X, int Y, struct mw_id map_id, struct mw_id type_id)
{
   mw_npc* newnpc= new mw_npc(); char npcnum[63]; int i;
   for (i=0; i<_MW_MAX_NPCS; i++) if (mw_npcs[i]==newnpc)
   {
       mpStrCopy(newnpc->base_fname, "npc"); mpIntToStr(npcnum, i); 
       mpStrAddStr(newnpc->base_fname, npcnum); mpStrAddStr(newnpc->base_fname, ".ini");
       newnpc->map_id.byte0=map_id.byte0; newnpc->map_id.byte1=map_id.byte1;
       newnpc->type_id.byte0=type_id.byte0; newnpc->type_id.byte1=type_id.byte1;
       newnpc->my_X=X; newnpc->my_Y=Y; 
       break;
   }
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]->number_at_map>=0)
      newnpc->send_my_info_to_sess_N(i);
}

void npc_progress(unsigned long sv_time)
{
   int i; for (i=0; i<_MW_MAX_NPCS; i++) if (mw_npcs[i]) mw_npcs[i]->progress(sv_time);
}

void npcs_save()
{
   int i; for (i=0; i<_MW_MAX_NPCS; i++) if (mw_npcs[i]) mw_npcs[i]->SaveToFile("_npc");
}

