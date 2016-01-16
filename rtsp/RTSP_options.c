
#include <stdio.h>
#include <string.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>

/*
 	****************************************************************
 	*			OPTIONS METHOD HANDLING
 	****************************************************************
*/

int RTSP_options(RTSP_buffer * rtsp)
{

	char *p;
	char trash[255];
	char url[255];
	char method[255];
	char ver[255];
	unsigned int cseq;


	// CSeq
	if ((p = strstr(rtsp->in_buffer, HDR_CSEQ)) == NULL) {
		send_reply(400, 0, rtsp);	/* Bad Request */
		return ERR_NOERROR;
	} else {
		if (sscanf(p, "%254s %d", trash, &(rtsp->rtsp_cseq)) != 2) {
			send_reply(400, 0, rtsp);	/* Bad Request */
			return ERR_NOERROR;
		}
	}
	cseq = rtsp->rtsp_cseq;

	sscanf(rtsp->in_buffer, " %31s %255s %31s ", method, url, ver);

//	INFOLOGG("%s %s %s ", method, url, ver);
	send_options_reply(rtsp, cseq);
	// See User-Agent 
	if ((p = strstr(rtsp->in_buffer, HDR_USER_AGENT)) != NULL) {
		char cut[strlen(p)];
		strcpy(cut, p);
		p = strstr(cut, "\n");
		cut[strlen(cut) - strlen(p) - 1] = '\0';
//		INFOLOGG("%s", cut);
	} else {
		INFOLOGG("- ");
	}

	return ERR_NOERROR;
}
