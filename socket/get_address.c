
#include <stdio.h>

#include <fenice/socket.h>
#include <unistd.h>
#include <netdb.h>

/* Return the IP address of this machine. */
char *get_address()
{
	static char Ip[256];
	char server[256];
	u_char addr1, addr2, addr3, addr4, temp;
	u_long InAddr;
	struct hostent *host;

	gethostname(server, 256);
	host = gethostbyname(server);

	temp = 0;
	InAddr = *(u_int32 *) host->h_addr;
	addr4 = (unsigned char) ((InAddr & 0xFF000000) >> 0x18);
	addr3 = (unsigned char) ((InAddr & 0x00FF0000) >> 0x10);
	addr2 = (unsigned char) ((InAddr & 0x0000FF00) >> 0x8);
	addr1 = (unsigned char) (InAddr & 0x000000FF);

#if (BYTE_ORDER == BIG_ENDIAN)
	temp = addr1;
	addr1 = addr4;
	addr4 = temp;
	temp = addr2;
	addr2 = addr3;
	addr3 = temp;
#endif

	sprintf(Ip, "%d.%d.%d.%d", addr1, addr2, addr3, addr4);

	return Ip;
}
