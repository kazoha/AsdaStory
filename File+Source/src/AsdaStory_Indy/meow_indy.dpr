{ THIS FILE IS A PART OF MEOW PROJECT (c) 2008 by Yuno and Urf         }
{ PURPOSE: this is a network interface dll for the server              }
{ the dll provides functions for init and shutdown and uses pointer    }
{ to external callback function which should work on server calls      }
{ PLATFORM PC + WINDOWS + DELPHI 6-8 or LAZARUS + INDY SDK (usually    }
{ in Delphi)                                                           }

library meow_indy;
uses
  SysUtils, StrUtils, Classes, IdBaseComponent, IdComponent,
  IdTCPServer, IdStack;

const max_connects=99;
      max_data_length=8192;
      crypt_key : array[0..31] of byte =
      ($87,$96,$B4,$42,$71,$C5,$57,$35,$3D,$11,$8B,$69,$77,$94,$59,$55,
       $95,$69,$D5,$95,$3C,$9A,$AA,$20,$E1,$15,$CD,$50,$AA,$E8,$04,$40);
                                      , IdBaseComponent, IdComponent,
  IdTCPServer, IdStack
type
  t_c_callback = function(
    n_connect: integer;
    is_map: integer;
    n_rcvd_bytes: integer;
    incom_buf: pointer
    ): Integer; stdcall;

TProgressThread = class(TThread)
  private
  ctr: integer;
  protected
  constructor Create(CreateSuspended: Boolean); virtual;
  procedure Execute; override;
end;

aServer  = class
  ChrServer: TIdTCPServer;
  MapServer: TIdTCPServer;
  srvTimer: TProgressThread;
  constructor Create();
  destructor Destroy();  override;
  procedure Chr_ServerConnect(AThread: TIdPeerThread);
  procedure Chr_ServerExecute(AThread: TIdPeerThread);
  procedure Chr_ServerDisconnect(AThread: TIdPeerThread);
  procedure Map_ServerConnect(AThread: TIdPeerThread);
  procedure Map_ServerExecute(AThread: TIdPeerThread);
  procedure Map_ServerDisconnect(AThread: TIdPeerThread);
  procedure callsvProgress();
  private
  { Private declarations }
  public
  { Public declarations }
end;


var
  svr: aServer;
  sv_callback: t_c_callback;
  sv_progress_callback: t_c_callback;
  map_connections: array[0..max_connects] of TIdPeerThread;
  chr_connections: array[0..max_connects] of TIdPeerThread;
  do_debug_output: integer;

procedure decrypt_pkt(var in_pkt: array of byte; n_bytes: integer);
var len_bytes: integer;
    i, d: integer;
    left_border, right_border: integer;
    pkt2pos: integer;
begin
    pkt2pos:=0;
    repeat
      len_bytes:=in_pkt[pkt2pos+1]+256*in_pkt[pkt2pos+2];
      left_border:=3;
      right_border:=len_bytes-2;
      if right_border<left_border then break;
      d:=(right_border-left_border+1) mod 8; if d>0 then right_border:=right_border-d;
      if right_border<left_border then break;
      for i:=left_border to right_border do in_pkt[i+pkt2pos]:=in_pkt[i+pkt2pos] xor crypt_key[i mod 32];
      pkt2pos:=pkt2pos+len_bytes;
    until pkt2pos>n_bytes;
end;

function ByteArrayToHexString(var ba: array of byte; length: integer): string;
var i, t: integer;
begin
  result:='';
  if length<1 then exit;
  for i:=0 to length-1 do begin
     t:=ba[i]; result:=result+inttohex(t, 2)+' ';
  end; //for i
end;

constructor TProgressThread.Create(CreateSuspended: Boolean);
begin
  inherited Create(CreateSuspended); ctr:=0;
end;

procedure TProgressThread.Execute;
begin
  ctr:=ctr+1;
  if ctr>7 then begin
    writeln('tyt');
    if assigned(sv_progress_callback) then sv_progress_callback(0,0,0,nil);
     ctr:=0;
  end;
//  sleep(1000);
end;

constructor aServer.Create();
var i: integer;
begin
  srvTimer:=TProgressThread.create(true);
  ChrServer:=TIdTCPServer.Create(nil);
  MapServer:=TIdTCPServer.Create(nil);
  ChrServer.DefaultPort:=35061;
  MapServer.DefaultPort:=35062;
  ChrServer.OnConnect:=Chr_ServerConnect;
  ChrServer.OnExecute:=Chr_ServerExecute;
  ChrServer.OnDisconnect:=Chr_ServerDisconnect;
  MapServer.OnConnect:=Map_ServerConnect;
  MapServer.OnExecute:=Map_ServerExecute;
  MapServer.OnDisconnect:=Map_ServerDisconnect;
  for i:=0 to max_connects do map_connections[i]:= nil;
  for i:=0 to max_connects do chr_connections[i]:= nil;
  writeln('Local Name: ',ChrServer.LocalName);
  writeln('On IP:',GStack.LocalAddress,':',ChrServer.DefaultPort);
  ChrServer.Active := True;
  MapServer.Active := True;
end;

destructor aServer.Destroy;
var i: integer;
begin
  ChrServer.Active := false;
  MapServer.Active := false;
  ChrServer.Destroy;
  MapServer.Destroy;
  if assigned(srvTimer) then srvTimer.Destroy;
  for i:=0 to max_connects do map_connections[i]:= nil;
  for i:=0 to max_connects do chr_connections[i]:= nil;
end;

procedure aServer.Chr_ServerConnect(AThread: TIdPeerThread);
var i: integer;
begin
  for i:=0 to max_connects do if not assigned(chr_connections[i]) then break;
  chr_connections[i]:=AThread;
end;

procedure aServer.Chr_ServerDisconnect(AThread: TIdPeerThread);
var i: integer;
begin
  for i:=0 to max_connects do if assigned(chr_connections[i]) then
    if chr_connections[i]=AThread then chr_connections[i]:=nil;
end;

procedure aServer.Map_ServerConnect(AThread: TIdPeerThread);
var i: integer;
begin
  for i:=0 to max_connects do if not assigned(map_connections[i]) then break;
  map_connections[i]:=AThread;
end;

procedure aServer.Map_ServerDisconnect(AThread: TIdPeerThread);
var i: integer;
begin
  for i:=0 to max_connects do if assigned(map_connections[i]) then
    if map_connections[i]=AThread then map_connections[i]:=nil;
end;

procedure aServer.Map_ServerExecute(AThread: TIdPeerThread);
var i, n: integer;
    sz: integer;
    incom_buf: array[0..8192] of byte;
    str: string;
begin
  for i:=0 to max_connects do if assigned(map_connections[i]) then
    if map_connections[i]=AThread then break; n:=i;
  if i>=max_connects then exit;
  for i:=0 to 8191 do incom_buf[i]:=0;
  sz:=AThread.Connection.ReadFromStack();
  AThread.Connection.ReadBuffer(incom_buf,sz);
  decrypt_pkt(incom_buf, sz);
  if do_debug_output<>0 then begin
    str:=ByteArrayToHexString(incom_buf, sz-1); writeln('in>'+str);
  end;
  sv_callback(n, 1, sz, @incom_buf[0]);
end;

procedure aServer.Chr_ServerExecute(AThread: TIdPeerThread);
var i, n: integer;
    sz: integer;
    incom_buf: array[0..8192] of byte;
    str: string;
begin
  for i:=0 to max_connects do if assigned(chr_connections[i]) then
    if chr_connections[i]=AThread then break; n:=i;
  if i>=max_connects then exit;
  for i:=0 to 8191 do incom_buf[i]:=0;
  sz:=AThread.Connection.ReadFromStack();
  AThread.Connection.ReadBuffer(incom_buf,sz);
  decrypt_pkt(incom_buf, sz);
  if do_debug_output<>0 then begin
    str:=ByteArrayToHexString(incom_buf, sz-1); writeln('in>'+str);
  end;
  sv_callback(n, 0, sz, @incom_buf[0]);
end;
procedure aServer.callsvProgress();
begin
  writeln('ppc');
  if assigned(sv_progress_callback) then
    sv_progress_callback(0,0,0,nil);
end;

procedure init(p: pointer; p1: pointer);  stdcall;
begin
 @sv_callback:=p;
 @sv_progress_callback:=p1;
 svr:=aServer.Create();
 svr.srvTimer.Priority := tpHigher;
 svr.srvTimer.Resume;
end;

procedure shutdown();  stdcall;
begin
 if assigned(svr) then svr.Destroy;
end;

procedure switch_debug();  stdcall;
begin
 if do_debug_output<>0 then do_debug_output:=0 else do_debug_output:=1;
end;


procedure test(n: integer; is_map: integer; sz: integer; p:pointer); stdcall;
var s: string; ss: string[255];
    l: integer;
begin
 s:='callback self test success'; l:=strlen(PChar(s)); ss:=s;
 sv_callback(-1, 0, l, @s[1]);
end;

procedure get_my_ip(n: integer; is_map: integer; sz: integer; p:pointer); stdcall;
var s: string; ss: string[255];
    l: integer;
begin
 s:=gstack.LocalAddress; l:=strlen(PChar(s)); ss:=s;
 sv_callback(-2, 0, l, @ss[1]);
end;

procedure write_to_n(n: integer; is_map: integer; sz: integer; p:pointer); stdcall;
var TheThread: TIdPeerThread ; i:integer;
    out_buf: array [0..2048] of byte;
    p_out_buf: PByteArray;
    str: string;
begin
  if n<0 then exit; if n>max_connects then exit; if sz<=0 then exit;
  TheThread:=nil;
  if is_map=0 then TheThread:=chr_connections[n] else TheThread:=map_connections[n];
  if not assigned(TheThread) then exit;
  for i:=0 to 2048 do out_buf[i]:=0;
  p_out_buf:=p;
  for i:=0 to sz do out_buf[i]:=p_out_buf[i];
  if do_debug_output<>0 then begin
    str:=ByteArrayToHexString(out_buf, sz-1); writeln('out>'+str);
  end;
  decrypt_pkt(out_buf, sz);
  TheThread.Connection.WriteBuffer(out_buf,sz-1);
end;

exports
  init, shutdown, test, write_to_n, get_my_ip, switch_debug;

begin
  sv_callback:=nil;
  sv_progress_callback:=nil;
  do_debug_output:=0;
end.


