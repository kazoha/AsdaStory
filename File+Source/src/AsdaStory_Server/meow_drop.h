/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains drop class declaration            */
#ifndef _MEOW_DROP_H_INCLUDED_
#define _MEOW_DROP_H_INCLUDED_
#include "meow_base.h"
class mw_drop : public mw_Savable
{
   public:
   mw_drop(); 
   virtual ~mw_drop();
   virtual void ReadParamFromString(char* tknleft, char* tknright);
   virtual void LoadFromFile(char* dirname);
   virtual void SaveToFile(char* dirname);
   virtual int getMyNumber();
   virtual void send_my_info_to_sess_N(int N_sess);
   virtual void progress(unsigned long sv_time);
   virtual void remove();
   short my_X, my_Y;
   short my_amount;
   int my_class;
   struct mw_id map_id;
   char base_fname[30];
   bool loadsuccess;
   bool remembers_sess[_MW_MAX_SESSIONS];
   protected:
   unsigned long age;
};
extern mw_drop* mw_drops[];
void load_drops();
void drops_save();
void put_drop(int X, int Y, int amount, struct mw_id map_id, struct mw_id type_id, int class_id);
bool drop_is_visible_to_sess_N(int N_sess, int N_drop);
void drop_progress(unsigned long sv_time);
mw_drop* get_drop_from_coords(int x, int y);
#endif //_MEOW_DROP_H_INCLUDED_
