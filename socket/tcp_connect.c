
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include <fenice/utils.h>
#include <fenice/socket.h>
#include <fenice/log.h>

tsocket tcp_connect(unsigned short port, char *addr)
{
	tsocket f;
	int on = 1;
	int one = 1;		/*used to set SO_KEEPALIVE */

	struct sockaddr_in s;
	int v = 1;
	if ((f = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		ERRORLOGG("socket() error in tcp_connect.");
		return ERR_GENERIC;
	}
	setsockopt(f, SOL_SOCKET, SO_REUSEADDR, (char *) &v, sizeof(int));
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = inet_addr(addr);	//htonl(addr);
	s.sin_port = htons(port);
	// set to non-blocking
	if (ioctl(f, FIONBIO, &on) < 0) {
		ERRORLOGG("ioctl() error in tcp_connect.");
		return ERR_GENERIC;
	}
	if (connect(f, (struct sockaddr *) &s, sizeof(s)) < 0) {
		ERRORLOGG("connect() error in tcp_connect.");
		return ERR_GENERIC;
	}
	if (setsockopt(f, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one)) < 0) {
		ERRORLOGG("setsockopt() SO_KEEPALIVE error in tcp_connect.");
		return ERR_GENERIC;
	}
	return f;
}
