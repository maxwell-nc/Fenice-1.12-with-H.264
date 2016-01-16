
#ifndef _SOCKETH
#define _SOCKETH

#include <config.h>
typedef unsigned short u_int16;

typedef unsigned long u_int32;

#ifndef BYTE_ORDER

#define LITTLE_ENDIAN	1234
#define BIG_ENDIAN	4321

#if defined(sun) || defined(__BIG_ENDIAN) || defined(NET_ENDIAN)
#define BYTE_ORDER	BIG_ENDIAN
#else
#define BYTE_ORDER	LITTLE_ENDIAN
#endif

#endif


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef int tsocket;

#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
#define MAXSOCKADDR 128		/*!< max socket address structure size */
struct sockaddr_storage {
	char padding[MAXSOCKADDR];
};
#endif				// HAVE_STRUCT_SOCKADDR_STORAGE

char *get_address();
char *sock_ntop_host(const struct sockaddr *, socklen_t, char *, size_t);
	//TCP
tsocket tcp_listen(unsigned short port);
tsocket tcp_accept(tsocket fd);
int tcp_read(tsocket fd, void *buffer, int nbytes);
int tcp_write(tsocket fd, void *buffer, int nbytes);
void tcp_close(tsocket s);
tsocket tcp_connect(unsigned short port, char *addr);
	//UDP
int udp_connect(unsigned short to_port, struct sockaddr *s_addr, int addr,
		tsocket * fd);
int udp_open(unsigned short port, struct sockaddr *s_addr, tsocket * fd);
int udp_close(tsocket fd);

#define MAX_INTERLVD_STMS 11
#endif
