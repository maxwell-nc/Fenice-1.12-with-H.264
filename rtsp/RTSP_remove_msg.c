
#include <string.h>

#include <fenice/rtsp.h>

void RTSP_remove_msg(int len, RTSP_buffer * rtsp)
{
	rtsp->in_size -= len;
	if (rtsp->in_size && len) {	/* discard the message from the in_buffer. */
		memmove(rtsp->in_buffer, &(rtsp->in_buffer[len]),
			RTSP_BUFFERSIZE - len);
		memset(&(rtsp->in_buffer[len]), 0, RTSP_BUFFERSIZE - len);
	}
}
