
#include <unistd.h>
#include <netinet/in.h>

#include <fenice/socket.h>
#include <fenice/log.h>

int tcp_read(tsocket fd, void *buffer, int nbytes)
{
	int n;

#if 0
	struct sockaddr_storage name;
	socklen_t namelen = sizeof(name);
	char addr_str[128];	/* Unix domain is largest */
#endif

	n = recv(fd, buffer, nbytes, 0);
	if (n < 0) {
		ERRORLOGG("recv() failure: %s", strerror(errno));
	}

#if 0
	if (n > 0) {
		if (getpeername(fd, (struct sockaddr *) &name, &namelen) < 0) {
			INFOLOGG("- - - ");
		} else {
			INFOLOGG("%s - - ",
					 sock_ntop_host((struct sockaddr *) &name,
									namelen, addr_str,
									sizeof(addr_str)));
		}
	}
#endif

	return n;
}
