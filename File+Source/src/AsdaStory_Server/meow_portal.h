/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains portal class declaration          */
#ifndef _MEOW_PORTAL_H_INCLUDED_
#define _MEOW_PORTAL_H_INCLUDED_
#include "meow_base.h"
class mw_portal : public mw_Savable
{
   public:
   mw_portal(); 
   virtual ~mw_portal();
   virtual void ReadParamFromString(char* tknleft, char* tknright);
   virtual void LoadFromFile(char* dirname);
   virtual void SaveToFile(char* dirname);
   virtual int getMyNumber();
   virtual void do_Warp(int sess_N);
   short my_X, my_Y, my_R;   
   struct mw_id map_id;
   struct mw_id map_to;
   short X_to, Y_to;   
   char base_fname[30];
   bool loadsuccess;
};
extern mw_portal* mw_portals[];
void load_portals();
void portals_save();
void set_portal(int X, int Y, struct mw_id map_id, struct mw_id map_to_id);

#endif //_MEOW_PORTAL_H_INCLUDED_
