
#include <stdio.h>
#include <string.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>

/*!
 * \return -1 on ERROR
 * \return RTSP_not_full (0) if a full RTSP message is NOT present in the in_buffer yet.
 * \return RTSP_method_rcvd (1) if a full RTSP message is present in the in_buffer and is
 * ready to be handled.
 * \return RTSP_interlvd_rcvd (2) if a complete RTP/RTCP interleaved packet is present.  
 * terminate on really ugly cases.
 */
int RTSP_full_msg_rcvd(
		RTSP_buffer * rtsp,
		int *hdr_len,
		int *body_len)
// This routine is from OMS.
{
	int eomh;		/* end of message header found */
	int mb;			/* message body exists */
	int tc;			/* terminator count */
	int ws;			/* white space */
	unsigned int ml;	/* total message length including any message body */
	int bl;			/* message body length */
	char c;			/* character */
	int control;
	char *p;

	// is there an interleaved RTP/RTCP packet?
	if (rtsp->in_buffer[0] == '$') {
		uint16 *intlvd_len = (uint16 *) & rtsp->in_buffer[2];

		if ((bl = ntohs(*intlvd_len)) <= rtsp->in_size) {
			// fnc_log(FNC_LOG_DEBUG,"Interleaved RTP or RTCP packet arrived (len: %hu).\n", bl);
			if (hdr_len)
				*hdr_len = 4;
			if (body_len)
				*body_len = bl;
			return RTSP_interlvd_rcvd;
		} else {
			DEBUGLOGG("Non-complete Interleaved RTP or RTCP packet arrived.\n");
			return RTSP_not_full;
		}

	}
	eomh = mb = ml = bl = 0;
	while (ml <= rtsp->in_size) {
		/* look for eol. */
		control = strcspn(&(rtsp->in_buffer[ml]), "\r\n");
		if (control > 0)
			ml += control;
		else
			return ERR_GENERIC;

		if (ml > rtsp->in_size)
			return RTSP_not_full;	/* haven't received the entire message yet. */
		/*
		 * work through terminaters and then check if it is the
		 * end of the message header.
		 */
		tc = ws = 0;
		while (!eomh && ((ml + tc + ws) < rtsp->in_size)) {
			c = rtsp->in_buffer[ml + tc + ws];
			if (c == '\r' || c == '\n')
				tc++;
			else if ((tc < 3) && ((c == ' ') || (c == '\t')))
				ws++;	/* white space between lf & cr is sloppy, but tolerated. */
			else
				break;
		}
		/*
		 * cr,lf pair only counts as one end of line terminator.
		 * Double line feeds are tolerated as end marker to the message header
		 * section of the message.  This is in keeping with RFC 2068,
		 * section 19.3 Tolerant Applications.
		 * Otherwise, CRLF is the legal end-of-line marker for all HTTP/1.1
		 * protocol compatible message elements.
		 */
		if ((tc > 2) || ((tc == 2) && (rtsp->in_buffer[ml] == rtsp->in_buffer[ml + 1])))
			eomh = 1;	/* must be the end of the message header */
		ml += tc + ws;

		if (eomh) {
			ml += bl;	/* add in the message body length, if collected earlier */
			if (ml <= rtsp->in_size)
				break;	/* all done finding the end of the message. */
		}

		if (ml >= rtsp->in_size)
			return RTSP_not_full;	/* haven't received the entire message yet. */

		/*
		 * check first token in each line to determine if there is
		 * a message body.
		 */
		if (!mb) {	/* content length token not yet encountered. */
			if (!strncasecmp
			    (&(rtsp->in_buffer[ml]), HDR_CONTENTLENGTH,
			     strlen(HDR_CONTENTLENGTH))) {
				mb = 1;	/* there is a message body. */
				ml += strlen(HDR_CONTENTLENGTH);
				while (ml < rtsp->in_size) {
					c = rtsp->in_buffer[ml];
					if ((c == ':') || (c == ' '))
						ml++;
					else
						break;
				}

				if (sscanf(&(rtsp->in_buffer[ml]), "%d", &bl) != 1) {
					ERRORLOGG("RTSP_full_msg_rcvd(): Invalid ContentLength encountered in message.\n");
					return ERR_GENERIC;
				}
			}
		}
	}

	if (hdr_len)
		*hdr_len = ml - bl;

	if (body_len) {
		/*
		 * go through any trailing nulls.  Some servers send null terminated strings
		 * following the body part of the message.  It is probably not strictly
		 * legal when the null byte is not included in the Content-Length count.
		 * However, it is tolerated here.
		 */
		for (tc = rtsp->in_size - ml, p = &(rtsp->in_buffer[ml]);
		     tc && (*p == '\0'); p++, bl++, tc--);
		*body_len = bl;
	}

	return RTSP_method_rcvd;
}
