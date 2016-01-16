
#include <config.h>

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <string.h>
#include <stdlib.h>

#include <fenice/rtp.h>
#include <fenice/utils.h>
#include <fenice/log.h>
#include <fenice/en_xmalloc.h>

#if ENABLE_DUMP
#include <fenice/debug.h>
#endif

int RTP_send_packet(RTP_session * session)
{
	unsigned char *packet = NULL;
	unsigned int hdr_size = 0;
	RTP_header r;		// 12 bytes
	int res = ERR_GENERIC;
	double s_time;
	double nextts;
	OMSSlot *slot = NULL;
	ssize_t psize_sent = 0;

//	TRACE_FUNC();

	if (!(slot = OMSbuff_getreader(session->cons))) {
		//This operation runs only if producer writes the slot
		s_time =
		    session->current_media->mtime -
		    session->current_media->mstart +
		    session->current_media->mstart_offset;
		if ((res = get_frame(session->current_media, &s_time)) != ERR_NOERROR) {
			ERRORLOGG("Some error occurred");
			return res;
		}
		session->cons->frames++;
		slot = OMSbuff_getreader(session->cons);
	} else { /*This runs if the consumer reads slot written in another RTP session */
		s_time = slot->timestamp - session->cons->firstts;
	}

	while (slot) {

		hdr_size = sizeof(r);
		r.version = 2;
		r.padding = 0;
		r.extension = 0;
		r.csrc_len = 0;
		r.marker = slot->marker;
		r.payload = session->current_media->description.payload_type;
//              r.seq_no = htons(session->seq++ + session->start_seq);
		r.seq_no = htons(slot->slot_seq + session->start_seq - 1);
		r.timestamp =
		    htonl(session->start_rtptime +
			  msec2tick(slot->timestamp, session->current_media) -
			  session->cons->firstts);
		r.ssrc = htonl(session->ssrc);
		packet = xcalloc(1, slot->data_size + hdr_size);
		memcpy(packet, &r, hdr_size);
		memcpy(packet + hdr_size, slot->data, slot->data_size);

		psize_sent = RTP_sendto(session, rtp_proto,
						packet,
						slot->data_size + hdr_size);
		if (psize_sent < 0) {
			DEBUGLOGG("RTP Packet Lost");
		} else {
#if ENABLE_DUMP
			char fname[255];
			char crtp[255];
			memset(fname, 0, sizeof(fname));
			strcpy(fname, "dump_fenice.");
			strcat(fname,
			       session->current_media->description.
			       encoding_name);
			strcat(fname, ".");
			sprintf(crtp, "%d", session->transport.rtp_fd);
			strcat(fname, crtp);
			if (strcmp
			    (session->current_media->description.encoding_name,
			     "MPV") == 0
			    || strcmp(session->current_media->description.
				      encoding_name, "MPA") == 0)
				dump_payload(packet + 16, psize_sent - 16,
					     fname);
			else
				dump_payload(packet + 12, psize_sent - 12,
					     fname);
#endif
			session->rtcp_stats[i_server].pkt_count++;
			session->rtcp_stats[i_server].octet_count +=
			    slot->data_size;
		}

		xfree(packet);

		OMSbuff_gotreader(session->cons);

		if ((nextts = OMSbuff_nextts(session->cons)) >= 0)
			nextts -= session->cons->firstts;
		if ((nextts == -1) || (nextts != s_time)) {
			if (session->current_media->description.delta_mtime)
				session->current_media->mtime += session->current_media->description.delta_mtime;	//emma
			else
				session->current_media->mtime +=
				    session->current_media->description.pkt_len;
			slot = NULL;
			session->cons->frames--;
		} else
			slot = OMSbuff_getreader(session->cons);

	}

	return ERR_NOERROR;
}
