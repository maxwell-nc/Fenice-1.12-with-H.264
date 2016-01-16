
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>

/*
 	****************************************************************
 	*			TEARDOWN METHOD HANDLING
 	****************************************************************
*/

int RTSP_teardown(RTSP_buffer * rtsp)
{
	long int session_id;
	char *p;
	RTSP_session *s;
	RTP_session *rtp_curr, *rtp_prev = NULL, *rtp_temp;
	int valid_url;
	char object[255], server[255], trash[255], *filename;
	unsigned short port;
	char url[255];
	unsigned int cseq;

	// CSeq
	if ((p = strstr(rtsp->in_buffer, HDR_CSEQ)) == NULL) {
		send_reply(400, 0, rtsp);	/* Bad Request */
		return ERR_PARSE;
	} else {
		if (sscanf(p, "%254s %d", trash, &(rtsp->rtsp_cseq)) != 2) {
			send_reply(400, 0, rtsp);	/* Bad Request */
			return ERR_PARSE;
		}
	}
	cseq = rtsp->rtsp_cseq;
	/* Extract the URL */
	if (!sscanf(rtsp->in_buffer, " %*s %254s ", url)) {
		send_reply(400, 0, rtsp);	/* bad request */
		return ERR_PARSE;
	}
	/* Validate the URL */
	switch (parse_url
		(url, server, sizeof(server), &port, object, sizeof(object))) {
	case 1:		// bad request
		send_reply(400, 0, rtsp);
		return ERR_PARSE;
		break;
	case -1:		// internal server error
		send_reply(500, 0, rtsp);
		return ERR_PARSE;
		break;
	default:
		break;
	}
	if (strcmp(server, prefs_get_hostname()) != 0) {	/* Currently this feature is disabled. */
		/* wrong server name */
		//      send_reply(404, 0 , rtsp); /* Not Found */
		//      return ERR_PARSE;
	}
	if (strstr(object, "../")) {
		/* disallow relative paths outside of current directory. */
		send_reply(403, 0, rtsp);	/* Forbidden */
		return ERR_PARSE;
	}
	if (strstr(object, "./")) {
		/* Disallow ./ */
		send_reply(403, 0, rtsp);	/* Forbidden */
		return ERR_PARSE;
	}
	p = strrchr(object, '.');
	valid_url = 0;
	if (p == NULL) {
		send_reply(415, 0, rtsp);	/* Unsupported media type */
		return ERR_PARSE;
	} else {
		valid_url = is_supported_url(p);
	}
	if (!valid_url) {
		send_reply(415, 0, rtsp);	/* Unsupported media type */
		return ERR_PARSE;
	}
	// Session
	if ((p = strstr(rtsp->in_buffer, HDR_SESSION)) != NULL) {
		if (sscanf(p, "%254s %ld", trash, &session_id) != 2) {
			send_reply(454, 0, rtsp);	/* Session Not Found */
			return ERR_PARSE;
			// return ERR_NOERROR;
		}
	} else {
		session_id = -1;
	}
	s = rtsp->session_list;
	if (s == NULL) {
		send_reply(415, 0, rtsp);	// Internal server error
		return ERR_GENERIC;
	}
	if (s->session_id != session_id) {
		send_reply(454, 0, rtsp);	/* Session Not Found */
		return ERR_PARSE;
	}

	INFOLOGG("TEARDOWN %s RTSP/1.0 ", url);
	send_teardown_reply(rtsp, session_id, cseq);
	// See User-Agent 
	if ((p = strstr(rtsp->in_buffer, HDR_USER_AGENT)) != NULL) {
		char cut[strlen(p)];
		strcpy(cut, p);
		p = strstr(cut, "\n");
		cut[strlen(cut) - strlen(p) - 1] = '\0';
		INFOLOGG("%s", cut);
	} else
		INFOLOGG("- ");


	if (strchr(object, '!'))	/*Compatibility with RealOne and RealPlayer */
		filename = strchr(object, '!') + 1;
	else
		filename = object;



	// Release all URI RTP session
	rtp_curr = s->rtp_session;
	while (rtp_curr != NULL) {
		if (strcmp(rtp_curr->current_media->filename, filename) == 0
		    || strcmp(rtp_curr->current_media->aggregate,
			      filename) == 0) {
			rtp_temp = rtp_curr;
			if (rtp_prev != NULL)
				rtp_prev->next = rtp_curr->next;
			else
				s->rtp_session = rtp_curr->next;
			rtp_curr = rtp_curr->next;
			// Release the scheduler entry
			schedule_remove(rtp_temp->sched_id);
			// Close connections
		} else {
			rtp_prev = rtp_curr;
			rtp_curr = rtp_curr->next;
		}
	}

	if (s->rtp_session == NULL) {
		// Close connection
		//close(s->fd);
		// Release the RTSP session
		free(rtsp->session_list);
		rtsp->session_list = NULL;
	}

	return ERR_NOERROR;
}
