/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains npc class declaration             */
#ifndef _MEOW_NPC_H_INCLUDED_
#define _MEOW_NPC_H_INCLUDED_
#include "meow_base.h"
class mw_npc : public mw_Savable
{
   public:
   mw_npc(); 
   virtual ~mw_npc();
   virtual void ReadParamFromString(char* tknleft, char* tknright);
   virtual void LoadFromFile(char* dirname);
   virtual void SaveToFile(char* dirname);
   virtual int getMyNumber();
   virtual void send_my_info_to_sess_N(int N_sess);
   virtual void progress(unsigned long sv_time);
   short my_X, my_Y;
   struct mw_id map_id;
   char base_fname[30];
   bool loadsuccess;
   bool remembers_sess[_MW_MAX_SESSIONS];
   bool is_mob;
};
extern mw_npc* mw_npcs[];
void load_npcs();
void npcs_save();
void spawn_npc(int X, int Y, struct mw_id map_id, struct mw_id type_id);
bool npc_is_visible_to_sess_N(int N_sess, int N_npc);
void npc_progress(unsigned long sv_time);

#endif //_MEOW_NPC_H_INCLUDED_
