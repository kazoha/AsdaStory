/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*       this file contains map  server thread           */
/*             declarations                              */
#ifndef _MAP_THREAD_H_INCLUDED_
#define _MAP_THREAD_H_INCLUDED_
#define MAP_CONNECT_PKT 0x37
#define MAP_GET_ID_PKT 0x20
#define MAP_ENTER_PKT 0xA1
#define MAP_REENTER_PKT 0xC2
#define MAP_READY_PKT 0xBB
#define MAP_MOVE_PKT 0xA6
#define MAP_CHAT_PKT 0xDC
#define MAP_EMOTE_PKT 0xB8
#define MAP_EQUIP_PKT0 0x88
#define MAP_EQUIP_PKT1 0x13
#define MAP_DROP_PKT0 0x8E
#define MAP_DROP_PKT1 0x13
#define MAP_PICK_PKT0 0x94
#define MAP_PICK_PKT1 0x13
#define MAP_BUY_PKT0 0x9C
#define MAP_BUY_PKT1 0x13
#define MAP_IM_BUY_PKT0 0xCE
#define MAP_IM_BUY_PKT1 0x13
#define MAP_SKILL_UP_PKT0 0x85
#define MAP_SKILL_UP_PKT1 0x14
#define MAP_USE_SKILL_ON_CHR_PKT0 0x88
#define MAP_USE_SKILL_ON_CHR_PKT1 0x14
#define MAP_ATK_PKT0 0xBA
#define MAP_ATK_PKT1 0x0F
#define MAP_ATKC_PKT0 0xBC
#define MAP_ATKC_PKT1 0x0F
#define MAP_QUIT_PKT0 0xCB
#define MAP_QUIT_PKT1 0x13
int map_exec(int n_connect, int n_rcvd_bytes, char* incom_buf);
#endif //_MAP_THREAD_H_INCLUDED_
