#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include <fenice/socket.h>
#include <fenice/utils.h>
#include <fenice/log.h>

int udp_connect(
		unsigned short to_port,
		struct sockaddr *s_addr,
		int addr,
		tsocket * fd)
{
	struct sockaddr_in s;
	int on = 1;		//,v=1;
	if (!*fd) {

		if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			ERRORLOGG("socket() error in udp_connect.");
			return ERR_GENERIC;
		}
		// set to non-blocking
		if (ioctl(*fd, FIONBIO, &on) < 0) {
			ERRORLOGG("ioctl() error in udp_connect.");
			return ERR_GENERIC;
		}
	}
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = addr;
	s.sin_port = htons(to_port);
	if (connect(*fd, (struct sockaddr *) &s, sizeof(s)) < 0) {
		ERRORLOGG("connect() error in udp_connect.");
		return ERR_GENERIC;
	}
	*s_addr = *((struct sockaddr *) &s);

	return ERR_NOERROR;
}
