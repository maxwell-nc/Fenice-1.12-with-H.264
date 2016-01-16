
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include <fenice/socket.h>
#include <fenice/utils.h>
#include <fenice/log.h>

int udp_open(
		unsigned short port,
		struct sockaddr *s_addr,
		tsocket * fd)
{
	struct sockaddr_in s;
	int on = 1;		//,v=1;

	if (!*fd) {
		if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			ERRORLOGG("socket() error in udp_open.");
			return ERR_GENERIC;
		}
		// set to non-blocking
		if (ioctl(*fd, FIONBIO, &on) < 0) {
			ERRORLOGG("ioctl() error in udp_open.");
			return ERR_GENERIC;
		}
	}

	s.sin_family = AF_INET;
	s.sin_addr.s_addr = htonl(INADDR_ANY);
	s.sin_port = htons(port);
	if (bind(*fd, (struct sockaddr *) &s, sizeof(s))) {
		ERRORLOGG("bind() error in udp_open.");
		return ERR_GENERIC;
	}
	*s_addr = *((struct sockaddr *) &s);

	return ERR_NOERROR;
}
