/*           this file is part of the MEOW project       */
/*              MicMac Online Emulated World             */
/*           the project is under GNU license            */
/*           copyright Yuno  and Urf (c) 2008            */
/*     this file contains dll_write function export       */
#ifndef _MEOW_MAIN_H_INCLUDED_
#define _MEOW_MAIN_H_INCLUDED_
int dll_write_n(int n_connect, int is_map, int n_send_bytes, char* out_buf);
int flt_bytes2int(char* bytes);
void int2flt_bytes(char* out_bytes, int in_int);
#endif //_MEOW_MAIN_H_INCLUDED_
