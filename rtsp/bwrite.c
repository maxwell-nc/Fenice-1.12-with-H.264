
#include <string.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>

int bwrite(
		char *buffer,
		unsigned short len,
		RTSP_buffer * rtsp)
{
	if ((rtsp->out_size + len) > (int) sizeof(rtsp->out_buffer)) {
		ERRORLOGG("bwrite(): not enough free space in out message buffer.");
		return ERR_ALLOC;
	}
	memcpy(&(rtsp->out_buffer[rtsp->out_size]), buffer, len);
	rtsp->out_buffer[rtsp->out_size + len] = '\0';
	rtsp->out_size += len;
	return ERR_NOERROR;
}
