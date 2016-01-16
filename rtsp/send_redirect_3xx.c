
#include <stdio.h>
#include <string.h>
#include <config.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>
#include <fenice/en_xmalloc.h>

uint32 send_redirect_3xx(RTSP_buffer * rtsp, char *object)
{
	char *r;		/* get reply message buffer pointer */
	uint8 *mb;		/* message body buffer pointer */
	uint32 mb_len;
	SD_descr *matching_descr;

	if (enum_media(object, &matching_descr) != ERR_NOERROR) {
		ERRORLOGG("SETUP request specified an object file which can be damaged.");
		send_reply(500, 0, rtsp);	/* Internal server error */
		return ERR_NOERROR;
	}
	//if(!strcasecmp(matching_descr->twin,"NONE") || !strcasecmp(matching_descr->twin,"")){
	if (!(matching_descr->flags & SD_FL_TWIN)) {
		send_reply(453, 0, rtsp);
		return ERR_NOERROR;
	}
	/* allocate buffer */
	mb_len = 2048;
	mb = xmalloc(mb_len);
	r = xmalloc(mb_len + 1512);

	/* build a reply message */
	sprintf(r, "%s %d %s" RTSP_EL "CSeq: %d" RTSP_EL "Server: %s/%s" RTSP_EL,
		RTSP_VER, 302, get_stat(302), rtsp->rtsp_cseq, PACKAGE, VERSION);
	sprintf(r + strlen(r), "Location: %s" RTSP_EL, matching_descr->twin);	/*twin of the first media of the aggregate movie */

	strcat(r, RTSP_EL);

	bwrite(r, (unsigned short) strlen(r), rtsp);

	xfree(mb);
	xfree(r);

//	INFOLOGG("REDIRECT response sent.\n");

	return ERR_NOERROR;

}
