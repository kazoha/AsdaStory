/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno and Urf (c) 2008             */
/*       this file contains character server thread      */
/*             implementation                            */
#include <iostream>
#include <io.h>
#include "meow_main.h"
#include "char_thread.h"
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include "meow_sess.h"
#include "meow_nice_pkt.h"


int char_exec(int n_connect, int n_rcvd_bytes, char* incom_buf)
{
    
    char out_buf[2048]; int i; for (i=0; i<2048; i++) out_buf[i]=0;
    	 
    if (n_rcvd_bytes<=8) return 0;
    
    if ((unsigned char)incom_buf[7]==CHR_LOGIN_PKT)
    {
  // login correto: 53        (antigo 33 | 53 senha)
      char* short_login=incom_buf+53; char* password=incom_buf+85;
      short_login[18]=0; password[18]=0; //assure terminators
      char login[255]; mpStrCopy(login,"_users/"); mpStrAddStr(login,short_login);
      printf("tentativa de login:\n", login, password);
     

	 mw_session* new_session= new mw_session();
      new_session->LoadFromFile(login);
      if (!new_session->loadsuccess)
      {
   //   printf("Conta %s entrou.\n",login);
	  //	  printf("\n\nComparação - Senha sistema: %s\nSenha digitada: %s\n\n",new_session->password, password); 
      	  
      	  
        if(short_login[0]=='@') //create account
        { 
          short_login=incom_buf+54; mpStrCopy(login,"_users/"); mpStrAddStr(login,short_login);
          mpStrCopy(new_session->login, login);
//        assure that login is really new
          new_session->LoadFromFile(login);
          if ((new_session->loadsuccess)||(short_login[0]=='.')||(short_login[0]=='_')
               ||(short_login[0]=='/')||(short_login[0]=='\\')||(short_login[0]=='*')
               ||(short_login[0]=='?'))
          {
            new_session->number_at_chr=n_connect;
            new_session->send_invalid_login();
            delete new_session;
            return 0;
          }// end if duplicate or invalid acc
          mpStrCopy(new_session->password, password);
          mkdir(login);
          new_session->SaveToFile(new_session->login);
          new_session->number_at_chr=n_connect;
          
         new_session->loadsuccess=true; // correção
            
            
            
        } else 
        {//login failed
          new_session->number_at_chr=n_connect;
          new_session->send_invalid_login();
          delete new_session;
          return 0;
        }//end if @
      }//end if not loadsuccess
      if (new_session->loadsuccess)
      {
         if (!mpStrCompare(new_session->password, password))
         {//login failed
       
            new_session->number_at_chr=n_connect;
            new_session->send_invalid_login();
            delete new_session;
            return 0;
         }//end if not password
         new_session->number_at_chr=n_connect;
   //  new_session->enter_map(); // só funciona com esse enter_map *1
   new_session->enter_char_create();



        for (i=0; i<=nice_box_reply0_sz; i++) out_buf[i]=nice_box_reply0[i];
          dll_write_n(n_connect, 0, nice_box_reply0_sz+1, &(out_buf[0])); 
          for (i=0; i<=open_box_reply_sz; i++) out_buf[i]=open_box_reply[i];
          dll_write_n(n_connect, 0, open_box_reply_sz+1, &(out_buf[0])); 
          for (i=0; i<2048; i++) out_buf[i]=0; for (i=0; i<=chr_info_pkt_sz; i++) out_buf[i]=chr_info_pkt[i];
          dll_write_n(n_connect, 0, open_box_reply_sz+1, &(out_buf[0])); 
}//end if loadsuccess
    } //end if CHR_LOGIN_PKT

    printf("pacote[7]: %u\n\n",(unsigned char)incom_buf[7]);
    if ((unsigned char)incom_buf[7]==CHR_CREATE_PKT) // IF LOGIN OK
              
      for (i=0; i<_MW_MAX_SESSIONS; i++) if (mw_sessions[i]) if (mw_sessions[i]->number_at_chr==n_connect)
      {
      	printf("criando personagem...\n");
  
  
      int vtncc = 0;
          FILE *fp;
fp=fopen("C:\\captura_entrarmapa.txt", "wb");
     while(vtncc < 93) {
          //	fprintf(fp4, "in_[%d] -  %u\n", vtnc, (unsigned char)incom_buf[vtnc]);
     		fprintf(fp, "incom[%d] -  %u\n", vtncc, (unsigned char)incom_buf[vtncc]);
     		printf("incom[%d] - %u\n",vtncc,(unsigned char)incom_buf[vtncc]);
     		vtncc+=1;
	 }	  
         mpStrCopy(mw_sessions[i]->chr.chr_name,&(incom_buf[40])); //40
         mw_sessions[i]->chr.constelation=incom_buf[64];//constelação :D OK!
         mw_sessions[i]->chr.gender=incom_buf[60];//60 
         mw_sessions[i]->chr.hair=incom_buf[61];//61
         mw_sessions[i]->chr.color=incom_buf[62];//62
         mw_sessions[i]->chr.face=incom_buf[63];//63 - 
         mw_sessions[i]->gear.slots[1].type_id.byte0=incom_buf[65];//68
         mw_sessions[i]->gear.slots[1].type_id.byte1=incom_buf[69];//69
         mw_sessions[i]->gear.slots[1]._param=1;
         mw_sessions[i]->gear.slots[4].type_id.byte0=incom_buf[66];//72
         mw_sessions[i]->gear.slots[4].type_id.byte1=incom_buf[73];//73
         mw_sessions[i]->gear.slots[4]._param=1;
         mw_sessions[i]->gear.slots[2].type_id.byte0=incom_buf[67];//76
         mw_sessions[i]->gear.slots[2].type_id.byte1=incom_buf[77];//77
         mw_sessions[i]->gear.slots[2]._param=1;
         mw_sessions[i]->gear.slots[3].type_id.byte0=incom_buf[68];//80
         mw_sessions[i]->gear.slots[3].type_id.byte1=incom_buf[81];//81
         mw_sessions[i]->gear.slots[3]._param=1;
         mw_sessions[i]->SaveToFile(mw_sessions[i]->login);
         mw_sessions[i]->number_at_chr=n_connect;
         mw_sessions[i]->enter_map();
        
         //chr_info_pkt
      }//end if CHR_CREATE_PKT
}
