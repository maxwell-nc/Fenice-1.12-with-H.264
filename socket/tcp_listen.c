
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include <fenice/socket.h>
#include <fenice/utils.h>
#include <fenice/log.h>

tsocket tcp_listen(unsigned short port)
{
	tsocket f;
	int on = 1;

	struct sockaddr_in s;
	int v = 1;

	if ((f = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ERRORLOGG("socket() error in tcp_listen.");
		return ERR_GENERIC;
	}

	setsockopt(f, SOL_SOCKET, SO_REUSEADDR, (char *) &v, sizeof(int));

	s.sin_family = AF_INET;
	s.sin_addr.s_addr = htonl(INADDR_ANY);
	s.sin_port = htons(port);

	if (bind(f, (struct sockaddr *) &s, sizeof(s))) {
		ERRORLOGG("bind() error in tcp_listen.");
		return ERR_GENERIC;
	}
	// set to non-blocking
	if (ioctl(f, FIONBIO, &on) < 0) {
		ERRORLOGG("ioctl() error in tcp_listen.");
		return ERR_GENERIC;
	}

	if (listen(f, SOMAXCONN) < 0) {
		ERRORLOGG("listen() error in tcp_listen.");
		return ERR_GENERIC;
	}

	return f;
}
