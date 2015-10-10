/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains session class implementation      */
#include "meow_sess.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include <stdlib.h>
#include <math.h>
#include "meow_nice_pkt.h"
#include "meow_main.h"
#include "meow_npc.h"
#include "meow_mob.h"
#include "meow_drop.h"
#include "meow_portal.h"

mw_session* mw_sessions[_MW_MAX_SESSIONS];
char my_ip[31];
static void do_parse_chat(int SessNumber, char *chat)
{
   if (chat[0]!='@') return; if (mw_sessions[SessNumber]==0) return;
   char cmd_chat[255]; int i; for (i=0; i<255; i++) cmd_chat[i]=0;
   i=1; while ((chat[i]!=0)&&((unsigned char)chat[i]!=0xFE)) {cmd_chat[i-1]=chat[i]; i++;}
   mpStrTrimLeft(cmd_chat, ' ');
   char cmd_token[63]; mpGetAndCutLeftField(cmd_token, cmd_chat, ' '); mpStrTrimLeft(cmd_chat, ' ');
   char token1[63]; mpGetAndCutLeftField(token1, cmd_chat, ' '); mpStrTrimLeft(cmd_chat, ' ');
   char token2[63]; mpGetAndCutLeftField(token2, cmd_chat, ' '); mpStrTrimLeft(cmd_chat, ' ');
   printf("session %d uses GM command %s\n", SessNumber, cmd_token);
   if (mpStrCompare(cmd_token,"mount")||mpStrCompare(cmd_token,"m")) 
   {
     struct mw_id veh_id; 
     veh_id.byte0=0; veh_id.byte1=0;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) veh_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) veh_id.byte1=strtoul(token2,0,16); 
     if (mpStrCompare(token1,"bike")) {veh_id.byte0=0x9E; veh_id.byte1=0x00;}
     if (mpStrCompare(token1,"sweep")) {veh_id.byte0=0xA5; veh_id.byte1=0x00;}
     if (mpStrCompare(token1,"kidbike")) {veh_id.byte0=0xB1; veh_id.byte1=0x00;}
     if (mpStrCompare(token1,"drake")) {veh_id.byte0=0xF3; veh_id.byte1=0x00;}
     if (mpStrCompare(token1,"track")) {veh_id.byte0=0xE1; veh_id.byte1=0x00;}
     if (mpStrCompare(token1,"pigs")) {veh_id.byte0=0xC1; veh_id.byte1=0x00;}
     for (i=0; i<_MW_MAX_SESSIONS; i++)
        if (sessA_is_visible_to_sess_B(mw_sessions[SessNumber]->getSessionNumber(), i))
          mw_sessions[SessNumber]->do_mount(mw_sessions[SessNumber]->chr.iid,veh_id, i);
   }
   if (mpStrCompare(cmd_token,"spawn")||mpStrCompare(cmd_token,"s")) 
   {
     struct mw_id npc_id; 
     npc_id.byte0=0x0F; npc_id.byte1=0x00;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) npc_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) npc_id.byte1=strtoul(token2,0,16); 
     if (mpStrCompare(token1,"redwolf")) {npc_id.byte0=0x0F; npc_id.byte1=0x00;}
     spawn_mob(mw_sessions[SessNumber]->chr.my_X+1,mw_sessions[SessNumber]->chr.my_Y+1, 
               mw_sessions[SessNumber]->chr.map_id, npc_id);
   }
   if (mpStrCompare(cmd_token,"eq")||mpStrCompare(cmd_token,"equip")) 
   {
     struct mw_id itm_id; itm_id.byte0=0xFD; itm_id.byte1=0x36;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) itm_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) itm_id.byte1=strtoul(token2,0,16); 
     char out_buf[2048]; 
     int sz=mw_sessions[SessNumber]->equip.DoAddToInventory(out_buf, itm_id);
     out_buf[5]=mw_sessions[SessNumber]->chr.iid.byte0;
     out_buf[6]=mw_sessions[SessNumber]->chr.iid.byte1;
     if (sz>20) dll_write_n(mw_sessions[SessNumber]->number_at_map, 1, sz, &(out_buf[0]));
   }
   if (mpStrCompare(cmd_token,"lt")||mpStrCompare(cmd_token,"loot")) 
   {
     struct mw_id itm_id; itm_id.byte0=0x4B; itm_id.byte1=0x29;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) itm_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) itm_id.byte1=strtoul(token2,0,16); 
     char out_buf[2048]; 
     int sz=mw_sessions[SessNumber]->loot.DoAddToInventory(out_buf, itm_id, 10);
     out_buf[5]=mw_sessions[SessNumber]->chr.iid.byte0;
     out_buf[6]=mw_sessions[SessNumber]->chr.iid.byte1;
     if (sz>20) dll_write_n(mw_sessions[SessNumber]->number_at_map, 1, sz, &(out_buf[0]));
   }
   if (mpStrCompare(cmd_token,"go")||mpStrCompare(cmd_token,"map")) 
   {
     struct mw_id map_id; map_id.byte0=mw_sessions[SessNumber]->chr.map_id.byte0; 
     map_id.byte1=mw_sessions[SessNumber]->chr.map_id.byte1;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) map_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) map_id.byte1=strtoul(token2,0,16); 
     if ((map_id.byte0==mw_sessions[SessNumber]->chr.map_id.byte0)&&
         (map_id.byte1==mw_sessions[SessNumber]->chr.map_id.byte1)) return;
     mw_sessions[SessNumber]->chr.map_id.byte0=map_id.byte0;
     mw_sessions[SessNumber]->chr.map_id.byte1=map_id.byte1;
     mw_sessions[SessNumber]->chr.my_X=150; mw_sessions[SessNumber]->chr.my_Y=150;
     char out_buf[2048]; for (i=0; i<2048; i++) out_buf[i]=0;

   //assembling map change packet:
     out_buf[3]=0; out_buf[4]=0;
     out_buf[5]=mw_sessions[SessNumber]->chr.iid.byte0; out_buf[6]=mw_sessions[SessNumber]->chr.iid.byte1;
     out_buf[7]=0xBF; out_buf[8]=0x13; out_buf[9]=0x01;
     char *ip=&(out_buf[10]); mpStrCopy(ip, my_ip); out_buf[30]=0xC8; out_buf[31]=0x0F; //port
     out_buf[32]=mw_sessions[SessNumber]->chr.map_id.byte0; out_buf[33]=mw_sessions[SessNumber]->chr.map_id.byte1;
     out_buf[34]=(div(mw_sessions[SessNumber]->chr.my_X,256)).rem; out_buf[35]=(div(mw_sessions[SessNumber]->chr.my_X,256)).quot;
     out_buf[36]=(div(mw_sessions[SessNumber]->chr.my_Y,256)).rem; out_buf[37]=(div(mw_sessions[SessNumber]->chr.my_Y,256)).quot;
     out_buf[72]=0xFE; out_buf[0]=0xFB; out_buf[1]=0x49; out_buf[2]=0;
     dll_write_n(mw_sessions[SessNumber]->number_at_map, 1, 0x4A, &(out_buf[0]));
   }
   if (mpStrCompare(cmd_token,"jump")||mpStrCompare(cmd_token,"j"))
   {
      int xx=100; int yy=100;
      if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<5)) xx=strtoul(token1,0,10); 
      if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<5)) yy=strtoul(token2,0,10);
      if (xx<30) xx=30; if (xx>450) xx=450;  if (yy<30) yy=30; if (yy>450) yy=450; 
      mw_sessions[SessNumber]->chr.my_X=xx; mw_sessions[SessNumber]->chr.my_Y=yy;
      mw_sessions[SessNumber]->do_move_other(mw_sessions[SessNumber]->chr.iid, xx-1, yy-1, xx,yy);
   }
   if (mpStrCompare(cmd_token,"npc")||mpStrCompare(cmd_token,"n")) 
   {
     struct mw_id npc_id; 
     npc_id.byte0=0x84; npc_id.byte1=0x00;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) npc_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) npc_id.byte1=strtoul(token2,0,16); 
     if (mpStrCompare(token1,"yumi")) {npc_id.byte0=0x84; npc_id.byte1=0x00;}
     if (mpStrCompare(token1,"goyo")) {npc_id.byte0=0x83; npc_id.byte1=0x00;}
     if (mpStrCompare(token1,"r2d2")) {npc_id.byte0=0x92; npc_id.byte1=0x00;}
     if (mpStrCompare(token1,"sandres")) {npc_id.byte0=0x04; npc_id.byte1=0x00;}
     spawn_npc(mw_sessions[SessNumber]->chr.my_X+1,mw_sessions[SessNumber]->chr.my_Y+1, 
               mw_sessions[SessNumber]->chr.map_id, npc_id);
   }
   if (mpStrCompare(cmd_token,"save")||mpStrCompare(cmd_token,"store")) 
      mw_sessions[SessNumber]->SaveToFile(mw_sessions[SessNumber]->login);
   if (mpStrCompare(cmd_token,"savenpc")) npcs_save();
   if (mpStrCompare(cmd_token,"loadnpc")) {load_npcs(); load_mobs();}
   if (mpStrCompare(cmd_token,"saveportals")) portals_save();
   if (mpStrCompare(cmd_token,"loadportals")) load_portals();
   if (mpStrCompare(cmd_token,"setportal")||mpStrCompare(cmd_token,"set")) 
   {
     struct mw_id map_to_id; 
     map_to_id.byte0=0x03; map_to_id.byte1=0x00;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) map_to_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) map_to_id.byte1=strtoul(token2,0,16); 
     if (mpStrCompare(token1,"alpen")) {map_to_id.byte0=0x03; map_to_id.byte1=0x00;}
     if (mpStrCompare(token1,"sunny")) {map_to_id.byte0=0x06; map_to_id.byte1=0x00;}
     if (mpStrCompare(token1,"rein")) {map_to_id.byte0=0x01; map_to_id.byte1=0x00;}
     set_portal(mw_sessions[SessNumber]->chr.my_X,mw_sessions[SessNumber]->chr.my_Y, 
               mw_sessions[SessNumber]->chr.map_id, map_to_id);
   }
   if (mpStrCompare(cmd_token,"savedrop")) drops_save();
   if (mpStrCompare(cmd_token,"loaddrop")) load_drops();
   if (mpStrCompare(cmd_token,"put")||mpStrCompare(cmd_token,"p")) 
   {
     struct mw_id drop_id; 
     drop_id.byte0=0x25; drop_id.byte1=0x2A;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) drop_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) drop_id.byte1=strtoul(token2,0,16); 
     put_drop(mw_sessions[SessNumber]->chr.my_X+1,mw_sessions[SessNumber]->chr.my_Y+1, 1,
               mw_sessions[SessNumber]->chr.map_id, drop_id, 2);
   }
   if (mpStrCompare(cmd_token,"effect")||mpStrCompare(cmd_token,"e")) 
   {
     struct mw_id effect_id; 
     effect_id.byte0=0x41; effect_id.byte1=0x00;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) effect_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) effect_id.byte1=strtoul(token2,0,16); 
     mw_sessions[SessNumber]->do_chr_effect(mw_sessions[SessNumber]->chr.iid, effect_id);
   }
   if (mpStrCompare(cmd_token,"effect2")||mpStrCompare(cmd_token,"e2")) 
   {
     struct mw_id effect_id; 
     effect_id.byte0=0x04; effect_id.byte1=0x13;
     if ((mpStrLen(token1)>1)&&(mpStrLen(token1)<4)) effect_id.byte0=strtoul(token1,0,16); 
     if ((mpStrLen(token2)>1)&&(mpStrLen(token2)<4)) effect_id.byte1=strtoul(token2,0,16); 
     mw_sessions[SessNumber]->do_chr_effect2(mw_sessions[SessNumber]->chr.iid, effect_id);
   }
}
bool sessA_is_visible_to_sess_B(int N_sessA, int N_sessB)
{
   if ((N_sessA<0)||(N_sessA>=_MW_MAX_SESSIONS)) return false;
   if ((N_sessB<0)||(N_sessB>=_MW_MAX_SESSIONS)) return false;
   if (!mw_sessions[N_sessA]) return false;
   if (!mw_sessions[N_sessB]) return false;
   if (mw_sessions[N_sessA]->number_at_map<0) return false;
   if (mw_sessions[N_sessB]->number_at_map<0) return false;
   if (!mw_sessions[N_sessA]->is_online) return false;
   if (!mw_sessions[N_sessB]->is_online) return false;
   if ((mw_sessions[N_sessA]->chr.map_id.byte0!=mw_sessions[N_sessB]->chr.map_id.byte0)||
       (mw_sessions[N_sessA]->chr.map_id.byte1!=mw_sessions[N_sessB]->chr.map_id.byte1)) return false;
   if (mpmabs(mw_sessions[N_sessA]->chr.my_X-mw_sessions[N_sessB]->chr.my_X)>=_MW_CLIENT_VIEW_DIST) return false;
   if (mpmabs(mw_sessions[N_sessA]->chr.my_Y-mw_sessions[N_sessB]->chr.my_Y)>=_MW_CLIENT_VIEW_DIST) return false;
   return true;
}



mw_session::mw_session()
{
  //mpStrCopy(login,"_");
 // mpStrCopy(password,"_");
  mpStrCopy(base_fname,"session.ini");
  number_at_chr=-1; number_at_map=-1; sess_pseudo_id=0;
  mov_spd[0]=0xAE; mov_spd[1]=0x47; mov_spd[2]=0xE1; mov_spd[3]=0x3E; //default spd
  loadsuccess=false;
  int i; for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]==0) 
  {mw_sessions[i]=this; break;}
  for (i=0; i<_MW_MAX_SESSIONS; i++) remembers_sess[i]=false;
  is_online=false; is_entering_map=false;
}
mw_session::~mw_session()
{
  int i; for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i])
    if (mw_sessions[i]==this) mw_sessions[i]=0;
}
void mw_session::ReadParamFromString(char* tknleft, char* tknright)
{
   if (mpStrCompare(tknleft,"login")) 
   { mpStrTrimLeft(tknright, ' '); mpStrCopy(login, tknright); return;  }
   if (mpStrCompare(tknleft,"password")) 
   { mpStrTrimLeft(tknright, ' '); mpStrCopy(password, tknright); return;  }
}
void mw_session::LoadFromFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/"); 
   mpStrAddStr(filename,base_fname);
   mpText* sessfile=ReadSytxTextFromFile(filename); if (!sessfile) return;
   if (sessfile->nstrings<=0) {mpDestroyText(&sessfile); return;}
   char s1[256]; char s2[256]; int i;
   for (i=0; i<sessfile->nstrings; i++)
   {
      mpStrCopy(s2,sessfile->strings[i]);  
      mpStrTrimLeft(s2, ' '); mpGetAndCutLeftField(s1, s2, ' ');
      ReadParamFromString(s1,s2);
   }
   mpDestroyText(&sessfile);
   if (!mpStrCompare(dirname,login)) return;
   chr.LoadFromFile(dirname);
   if ((chr.vehicle_id.byte0==0)&&(chr.vehicle_id.byte1==0))
   {
     mov_spd[0]=0xAE; mov_spd[1]=0x47; mov_spd[2]=0xE1; mov_spd[3]=0x3E; //default spd
   } 
   else
   {
     mov_spd[0]=0xAE; mov_spd[1]=0x47; mov_spd[2]=0xE1; mov_spd[3]=0x3F; //fast spd
   } 
   loot.LoadFromFile(dirname);
   equip.LoadFromFile(dirname);
   gear.LoadFromFile(dirname);
   skills.LoadFromFile(dirname);
   loadsuccess=true;
}
void mw_session::SaveToFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/"); 
   mpStrAddStr(filename,base_fname);
   mpText* sessfile=CreateEmptyText(); if (!sessfile) return;
   char str[255]; char str2[255];
   mpStrCopy(str, "login "); mpStrAddStr(str, login);
 //  printf("login %s",str);
   mpAppendStrToText(sessfile, str);
   mpStrCopy(str, "password "); mpStrAddStr(str, password);
  // printf("senha %s ",str);
   mpAppendStrToText(sessfile, str);
   mpStrCopy(str, "end");
   mpAppendStrToText(sessfile, str);
   WriteTextToFile (filename, sessfile);
   mpDestroyText(&sessfile);
   chr.SaveToFile(dirname);
   loot.SaveToFile(dirname);
   equip.SaveToFile(dirname);
   gear.SaveToFile(dirname);
   skills.SaveToFile(dirname);
}
void mw_session::progress(unsigned long sv_time)
{
   if (number_at_map<0) return; if (!is_online) return;
   int i; for (i=0; i<_MW_MAX_SESSIONS; i++)
     if (!sessA_is_visible_to_sess_B(i, getSessionNumber())) remembers_sess[i]=false;
   for (i=0; i<_MW_MAX_SESSIONS; i++)
     if (sessA_is_visible_to_sess_B(i, getSessionNumber())) if(!remembers_sess[i])
       get_inform_abt_N(i);
   char out_buf[2048];
   int sz=loot.SetGold(chr.money, out_buf);
   if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
}
int mw_session::getSessionNumber()
{
   int i;
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]==this) return i;
   return -1;
}
void mw_session::send_invalid_login()
{//use this method if login failed
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[7]=0xEC; out_buf[8]=0x03; out_buf[9]=0x02; out_buf[10]=0x00;
   out_buf[11]=0xFF; out_buf[12]=0xFF; out_buf[13]=0xFF; out_buf[14]=0xFF;   
   out_buf[15]=0x01; out_buf[16]=0x75; out_buf[17]=0x05; out_buf[18]=0x16;
   out_buf[25]=0xFE; out_buf[0]=0xFB; out_buf[1]=0x1A; out_buf[2]=0x0;
   dll_write_n(number_at_chr, 0, 27, &(out_buf[0]));
}
void mw_session::enter_char_create() //*2 -- apenas enviando esses pacotes, ele recebe o incom_buf[7]= (int)246, responsável por criar o char
{
	  char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[7]=0xEC; out_buf[8]=0x03; out_buf[9]=0x01; out_buf[10]=0x00;
   sess_pseudo_id=getSessionNumber()+110;
   out_buf[11]=div(sess_pseudo_id,256).rem; out_buf[12]=div(sess_pseudo_id,256).quot;
   out_buf[13]=0; out_buf[14]=0;   
   out_buf[15]=0x01; out_buf[16]=0x75; out_buf[17]=0x05; out_buf[18]=0x16;
   out_buf[25]=0xFE; out_buf[0]=0xFB; out_buf[1]=0x1A; out_buf[2]=0x0;
   dll_write_n(number_at_chr, 0, 27, &(out_buf[0]));
   
   
   
}

void mw_session::enter_map()
{//makes 3 essential packets from char server to client for it to
 //connect to map server and enter location
	 
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[7]=0xEC; out_buf[8]=0x03; out_buf[9]=0x01; out_buf[10]=0x00;
   sess_pseudo_id=getSessionNumber()+110;
   out_buf[11]=div(sess_pseudo_id,256).rem; out_buf[12]=div(sess_pseudo_id,256).quot;
   out_buf[13]=0; out_buf[14]=0;   
   out_buf[15]=0x01; out_buf[16]=0x75; out_buf[17]=0x05; out_buf[18]=0x16;
   out_buf[25]=0xFE; out_buf[0]=0xFB; out_buf[1]=0x1A; out_buf[2]=0x0; 
   dll_write_n(number_at_chr, 0, 27, &(out_buf[0])); // pacote 1
   printf ("session %d assigned code %d\n", getSessionNumber(), sess_pseudo_id);
   for (i=0; i<2048; i++) out_buf[i]=0; for (i=0; i<=chr_info_pkt_sz; i++) out_buf[i]=chr_info_pkt[i];
   out_buf[9]=div(sess_pseudo_id,256).rem; out_buf[10]=div(sess_pseudo_id,256).quot; out_buf[11]=0; out_buf[12]=0; 
   out_buf[13]=div(sess_pseudo_id,256).rem; out_buf[14]=div(sess_pseudo_id,256).quot; out_buf[15]=0; out_buf[16]=0; 
   char *nm=&(out_buf[17]); mpStrCopy(nm, chr.chr_name);
   out_buf[40]=chr.gender;
   out_buf[41]=chr.job.byte0; out_buf[42]=chr.job.byte1; out_buf[43]=chr.lvl;
   out_buf[52]=div(chr.sk_pnt,256).rem; out_buf[53]=div(chr.sk_pnt,256).quot;
   out_buf[57]=(div(chr.m_weight,256)).rem;/*lower byte*/  out_buf[58]=(div(chr.m_weight,256)).quot;//elder byte
   out_buf[59]=(div(chr.weight,256)).rem;  /*lower byte*/  out_buf[60]=(div(chr.weight,256)).quot;  //elder byte
   out_buf[61]=(div(chr.macs,256)).rem;    /*lower byte*/  out_buf[62]=(div(chr.macs,256)).quot;    //elder byte   
   out_buf[79]=chr.hair; out_buf[80]=chr.color; out_buf[81]=chr.face; out_buf[82]=chr.constelation;
   out_buf[82]=(div(chr.m_hp,256)).rem;    /*lower byte max hp*/  out_buf[83]=(div(chr.m_hp,256)).quot;//elder byte
   out_buf[84]=(div(chr.m_mp,256)).rem;    /*lower byte max mp*/  out_buf[85]=(div(chr.m_mp,256)).quot;//elder byte
   out_buf[86]=(div(chr.hp,256)).rem;      /*lower byte hp*/      out_buf[87]=(div(chr.hp,256)).quot;//elder byte
   out_buf[88]=(div(chr.mp,256)).rem;      /*lower byte mp*/      out_buf[89]=(div(chr.mp,256)).quot;//elder byte
      //90-159 - char stats TODO :set them
      
  

	 
    dll_write_n(number_at_chr, 0, chr_info_pkt_sz+1, &(out_buf[0])); // pacote 2
      
   for (i=0; i<2048; i++) out_buf[i]=0;
     
   int sz=gear.MakeInfoPkt(out_buf); if (sz>20) 
   {
     out_buf[10]=div(sess_pseudo_id,256).rem; out_buf[11]=div(sess_pseudo_id,256).quot; 
     out_buf[12]=0; out_buf[13]=0; dll_write_n(number_at_chr, 0, sz, &(out_buf[0]));  
   }
   for (i=0; i<2048; i++) out_buf[i]=0;
   //assembling map info packet:
   out_buf[7]=0xFD; out_buf[8]=0x03;  char *ip=&(out_buf[13]); mpStrCopy(ip, my_ip);
   out_buf[33]=0xC8; out_buf[34]=0x0F; //port
   out_buf[35]=chr.map_id.byte0; out_buf[36]=chr.map_id.byte1;
   out_buf[37]=(div(chr.my_X,256)).rem; out_buf[38]=(div(chr.my_X,256)).quot;
   out_buf[39]=(div(chr.my_Y,256)).rem; out_buf[40]=(div(chr.my_Y,256)).quot;
//   out_buf[45]=0xFE; sz=47;  out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0;
   out_buf[44]=0xFE; sz=46;  out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0;	  printf ("pacote para entrar no mapa\n");
   dll_write_n(number_at_chr, 0, sz, &(out_buf[0])); // pacote 3
   //number_at_chr=-1;//DONT DO IT HERE! (it must be done in map thread)
  
   // *3 (recebe 21 pelo wpe) não está retornando o in>FB 3A 00 00 00 00 00 20 4E 0A 45 F5 C9 97 9B 00 00 9C 82 FF 98 88 98 E0 6A FC 03 5F 97 62 02 B8 42 03 00 00 00 FF FF FF FF FF FF FF FF FF FF FF FF 6E 00 00 00 FF FF FF

}
void mw_session::reply_id(bool do_double)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   for (i=0; i<=map_send_id_pkt_sz; i++) out_buf[i]=map_send_id_pkt[i];
   chr.iid.byte0=(div(getSessionNumber(),256)).rem;//lower byte
   chr.iid.byte1=(div(getSessionNumber(),256)).quot;//elder byte
   printf ("session %d chr id %d %d\n", getSessionNumber(), (int)chr.iid.byte0, (int)chr.iid.byte1);
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;
   

	 
	 
   dll_write_n(number_at_map, 1, map_send_id_pkt_sz+1, &(out_buf[0]));
   if (do_double) dll_write_n(number_at_map, 1, map_send_id_pkt_sz+1, &(out_buf[0]));
   is_entering_map=true; dd_counter=0;
}
void mw_session::map_reply_to_map_enter(bool do_double)
{//makes responce of map server to client connection
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   for (i=0; i<=map_enter_pkt1_sz; i++) out_buf[i]=map_enter_pkt1[i];
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;   
   dll_write_n(number_at_map, 1, map_enter_pkt1_sz+1, &(out_buf[0]));
   if (do_double) dll_write_n(number_at_map, 1, map_enter_pkt1_sz+1, &(out_buf[0]));
   for (i=0; i<2048; i++) out_buf[i]=0;
   for (i=0; i<=map_enter_pkt2_sz; i++) out_buf[i]=map_enter_pkt2[i];
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;   
   dll_write_n(number_at_map, 1, map_enter_pkt2_sz+1, &(out_buf[0]));
   if (do_double) dll_write_n(number_at_map, 1, map_enter_pkt2_sz+1, &(out_buf[0]));
   int sz=equip.MakeInfoPkt(out_buf); if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   if (do_double) if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   sz=loot.MakeInfoPkt(out_buf); if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   if (do_double) if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   sz=skills.MakeInfoPkt(out_buf); if (sz>10) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   if (do_double) if (sz>10) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   sz=chr.MakeSkillInfoPkt(out_buf); if (sz>10) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   if (do_double) if (sz>10) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   for (i=0; i<2048; i++) out_buf[i]=0;
   for (i=0; i<=map_setcoords_pkt_sz; i++) out_buf[i]=map_setcoords_pkt[i];
   out_buf[3]=chr.iid.byte0; out_buf[4]=chr.iid.byte1;   
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;   
   out_buf[9]=chr.iid.byte0; out_buf[10]=chr.iid.byte1;   
   out_buf[11]=(div(chr.my_X,256)).rem; //lower byte
   out_buf[12]=(div(chr.my_X,256)).quot; //elder byte
   out_buf[13]=(div(chr.my_Y,256)).rem; //lower byte
   out_buf[14]=(div(chr.my_Y,256)).quot; //elder byte
   dll_write_n(number_at_map, 1, map_setcoords_pkt_sz+1, &(out_buf[0]));
   if (do_double) dll_write_n(number_at_map, 1, map_setcoords_pkt_sz+1, &(out_buf[0]));
}
void mw_session::map_post_enter()
{
 //finalizes session enter
   is_entering_map=false; is_online=true;
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   for (i=0; i<_MW_MAX_SESSIONS; i++) do_inform_N_abt_me(i);
   for (i=0; i<_MW_MAX_SESSIONS; i++) get_inform_abt_N(i);
   if ((chr.vehicle_id.byte0!=0)||(chr.vehicle_id.byte1!=0)) do_mount(chr.iid, chr.vehicle_id, getSessionNumber());
}
void mw_session::map_disconnect()
{
   //is_online=false;
   number_at_map=-1;  
}

int mw_session::my_info_pkt_prepare(char* out_buf)
{
   int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=chr.iid.byte0; out_buf[4]=chr.iid.byte1; //from me
   out_buf[5]=0; out_buf[6]=0; //to any
   out_buf[7]=0xA5; out_buf[8]=0x0F; //pkt
   out_buf[9]=chr.iid.byte0; out_buf[10]=chr.iid.byte1; //abt me
   out_buf[11]=(div(chr.my_X,256)).rem; //lower byte
   out_buf[12]=(div(chr.my_X,256)).quot; //elder byte
   out_buf[13]=(div(chr.my_Y,256)).rem; //lower byte
   out_buf[14]=(div(chr.my_Y,256)).quot; //elder byte
   //15-19 skip
   out_buf[15]=0x01; out_buf[16]=0xD0; out_buf[17]=0xF4; out_buf[18]=0x03;//??
   //15-19 skip
   mpStrCopy(&(out_buf[20]), chr.chr_name);
   out_buf[40]=0x0B;//??
   out_buf[43]=chr.gender;
   out_buf[44]=chr.job.byte0; out_buf[45]=chr.job.byte1;
   out_buf[57]=0x03;//??
   //43-57 skip quests
   out_buf[60]=(div(chr.m_weight,256)).rem;
   out_buf[61]=(div(chr.m_weight,256)).quot;
   out_buf[62]=(div(chr.weight,256)).rem;
   out_buf[63]=(div(chr.weight,256)).quot;
   out_buf[64]=(div(chr.macs,256)).rem;
   out_buf[65]=(div(chr.macs,256)).quot;
   out_buf[73]=0x03;//??
   //65-71 skip something unnown - usually zeros there
   out_buf[78]=0xFF; out_buf[79]=0xFF;//separator
   out_buf[80]=0; out_buf[81]=0;
  // out_buf[80]=chr.constelation; out_buf[81]=0;
   out_buf[82]=chr.hair; out_buf[83]=chr.color; out_buf[84]=chr.face; out_buf[85]=chr.constelation;
   out_buf[86]=chr.iid.byte0; out_buf[87]=chr.iid.byte0;  //repeat id
   //gearment:
   int j=110; for (i=0; i<=10; i++) j=gear.MakeSlotSegment(out_buf,j,i);

//temp finalize
   out_buf[j]=0xFE; out_buf[0]=0xFB; //leader and tail
   out_buf[1]=(div(j+1,256)).rem; out_buf[2]=(div(j+1,256)).quot; //size
   int sz=j+2; 
   return sz;
}
void mw_session::do_inform_N_abt_me(int N)
{
   if ((N<0)||(N>=_MW_MAX_SESSIONS)) return;
   if (!mw_sessions[N]) return; if (mw_sessions[N]==this)  return;
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   int sz=my_info_pkt_prepare(out_buf);
   if (sessA_is_visible_to_sess_B(getSessionNumber(), N))
   { 
      dll_write_n(mw_sessions[N]->number_at_map, 1, sz, &(out_buf[0])); //inform N
      mw_sessions[N]->remembers_sess[getSessionNumber()]=true;
      if ((chr.vehicle_id.byte0!=0)||(chr.vehicle_id.byte1!=0)) do_mount(chr.iid, chr.vehicle_id, N);
   }
}
void mw_session::get_inform_abt_N(int N)
{
   if ((N<0)||(N>=_MW_MAX_SESSIONS)) return;
   if (!mw_sessions[N]) return; if (mw_sessions[N]==this)  return;
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   int sz=mw_sessions[N]->my_info_pkt_prepare(out_buf);
   if (sessA_is_visible_to_sess_B(getSessionNumber(), N))
   {
      dll_write_n(number_at_map, 1, sz, &(out_buf[0])); //get inform abt N
      remembers_sess[N]=true;
      if ((mw_sessions[N]->chr.vehicle_id.byte0!=0)||(mw_sessions[N]->chr.vehicle_id.byte1!=0)) 
        do_mount(mw_sessions[N]->chr.iid, mw_sessions[N]->chr.vehicle_id, getSessionNumber());
   }
}

void mw_session::do_move(char* incom_buf)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=chr.iid.byte0; out_buf[4]=chr.iid.byte1; //from
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; //to
   out_buf[7]=0xA7; out_buf[8]=0x0F; out_buf[9]=0x01; //pkt
   out_buf[10]=chr.iid.byte0; out_buf[11]=chr.iid.byte1; //to
   out_buf[12]=0x02;   //wtf? dunno watsit but it dosent work without it
   int X0=flt_bytes2int(&(incom_buf[30]));
   int Y0=flt_bytes2int(&(incom_buf[34]));
   int X1=flt_bytes2int(&(incom_buf[38]));
   int Y1=flt_bytes2int(&(incom_buf[42]));
   printf ("move %d %d %d %d\n",X0,Y0,X1,Y1);
   chr.my_X=X1; chr.my_Y=Y1;
   for (i=14; i<=29; i++) out_buf[i]=incom_buf[i+16];
   out_buf[30]=mov_spd[0]; out_buf[31]=mov_spd[1]; out_buf[32]=mov_spd[2]; out_buf[33]=mov_spd[3]; //spd
   out_buf[34]=0x00; out_buf[35]=0xFE;  //end
   int sz;
   out_buf[0]=0xFB; out_buf[1]=36; out_buf[2]=0; sz=37; //leader and size
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]!=this)
     if (!sessA_is_visible_to_sess_B(getSessionNumber(), i))
     {
       remembers_sess[i]=false; mw_sessions[i]->remembers_sess[getSessionNumber()]=false;
     }
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]!=this)
     if (sessA_is_visible_to_sess_B(getSessionNumber(), i))
        mw_sessions[i]->do_move_other(chr.iid, X0,Y0,X1,Y1); //inform others abt move
   for (i=0; i<_MW_MAX_PORTALS; i++) if (mw_portals[i]) mw_portals[i]->do_Warp(getSessionNumber());
}
void mw_session::do_move_other(struct mw_id id, int X0, int Y0, int X1, int Y1)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=id.byte0; out_buf[4]=id.byte1; //from
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; //to
   out_buf[7]=0xA7; out_buf[8]=0x0F; out_buf[9]=0x01; //pkt
   out_buf[10]=id.byte0; out_buf[11]=id.byte1; //to
   out_buf[12]=0x02;   //wtf? dunno watsit but it dosent worl without it
   int2flt_bytes(&(out_buf[14]), X0); int2flt_bytes(&(out_buf[18]), Y0);
   int2flt_bytes(&(out_buf[22]), X1); int2flt_bytes(&(out_buf[26]), Y1);
   out_buf[30]=0xAE; out_buf[31]=0x47; out_buf[32]=0xE1; out_buf[33]=0x3E; //default spd
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) 
     if ((mw_sessions[i]->chr.iid.byte0==id.byte0)&&(mw_sessions[i]->chr.iid.byte1==id.byte1))
     {
        out_buf[30]=mw_sessions[i]->mov_spd[0]; out_buf[31]=mw_sessions[i]->mov_spd[1];
        out_buf[32]=mw_sessions[i]->mov_spd[2]; out_buf[33]=mw_sessions[i]->mov_spd[3]; //spd
        break;
     }
   out_buf[34]=0x00; out_buf[35]=0xFE;  //end
   int sz;
   out_buf[0]=0xFB; out_buf[1]=36; out_buf[2]=0; sz=37; //leader and size
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
}
void mw_session::do_chat(char* incom_buf)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   char* chat=&(incom_buf[9]);
   if (chat[0]=='@') { do_parse_chat(getSessionNumber(), chat); return;}
   out_buf[3]=chr.iid.byte0; out_buf[4]=chr.iid.byte1;
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;
   out_buf[7]=0xDD; out_buf[8]=0x13;
   out_buf[9]=chr.iid.byte0; out_buf[10]=chr.iid.byte1;
   i=0; while ((chat[i]!=0)&&((unsigned char)chat[i]!=0xFE)) {out_buf[i+11]=chat[i]; i++;}
   int sz;
   out_buf[i+11]=0xFE; i++; sz=i+12;
   out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0; //leader and size
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]!=this)
    if (mw_sessions[i]->number_at_map>=0) mw_sessions[i]->do_chat_other(chr.iid, chat); //inform others abt move
}
void mw_session::do_chat_other(struct mw_id id, char* chat_str)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=id.byte0; out_buf[4]=id.byte1;
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;
   out_buf[7]=0xDD; out_buf[8]=0x13;
   out_buf[9]=id.byte0; out_buf[10]=id.byte1;
   i=0; while ((chat_str[i]!=0)&&((unsigned char)chat_str[i]!=0xFE)) {out_buf[i+11]=chat_str[i]; i++;}
   int sz;
   out_buf[i+11]=0xFE; i++; sz=i+12;
   out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0; //leader and size
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
}
void mw_session::do_mount(struct mw_id rider_id, struct mw_id vehicle_id, int sess_N_to_inform)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   if ((chr.iid.byte0==rider_id.byte0)&&(chr.iid.byte1==rider_id.byte1))
   {//if rider is me
     chr.vehicle_id.byte0=vehicle_id.byte0; chr.vehicle_id.byte1=vehicle_id.byte1;
     if ((chr.vehicle_id.byte0==0)&&(chr.vehicle_id.byte1==0))
     { mov_spd[0]=0xAE; mov_spd[1]=0x47; mov_spd[2]=0xE1; mov_spd[3]=0x3E; } 
     else
     { mov_spd[0]=0xAE; mov_spd[1]=0x47; mov_spd[2]=0xE1; mov_spd[3]=0x3F; } 
   }//end if me mounting
   out_buf[3]=rider_id.byte0; out_buf[4]=rider_id.byte1; //from rider
   out_buf[5]=0; out_buf[6]=0; //to all
   out_buf[7]=0xE1; out_buf[8]=0x0F; //pkt
   out_buf[9]=rider_id.byte0; out_buf[10]=rider_id.byte1; //rider id
   out_buf[11]=0; int j=12;
   for (i=0; i<=56; i++) 
   { out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF;
     out_buf[j++]=0xFF; out_buf[j++]=0x00; }
   for (i=0; i<=7; i++) out_buf[j++]=0xFF;
   for (i=0; i<=14; i++) 
   { out_buf[j++]=0x00; out_buf[j++]=0x00; out_buf[j++]=0x00; out_buf[j++]=0x00;
     out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF;
     out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF; }
   out_buf[j-4]=vehicle_id.byte0; out_buf[j-3]=vehicle_id.byte1; out_buf[j-2]=0; out_buf[j-1]=0;
   for (i=0; i<=3; i++) out_buf[j++]=0xFF; out_buf[j++]=0x00; out_buf[j++]=0x00;
   for (i=0; i<=3; i++) out_buf[j++]=0xFF;
   for (i=0; i<=12; i++) 
   { out_buf[j++]=0x00; out_buf[j++]=0x00; out_buf[j++]=0x00; out_buf[j++]=0x00;
     out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF;
     out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF; }
   for (i=0; i<=3; i++) out_buf[j++]=0x00;
   out_buf[j++]=0xFF; out_buf[j++]=0xFF;
   out_buf[j++]=0xFE;
   out_buf[0]=0xFB;
   out_buf[1]=(div(j,256)).rem;//lower byte
   out_buf[2]=(div(j,256)).quot;//elder byte

   if (sessA_is_visible_to_sess_B(getSessionNumber(), sess_N_to_inform))
     dll_write_n(mw_sessions[sess_N_to_inform]->number_at_map, 1, j+1, &(out_buf[0]));
}

void mw_session::do_emote(char* incom_buf)
{
   int i; for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]!=this)
    if (mw_sessions[i]->number_at_map>=0) mw_sessions[i]->do_emote_other(chr.iid, incom_buf[27], incom_buf[33], incom_buf[37]); 
}
void mw_session::do_emote_other(struct mw_id chr_id_to_emote, char emotion, char angle1, char angle2)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; //to
   out_buf[7]=0xB9; out_buf[8]=0x0F; 
   out_buf[9]=chr_id_to_emote.byte0; out_buf[10]=chr_id_to_emote.byte1; //to
   out_buf[11]=emotion; out_buf[17]=angle1; out_buf[21]=angle2;
   out_buf[25]=0xFE; int sz=27;
   out_buf[0]=0xFB; out_buf[1]=26; out_buf[2]=0; //leader and size
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
}
void mw_session::do_equip(char* incom_buf)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; //to
   out_buf[7]=0x89; out_buf[8]=0x13;
   out_buf[9]=0x01; out_buf[10]=0x02;
   char equip_[255];  for (i=0;i<=21;i++) equip_[i]=incom_buf[i+33];
   for (i=0;i<=21;i++) out_buf[11+i]=equip_[i];
   i=22; out_buf[i+11]=0xFE; i++; int sz=i+12;
   out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0; //leader and size
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   mw_inventory* inv_from=0; mw_inventory* inv_to=0;
   if  (equip_[4]==1) inv_from=&equip; //from equipment
   if  (equip_[4]==3) inv_from=&gear; //from gearment
   if  (equip_[15]==1) inv_to=&equip; //to equipment
   if  (equip_[15]==3) inv_to=&gear; //to gearment
   int n_slot_from=equip_[0]; int n_slot_to=equip_[11];
   if  (inv_from&&inv_to) 
   {
     inv_to->SwapSlots(n_slot_to, inv_from, n_slot_from);
     printf ("moved from %d slot %d to %d slot %d",equip_[4],n_slot_from,equip_[15],n_slot_to);
   }
   bool diseq=false;  if ((inv_from==&gear)&&(inv_to==&equip)) diseq=true;
   if ((inv_to==&gear)||(inv_to==&equip)) for (i=0; i<_MW_MAX_SESSIONS; i++) 
     if (mw_sessions[i]) if (mw_sessions[i]!=this) if (mw_sessions[i]->number_at_map>=0) 
       if (sessA_is_visible_to_sess_B(getSessionNumber(), i))
         mw_sessions[i]->do_equip_other(chr.iid, inv_to->slots[n_slot_to].type_id, n_slot_from, n_slot_to, diseq); 
}
void mw_session::do_equip_other(struct mw_id chr_id_to_equip, struct mw_id itemid, int n_slot_from, int n_slot_to, bool diseq)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; //to
//   if (diseq) out_buf[7]=0xB5; else out_buf[7]=0xB7;
   if (diseq) out_buf[7]=0xB7; else out_buf[7]=0xB5;
   out_buf[8]=0x0F;
   out_buf[9]=chr_id_to_equip.byte0; out_buf[10]=chr_id_to_equip.byte1; //abt
   if ((unsigned char)out_buf[7]==0xB7) out_buf[11]=n_slot_from; else out_buf[11]=n_slot_to; 
   out_buf[13]=itemid.byte0; out_buf[14]=itemid.byte1; //MAYBE ZERO HERE lets try - if fails it would need get item id before it taken off
   out_buf[15]=0x00; out_buf[16]=0x00;
   out_buf[17]=0xFE; int sz=19;
   out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0;
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
}
void mw_session::do_simple_atk(char* incom_buf)
{
   struct mw_id mob_id; mob_id.byte0=incom_buf[33]; mob_id.byte1=incom_buf[34];
   int mob_n=(unsigned char)mob_id.byte0+256*(unsigned char)mob_id.byte1;
   if (!mw_npcs[mob_n]) return; if (!mw_npcs[mob_n]->is_mob) return;
   mw_mob* the_mob = (mw_mob*)mw_npcs[mob_n];
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;

   out_buf[3]=chr.iid.byte0; out_buf[4]=chr.iid.byte1; //from self
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; //to self
   out_buf[7]=0xBD; out_buf[8]=0x0F;
   out_buf[9]=chr.iid.byte0; out_buf[10]=chr.iid.byte1; //abt self
   out_buf[11]=mob_id.byte0; out_buf[12]=mob_id.byte1; //on mob
   int mob_dmg[4]={11,12,13,14};
   out_buf[13]=(div(the_mob->hp,256)).rem; out_buf[14]=(div(the_mob->hp,256)).quot; 
   out_buf[17]=(div(mob_dmg[0],256)).rem; out_buf[18]=(div(mob_dmg[0],256)).quot;
   out_buf[21]=(div(mob_dmg[1],256)).rem; out_buf[22]=(div(mob_dmg[1],256)).quot;
//   out_buf[24]=0x80; //status
   out_buf[25]=(div(mob_dmg[2],256)).rem; out_buf[26]=(div(mob_dmg[2],256)).quot;
   out_buf[29]=(div(mob_dmg[3],256)).rem; out_buf[30]=(div(mob_dmg[3],256)).quot;
   int j=34; out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF;
   out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF;
   j=58; out_buf[j++]=0x43; out_buf[j++]=0xAE; out_buf[j++]=0xEA;
   out_buf[73]=0xFE; int sz=75;
   out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0;
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) 
     if (mw_sessions[i]!=this) if (mw_sessions[i]->number_at_map>=0) 
       if (sessA_is_visible_to_sess_B(getSessionNumber(), i))
         mw_sessions[i]->do_simple_atk_other(chr.iid, mob_id, mob_dmg); 
   
   if ((rand()-rand())>0) the_mob->atk_back(getSessionNumber());
   the_mob->hp=the_mob->hp-mob_dmg[0]-mob_dmg[1]-mob_dmg[2]-mob_dmg[3];
   if (the_mob->hp<0) the_mob->die();
}
void mw_session::do_simple_atk_other(struct mw_id chr_id_attaker, struct mw_id mobid, int dmg[])
{
   int mob_n=(unsigned char)mobid.byte0+256*(unsigned char)mobid.byte1;
   if (!mw_npcs[mob_n]) return; if (!mw_npcs[mob_n]->is_mob) return;
   mw_mob* the_mob = (mw_mob*)mw_npcs[mob_n];
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=0; out_buf[4]=0; //from server
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; //to me
   out_buf[7]=0xBD; out_buf[8]=0x0F;
   out_buf[9]=chr_id_attaker.byte0; out_buf[10]=chr_id_attaker.byte1; //abt self
   out_buf[11]=mobid.byte0; out_buf[12]=mobid.byte1; //on mob
   out_buf[13]=(div(the_mob->hp,256)).rem; out_buf[14]=(div(the_mob->hp,256)).quot; 
   out_buf[17]=(div(dmg[0],256)).rem; out_buf[18]=(div(dmg[0],256)).quot;
   out_buf[21]=(div(dmg[1],256)).rem; out_buf[22]=(div(dmg[1],256)).quot;
   int j=34; out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF;
   out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF; out_buf[j++]=0xFF;
   j=58; out_buf[j++]=0x43; out_buf[j++]=0xAE; out_buf[j++]=0xEA;
   out_buf[73]=0xFE; int sz=75;
   out_buf[0]=0xFB; out_buf[1]=sz-1; out_buf[2]=0;
   dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
}
void mw_session::do_drop_item(char* incom_buf)
{
  //incom_buf[37] - n inventory
  //incom_buf[33] - n cell
  int drop_amount=(unsigned char)incom_buf[38];
  mw_inventory* drop_from=0;
  if (incom_buf[37]==1) drop_from=&equip;
  if (incom_buf[37]==2) drop_from=&loot;
  if (incom_buf[37]==3) drop_from=&gear;
  if (drop_from==0) return;
  int n_cell=(unsigned char)incom_buf[33];
  if ((n_cell<0)||(n_cell>30)) return;
  struct mw_id drop_id; 
  drop_id.byte0=drop_from->slots[n_cell].type_id.byte0; drop_id.byte1=drop_from->slots[n_cell].type_id.byte1;
  if ((drop_id.byte0==0)&&(drop_id.byte1==0)) return;
  if (incom_buf[37]==2) if (drop_from->slots[n_cell].durability<drop_amount) return;
  if (incom_buf[37]==2) if (drop_from->slots[n_cell].durability==drop_amount) drop_from->ClearSlot(n_cell);
  if (incom_buf[37]==2) if (drop_from->slots[n_cell].durability>drop_amount) drop_from->slots[n_cell].durability-=drop_amount;
  if ((incom_buf[37]==1)||(incom_buf[37]==3))  { drop_from->ClearSlot(n_cell); drop_amount=1; }
  put_drop(chr.my_X+1, chr.my_Y+1, drop_amount, chr.map_id, drop_id, drop_from->inv_type);
  char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
  int sz=0;
  if (incom_buf[37]==1) 
  { sz=equip.MakeSlotInfoPkt(out_buf, n_cell); if (sz>10) dll_write_n(number_at_map, 1, sz, &(out_buf[0])); }
  if (incom_buf[37]==3) 
  { sz=gear.MakeSlotInfoPkt(out_buf, n_cell); if (sz>10) dll_write_n(number_at_map, 1, sz, &(out_buf[0])); }
  if (incom_buf[37]==2) 
  { sz=loot.MakeSlotInfoPkt(out_buf, n_cell); if (sz>10) dll_write_n(number_at_map, 1, sz, &(out_buf[0])); }
}
void mw_session::do_pick_item(char* incom_buf)
{
  int itm_x=(unsigned char)incom_buf[37]+256*(unsigned char)incom_buf[38];
  int itm_y=(unsigned char)incom_buf[39]+256*(unsigned char)incom_buf[40];
  mw_drop* itm=get_drop_from_coords(itm_x, itm_y);
  mpDebugPrint("picking from %d %d \n", itm_x, itm_y);
  if (!itm) return; int sz=0;
  char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
  if (itm->my_class==2) sz=loot.DoAddToInventory(out_buf, itm->type_id, itm->my_amount);
  else if (itm->my_class==1) sz=equip.DoAddToInventory(out_buf, itm->type_id);
  out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;
  if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
  itm->remove();
}
void mw_session::do_buy_item(char* incom_buf)
{
   struct mw_id itm_id; bool is_equip=false;
   itm_id.byte0=incom_buf[33];
   itm_id.byte1=incom_buf[34];
   int amount=(unsigned char)incom_buf[37]+256*(unsigned char)incom_buf[38];
   if ((incom_buf[37]==0x00)&&(incom_buf[38]==0x00)&&(incom_buf[39]==0x00)&&(incom_buf[40]==0x00)) is_equip=true;
   if (((unsigned char)incom_buf[37]==0xFF)&&((unsigned char)incom_buf[38]==0xFF)&&(incom_buf[39]==0x00)&&(incom_buf[40]==0x00)) is_equip=true;
   char out_buf[2048]; int sz=0;
   if (is_equip) sz=equip.DoAddToInventory(out_buf, itm_id);
   else sz=loot.DoAddToInventory(out_buf, itm_id, amount);
   out_buf[5]=chr.iid.byte0;
   out_buf[6]=chr.iid.byte1;
   if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
}
void mw_session::do_im_buy_item(char* incom_buf)
{
   struct mw_id itm_id; bool is_equip=false;
   itm_id.byte0=incom_buf[36];
   itm_id.byte1=incom_buf[37];
   int amount=(unsigned char)incom_buf[38]+256*(unsigned char)incom_buf[39];
   is_equip=true;
   char out_buf[2048]; int sz=0;
   if (is_equip) sz=equip.DoAddToInventory(out_buf, itm_id);
   else sz=loot.DoAddToInventory(out_buf, itm_id, amount);
   out_buf[5]=chr.iid.byte0;
   out_buf[6]=chr.iid.byte1;
   if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
}
void mw_session::do_chr_effect(struct mw_id chr_id_effector, struct mw_id effect_id)
{
  char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
  out_buf[3]=chr_id_effector.byte0; out_buf[4]=chr_id_effector.byte1;
  out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;
  out_buf[7]=0x98; out_buf[8]=0x14; //packet
  out_buf[9]=chr_id_effector.byte0; out_buf[10]=chr_id_effector.byte1; 
  out_buf[11]=effect_id.byte0; out_buf[12]=effect_id.byte1; //effect id  (skill id?)
  out_buf[13]=effect_id.byte0; out_buf[14]=effect_id.byte1; //status id
  out_buf[19]=0xFE; //sz1:=21;
  out_buf[0]=0xFB; out_buf[1]=20; //leader and size of 1st pkt
  dll_write_n(number_at_map, 1, 21, &(out_buf[0]));
}
void mw_session::do_chr_effect2(struct mw_id chr_id_effector, struct mw_id effect_id)
{
  char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
  out_buf[3]=chr_id_effector.byte0; out_buf[4]=chr_id_effector.byte1;
  out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1;
  out_buf[7]=0xDA; out_buf[8]=0x0F; //packet
  out_buf[9]=chr_id_effector.byte0; out_buf[10]=chr_id_effector.byte1; 
  out_buf[15]=effect_id.byte0; out_buf[16]=effect_id.byte1; //effect id  (skill id?)
  out_buf[17]=0x01;
  out_buf[23]=0xFE; //sz1:=21;
  out_buf[0]=0xFB; out_buf[1]=0x18; //leader and size of 1st pkt
  dll_write_n(number_at_map, 1, 0x18+1, &(out_buf[0]));
}
void mw_session::do_skill_up(char* incom_buf)
{
   struct mw_id sk_id; 
   sk_id.byte0=incom_buf[33]; sk_id.byte1=incom_buf[34];
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   int sz=skills.DoAddToInventory(out_buf, sk_id);
   if (sz>20) dll_write_n(number_at_map, 1, sz, &(out_buf[0]));
   for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[7]=0x86; out_buf[8]=0x14; //packet
   out_buf[9]=0x01; chr.sk_pnt--;
   out_buf[10]=div(chr.sk_pnt,256).rem; out_buf[11]=div(chr.sk_pnt,256).quot;
   out_buf[12]=sk_id.byte0; out_buf[13]=sk_id.byte1;
   out_buf[14]=skills.getSkillLvl(sk_id);   
   out_buf[97]=0xFE; 
   out_buf[0]=0xFB; out_buf[1]=98; //leader and size of 1st pkt
   dll_write_n(number_at_map, 1, 99, &(out_buf[0]));
}
void mw_session::do_use_skill_on_mob(char* incom_buf)
{
   struct mw_id tgt_id; struct mw_id skil_id; 
   tgt_id.byte0=incom_buf[41]; tgt_id.byte1=incom_buf[42];
   skil_id.byte0=incom_buf[33]; skil_id.byte1=incom_buf[34];
   int mob_n=(unsigned char)tgt_id.byte0+256*(unsigned char)tgt_id.byte1;
   if (!mw_npcs[mob_n]) return; if (!mw_npcs[mob_n]->is_mob) return;
   mw_mob* the_mob = (mw_mob*)mw_npcs[mob_n];
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=chr.iid.byte0; out_buf[4]=chr.iid.byte1; 
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; 
   out_buf[7]=0x96; out_buf[8]=0x14; //packet
   out_buf[9]=chr.iid.byte0; out_buf[10]=chr.iid.byte1; 
   out_buf[11]=skil_id.byte0; out_buf[12]=skil_id.byte1;  out_buf[13]=0x01; out_buf[15]=0x01; 
   out_buf[17]=tgt_id.byte0; out_buf[18]=tgt_id.byte1; 
   int mob_dmg=136;
   out_buf[19]=div(mob_dmg, 256).rem; out_buf[20]=div(mob_dmg, 256).quot;
   out_buf[23]=1; // if zero - atk misses
   the_mob->hp=the_mob->hp-mob_dmg;
   out_buf[27]=div(the_mob->hp, 256).rem; out_buf[28]=div(the_mob->hp, 256).quot;
   out_buf[45]=0xFE; 
   out_buf[0]=0xFB; out_buf[1]=46; //leader and size of 1st pkt
   dll_write_n(number_at_map, 1, 47, &(out_buf[0]));
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) 
     if (mw_sessions[i]!=this) if (mw_sessions[i]->number_at_map>=0) 
       if (sessA_is_visible_to_sess_B(getSessionNumber(), i))
         mw_sessions[i]->do_see_skill_on_mob(chr.iid, tgt_id, skil_id, mob_dmg, the_mob->hp); 
   if (the_mob->hp<0) the_mob->die();
   else if ((rand()-rand())>0) the_mob->atk_back(getSessionNumber());
}

void mw_session::do_use_skill_on_chr(char* incom_buf)
{
   if ((incom_buf[39]!=0)&&(incom_buf[40]==0)) {do_use_skill_on_mob(incom_buf); return;}
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   struct mw_id tgt_id; struct mw_id skil_id; 
   tgt_id.byte0=incom_buf[41]; tgt_id.byte1=incom_buf[42];
   skil_id.byte0=incom_buf[33]; skil_id.byte1=incom_buf[34];
   out_buf[3]=chr.iid.byte0; out_buf[4]=chr.iid.byte1; 
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; 
   out_buf[7]=0x89; out_buf[8]=0x14; //packet
   out_buf[9]=0x01; out_buf[10]=0x02; out_buf[11]=0x00; //as is (sic)
   out_buf[12]=skil_id.byte0; out_buf[13]=skil_id.byte1;  out_buf[14]=0x01; 
   out_buf[15]=0x07; out_buf[17]=0x01; //animation switched on
   out_buf[18]=tgt_id.byte0; out_buf[19]=tgt_id.byte1; 
   out_buf[24]=0x03; //skill success
   out_buf[25]=0x01; out_buf[46]=0xFE; 
   out_buf[0]=0xFB; out_buf[1]=47; //leader and size of 1st pkt
   if (skills.getSkillType(skil_id)==1) do_chr_effect(tgt_id, skills.getSkillEffect(skil_id));
   if (skills.getSkillType(skil_id)==2) do_chr_effect2(tgt_id, skills.getSkillEffect(skil_id));
   dll_write_n(number_at_map, 1, 48, &(out_buf[0]));
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) 
     if (mw_sessions[i]!=this) if (mw_sessions[i]->number_at_map>=0) 
       if (sessA_is_visible_to_sess_B(getSessionNumber(), i))
         mw_sessions[i]->do_see_skill_on_chr(tgt_id, skil_id, skills.getSkillEffect(skil_id), skills.getSkillType(skil_id)); 
}
void mw_session::do_see_skill_on_chr(struct mw_id tgt_id, struct mw_id skl_id, struct mw_id eff_id, int skl_typ)
{
   if (skl_typ==1) do_chr_effect(tgt_id, eff_id);
   if (skl_typ==2) do_chr_effect2(tgt_id, eff_id);
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; 
   out_buf[7]=0x89; out_buf[8]=0x14; //packet
   out_buf[9]=0x01; out_buf[10]=0x02; out_buf[11]=0x00; //as is (sic)
   out_buf[12]=skl_id.byte0; out_buf[13]=skl_id.byte1;  out_buf[14]=0x01; 
   out_buf[15]=0x07; out_buf[17]=0x01; //animation switched on
   out_buf[18]=tgt_id.byte0; out_buf[19]=tgt_id.byte1; 
   out_buf[24]=0x03; //skill success
   out_buf[25]=0x01; out_buf[46]=0xFE; 
   out_buf[0]=0xFB; out_buf[1]=47; //leader and size of 1st pkt
   dll_write_n(number_at_map, 1, 48, &(out_buf[0]));
}
void mw_session::do_see_skill_on_mob(struct mw_id caster_id, struct mw_id tgt_id, struct mw_id skl_id, int mob_dmg, int rem_hp)
{
   char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
   out_buf[3]=caster_id.byte0; out_buf[4]=caster_id.byte1; 
   out_buf[5]=chr.iid.byte0; out_buf[6]=chr.iid.byte1; 
   out_buf[7]=0x96; out_buf[8]=0x14; //packet
   out_buf[9]=caster_id.byte0; out_buf[10]=caster_id.byte1; 
   out_buf[11]=skl_id.byte0; out_buf[12]=skl_id.byte1;  out_buf[13]=0x01; out_buf[15]=0x01; 
   out_buf[17]=tgt_id.byte0; out_buf[18]=tgt_id.byte1; 
   out_buf[19]=div(mob_dmg, 256).rem; out_buf[20]=div(mob_dmg, 256).quot;
   out_buf[23]=1; // if zero - atk misses
   out_buf[27]=div(rem_hp, 256).rem; out_buf[28]=div(rem_hp, 256).quot;
   out_buf[45]=0xFE; 
   out_buf[0]=0xFB; out_buf[1]=46; //leader and size of 1st pkt
   dll_write_n(number_at_map, 1, 47, &(out_buf[0]));
}

void sessions_progress(unsigned long sv_time)
{
   int i; for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) mw_sessions[i]->progress(sv_time);
}
