// ==========================================================
// IW5M project
// 
// Component: client
// Sub-component: dw
// Purpose: DW entry header file
//
// Initial author: NTAuthority
// Started: 2012-07-09
// ==========================================================

#pragma once

// fixed dedi key
#define DEDI_KEY "Hard work, by these words guarded. Please don't steal. -- NTA"
#define DEDI_KEY_HASH "\x94\x04\x2F\x8F\xE3\xDF\x0B\x54\x0D\xFF\xBA\x18\x05\x4F\x8C\xBA\x49\x17\x0A\x78\x5F\x8B\xBA\x25"
#define DEDI_KEY_OB "\x7C\x52\x34\x27\x62\x45\x2E\x31\x2D\x1E\x64\x55\x4B\x15\x4D\x5B\x21\x37\x52\x63\x33\x5E\x31\x52\x4A\x65\x57\x33\x51\x41\x27\x55\x53\x1C\x12\x69\x2A\x51\x22\x4B\x56\x11\x52\x59\x28\x10\x36\x10\x46\x36\x24\x51\x2F\x1B\x66\x68\x1E\x61\x0F\x67\x00\x32\x44\x39\x33\x45\x4B\x32\x34\x39\x35\x37\x38\x33\x4B\x44\x4B\x4E\x39\x32\x39\x33\x39\x44\x4B"

#define GLOBAL_KEY "\x56\x2f\x27\x61\xdf\xf9\x51\xd4\x30\xa7\x6a\xfb\xe7\xa1\x32\x73\x1e\x3d\xca\xa6\x6e\xd8\xa2\x40"

// socket hook entry points
int WINAPI dw_connect(SOCKET socket, const sockaddr* name, int namelen);
int WINAPI dw_send(SOCKET socket, const char* buf, int len, int flags);
int WINAPI dw_sendto(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int tolen);
int WINAPI dw_recv(SOCKET socket, char* buf, int len, int flags);
int WINAPI dw_recvfrom(SOCKET socket, char* buf, int len, int flags, sockaddr* from, int* fromlen);
int	WINAPI dw_select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const timeval* timeout);

// handler functions
void dw_handle_packet(const char* buf, int len);
bool dw_packet_available();
void dw_queue_packet(char* buf, int len);
int dw_dequeue_packet(char* buf, int len);

void dw_build_game_ticket(char* buf, char* key, int gameID);
void dw_build_lsg_ticket(char* buf, char* key);

void dw_init();

// crypto functions
void dw_calculate_iv(unsigned int seed, BYTE* iv);
void dw_get_global_key(BYTE* key);
void dw_set_global_key(const BYTE* key);
void dw_decrypt_data(const char* ctext, BYTE* iv, BYTE* key, char* ptext, int len);
void dw_encrypt_data(const char* ptext, BYTE* iv, BYTE* key, char* ctext, int len);

// specific handlers
void dw_handle_auth_message(int type, const char* buf, int len);
void dw_handle_lobby_message(int type, const char* buf, int len);
void dw_handle_storage_message(int type, const char* buf, int len);
void dw_handle_tutils_message(int type, const char* buf, int len);