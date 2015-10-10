/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*       this file contains map  server thread           */
/*             implementation                            */
#include "meow_main.h"
#include "map_thread.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include "meow_sess.h"
#include "meow_base.h"
#include "meow_nice_pkt.h"
int map_exec(int n_connect, int n_rcvd_bytes, char* incom_buf)
{
   if (n_rcvd_bytes<=8) return 0;
   int i; char out_buf[2048]; for (i=0; i<2048; i++) out_buf[i]=0;
   if ((unsigned char)incom_buf[7]==MAP_CONNECT_PKT)
   {//gree all independently of id or smth
      for (i=0; i<=map_greeting_sz; i++) out_buf[i]=map_greeting[i];
      dll_write_n(n_connect, 1, map_greeting_sz+1, &(out_buf[0])); 
 //     dll_write_n(n_connect, 1, map_greeting_sz+1, &(out_buf[0]));
      return 0;
   }
   if ((unsigned char)incom_buf[7]==MAP_GET_ID_PKT) 
   {
      int s_p_id=0;
      mw_session* n_session=0;
      s_p_id=incom_buf[49]+incom_buf[50]*256; // incom_buf[51] incom_buf[52] ignored for now
      printf("t to connect as id %d\n", s_p_id);
      for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]->sess_pseudo_id!=0)
        if (mw_sessions[i]->sess_pseudo_id==s_p_id){ n_session=mw_sessions[i]; break;}
      if (n_session)
      {
         n_session->number_at_map=n_connect;
         n_session->number_at_chr=-1;
         n_session->reply_id(false);
         printf("assigned session %d\n", n_session->getSessionNumber());
      }
      return 0;
   }
   
   mw_session* a_session=0; 
   for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) 
      if (mw_sessions[i]->number_at_map==n_connect) {a_session=mw_sessions[i]; break;}
   if (!a_session) return 0;
   if  ((unsigned char)incom_buf[7]==MAP_ENTER_PKT)  a_session->map_reply_to_map_enter(false);
   if  ((unsigned char)incom_buf[7]==MAP_REENTER_PKT)  a_session->map_reply_to_map_enter(false);
   if ((unsigned char)incom_buf[7]==MAP_READY_PKT) a_session->map_post_enter();
   if ((unsigned char)incom_buf[7]==_MW_IDLE_PKT) 
     if (a_session->number_at_map>=0) if (a_session->is_entering_map) 
     {
       if (a_session->dd_counter<5) {a_session->dd_counter++; return 0;}
       a_session->dd_counter=0;
       a_session->reply_id(true);
       a_session->map_reply_to_map_enter(true);
     }
   if (((unsigned char)incom_buf[7]==MAP_QUIT_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_QUIT_PKT1)) 
       {printf("session %d disconnected\n", a_session->getSessionNumber()); a_session->map_disconnect(); return 0;}
   if ((unsigned char)incom_buf[7]==MAP_MOVE_PKT) {a_session->is_entering_map=false; a_session->do_move(incom_buf);}
   if ((unsigned char)incom_buf[7]==MAP_CHAT_PKT) {a_session->is_entering_map=false; a_session->do_chat(incom_buf);}
   if ((unsigned char)incom_buf[7]==MAP_EMOTE_PKT) a_session->do_emote(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_EQUIP_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_EQUIP_PKT1)) a_session->do_equip(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_ATK_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_ATK_PKT1)) a_session->do_simple_atk(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_ATKC_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_ATKC_PKT1)) a_session->do_simple_atk(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_DROP_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_DROP_PKT1)) a_session->do_drop_item(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_PICK_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_PICK_PKT1)) a_session->do_pick_item(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_BUY_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_BUY_PKT1)) a_session->do_buy_item(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_IM_BUY_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_IM_BUY_PKT1)) a_session->do_im_buy_item(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_SKILL_UP_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_SKILL_UP_PKT1)) a_session->do_skill_up(incom_buf);
   if (((unsigned char)incom_buf[7]==MAP_USE_SKILL_ON_CHR_PKT0)&&
       ((unsigned char)incom_buf[8]==MAP_USE_SKILL_ON_CHR_PKT1)) a_session->do_use_skill_on_chr(incom_buf);
}
