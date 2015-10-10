/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains mob class implementation          */
#include "meow_mob.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include "meow_sess.h"
#include "meow_main.h"
#include <stdlib.h>
mw_mob::mw_mob()
{
   type_id.byte0=0x0F; type_id.byte1=0; //default Red Wolfy
   mpStrCopy(base_fname,"a_mob.ini");
   skip_counter=0; is_mob=true;
   spawn_X=0; spawn_Y=0; hp=500; spawn_hp=0;
}
void mw_mob::ReadParamFromString(char* tknleft, char* tknright)
{
   mw_npc::ReadParamFromString(tknleft, tknright);
}
void mw_mob::AddInfoToSave(void* fText)
{
   mpText* npcfile=(mpText*)fText;
   if (!npcfile) return;
}
void mw_mob::send_mob_move_to_sess_N(int N_sess, int X0, int Y0, int X1, int Y1)
{
   if (!npc_is_visible_to_sess_N(N_sess, getMyNumber())) return;
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from server
   out_buf[5]=0; out_buf[6]=0; //to any
   out_buf[7]=0xAD; out_buf[8]=0x0F; //pkt
   out_buf[9]=iid.byte0; out_buf[10]=iid.byte1; //individual id on location
   out_buf[11]=type_id.byte0; out_buf[12]=type_id.byte1; //npc type
   out_buf[13]=0xEC; out_buf[17]=0x02; //state neutral
   out_buf[18]=0xDE; out_buf[19]=0x02; //hp?
   out_buf[22]=0x60; out_buf[23]=0x07; out_buf[24]=0x10; out_buf[25]=0x27;
   out_buf[28]=(div(X0,256)).rem; //coords from
   out_buf[29]=(div(X0,256)).quot;
   out_buf[30]=(div(Y0,256)).rem;
   out_buf[31]=(div(Y0,256)).quot;
   out_buf[32]=(div(X1,256)).rem; //coords to
   out_buf[33]=(div(X1,256)).quot;
   out_buf[34]=(div(Y1,256)).rem;
   out_buf[35]=(div(Y1,256)).quot;
   out_buf[36]=0x04; out_buf[37]=0x00; //04 is important - dosnt work without
   for (i=38;i<=71;i++) out_buf[i]=0x00;
   out_buf[72]=0xFE;
   out_buf[0]=0xFB;  out_buf[1]=73;
   dll_write_n(mw_sessions[N_sess]->number_at_map, 1, 74, &(out_buf[0]));
}
void mw_mob::send_my_info_to_sess_N(int N_sess)
{
   if (!npc_is_visible_to_sess_N(N_sess, getMyNumber())) return;
}
void mw_mob::progress(unsigned long sv_time)
{
   if (skip_counter<5) {skip_counter++; return;}
   skip_counter=0;
   if ((spawn_X==0)&&(my_X>10)&&(my_X<490)) spawn_X=my_X;
   if ((spawn_Y==0)&&(my_Y>10)&&(my_Y<490)) spawn_Y=my_Y;
   if ((spawn_hp==0)&&(hp>0)) spawn_hp=hp;
   int X0=my_X; int Y0=my_Y;
   my_X+=(rand()-rand())/10000; my_Y+=(rand()-rand())/10000;
   int i; for (i=0; i<_MW_MAX_SESSIONS; i++)
     if (npc_is_visible_to_sess_N(i, getMyNumber())) 
       send_mob_move_to_sess_N(i, X0, Y0, my_X, my_Y);
}
void mw_mob::atk_back(int N_sess)
{
   if (!npc_is_visible_to_sess_N(N_sess, getMyNumber())) return;
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from server
   out_buf[5]=mw_sessions[N_sess]->chr.iid.byte0; out_buf[6]=mw_sessions[N_sess]->chr.iid.byte1; //to player
   out_buf[7]=0xAF; out_buf[8]=0x0F;
   out_buf[9]=mw_sessions[N_sess]->chr.iid.byte0; out_buf[10]=mw_sessions[N_sess]->chr.iid.byte1; //abt player
   out_buf[11]=0x26; out_buf[11]=0x00; //??
   out_buf[13]=iid.byte0; out_buf[14]=iid.byte1; //mob id
   out_buf[15]=3;//??
   out_buf[16]=(div(my_X,256)).rem; out_buf[17]=(div(my_X,256)).quot;
   out_buf[18]=(div(my_Y,256)).rem; out_buf[19]=(div(my_Y,256)).quot;
   int mob_dmg[3]={10,11,12};
   out_buf[28]=0x10; out_buf[29]=0x27; //attack sequence
   out_buf[30]=(div(mob_dmg[0],256)).rem; out_buf[31]=(div(mob_dmg[0],256)).quot;
   out_buf[42]=0xFE; int sz=44; //shorter size causes permanent criticals
   out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0;
   dll_write_n(mw_sessions[N_sess]->number_at_map, 1, sz, &(out_buf[0]));
}
void mw_mob::die()
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from server
   out_buf[7]=0xB1; out_buf[8]=0x0F;
   out_buf[9]=0x26; out_buf[10]=0x00; //??
   out_buf[11]=iid.byte0; out_buf[12]=iid.byte1; //mob id
   out_buf[42]=0xFE; int sz=44; 
   out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0;
   int N_sess; for (N_sess=0; N_sess<_MW_MAX_SESSIONS; N_sess++)
   if (npc_is_visible_to_sess_N(N_sess, getMyNumber()))
   {
     out_buf[5]=mw_sessions[N_sess]->chr.iid.byte0; out_buf[6]=mw_sessions[N_sess]->chr.iid.byte1; //to player
     dll_write_n(mw_sessions[N_sess]->number_at_map, 1, sz, &(out_buf[0]));
   }
   my_X=spawn_X; my_Y=spawn_Y; hp=spawn_hp; skip_counter=0;
}

void load_mobs()
{
   char npcnum[63]; int i; for (i=0; i<_MW_MAX_NPCS; i++)
   {
       mw_mob* newnpc= new mw_mob(); mpStrCopy(newnpc->base_fname, "mob");
       mpIntToStr(npcnum, i); mpStrAddStr(newnpc->base_fname, npcnum);
       mpStrAddStr(newnpc->base_fname, ".ini");
       newnpc->LoadFromFile("_npc");
       if (!newnpc->loadsuccess) { delete newnpc; continue;}
   }
}

void spawn_mob(int X, int Y, struct mw_id map_id, struct mw_id type_id)
{
   mw_mob* newmob= new mw_mob(); char mobnum[63]; int i;
   for (i=0; i<_MW_MAX_NPCS; i++) if (mw_npcs[i]==newmob)
   {
       mpStrCopy(newmob->base_fname, "mob"); mpIntToStr(mobnum, i); 
       mpStrAddStr(newmob->base_fname, mobnum); mpStrAddStr(newmob->base_fname, ".ini");
       newmob->map_id.byte0=map_id.byte0; newmob->map_id.byte1=map_id.byte1;
       newmob->type_id.byte0=type_id.byte0; newmob->type_id.byte1=type_id.byte1;
       newmob->my_X=X; newmob->my_Y=Y; 
       break;
   }
//   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]->number_at_map>=0)
//      newmob->send_my_info_to_sess_N(i);
}


