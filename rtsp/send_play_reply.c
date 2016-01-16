
#include <stdio.h>
#include <string.h>

#include <config.h>
#include <fenice/rtsp.h>
#include <fenice/log.h>

int send_play_reply(RTSP_buffer * rtsp, char *object,
		    RTSP_session * rtsp_session)
{
	char r[1024];
	char temp[30];
	RTP_session *p = rtsp_session->rtp_session;

	TRACE_FUNC();

	/* build a reply message */
	sprintf(r,
		"%s %d %s" RTSP_EL "CSeq: %d" RTSP_EL "Server: %s/%s" RTSP_EL,
		RTSP_VER, 200, get_stat(200), rtsp->rtsp_cseq, PACKAGE,
		VERSION);
	add_time_stamp(r, 0);
	strcat(r, "Session: ");
	sprintf(temp, "%d", rtsp_session->session_id);
	strcat(r, temp);
	strcat(r, RTSP_EL);
	// strcat(r, "RTP-info: url=");
	strcat(r, "RTP-info: ");
	// strcat(r, object);
	// strcat(r, ";");
	do {
		strcat(r, "url=");
		// strcat(r, object);
		// TODO: we MUST be sure to send the correct url 
		sprintf(r + strlen(r), "rtsp://%s/%s/%s!%s",
			prefs_get_hostname(), p->sd_filename, p->sd_filename,
			p->current_media->filename);
		strcat(r, ";");
		sprintf(r + strlen(r), "seq=%u;rtptime=%u", p->start_seq,
			p->start_rtptime);
		if (p->next != NULL) {
			strcat(r, ",");
		} else {
			// strcat(r, "\r\n\r\n");
			strcat(r, RTSP_EL);
		}
		p = p->next;
	} while (p != NULL);
	// end of message
	strcat(r, RTSP_EL);

	bwrite(r, (unsigned short) strlen(r), rtsp);

//	INFOLOGG("200 - %s ", object);

	return ERR_NOERROR;
}
