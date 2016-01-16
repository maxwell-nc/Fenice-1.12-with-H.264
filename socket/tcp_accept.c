
#include <string.h>

#include <fenice/socket.h>

tsocket tcp_accept(tsocket fd)
{
	tsocket f;
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);
	f = accept(fd, (struct sockaddr *)&addr, &addrlen);

	return f;
}
