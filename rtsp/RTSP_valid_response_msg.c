
#include <stdio.h>
#include <string.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>

int RTSP_valid_response_msg(
		unsigned short *status,
		char *msg,
		RTSP_buffer * rtsp)
// This routine is from OMS.
{
	char ver[32], trash[15];
	unsigned int stat;
	unsigned int seq;
	int pcnt;		/* parameter count */

	*ver = *msg = '\0';
	/* assuming "stat" may not be zero (probably faulty) */
	stat = 0;

	pcnt = sscanf(rtsp->in_buffer, " %31s %u %s %s %u\n%255s ",
				  ver, &stat, trash, trash, &seq, msg);

	/* check for a valid response token. */
	if STRNCASECMP(ver, !=, "RTCP/", 5) {
		return 0;  /* not a response message */
	}

#if 0
	if (strncasecmp(ver, "RTSP/", 5))
		return 0;	/* not a response message */
#endif

	/* confirm that at least the version, status code and sequence are there. */
	if (pcnt < 3 || stat == 0)
		return 0;	/* not a response message */

	/*
	 * here is where code can be added to reject the message if the RTSP
	 * version is not compatible.
	 */

	/* check if the sequence number is valid in this response message. */
	if (rtsp->rtsp_cseq != seq + 1) {
		ERRORLOGG("Invalid sequence number returned in response.");
		return ERR_GENERIC;
	}

	*status = stat;
	return 1;
}
