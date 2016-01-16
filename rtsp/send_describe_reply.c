
#include <stdio.h>
#include <string.h>

#include <config.h>
#include <fenice/rtsp.h>
#include <fenice/en_xmalloc.h>

int send_describe_reply(
		RTSP_buffer * rtsp,
		char *object,
		description_format descr_format,
		char *descr)
{
	char *r;		/* get reply message buffer pointer */
	char *mb;		/* message body buffer pointer */
	int mb_len;


	/* allocate buffer */
	mb_len = 2048;
	mb = xmalloc(mb_len);
	r = xmalloc(mb_len + 1512);

	/*describe */
	sprintf(r,
		"%s %d %s" RTSP_EL "CSeq: %d" RTSP_EL "Server: %s/%s" RTSP_EL,
		RTSP_VER, 200, get_stat(200), rtsp->rtsp_cseq, PACKAGE,
		VERSION);
	add_time_stamp(r, 0);
	switch (descr_format) {
		// Add new formats here
	case df_SDP_format:{
			strcat(r, "Content-Type: application/sdp" RTSP_EL);
			break;
		}
	}
	sprintf(r + strlen(r), "Content-Base: rtsp://%s/%s/" RTSP_EL,
		prefs_get_hostname(), object);
	sprintf(r + strlen(r), "Content-Length: %u" RTSP_EL, strlen(descr));
	// end of message
	strcat(r, RTSP_EL);

	// concatenate description
	strcat(r, descr);
	bwrite(r, (unsigned short) strlen(r), rtsp);

	xfree(mb);
	xfree(r);

//	INFOLOGG("200 %d %s ", strlen(descr), object);

	return ERR_NOERROR;
}
