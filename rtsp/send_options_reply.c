
#include <stdio.h>
#include <string.h>

#include <fenice/rtsp.h>

int send_options_reply(RTSP_buffer * rtsp, long cseq)
{
	char r[1024];
	sprintf(r, "%s %d %s" RTSP_EL "CSeq: %ld" RTSP_EL, RTSP_VER, 200,
		get_stat(200), cseq);
	strcat(r, "Public: OPTIONS,DESCRIBE,SETUP,PLAY,PAUSE,TEARDOWN" RTSP_EL);
	strcat(r, RTSP_EL);
	bwrite(r, (unsigned short) strlen(r), rtsp);

//	INFOLOGG("200 - - ");

	return ERR_NOERROR;
}
