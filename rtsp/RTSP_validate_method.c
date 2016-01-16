
#include <stdio.h>
#include <string.h>

#include <fenice/rtsp.h>

/* Return -1 if something doesn't work in the request */
int RTSP_validate_method(RTSP_buffer * rtsp)
{
	char method[32], hdr[16];
	char object[256];
	char ver[32];
	unsigned int seq;
	int pcnt;		/* parameter count */
	int mid = ERR_GENERIC;

	*method = *object = '\0';
	seq = 0;

	/* parse first line of message header as if it were a request message */

	if ((pcnt =
	     sscanf(rtsp->in_buffer, " %31s %255s %31s\n%15s %u ", method,
		    object, ver, hdr, &seq)) != 5)
		return ERR_GENERIC;

	if (!strstr(hdr, HDR_CSEQ))
		return ERR_GENERIC;

	if (strcmp(method, RTSP_METHOD_DESCRIBE) == 0) {
		mid = RTSP_ID_DESCRIBE;
	}
	if (strcmp(method, RTSP_METHOD_ANNOUNCE) == 0) {
		mid = RTSP_ID_ANNOUNCE;
	}
	if (strcmp(method, RTSP_METHOD_GET_PARAMETERS) == 0) {
		mid = RTSP_ID_GET_PARAMETERS;
	}
	if (strcmp(method, RTSP_METHOD_OPTIONS) == 0) {
		mid = RTSP_ID_OPTIONS;
	}
	if (strcmp(method, RTSP_METHOD_PAUSE) == 0) {
		mid = RTSP_ID_PAUSE;
	}
	if (strcmp(method, RTSP_METHOD_PLAY) == 0) {
		mid = RTSP_ID_PLAY;
	}
	if (strcmp(method, RTSP_METHOD_RECORD) == 0) {
		mid = RTSP_ID_RECORD;
	}
	if (strcmp(method, RTSP_METHOD_REDIRECT) == 0) {
		mid = RTSP_ID_REDIRECT;
	}
	if (strcmp(method, RTSP_METHOD_SETUP) == 0) {
		mid = RTSP_ID_SETUP;
	}
	if (strcmp(method, RTSP_METHOD_SET_PARAMETER) == 0) {
		mid = RTSP_ID_SET_PARAMETER;
	}
	if (strcmp(method, RTSP_METHOD_TEARDOWN) == 0) {
		mid = RTSP_ID_TEARDOWN;
	}

	rtsp->rtsp_cseq = seq;	/* set the current method request seq. number. */
	return mid;
}
