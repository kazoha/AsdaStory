/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains inventory class declaration       */
#ifndef _MEOW_INV_H_INCLUDED_
#define _MEOW_INV_H_INCLUDED_
#include "meow_base.h"
class mw_inventory : public mw_Savable
{
   public:
   mw_inventory(); 
   virtual ~mw_inventory();
   virtual void ReadParamFromString(char* tknleft, char* tknright);
   virtual void LoadFromFile(char* dirname);
   virtual void SaveToFile(char* dirname);
   virtual void ClearSlot(int n_slot);
   virtual void CopySlot(int n_slot_to, mw_inventory* inv_from, int n_slot_from);
   virtual void SwapSlots(int n_slot_to, mw_inventory* inv_from, int n_slot_from);
   virtual int MakeSlotSegment(char *buffer, int counter, int n_slot);
   virtual int MakeInfoPkt(char *buffer);
   virtual int MakeSlotInfoPkt(char *buffer, int n_slot);
   virtual int DoAddToInventory(char *buffer, struct mw_id itm_id);
   struct mw_item_desc slots[30];
   char base_fname[30];
   int inv_type;
   protected:
   int slot_counter;
};
class mw_loot: public mw_inventory
{
   public:
   mw_loot(); 
   virtual int MakeSlotSegment(char *buffer, int counter, int n_slot);
   virtual int MakeInfoPkt(char *buffer);
   int SetGold(int amount, char *buffer);
   virtual int DoAddToInventory(char *buffer, struct mw_id itm_id);
   virtual int DoAddToInventory(char *buffer, struct mw_id itm_id, int N);
};
class mw_equip: public mw_inventory
{
   public:
   mw_equip(); 
};
class mw_gear: public mw_inventory
{
   public:
   virtual int MakeInfoPkt(char *buffer);
   mw_gear(); 
};
class mw_skills: public mw_inventory
{
   public:
   virtual int MakeSlotSegment(char *buffer, int counter, int n_slot);
   virtual int MakeInfoPkt(char *buffer);
   virtual int DoAddToInventory(char *buffer, struct mw_id itm_id);
   virtual int getSkillLvl(struct mw_id skill_id);
   virtual struct mw_id getSkillEffect(struct mw_id skill_id);
   virtual int getSkillType(struct mw_id skill_id);
   mw_skills(); 
};
#endif //_MEOW_INV_H_INCLUDED_
