/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains session class declaratassignedion         */
#ifndef _MEOW_SESS_H_INCLUDED_
#include "meow_char.h"
#include "meow_inv.h"
#include "meow_base.h"

class mw_session : public mw_Savable
{
   public:
   mw_session();
   virtual ~mw_session();
   virtual void ReadParamFromString(char* tknleft, char* tknright);
   virtual void LoadFromFile(char* dirname);
   virtual void SaveToFile(char* dirname);
   virtual void progress(unsigned long sv_time);
   int getSessionNumber();
   void send_invalid_login();
   void enter_map();
   void enter_char_create();
   void reply_id(bool do_double);
   void map_reply_to_map_enter(bool do_double);
   void map_post_enter();
   void map_disconnect();
   int my_info_pkt_prepare(char* out_buf);
   void do_inform_N_abt_me(int N);
   void get_inform_abt_N(int N);
   void do_move(char* incom_buf);
   void do_move_other(struct mw_id id, int X0, int Y0, int X1, int Y1);
   void do_chat(char* incom_buf);
   void do_chat_other(struct mw_id id, char* chat_str);
   void do_mount(struct mw_id rider_id, struct mw_id vehicle_id, int sess_N_to_inform);
   void do_emote(char* incom_buf);
   void do_emote_other(struct mw_id chr_id_to_emote, char emotion, char angle1, char angle2);
   void do_equip(char* incom_buf);
   void do_equip_other(struct mw_id chr_id_to_equip, struct mw_id itemid, int n_slot_from, int n_slot_to, bool diseq);
   void do_simple_atk(char* incom_buf);
   void do_simple_atk_other(struct mw_id chr_id_attaker, struct mw_id mobid, int dmg[]);
   void do_drop_item(char* incom_buf);
   void do_pick_item(char* incom_buf);
   void do_buy_item(char* incom_buf);
   void do_im_buy_item(char* incom_buf);
   void do_chr_effect(struct mw_id chr_id_effector, struct mw_id effect_id);
   void do_chr_effect2(struct mw_id chr_id_effector, struct mw_id effect_id);
   void do_skill_up(char* incom_buf);
   void do_use_skill_on_chr(char* incom_buf);
   void do_use_skill_on_mob(char* incom_buf);
   void do_see_skill_on_chr(struct mw_id tgt_id, struct mw_id skl_id, struct mw_id eff_id, int skl_typ);
   void do_see_skill_on_mob(struct mw_id caster_id, struct mw_id tgt_id, struct mw_id skl_id, int mob_dmg, int rem_hp);
   mw_char chr;
   mw_loot loot;
   mw_equip equip;
   mw_gear gear;
   mw_skills skills;
   char login[53];
   char password[85];
   int number_at_chr;
   int number_at_map;
   int sess_pseudo_id;
   char base_fname[30];
   bool loadsuccess;
   char mov_spd[4];
   bool remembers_sess[_MW_MAX_SESSIONS];
   bool is_entering_map; int dd_counter;
   bool is_online;
};
bool sessA_is_visible_to_sess_B(int N_sessA, int N_sessB);
extern mw_session* mw_sessions[];
extern char my_ip[];
void sessions_progress(unsigned long sv_time);
#define _MEOW_SESS_H_INCLUDED_
#endif //_MEOW_SESS_H_INCLUDED_
