/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains mob class declaration             */
#ifndef _MEOW_MOB_H_INCLUDED_
#define _MEOW_MOB_H_INCLUDED_
#include "meow_npc.h"
class mw_mob : public mw_npc
{
   public:
   mw_mob(); 
   virtual void ReadParamFromString(char* tknleft, char* tknright);
   virtual void AddInfoToSave(void* fText);
   virtual void send_mob_move_to_sess_N(int N_sess, int X0, int Y0, int X1, int Y1);
   virtual void send_my_info_to_sess_N(int N_sess);
   virtual void progress(unsigned long sv_time);
   virtual void atk_back(int N_sess);
   virtual void die();
   int skip_counter;
   int hp;
   int spawn_X, spawn_Y, spawn_hp;
};
void load_mobs();
void spawn_mob(int X, int Y, struct mw_id map_id, struct mw_id type_id);

#endif //_MEOW_MOB_H_INCLUDED_
