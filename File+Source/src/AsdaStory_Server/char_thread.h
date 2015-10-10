/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*       this file contains character server thread      */
/*             declarations                              */
#ifndef _CHAR_THREAD_H_INCLUDED_
#define _CHAR_THREAD_H_INCLUDED_

#define CHR_CONNECT_PKT 0x37
#define CHR_LOGIN_PKT 0xEB
#define CHR_ENTER1_PKT 0xF4
#define CHR_ENTER_BOX_PKT 0xED
#define CHR_CREATE_PKT 0xF6
#define CHR_ENTER_MAP_PKT 0xF8
//CHR_CREATE_PKT 0xF6
int char_exec(int n_connect, int n_rcvd_bytes, char* incom_buf);
#endif //_CHAR_THREAD_H_INCLUDED_
