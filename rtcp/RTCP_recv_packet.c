
#include <stdio.h>
#include <netinet/in.h>

#include <fenice/rtcp.h>
#include <fenice/utils.h>
#include <fenice/log.h>

int RTCP_recv_packet(RTP_session * session)
{
	short len = 0;

	//DEBUGHEXLOGG((char *)session->rtcp_inbuffer, sizeof(session->rtcp_inbuffer), "RTCP IN");
	for (len = 0; len < session->rtcp_insize;
	     len += (ntohs(*((short *) &(session->rtcp_inbuffer[len + 2]))) + 1) * 4) {
		switch (session->rtcp_inbuffer[1 + len]) {
			case SR:{
				int ssrc_count, i;
				unsigned char tmp[4];
				INFOLOGG("RTCP SR packet received");
				session->rtcp_stats[i_client].SR_received += 1;
				session->rtcp_stats[i_client].pkt_count =
				    *((int *)&(session->rtcp_inbuffer[20 + len]));
				session->rtcp_stats[i_client].octet_count =
				    *((int *)&(session->rtcp_inbuffer[24 + len]));
				ssrc_count = session->rtcp_inbuffer[0 + len] & 0x1f;
				for (i = 0; i < ssrc_count; ++i) {
					session->rtcp_stats[i_client].fract_lost =
					    session->rtcp_inbuffer[32 + len];
					tmp[0] = 0;
					tmp[1] = session->rtcp_inbuffer[33 + len];
					tmp[2] = session->rtcp_inbuffer[34 + len];
					tmp[3] = session->rtcp_inbuffer[35 + len];
					session->rtcp_stats[i_client].pkt_lost = ntohl(*((int *) tmp));
					session->rtcp_stats[i_client].highest_seq_no =
					    ntohl(session->rtcp_inbuffer[36 + len]);
					session->rtcp_stats[i_client].jitter =
					    ntohl(session->rtcp_inbuffer[40 + len]);
					session->rtcp_stats[i_client].last_SR =
					    ntohl(session->rtcp_inbuffer[44 + len]);
					session->rtcp_stats[i_client].delay_since_last_SR =
					    ntohl(session->rtcp_inbuffer[48 + len]);
				}
				break;
			}
			case RR:{
				int ssrc_count, i;
				unsigned char tmp[4];
				//INFOLOGG("RTCP RR packet received");
				session->rtcp_stats[i_client].RR_received += 1;
				ssrc_count = session->rtcp_inbuffer[0 + len] & 0x1f;
				for (i = 0; i < ssrc_count; ++i) {
					session->rtcp_stats[i_client].fract_lost =
					    session->rtcp_inbuffer[12 + len];
					tmp[0] = 0;
					tmp[1] = session->rtcp_inbuffer[13 + len];
					tmp[2] = session->rtcp_inbuffer[14 + len];
					tmp[3] = session->rtcp_inbuffer[15 + len];
					session->rtcp_stats[i_client].pkt_lost = ntohl(*((int *) tmp));
					session->rtcp_stats[i_client].highest_seq_no =
					    ntohl(session->rtcp_inbuffer[16 + len]);
					session->rtcp_stats[i_client].jitter =
					    ntohl(session->rtcp_inbuffer[20 + len]);
					session->rtcp_stats[i_client].last_SR =
					    ntohl(session->rtcp_inbuffer[24 + len]);
					session->rtcp_stats[i_client].delay_since_last_SR =
					    ntohl(session->rtcp_inbuffer[28 + len]);
				}
				break;
			}
		case SDES:{
				//INFOLOGG("RTCP SDES packet received");
				switch (session->rtcp_inbuffer[8]) {
				case CNAME:{
						session->rtcp_stats[1].dest_SSRC =
						    ntohs(*((int *) &(session->rtcp_inbuffer[4])));
						break;
					}
				case NAME:{
						break;
					}
				case EMAIL:{
						break;
					}
				case PHONE:{
						break;
					}
				case LOC:{
						break;
					}
				case TOOL:{
						break;
					}
				case NOTE:{
						break;
					}
				case PRIV:{
						break;
					}
				}
				break;
			}
		case BYE:{
				INFOLOGG("RTCP BYE packet received");
				break;
			}
		case APP:{
				INFOLOGG("RTCP APP packet received");
				break;
			}
		default:{
				INFOLOGG("Unknown RTCP received and ignored.");
				return ERR_NOERROR;
			}
		}
	}
	return ERR_NOERROR;
}
