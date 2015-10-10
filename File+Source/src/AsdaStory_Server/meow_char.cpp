/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains character class implementation    */
#include "meow_char.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include <stdlib.h>
mw_char::mw_char()
{
   mpStrCopy(chr_name,"");
   mpStrCopy(chr_guild,"");
   lvl=1; job.byte0=1;  job.byte1=0;
   m_hp=128; m_mp=50; exp=0; money=500; sk_pnt=0;
   hp=m_hp; mp=m_mp;
   face=0; color=0; hair=0; gender=01; constelation=01;
   chr_str=10; chr_acc=10; chr_int=10; chr_agi=10; chr_def=10; chr_vit=10;
   hp_rec=3; mp_rec=3; unkwn_prm1=0; unkwn_prm2=0; unkwn_prm3=0;
   int i; for (i=0; i<6; i++)
   { elm_atk_bonuses[i]=0; elm_def_bonuses[i]=0; }
   my_X=125; my_Y=390;
   map_id.byte0=0x03;
   map_id.byte1=0x00;
   m_weight=6700; weight=780; macs=0;
   vehicle_id.byte0=0x00; vehicle_id.byte1=0x00;
   iid.byte0=0; iid.byte1=0;
}
mw_char::~mw_char()
{
}
void mw_char::ReadParamFromString(char* tknleft, char* tknright)
{
   char tkn[256]; int i;
   if (mpStrCompare(tknleft,"name")) 
   { mpStrTrimLeft(tknright, ' '); mpStrCopy(chr_name, tknright); return;  }
   if (mpStrCompare(tknleft,"guild")) 
   { mpStrTrimLeft(tknright, ' '); mpStrCopy(chr_guild, tknright); return;  }
   if (mpStrCompare(tknleft,"lvl")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); lvl=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"face")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); face=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"color")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); color=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"hair")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); hair=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"gender")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); gender=mpStrToInt(tkn);  return; }
     if (mpStrCompare(tknleft,"constelation")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); gender=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"job"))
   { 
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); job.byte0=strtoul(tkn,0,16);
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); job.byte1=strtoul(tkn,0,16);
   }
   if (mpStrCompare(tknleft,"map"))
   { 
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); map_id.byte0=strtoul(tkn,0,16);
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); map_id.byte1=strtoul(tkn,0,16);
   }
   if (mpStrCompare(tknleft,"vehicle"))
   { 
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); vehicle_id.byte0=strtoul(tkn,0,16);
      mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); vehicle_id.byte1=strtoul(tkn,0,16);
   }
   if (mpStrCompare(tknleft,"weight")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); weight=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"mweight")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); m_weight=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"macs")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); macs=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"mhp")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); m_hp=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"mmp")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); m_mp=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"hp")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); hp=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"mp")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); mp=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"exp")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); exp=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"skillpnt")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); sk_pnt=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"money")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); money=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"str")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); chr_str=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"acc")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); chr_acc=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"int")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); chr_int=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"agi")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); chr_agi=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"def")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); chr_def=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"vit")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); chr_vit=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"hprec")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); hp_rec=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"mprec")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); mp_rec=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"X")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_X=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"Y")) 
   { mpStrTrimLeft(tknright, ' '); mpGetAndCutLeftField(tkn, tknright, ' '); my_Y=mpStrToInt(tkn);  return; }
   if (mpStrCompare(tknleft,"atk_bonuses")) 
   {
      for (i=0; i<6; i++)
      {
        mpStrTrimLeft(tknright, ' '); 
        mpGetAndCutLeftField(tkn, tknright, ' '); 
        elm_atk_bonuses[i]=mpStrToInt(tkn);
      }
      return;
   }
   if (mpStrCompare(tknleft,"def_bonuses")) 
   {
      for (i=0; i<6; i++)
      {
        mpStrTrimLeft(tknright, ' '); 
        mpGetAndCutLeftField(tkn, tknright, ' '); 
        elm_def_bonuses[i]=mpStrToInt(tkn);
      }
      return;
   }   
}
void mw_char::LoadFromFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/char.ini");
   mpText* charfile=ReadSytxTextFromFile(filename); if (!charfile) return;
   if (charfile->nstrings<=0) {mpDestroyText(&charfile); return;}
   char s1[256]; char s2[256]; int i;
   for (i=0; i<charfile->nstrings; i++)
   {
      mpStrCopy(s2,charfile->strings[i]);  
      mpStrTrimLeft(s2, ' '); mpGetAndCutLeftField(s1, s2, ' ');
      ReadParamFromString(s1,s2);
   }
   mpDestroyText(&charfile); 
}
void mw_char::SaveToFile(char* dirname)
{
   if (!dirname) return; if (mpStrLen(dirname)<1) return;
   char filename[255]; mpStrCopy(filename, dirname); mpStrAddStr(filename,"/char.ini");
   mpText* charfile=CreateEmptyText(); if (!charfile) return;
   char str[255]; char str2[255]; int i;
   mpStrCopy(str, "name "); mpStrAddStr(str, chr_name);
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "guild "); mpStrAddStr(str, chr_guild);
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "lvl "); mpIntToStr(str2, lvl); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "face "); mpIntToStr(str2, face); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "color "); mpIntToStr(str2, color); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "hair "); mpIntToStr(str2, hair); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "gender "); mpIntToStr(str2, gender); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
      mpStrCopy(str, "constelation "); mpIntToStr(str2, gender); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "job "); itoa(job.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(job.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "iid "); itoa(iid.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(iid.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "map "); itoa(map_id.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(map_id.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "macs "); mpIntToStr(str2, macs); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "mweight "); mpIntToStr(str2, m_weight); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "weight "); mpIntToStr(str2, weight); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "mhp "); mpIntToStr(str2, m_hp); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "mmp "); mpIntToStr(str2, m_mp); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "hp "); mpIntToStr(str2, hp); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "mp "); mpIntToStr(str2, mp); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "exp "); mpIntToStr(str2, exp); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "skillpnt "); mpIntToStr(str2, sk_pnt); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "money "); mpIntToStr(str2, money); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "str "); mpIntToStr(str2, chr_str); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "acc "); mpIntToStr(str2, chr_acc); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "int "); mpIntToStr(str2, chr_int); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "agi "); mpIntToStr(str2, chr_agi); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "def "); mpIntToStr(str2, chr_def); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "vit "); mpIntToStr(str2, chr_vit); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "hprec "); mpIntToStr(str2, hp_rec); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "mprec "); mpIntToStr(str2, mp_rec); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "atk_bonuses");
   for (i=0; i<6; i++) {mpIntToStr(str2, elm_atk_bonuses[i]); mpStrAddStr(str, " "); mpStrAddStr(str, str2); }
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "def_bonuses");
   for (i=0; i<6; i++) {mpIntToStr(str2, elm_def_bonuses[i]); mpStrAddStr(str, " "); mpStrAddStr(str, str2); }
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "vehicle "); itoa(vehicle_id.byte0, str2, 16); mpStrAddStr(str, str2); 
   mpStrAddStr(str, " "); itoa(vehicle_id.byte1, str2, 16); mpStrAddStr(str, str2);
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "X "); mpIntToStr(str2, my_X); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "Y "); mpIntToStr(str2, my_Y); mpStrAddStr(str, str2); 
   mpAppendStrToText(charfile, str);
   mpStrCopy(str, "end");
   mpAppendStrToText(charfile, str);
   WriteTextToFile (filename, charfile);
}

int mw_char::MakeSkillInfoPkt(char *buffer)
{
   int i; for (i=0; i<2048; i++) buffer[i]=0;
   if ((job.byte0==0)&&(job.byte1==0)) return 0;
   int j=7;
   buffer[j++]=0xC4; buffer[j++]=0x13;
   buffer[j++]=0x01; buffer[j++]=0xAD; 
   buffer[j++]=0x6C; buffer[j++]=0xDC; 
   buffer[j++]=0x04; buffer[j++]=0x00;
   buffer[j]=0xFE; buffer[0]=0xFB; 
   buffer[1]=div(j+1, 256).rem; buffer[2]=div(j+1, 256).quot;
   return j+2;
}
