/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains base data structures declaration  */
#ifndef _MEOW_BASE_H_INCLUDED_
#define _MEOW_BASE_H_INCLUDED_
struct mw_id 
{
  unsigned char byte0;
  unsigned char byte1;
};
struct mw_item_desc
{
   struct mw_id iid;     //used as class for skill 00 = passive 01 - status 02 - statusless 03 - attack
   struct mw_id type_id;
   struct mw_id gem1_id; //used as lvl for skills
   struct mw_id gem2_id; //used as effect id for skills
   struct mw_id gem3_id;
   struct mw_id gem4_id;
   short  _param;
   unsigned short weight;
   unsigned short durability; //used as number for loots
}; 
class mw_Savable
{
   public:
   struct mw_id iid;
   struct mw_id type_id;
   mw_Savable() {iid.byte0=0; iid.byte1=0; type_id.byte0=0; type_id.byte1=0; };
   virtual ~mw_Savable() {};
   virtual void LoadFromFile(char* dirname) {};
   virtual void SaveToFile(char* dirname) {};
   virtual void AddInfoToSave(void* fText) {};
};
#define _MW_MAX_NPCS 1000
#define _MW_MAX_DROPS 5000
#define _MW_MAX_PORTALS 100
#define _MW_MAX_SESSIONS 10
#define _MW_CLIENT_VIEW_DIST 35
#define _MW_IDLE_PKT 0xE8

#endif //_MEOW_BASE_H_INCLUDED_
