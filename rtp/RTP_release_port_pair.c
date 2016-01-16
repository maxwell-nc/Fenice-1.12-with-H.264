
#include <fenice/rtp.h>
#include <fenice/utils.h>

extern int start_port;
extern int port_pool[ONE_FORK_MAX_CONNECTION];

int RTP_release_port_pair(port_pair * pair)
{
	int i;
	for (i = 0; i < ONE_FORK_MAX_CONNECTION; ++i) {
		if (port_pool[i] == 0) {
			port_pool[i] =
			    (pair->RTP - start_port) / 2 + start_port;
			return ERR_NOERROR;
		}
	}
	return ERR_GENERIC;
}
