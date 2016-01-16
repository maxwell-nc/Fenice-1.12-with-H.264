
#include <fenice/rtp.h>

uint32 start_port = RTP_DEFAULT_PORT;
uint32 port_pool[ONE_FORK_MAX_CONNECTION];

void RTP_port_pool_init(int port)
{
	int i;
	start_port = port;
	for (i = 0; i < ONE_FORK_MAX_CONNECTION; ++i) {
		port_pool[i] = i + start_port;
	}
}
