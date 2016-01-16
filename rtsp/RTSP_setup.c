
#include <stdio.h>
#include <string.h>
#include <sys/time.h>		// shawill: for gettimeofday
#include <stdlib.h>		// shawill: for rand, srand
#include <unistd.h>		// shawill: for dup
// shawill: for inet_aton
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fenice/rtsp.h>
#include <fenice/multicast.h>
#include <fenice/log.h>
#include <fenice/en_xmalloc.h>

#define SOCKPAIRTYPE SOCK_DGRAM

int RTSP_setup(RTSP_buffer * rtsp, RTSP_session ** new_session)
{
	char address[16];
	char object[255], server[255];
	char url[255];
	unsigned short port;
	RTSP_session *rtsp_s;
	RTP_session *rtp_s, *rtp_s_prec;
	int SessionID = 0;
	struct timeval now_tmp;
	char *p /* = NULL */ ;
	unsigned int start_seq, start_rtptime;
	char transport_str[255];
	media_entry *list, *matching_me, req;
	struct sockaddr_storage rtsp_peer;
	socklen_t namelen = sizeof(rtsp_peer);
	unsigned long ssrc;
	SD_descr *matching_descr;
	unsigned char is_multicast_dad = 1;	//unicast and the first multicast
	RTP_transport transport;
	char *saved_ptr, *transport_tkn;
	int max_interlvd;
	int sdpair[2];
	RTSP_interleaved *intlvd;

	TRACE_FUNC();

	// init
	memset(&req, 0, sizeof(req));
	memset(&transport, 0, sizeof(transport));

	// Parse the input message

	/* Get the URL */
	if (!sscanf(rtsp->in_buffer, " %*s %254s ", url)) {
		send_reply(400, 0, rtsp);	/* bad request */
		return ERR_NOERROR;
	}
	/* Validate the URL */
	DEBUGLOGG("parse [%s] is valid or not");
	switch (parse_url(url, server, sizeof(server), &port, object, sizeof(object))) {	//object is requested file's name
		case URL_BAD: // 1:		// bad request
			send_reply(400, 0, rtsp);
			return ERR_NOERROR;
		case URL_ERROR: //-1:		// interanl server error
			send_reply(500, 0, rtsp);
			return ERR_NOERROR;
			break;
		default:
			break;
	}

	DEBUGLOGG("server = %s", server);
	DEBUGLOGG("prefs_get_hostname() = %s", prefs_get_hostname());
	if (strcmp(server, prefs_get_hostname()) != 0) {	/* Currently this feature is disabled. */
		/* wrong server name */
		//      send_reply(404, 0 , rtsp); /* Not Found */
		//      return ERR_NOERROR;
	}
	if (strstr(object, "../")) {
		/* disallow relative paths outside of current directory. */
		send_reply(403, 0, rtsp);	/* Forbidden */
		return ERR_NOERROR;
	}
	if (strstr(object, "./")) {
		/* Disallow the ./ */
		send_reply(403, 0, rtsp);	/* Forbidden */
		return ERR_NOERROR;
	}

	if (!(p = strrchr(object, '.'))) {	// if filename is without extension
		send_reply(415, 0, rtsp);	/* Unsupported media type */
		return ERR_NOERROR;
	} else if (!is_supported_url(p)) {	//if filename's extension is not valid
		send_reply(415, 0, rtsp);	/* Unsupported media type */
		return ERR_NOERROR;
	}
	if (!(p = strchr(object, '!'))) {	//if '!' is not present then a file has not been specified
		send_reply(500, 0, rtsp);	/* Internal server error */
		return ERR_NOERROR;
	} else {
		// SETUP name.sd!stream
		strcpy(req.filename, p + 1);
		req.flags |= ME_FILENAME;

		*p = '\0';
	}

// ------------ START PATCH
	{
		char temp[255];
		char *pd = NULL;

		strcpy(temp, object);
#if 0
		printf("%s\n", object);
		// BEGIN 
		// if ( (p = strstr(temp, "/")) ) {
		if ((p = strchr(temp, '/'))) {
			strcpy(object, p + 1);	// CRITIC. 
		}
		printf("%s\n", temp);
#endif
		// pd = strstr(p, ".sd");       // this part is usefull in order to
		pd = strstr(temp, ".sd");
		if ((p = strstr(pd + 1, ".sd"))) {	// have compatibility with RealOne
			strcpy(object, pd + 4);	// CRITIC. 
		}		//Note: It's a critic part
		// END 
	}
// ------------ END PATCH
	if (enum_media(object, &matching_descr) != ERR_NOERROR) {
		ERRORLOGG("%s", "Not support media type");
		send_reply(500, 0, rtsp);	/* Internal server error */
		return ERR_NOERROR;
	}
	list = matching_descr->me_list;

	if (get_media_entry(&req, list, &matching_me) == ERR_NOT_FOUND) {
		ERRORLOGG("%s", "Not found media data");
		send_reply(404, 0, rtsp);	/* Not found */
		return ERR_NOERROR;
	}
	// Get the CSeq 
	if ((p = strstr(rtsp->in_buffer, HDR_CSEQ)) == NULL) {
		ERRORLOGG("%s", "Bad request");
		send_reply(400, 0, rtsp);	/* Bad Request */
		return ERR_NOERROR;
	} else {
		if (sscanf(p, "%*s %d", &(rtsp->rtsp_cseq)) != 1) {
			ERRORLOGG("%s", "Bad request");
			send_reply(400, 0, rtsp);	/* Bad Request */
			return ERR_NOERROR;
		}
	}
	/*if ((p = strstr(rtsp->in_buffer, "ssrc")) != NULL) {
	   p = strchr(p, '=');
	   sscanf(p + 1, "%lu", &ssrc);
	   } else { */
	ssrc = random32(0);
	//}

	// Start parsing the Transport header
	if ((p = strstr(rtsp->in_buffer, HDR_TRANSPORT)) == NULL) {
		send_reply(406, "Require: Transport settings" /* of rtp/udp;port=nnnn. " */ , rtsp);	/* Not Acceptable */
		return ERR_NOERROR;
	}
	if (sscanf(p, "%*10s%255s", transport_str) != 1) {
		ERRORLOGG("SETUP request malformed: Transport string is empty");
		send_reply(400, 0, rtsp);	/* Bad Request */
		return ERR_NOERROR;
	}
	// printf("transport: %s\n", transport_str); // XXX tmp.

	// tokenize the coma seaparated list of transport settings:
	if (!(transport_tkn = strtok_r(transport_str, ",", &saved_ptr))) {
		ERRORLOGG("Malformed Transport string from client");
		send_reply(400, 0, rtsp);	/* Bad Request */
		return ERR_NOERROR;
	}

	if (getpeername(rtsp->fd, (struct sockaddr *)&rtsp_peer, &namelen) != 0) {
		send_reply(415, 0, rtsp);	// Internal server error
		return ERR_GENERIC;
	}

	transport.type = RTP_no_transport;
	do {			// search a good transport string
		if ((p = strstr(transport_tkn, RTSP_RTP_AVP))) {	// Transport: RTP/AVP
			p += strlen(RTSP_RTP_AVP);
			if (!*p || (*p == ';') || (*p == ' ')) {
				if (strstr(transport_tkn, "unicast")) {
					if ((p = strstr(transport_tkn, "client_port"))) {
						p = strstr(p, "=");
						sscanf(p + 1, "%d", &(transport.u.udp.cli_ports.RTP));
						p = strstr(p, "-");
						sscanf(p + 1, "%d", &(transport.u.udp.cli_ports.RTCP));
					}
					if (RTP_get_port_pair(&transport.u.udp.ser_ports) != ERR_NOERROR) {
						send_reply(500, 0, rtsp);	/* Internal server error */
						return ERR_GENERIC;
					}
					// strcpy(address, get_address());
					udp_open(transport.u.udp.ser_ports.RTP, &transport.u.udp.rtp_peer, &transport.rtp_fd);	//bind
					udp_open(transport.u.udp.ser_ports.RTCP, &transport.u.udp.rtcp_in_peer, &transport.rtcp_fd_in);	//bind
					//UDP connection for outgoing RTP packets
					udp_connect(transport.u.udp.cli_ports.RTP,
						    &transport.u.udp.rtp_peer,
						    (*((struct sockaddr_in *) (&rtsp_peer))).sin_addr.s_addr,
						    &transport.rtp_fd);
					//UDP connection for outgoing RTCP packets
					transport.rtcp_fd_out = transport.rtcp_fd_in; 
					udp_connect(transport.u.udp.cli_ports.RTCP,
						    &transport.u.udp.rtcp_out_peer,
						    (*((struct sockaddr_in *) (&rtsp_peer))).sin_addr.s_addr,
						    &transport.rtcp_fd_out);

					transport.u.udp.is_multicast = 0;
				} else if (matching_descr->flags & SD_FL_MULTICAST) {	/*multicast */
					// TODO: make the difference between only multicast allowed or unicast fallback allowed.
					transport.u.udp.cli_ports.RTP =
					    transport.u.udp.ser_ports.RTP =
					    matching_me->rtp_multicast_port;
					transport.u.udp.cli_ports.RTCP =
					    transport.u.udp.ser_ports.RTCP =
					    matching_me->rtp_multicast_port + 1;

					is_multicast_dad = 0;
					if (!(matching_descr->flags & SD_FL_MULTICAST_PORT)) {
						struct in_addr inp;
						unsigned char ttl = DEFAULT_TTL;
						struct ip_mreq mreq;

						mreq.imr_multiaddr.s_addr =
						    inet_addr(matching_descr->multicast);
						mreq.imr_interface.s_addr = INADDR_ANY;
						setsockopt(transport.rtp_fd,
							   IPPROTO_IP,
							   IP_ADD_MEMBERSHIP,
							   &mreq, sizeof(mreq));
						setsockopt(transport.rtp_fd,
							   IPPROTO_IP,
							   IP_MULTICAST_TTL,
							   &ttl, sizeof(ttl));

						is_multicast_dad = 1;
						strcpy(address, matching_descr->multicast);
						//RTP outgoing packets
						inet_aton(address, &inp);
						udp_connect(transport.u.udp.
							    ser_ports.RTP,
							    &transport.u.udp.
							    rtp_peer,
							    inp.s_addr,
							    &transport.rtp_fd);
						//RTCP outgoing packets
						inet_aton(address, &inp);
						udp_connect(transport.u.udp.ser_ports.RTCP,
							    &transport.u.udp.rtcp_out_peer,
							    inp.s_addr,
							    &transport.rtcp_fd_out);
						//udp_open(transport.u.udp.ser_ports.RTCP, &(sp2->rtcp_in_peer), &(sp2->rtcp_fd_in));   //bind 

						if (matching_me->next == NULL)
							matching_descr->flags |= SD_FL_MULTICAST_PORT;

						matching_me->rtp_multicast_port =
						    transport.u.udp.ser_ports.RTP;
						transport.u.udp.is_multicast = 1;
						INFOLOGG("Set up socket for multicast ok");
					}
				} else
					continue;

				transport.type = RTP_rtp_avp;
				break;	// found a valid transport
			} else if (!strncmp(p, "/TCP", 4)) {	// Transport: RTP/AVP/TCP;interleaved=x-y // XXX still not finished
				intlvd = xnew0(RTSP_interleaved);

				if ((p = strstr(transport_tkn, "interleaved"))) {
					p = strstr(p, "=");
					sscanf(p + 1, "%hu", &(transport.u.tcp.interleaved.RTP));
					if ((p = strstr(p, "-")))
						sscanf(p + 1, "%hu", &(transport.u.tcp.interleaved.RTCP));
					else
						transport.u.tcp.interleaved.RTCP =
						    transport.u.tcp.interleaved.RTP + 1;
						    
					if ( (transport.u.tcp.interleaved.RTP > 255) || (transport.u.tcp.interleaved.RTCP > 255) ) {
						ERRORLOGG("Interleaved channel number suggested from client too high\n");
						send_reply(400, "Interleaved channel number suggested from client too high", rtsp);
						return ERR_GENERIC;
					}
				} else {	// search for max used interleved channel.
					max_interlvd = -1;
					for (rtp_s =
					     (rtsp->session_list) ? rtsp->session_list->rtp_session : NULL;
					     rtp_s; rtp_s = rtp_s->next)
						max_interlvd =
						    max(max_interlvd,
							(rtp_s->transport.type == RTP_rtp_avp_tcp) ?
							rtp_s->transport.u.tcp.interleaved.RTCP : -1);
					transport.u.tcp.interleaved.RTP = max_interlvd + 1;
					transport.u.tcp.interleaved.RTCP = max_interlvd + 2;
					    
					if ( (transport.u.tcp.interleaved.RTP > 255) || (transport.u.tcp.interleaved.RTCP > 255) ) {
						ERRORLOGG("Interleaved channel number already reached max\n");
						send_reply(500, "Interleaved channel number already reached max", rtsp);
						return ERR_GENERIC;
					}
				}

				intlvd = xnew0(RTSP_interleaved);
				intlvd->proto.tcp.rtp_ch = transport.u.tcp.interleaved.RTP;
				intlvd->proto.tcp.rtcp_ch = transport.u.tcp.interleaved.RTCP;
				
				// RTP local sockpair
				if ( socketpair(AF_LOCAL, SOCKPAIRTYPE, 0, sdpair) < 0) {
					ERRORLOGG("Cannot create AF_LOCAL socketpair for rtp");
					send_reply(500, 0, rtsp);
					free(intlvd);
					return ERR_GENERIC;
				}
				transport.rtp_fd = sdpair[0];
				intlvd->rtp_fd = sdpair[1];
				// RTCP local sockpair
				if ( socketpair(AF_LOCAL, SOCKPAIRTYPE, 0, sdpair) < 0) {
					ERRORLOGG("Cannot create AF_LOCAL socketpair for rtcp");
					send_reply(500, 0, rtsp);
					close(transport.rtp_fd);
					close(intlvd->rtp_fd);
					free(intlvd);
					return ERR_GENERIC;
				}
				
				transport.rtcp_fd_out = sdpair[0];
				intlvd->rtcp_fd = sdpair[1];
				
				transport.rtcp_fd_in = -1;
				
				// insert new interleaved stream in the list
				intlvd->next = rtsp->interleaved;
				rtsp->interleaved = intlvd;

				transport.type = RTP_rtp_avp_tcp;
				break;	// found a valid transport
			}
		}
	} while ((transport_tkn = strtok_r(NULL, ",", &saved_ptr)));
	// printf("rtp transport: %d\n", transport.type);

	if (transport.type == RTP_no_transport) {
		// fnc_log(FNC_LOG_ERR,"Unsupported Transport\n");
		send_reply(461, "Unsupported Transport", rtsp);	/* Bad Request */
		return ERR_NOERROR;
	}
	// If there's a Session header we have an aggregate control
	if ((p = strstr(rtsp->in_buffer, HDR_SESSION)) != NULL) {
		if (sscanf(p, "%*s %d", &SessionID) != 1) {
			send_reply(454, 0, rtsp);	/* Session Not Found */
			return ERR_NOERROR;
		}
	} else {
		// Generate a random Session number
		gettimeofday(&now_tmp, 0);
		srand((now_tmp.tv_sec * 1000) + (now_tmp.tv_usec / 1000));
		SessionID = 1 + (int) (10.0 * rand() / (100000 + 1.0));
		if (SessionID == 0) {
			SessionID++;
		}
	}

	// Add an RTSP session if necessary
	if (!rtsp->session_list) {
		rtsp->session_list = xnew0(RTSP_session);
	}
	rtsp_s = rtsp->session_list;

	// Setup the RTP session
	if (rtsp->session_list->rtp_session == NULL) {
		rtsp->session_list->rtp_session = xnew0(RTP_session);
		rtp_s = rtsp->session_list->rtp_session;
	} else {
		for (rtp_s = rtsp_s->rtp_session; rtp_s != NULL;
		     rtp_s = rtp_s->next) {
			rtp_s_prec = rtp_s;
		}
		rtp_s_prec->next = xnew0(RTP_session);
		rtp_s = rtp_s_prec->next;
	}


	start_seq = 1 + (unsigned int) (rand() % (0xFFFF));
	start_rtptime = 1 + (unsigned int) (rand() % (0xFFFFFFFF));
	if (start_seq == 0) {
		start_seq++;
	}
	if (start_rtptime == 0) {
		start_rtptime++;
	}
	rtp_s->pause = 1;
	strcpy(rtp_s->sd_filename, object);
	/*xxx */
	rtp_s->current_media = xnew0(media_entry);

	// if(!(matching_descr->flags & SD_FL_MULTICAST_PORT)){
	if (is_multicast_dad) {
		if (mediacpy(&rtp_s->current_media, &matching_me)) {
			send_reply(500, 0, rtsp);	/* Internal server error */
			return ERR_GENERIC;
		}
	}

	gettimeofday(&now_tmp, 0);
	srand((now_tmp.tv_sec * 1000) + (now_tmp.tv_usec / 1000));
	rtp_s->start_rtptime = start_rtptime;
	rtp_s->start_seq = start_seq;
	memcpy(&rtp_s->transport, &transport, sizeof(transport));
	rtp_s->is_multicast_dad = is_multicast_dad;

	/*xxx */
	rtp_s->sd_descr = matching_descr;

	rtp_s->sched_id = schedule_add(rtp_s);


	rtp_s->ssrc = ssrc;
	// Setup the RTSP session       
	rtsp_s->session_id = SessionID;
	*new_session = rtsp_s;

	INFOLOGG("SETUP %s RTSP/1.0 ", url);
	send_setup_reply(rtsp, rtsp_s, matching_descr, rtp_s);
  
	return ERR_NOERROR;
}
