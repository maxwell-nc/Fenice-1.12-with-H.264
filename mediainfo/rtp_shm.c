
#include <fenice/mediainfo.h>
#include <fenice/utils.h>
#include <fenice/log.h>

int load_RTP_SHM(media_entry * p)
{
	DEBUGLOGG("loading RTP_SHM...\n");

	if (!(p->description.flags & MED_PKT_LEN)) {
		if (!(p->description.flags & MED_FRAME_RATE)) {
			return ERR_PARSE;
		}
		p->description.pkt_len =
		    1 / (double) p->description.frame_rate * 1000;
		p->description.flags |= MED_PKT_LEN;
	}
	p->description.delta_mtime = p->description.pkt_len;
	return ERR_NOERROR;
}


int free_RTP_SHM(void *stat)
{
	return 0;
}
