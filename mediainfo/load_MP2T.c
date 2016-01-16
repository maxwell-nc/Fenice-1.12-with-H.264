
#include <fenice/utils.h>
#include <fenice/mpeg_ts.h>

int load_MP2T(media_entry * p)
{
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
