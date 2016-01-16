
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fenice/multicast.h>

uint32 is_valid_multicast_address(char *ip)
{
	/* This only works on IPv4 for right now */
	struct in_addr haddr;
	unsigned int addr;

	if (!ip)
		return ERR_PARSE;

	if (!inet_aton(ip, &haddr))
		return ERR_PARSE;	/* not a valid address */

	addr = htonl(haddr.s_addr);

	if ((addr & 0xF0000000) == 0xE0000000)
		return ERR_NOERROR;

	return ERR_GENERIC;

}
