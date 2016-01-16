
#include <stdio.h>
#include <string.h>

#include <fenice/rtsp.h>
#include <fenice/sdp.h>
#include <fenice/log.h>

/*
 	****************************************************************
 	*			DESCRIBE METHOD HANDLING
 	****************************************************************
*/
int RTSP_describe(RTSP_buffer * rtsp)
{
	int valid_url, res;
	char object[255], server[255], trash[255];
	char *p;
	unsigned short port;
	char url[255];
	int url_valid;
	media_entry media, req;
	description_format descr_format = df_SDP_format;	// shawill put to some default
	char descr[MAX_DESCR_LENGTH];

	/* Extract la URL */
	if (!sscanf(rtsp->in_buffer, " %*s %254s ", url)) {
		send_reply(400, 0, rtsp);	/* bad request */
		return ERR_NOERROR;
	}
//	DEBUGLOGG("rtsp->in_buffer = %s", rtsp->in_buffer);
	/* Validate the URL */
	url_valid=parse_url(url,server,sizeof(server),&port,object,sizeof(object));
	switch (url_valid) {
		case URL_BAD: //1:		// bad request
			send_reply(400, 0, rtsp);
			return ERR_NOERROR;
			break;
		case URL_ERROR: // -1:		// internal server error
			send_reply(500, 0, rtsp);
			return ERR_NOERROR;
			break;
		case URL_OK:
			break;
		default:
			break;
	}

	if (strstr(object, "../")) {
		/* disallow relative paths outside of current directory. */
		send_reply(403, 0, rtsp);	/* Forbidden */
		return ERR_NOERROR;
	}
	if (strstr(object, "./")) {
		/* Disallow ./ */
		send_reply(403, 0, rtsp);	/* Forbidden */
		return ERR_NOERROR;
	}

	p = strrchr(object, '.');
	valid_url = 0;
	if (p == NULL) {
		send_reply(415, 0, rtsp);	/* Unsupported media type */
		return ERR_NOERROR;
	} else {
		valid_url = is_supported_url(p);
	}
	if (!valid_url) {
		send_reply(415, 0, rtsp);	/* Unsupported media type */
		return ERR_NOERROR;
	}
	// Disallow Header REQUIRE
	if (strstr(rtsp->in_buffer, HDR_REQUIRE)) {
		send_reply(551, 0, rtsp);	/* Option not supported */
		return ERR_NOERROR;
	}


	/* Get the description format. SDP is recomended */
	if (strstr(rtsp->in_buffer, HDR_ACCEPT) != NULL) {
		if (strstr(rtsp->in_buffer, "application/sdp") != NULL) {
			descr_format = df_SDP_format;
		} else {
			// Add here new description formats
			send_reply(551, 0, rtsp);	/* Option not supported */
			return ERR_NOERROR;
		}
	}
	// Get the CSeq 
	if ((p = strstr(rtsp->in_buffer, HDR_CSEQ)) == NULL) {
		send_reply(400, 0, rtsp);	/* Bad Request */
		return ERR_NOERROR;
	} else {
		if (sscanf(p, "%254s %d", trash, &(rtsp->rtsp_cseq)) != 2) {
			send_reply(400, 0, rtsp);	/* Bad Request */
			return ERR_NOERROR;
		}
	}


	memset(&media, 0, sizeof(media));
	memset(&req, 0, sizeof(req));
	req.flags = ME_DESCR_FORMAT;
	req.descr_format = descr_format;
	res = get_media_descr(object, &req, &media, descr);
	if (res == ERR_NOT_FOUND) {
		ERRORLOGG("Not found media [%s]", descr);
		send_reply(404, 0, rtsp);	// Not found
		return ERR_NOERROR;
	}
	if (res == ERR_PARSE || res == ERR_GENERIC || res == ERR_ALLOC) {
		ERRORLOGG("Internal Server Error");
		send_reply(500, 0, rtsp);	// Internal server error
		return ERR_NOERROR;
	}

	if (max_connection() == ERR_GENERIC) {
		/*redirect */
		return send_redirect_3xx(rtsp, object);
	}


	INFOLOGG("DESCRIBE %s RTSP/1.0 ", url);
	send_describe_reply(rtsp, object, descr_format, descr);

	// See User-Agent 
	if ((p = strstr(rtsp->in_buffer, HDR_USER_AGENT)) != NULL) {
		char cut[strlen(p)];
		strcpy(cut, p);
		p = strstr(cut, "\n");
		cut[strlen(cut) - strlen(p) - 1] = '\0';
		INFOLOGG("%s", cut);
	} else
		INFOLOGG("- ");

	return ERR_NOERROR;
}
