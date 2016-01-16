
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fenice/rtcp.h>
#include <fenice/utils.h>
#include <fenice/log.h>
#include <fenice/en_xmalloc.h>

int RTCP_send_packet(RTP_session * session, rtcp_pkt_type type)
{
	unsigned char *pkt = NULL;
	RTCP_header hdr;
	uint32 pkt_size = 0, hdr_s = 0;

	hdr.version = 2;
	hdr.padding = 0;
	hdr.pt = type;
	hdr_s = sizeof(hdr);
	switch (type) {
		case SR:{
			struct timeval ntp_time;
			RTCP_header_SR hdr_sr;
			int hdr_sr_s;
			//printf("SR\n");
			hdr_sr_s = sizeof(hdr_sr);
			pkt_size = hdr_s + hdr_sr_s;
			hdr.length = htons((pkt_size >> 2) - 1);
			hdr.count = 0;
			hdr_sr.ssrc = htonl(session->ssrc);

			gettimeofday(&ntp_time, NULL);
			hdr_sr.ntp_timestampH =
					htonl((unsigned int) ntp_time.tv_sec);
			hdr_sr.ntp_timestampL =
					htonl((unsigned int) ntp_time.tv_usec);
			hdr_sr.rtp_timestamp =
					htonl((unsigned int) ((double) ntp_time.tv_sec +
										  (double) ntp_time.tv_usec /
										  1000000.) *
						  session->current_media->description.clock_rate +
								  session->start_rtptime);

			hdr_sr.pkt_count =
					session->rtcp_stats[i_server].pkt_count;
			hdr_sr.octet_count =
					session->rtcp_stats[i_server].octet_count;
			pkt = xcalloc(1, pkt_size);
			memcpy(pkt, &hdr, hdr_s);
			memcpy(pkt + hdr_s, &hdr_sr, hdr_sr_s);
//			DEBUGHEXLOGG((char *)pkt, pkt_size, "RTCP SR OUT");
			//fprintf(stderr,"pkt_size=%d,hdr_s=%d,hdr_sr_s=%d\n",pkt_size,hdr_s,hdr_sr_s);
			break;
		}
		case RR:{
			RTCP_header_RR hdr_rr;
			int hdr_rr_s;
			//printf("RR\n");
			hdr_rr_s = sizeof(hdr_rr);
			pkt_size = hdr_s + hdr_rr_s;
			hdr.length = htons((pkt_size >> 2) - 1);
			hdr.count = 0;
			hdr_rr.ssrc = htonl(session->ssrc);
			pkt = xcalloc(1, pkt_size);
			memcpy(pkt, &hdr, hdr_s);
			memcpy(pkt + hdr_s, &hdr_rr, hdr_rr_s);
//			DEBUGHEXLOGG((char *)pkt, pkt_size, "RTCP RR OUT");
			break;
		}
		case SDES:{
			RTCP_header_SDES hdr_sdes;
			char *name;
			int hdr_sdes_s, name_s;

			//printf("SDES\n");
			name = prefs_get_hostname();
			name_s = strlen(name);
			hdr_sdes_s = sizeof(hdr_sdes);

			pkt_size =
			    (((hdr_s + hdr_sdes_s + name_s) % 4) ? 1 : 0) +
			    (hdr_s + hdr_sdes_s + name_s);
			hdr.length = htons((pkt_size >> 2) - 1);
			pkt = xcalloc(1, pkt_size);
			hdr.count = 1;
			hdr_sdes.ssrc = htonl(session->ssrc);
			hdr_sdes.attr_name = (unsigned char)htonl(1);	// 1=CNAME
			hdr_sdes.len = htonl(name_s);
			memcpy(pkt, &hdr, hdr_s);
			memcpy(pkt + hdr_s, &hdr_sdes, hdr_sdes_s);
			memcpy(pkt + hdr_s + hdr_sdes_s, name, name_s);
//			DEBUGHEXLOGG((char *)pkt, pkt_size, "RTCP SDES OUT");
			//fprintf(stderr,"pkt_size=%d,hdr_s=%d,hdr_sdes_s=%d,name_s=%d\n",pkt_size,hdr_s,hdr_sdes_s,name_s);

			break;
		}
		case BYE:{
			RTCP_header_BYE hdr_bye;
			int hdr_bye_s;
			char *reason = "The medium is over.";
			//printf("BYE\n");
			hdr_bye_s = sizeof(hdr_bye);
			pkt_size = hdr_s + hdr_bye_s;
			hdr.length = htons((pkt_size >> 2) - 1);
			hdr.count = 1;
			hdr_bye.ssrc = htonl(session->ssrc);
			hdr_bye.length = htonl(strlen(reason));
			hdr_bye.ssrc = htonl(session->ssrc);
			pkt = xcalloc(1, pkt_size);
			memcpy(pkt, &hdr, hdr_s);
			memcpy(pkt + hdr_s, &hdr_bye, hdr_bye_s);
//			DEBUGHEXLOGG((char *)pkt, pkt_size, "RTCP BYE OUT");
			break;
		}
		default:{
			//printf("DEFAULT\n");
			return ERR_NOERROR;
		}
	}
	if (session->rtcp_outsize + pkt_size <= sizeof(session->rtcp_outbuffer)) {
		memcpy(session->rtcp_outbuffer + session->rtcp_outsize, pkt,
		       pkt_size);
		session->rtcp_outsize += pkt_size;
	} else {
		INFOLOGG("Output RTCP packet lost.");
	}
	free(pkt);
	return ERR_NOERROR;
}
