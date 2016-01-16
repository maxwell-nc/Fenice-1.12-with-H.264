
#include <string.h>

#include <fenice/rtsp.h>
#include <fenice/en_xmalloc.h>

void RTSP_initserver(
		RTSP_buffer * rtsp,
		tsocket fd,
		RTSP_proto proto)
{
	rtsp->fd = fd;
	rtsp->proto = proto;
	rtsp->session_list = xnew0(RTSP_session);
	rtsp->session_list->session_id = -1;
}
