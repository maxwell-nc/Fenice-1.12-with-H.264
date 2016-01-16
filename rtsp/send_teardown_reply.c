
#include <stdio.h>
#include <string.h>

#include <config.h>
#include <fenice/rtsp.h>
#include <fenice/log.h>

int send_teardown_reply(RTSP_buffer * rtsp, long session_id, long cseq)
{
	char r[1024];
	char temp[30];
	/* build a reply message */
	sprintf(r,
		"%s %d %s" RTSP_EL "CSeq: %ld" RTSP_EL "Server: %s/%s" RTSP_EL,
		RTSP_VER, 200, get_stat(200), cseq, PACKAGE, VERSION);
	add_time_stamp(r, 0);
	strcat(r, "Session: ");
	sprintf(temp, "%ld", session_id);
	strcat(r, temp);
	// strcat(r, "\r\n\r\n");
	strcat(r, RTSP_EL RTSP_EL);
	bwrite(r, (unsigned short) strlen(r), rtsp);

	INFOLOGG("200 - - ");

	return ERR_NOERROR;
}
