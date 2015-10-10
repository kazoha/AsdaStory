/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*  this file contains the main entrypoint to executable */
#include <cstdlib>
#include <iostream>
#include "mp_sys/mp_sys.h"
#include "mp_sys/mp_strings.h"
#include <windows.h>
#include "char_thread.h"
#include "map_thread.h"
#include "meow_sess.h"
#include "meow_npc.h"
#include "meow_portal.h"
#include "meow_drop.h"

using namespace std;

typedef void (_stdcall func1) (void* p, void* p2);
typedef void (_stdcall func2) ();
typedef void (_stdcall func3) (int n_connect, int is_map, int n_send_bytes, char* out_buf);
func1 *_dll_init_=0;
func3 *_dll_test_=0;
func2 *_dll_shutdown_=0;
func2 *_dll_debug_switch_=0;
func3 *_dll_write_n_=0;
func3 *_dll_get_my_ip_=0;
int sv_progr_counter=0;
unsigned long server_time=0;

int _stdcall sv_exec(int n_connect, int is_map, int n_rcvd_bytes, char* incom_buf);
int _stdcall sv_progress(int n_connect, int is_map, int n_rcvd_bytes, char* incom_buf);
int main(int argc, char *argv[])
{
  HINSTANCE h=LoadLibrary("meow_indy.dll");
  _dll_init_=(func1*)GetProcAddress(h, "init");
  _dll_test_=(func3*)GetProcAddress(h, "test");
  _dll_shutdown_=(func2*)GetProcAddress(h, "shutdown");
  _dll_write_n_=(func3*)GetProcAddress(h, "write_to_n");
  _dll_get_my_ip_=(func3*)GetProcAddress(h, "get_my_ip");
  _dll_debug_switch_=(func2*)GetProcAddress(h, "switch_debug");
  int i; for (i=0; i<_MW_MAX_SESSIONS; i++) mw_sessions[i]=0;
  for (i=0; i<_MW_MAX_NPCS; i++) mw_npcs[i]=0;
  for (i=0; i<_MW_MAX_DROPS; i++) mw_drops[i]=0;
  for (i=0; i<_MW_MAX_PORTALS; i++) mw_portals[i]=0; load_portals();
  _dll_init_((void*)sv_exec, (void*)sv_progress); _dll_test_(0,0,0,"test");
  _dll_get_my_ip_(0,0,0,"test");
  _dll_debug_switch_();
  while (true)  
  {
        Sleep(1000);
        sv_progress(0, 0, 0, 0);
  };
  system("PAUSE");
  _dll_shutdown_();
  return EXIT_SUCCESS;
}

int _stdcall sv_exec(int n_connect, int is_map, int n_rcvd_bytes, char* incom_buf)
{
  if (n_connect==-1) {printf ("loopback test sucess\n"); printf("b: %s\n", incom_buf); return 0;}
  if (n_connect==-2) {incom_buf[n_rcvd_bytes]=0; mpStrCopy(my_ip, incom_buf); printf("ip: %s\n", my_ip); return 0;}
  printf("incom_stream\n");
  if (is_map) map_exec(n_connect, n_rcvd_bytes, incom_buf); 
  else char_exec(n_connect, n_rcvd_bytes, incom_buf); 
  return 0;
}
int _stdcall sv_progress(int n_connect, int is_map, int n_rcvd_bytes, char* incom_buf)
{
    sv_progr_counter++; server_time+=1000;
    if (sv_progr_counter>10) {printf("server progress\n"); sv_progr_counter=0;}
    npc_progress(server_time);
    drop_progress(server_time);
    sessions_progress(server_time);
}
int dll_write_n(int n_connect, int is_map, int n_send_bytes, char* out_buf)
{
    if (!_dll_write_n_) {printf("no network write found!");return 0;}
    _dll_write_n_(n_connect, is_map, n_send_bytes, out_buf);
    return 0;
}


int flt_bytes2int(char* bytes)
{
    if (!bytes) return 0;
    char* pb; float fbuf; pb=(char*)&fbuf;
    pb[0]=bytes[0]; pb[1]=bytes[1]; pb[2]=bytes[2]; pb[3]=bytes[3];
    int i=(int)fbuf; i/=100; return i;
}
void int2flt_bytes(char* out_bytes, int in_int)
{
    if (!out_bytes) return;
    char* pb; float fbuf; pb=(char*)&fbuf;
    int i=in_int; i*=100; fbuf=i;
    out_bytes[0]=pb[0]; out_bytes[1]=pb[1]; out_bytes[2]=pb[2]; out_bytes[3]=pb[3];
}
