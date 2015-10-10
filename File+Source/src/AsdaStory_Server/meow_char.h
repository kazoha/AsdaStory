/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains character class declaration       */
#ifndef _MEOW_CHAR_H_INCLUDED_
#define _MEOW_CHAR_H_INCLUDED_
#include "meow_base.h"
class mw_char : public mw_Savable
{
   public:
   mw_char(); 
   virtual ~mw_char();
   virtual void ReadParamFromString(char* tknleft, char* tknright);
   virtual void LoadFromFile(char* dirname);
   virtual void SaveToFile(char* dirname);
   virtual int MakeSkillInfoPkt(char *buffer);
   char chr_name[63];
   char chr_guild[63];
   int lvl;
   struct mw_id job;
   int sk_pnt;
   char face, color, hair, gender, constelation;
   int m_hp, m_mp, exp, money;
   int m_weight, weight;
   int macs;
   int hp, mp;
   int chr_str, chr_acc, chr_int, chr_agi, chr_def, chr_vit;
   int hp_rec, mp_rec, unkwn_prm1, unkwn_prm2, unkwn_prm3;
   int elm_atk_bonuses[6];
   int elm_def_bonuses[6];
   short my_X, my_Y;
   struct mw_id map_id;
   struct mw_id vehicle_id;
};
#endif //_MEOW_CHAR_H_INCLUDED_
