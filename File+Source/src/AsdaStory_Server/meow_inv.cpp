/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains inventory class implementation    */
#include "meow_inv.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include <stdlib.h>
char equip_segment[]=
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 
 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int equip_segment_sz=sizeof(equip_segment);

#define _MW_INV_SKILLS 0
#define _MW_INV_EQUIP 1
#define _MW_INV_LOOT  2
#define _MW_INV_GEAR  3

mw_inventory::mw_inventory()
{
   mpStrCopy(base_fname,"equip.ini");
   slot_counter=0; inv_type=_MW_INV_EQUIP;
   int i; for (i=0; i<30; i++) ClearSlot(i);
}
mw_inventory::~mw_inventory()
{
}
void mw_inventory::ReadParamFromString(char* tknleft, char* tknright)
{
   char tkn[256];
   if (!mpStrCompare(tknleft,"slot")) return;
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].iid.byte0=strtoul(tkn,0,16); 
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].iid.byte1=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].type_id.byte0=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].type_id.byte1=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].gem1_id.byte0=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].gem1_id.byte1=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].gem2_id.byte0=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].gem2_id.byte1=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].gem3_id.byte0=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].gem3_id.byte1=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].gem4_id.byte0=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].gem4_id.byte1=strtoul(tkn,0,16);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter]._param=mpStrToInt(tkn);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].weight=mpStrToInt(tkn);
   mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); slots[slot_counter].durability=mpStrToInt(tkn);
   slot_counter++;
}
void mw_inventory::LoadFromFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/"); 
   mpStrAddStr(filename,base_fname);
   mpText* invfile=ReadSytxTextFromFile(filename); if (!invfile) return;
   if (invfile->nstrings<=0) {mpDestroyText(&invfile); return;}
   char s1[256]; char s2[256]; int i;
   slot_counter=0;
   for (i=0; i<invfile->nstrings; i++)
   {
      mpStrCopy(s2,invfile->strings[i]);  
      mpStrTrimLeft(s2, ' '); mpGetAndCutLeftField(s1, s2, ' ');
      ReadParamFromString(s1,s2);
   }
   mpDestroyText(&invfile); 
}
void mw_inventory::SaveToFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/"); 
   mpStrAddStr(filename,base_fname);
   mpText* invfile=CreateEmptyText(); if (!invfile) return;
   char str[255]; char str2[255];
   for (slot_counter=0;slot_counter<30;slot_counter++)
   {
      mpStrCopy(str, "slot "); 
      itoa(slots[slot_counter].iid.byte0, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].iid.byte1, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].type_id.byte0, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].type_id.byte1, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].gem1_id.byte0, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].gem1_id.byte1, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].gem2_id.byte0, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].gem2_id.byte1, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].gem3_id.byte0, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].gem3_id.byte1, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].gem4_id.byte0, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      itoa(slots[slot_counter].gem4_id.byte1, str2, 16); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      mpIntToStr(str2, slots[slot_counter]._param); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      mpIntToStr(str2, slots[slot_counter].weight); mpStrAddStr(str, str2); mpStrAddStr(str, " ");
      mpIntToStr(str2, slots[slot_counter].durability); mpStrAddStr(str, str2);
      mpAppendStrToText(invfile, str);
   }
   WriteTextToFile (filename, invfile);
   mpDestroyText(&invfile); 
}
void mw_inventory::ClearSlot(int n_slot)
{
   slots[n_slot].iid.byte0=0;
   slots[n_slot].iid.byte1=0;
   slots[n_slot].type_id.byte0=0;
   slots[n_slot].type_id.byte1=0;
   slots[n_slot].gem1_id.byte0=0;
   slots[n_slot].gem1_id.byte1=0;
   slots[n_slot].gem2_id.byte0=0;
   slots[n_slot].gem2_id.byte1=0;
   slots[n_slot].gem3_id.byte0=0;
   slots[n_slot].gem3_id.byte1=0;
   slots[n_slot].gem4_id.byte0=0;
   slots[n_slot].gem4_id.byte1=0;
   slots[n_slot]._param=0;
   slots[n_slot].weight=10;
   slots[n_slot].durability=100;
}

void mw_inventory::CopySlot(int n_slot_to, mw_inventory* inv_from, int n_slot_from)
{
   if(!inv_from) return; if (n_slot_from<0) return; if (n_slot_from>=30) return;
   if (n_slot_to<0) return; if (n_slot_to>=30) return;
   mpmemcpy(&(slots[n_slot_to]),&(inv_from->slots[n_slot_from]), sizeof(struct mw_item_desc));
}
void mw_inventory::SwapSlots(int n_slot_to, mw_inventory* inv_from, int n_slot_from)
{
   if(!inv_from) return; if (n_slot_from<0) return; if (n_slot_from>=30) return;
   if (n_slot_to<0) return; if (n_slot_to>=30) return;
   struct mw_item_desc tmpslot;
   mpmemcpy(&tmpslot,&(slots[n_slot_to]), sizeof(struct mw_item_desc));
   mpmemcpy(&(slots[n_slot_to]),&(inv_from->slots[n_slot_from]), sizeof(struct mw_item_desc));
   mpmemcpy(&(inv_from->slots[n_slot_from]),&tmpslot, sizeof(struct mw_item_desc));
}
int mw_inventory::MakeSlotSegment(char *buffer, int counter, int n_slot)
{
   if ((n_slot<0)||(n_slot>=30)) return counter;
   int j=counter; char* segment=&(buffer[j]);
   int i; for (i=0; i<=33; i++) {segment[i]=0; j++;}
   segment[31]=0xFF; segment[32]=0xFF;
   segment[0]=slots[n_slot].type_id.byte0; segment[1]=slots[n_slot].type_id.byte1;
   segment[4]=inv_type; segment[5]=n_slot;
   segment[13]=slots[n_slot].durability;
   segment[14]=div(slots[n_slot].weight, 256).rem;
   segment[15]=div(slots[n_slot].weight, 256).quot;
   segment[16]=slots[n_slot].gem1_id.byte0; segment[17]=slots[n_slot].gem1_id.byte1;
   segment[18]=slots[n_slot].gem2_id.byte0; segment[19]=slots[n_slot].gem2_id.byte1;
   segment[20]=slots[n_slot].gem3_id.byte0; segment[21]=slots[n_slot].gem3_id.byte1;
   segment[22]=slots[n_slot].gem4_id.byte0; segment[23]=slots[n_slot].gem4_id.byte1;
   return j;
}

int mw_inventory::MakeInfoPkt(char *buffer)
{
   bool iventory_has_item=false;
   int i; for (i=0; i<30; i++) if ((slots[i].type_id.byte0!=0)||(slots[i].type_id.byte1!=0)) iventory_has_item=true;
   if (!iventory_has_item) return 0;
   for (i=0; i<2048; i++) buffer[i]=0;
   int j=7; buffer[j++]=0xCD; buffer[j++]=0x0F;
   for (i=0; i<30; i++) if ((slots[i].type_id.byte0!=0)||(slots[i].type_id.byte1!=0))
     j=MakeSlotSegment(buffer, j, i);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}
int mw_inventory::MakeSlotInfoPkt(char *buffer, int n_slot)
{
   int i; for (i=0; i<2048; i++) buffer[i]=0;
   int j=7; buffer[j++]=0x95; buffer[j++]=0x13; buffer[j++]=0x01; buffer[j++]=0xFF; buffer[j++]=0xFF; 
   j=MakeSlotSegment(buffer, j, n_slot);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}
int mw_inventory::DoAddToInventory(char *buffer, struct mw_id itm_id)
{
   if ((itm_id.byte0==0)&&(itm_id.byte1==0)) return 0;
   bool iventory_has_free_space=false; int n_free_cell=-1;
   int i; for (i=0; i<30; i++) 
     if ((slots[i].type_id.byte0==0)&&(slots[i].type_id.byte1==0)) 
        { iventory_has_free_space=true; n_free_cell=i; break; }
   if (!iventory_has_free_space) return 0;
   slots[n_free_cell].type_id.byte0=itm_id.byte0;
   slots[n_free_cell].type_id.byte1=itm_id.byte1;
   for (i=0; i<2048; i++) buffer[i]=0;
//   int j=7; buffer[j++]=0xCD; buffer[j++]=0x0F;
   int j=7; buffer[j++]=0x95; buffer[j++]=0x13; buffer[j++]=0x01; buffer[j++]=0xFF; buffer[j++]=0xFF; 
   j=MakeSlotSegment(buffer, j, n_free_cell);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}


mw_equip::mw_equip()
{
   mpStrCopy(base_fname,"equip.ini");
   inv_type=_MW_INV_EQUIP;
}
mw_loot::mw_loot()
{
   mpStrCopy(base_fname,"loot.ini");
   inv_type=_MW_INV_LOOT;
}
int mw_loot::MakeInfoPkt(char *buffer)
{
   int i; for (i=0; i<2048; i++) buffer[i]=0;
   int j=7; buffer[j++]=0xD0; buffer[j++]=0x0F;
   for (i=0; i<30; i++) if ((slots[i].type_id.byte0!=0)||(slots[i].type_id.byte1!=0))
     j=MakeSlotSegment(buffer, j, i);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}
int mw_loot::MakeSlotSegment(char *buffer, int counter, int n_slot)
{
   int j=counter; char* segment=&(buffer[j]);
   int res=mw_inventory::MakeSlotSegment(buffer, counter, n_slot);
   if (res<=0) return res;
   segment[13]=0; //durability is amount here;
   segment[9]=div(slots[n_slot].durability, 256).rem;
   segment[10]=div(slots[n_slot].durability, 256).quot;
   return res;
}
int mw_loot::SetGold(int amount, char *buffer)
{
   int n_tgt_cell=-1;
   int i; for (i=0; i<30; i++) 
     if ((slots[i].type_id.byte0==0x37)&&(slots[i].type_id.byte1==0x29)) 
        { n_tgt_cell=i; break; }
   if (n_tgt_cell<0) return 0;
   if (slots[n_tgt_cell].durability==amount) return 0;
   slots[n_tgt_cell].durability=amount;
   for (i=0; i<2048; i++) buffer[i]=0;
   int j=7; buffer[j++]=0x95; buffer[j++]=0x13; buffer[j++]=0x01; buffer[j++]=0xFF; buffer[j++]=0xFF; 
   j=MakeSlotSegment(buffer, j, n_tgt_cell);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}

int mw_loot::DoAddToInventory(char *buffer, struct mw_id itm_id, int N)
{
   if ((itm_id.byte0==0)&&(itm_id.byte1==0)) return 0; if (N<=0) return 0;
   int n_tgt_cell=-1;
   int i; for (i=0; i<30; i++) 
     if ((slots[i].type_id.byte0==itm_id.byte0)&&(slots[i].type_id.byte1==itm_id.byte1)) 
        { n_tgt_cell=i; break; }
   if (n_tgt_cell<0) for (i=0; i<30; i++)
     if ((slots[i].type_id.byte0==0)&&(slots[i].type_id.byte1==0)) 
        { n_tgt_cell=i; break; }
   if (n_tgt_cell<0) return 0;
   slots[n_tgt_cell].type_id.byte0=itm_id.byte0;
   slots[n_tgt_cell].type_id.byte1=itm_id.byte1;
   slots[n_tgt_cell].durability+=N;
   for (i=0; i<2048; i++) buffer[i]=0;
   int j=7; buffer[j++]=0x95; buffer[j++]=0x13; buffer[j++]=0x01; buffer[j++]=0xFF; buffer[j++]=0xFF; 
   j=MakeSlotSegment(buffer, j, n_tgt_cell);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}
int mw_loot::DoAddToInventory(char *buffer, struct mw_id itm_id)
{
    return DoAddToInventory(buffer, itm_id, 1);
}

mw_gear::mw_gear()
{
   inv_type=_MW_INV_GEAR;
   mpStrCopy(base_fname,"gear.ini");
}
int mw_gear::MakeInfoPkt(char *buffer)
{
   int i; for (i=0; i<2048; i++) buffer[i]=0;
   int j=7; buffer[j++]=0x03; buffer[j++]=0x04;
   buffer[j++]=0x01; j++; j++; j++; j++; //skip session id
   for (i=0; i<=10; i++) j=MakeSlotSegment(buffer, j, i);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}

mw_skills::mw_skills()
{
   mpStrCopy(base_fname,"skills.ini");
   inv_type=_MW_INV_SKILLS;
}

int mw_skills::MakeSlotSegment(char *buffer, int counter, int n_slot)
{
   if ((n_slot<0)||(n_slot>=30)) return counter;
   int j=counter; char* segment=&(buffer[j]);
   int i; for (i=0; i<=32; i++) {segment[i]=0; j++;}
   segment[0]=slots[n_slot].type_id.byte0; segment[1]=slots[n_slot].type_id.byte1;
   segment[2]=slots[n_slot].gem1_id.byte0;
   return j;
}
int mw_skills::MakeInfoPkt(char *buffer)
{
   bool iventory_has_item=false;
   int i; for (i=0; i<30; i++) if ((slots[i].type_id.byte0!=0)||(slots[i].type_id.byte1!=0)) iventory_has_item=true;
   if (!iventory_has_item) return 0;
   for (i=0; i<2048; i++) buffer[i]=0;
   int j=7; buffer[j++]=0x83; buffer[j++]=0x14;
   for (i=0; i<=30; i++) if ((slots[i].type_id.byte0!=0)||(slots[i].type_id.byte1!=0)) j=MakeSlotSegment(buffer, j, i);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}
int mw_skills::DoAddToInventory(char *buffer, struct mw_id itm_id)
{
   if ((itm_id.byte0==0)&&(itm_id.byte1==0)) return 0; 
   int n_tgt_cell=-1;
   int i; for (i=0; i<30; i++) 
     if ((slots[i].type_id.byte0==itm_id.byte0)&&(slots[i].type_id.byte1==itm_id.byte1)) 
        { n_tgt_cell=i; break; }
   if (n_tgt_cell<0) for (i=0; i<30; i++)
     if ((slots[i].type_id.byte0==0)&&(slots[i].type_id.byte1==0)) 
        { n_tgt_cell=i; break; }
   if (n_tgt_cell<0) return 0;
   slots[n_tgt_cell].type_id.byte0=itm_id.byte0;
   slots[n_tgt_cell].type_id.byte1=itm_id.byte1;
   slots[n_tgt_cell].gem1_id.byte0++;
   for (i=0; i<2048; i++) buffer[i]=0;
   int j=7; buffer[j++]=0x83; buffer[j++]=0x14;  
   j=MakeSlotSegment(buffer, j, n_tgt_cell);
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}
int mw_skills::getSkillLvl(struct mw_id skill_id)
{
   if ((skill_id.byte0==0)&&(skill_id.byte1==0)) return 0; 
   int i; for (i=0; i<30; i++) 
     if ((slots[i].type_id.byte0==skill_id.byte0)&&(slots[i].type_id.byte1==skill_id.byte1)) 
       return slots[i].gem1_id.byte0;
   return 0;
}
struct mw_id mw_skills::getSkillEffect(struct mw_id skill_id)
{
   struct mw_id res; res.byte0=0; res.byte1=0;
   if ((skill_id.byte0==0)&&(skill_id.byte1==0)) return res; 
   int i; for (i=0; i<30; i++) 
     if ((slots[i].type_id.byte0==skill_id.byte0)&&(slots[i].type_id.byte1==skill_id.byte1)) 
       return slots[i].gem2_id;
   return res;
}
int mw_skills::getSkillType(struct mw_id skill_id)
{
   if ((skill_id.byte0==0)&&(skill_id.byte1==0)) return 0; 
   int i; for (i=0; i<30; i++)
     if ((slots[i].type_id.byte0==skill_id.byte0)&&(slots[i].type_id.byte1==skill_id.byte1)) 
       return slots[i].iid.byte0;
   return 0;
}

