
#include <fenice/rtsp.h>

void RTSP_discard_msg(RTSP_buffer * rtsp)
{
	int hlen, blen;

	if (RTSP_msg_len(rtsp, &hlen, &blen) > 0)
		RTSP_remove_msg(hlen + blen, rtsp);
}
