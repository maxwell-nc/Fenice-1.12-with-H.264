
#include <stdio.h>
#include <string.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>
#include <fenice/en_xmalloc.h>

int send_reply(int err, char *addon, RTSP_buffer * rtsp)
{
	unsigned int len;
	char *b;
	int res;
	char method[32];
	char object[256];
	char ver[32];


	if (addon != NULL) {
		len = 256 + strlen(addon);
	} else {
		len = 256;
	}

	b = xmalloc(len);
	if (b == NULL) {
		ERRORLOGG("send_reply(): memory allocation error.");
		return ERR_ALLOC;
	}
	memset(b, 0, len);
	sprintf(b, "%s %d %s" RTSP_EL "CSeq: %d" RTSP_EL, RTSP_VER, err,
		get_stat(err), rtsp->rtsp_cseq);
	//---patch coerenza con rfc in caso di errore
	// strcat(b, "\r\n");
	strcat(b, RTSP_EL);

	res = bwrite(b, (unsigned short) strlen(b), rtsp);
	xfree(b);

	sscanf(rtsp->in_buffer, " %31s %255s %31s ", method, object, ver);
//	INFOLOGG("%s %s %s %d - - ", method, object, ver, err);

	return res;
}
