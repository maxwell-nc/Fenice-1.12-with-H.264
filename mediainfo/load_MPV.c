
#include <stdio.h>

#include <fenice/utils.h>
#include <fenice/mpeg.h>
#include <fenice/log.h>

int load_MPV(media_entry * p)
{
	int ret;

	DEBUGLOGG("Call %s", __func__);

	// try if the file exists and is readable.
	// no other actions will be performed on file for the moment.
	if ((ret = mediaopen(p)) < 0)
		return ret;
	mediaclose(p);

	if (!(p->description.flags & MED_PKT_LEN)) {
		if (!(p->description.flags & MED_FRAME_RATE)) {
			return ERR_PARSE;
		}
		p->description.pkt_len =
		    1 / (double) p->description.frame_rate * 1000;
		p->description.flags |= MED_PKT_LEN;
	}
	p->description.delta_mtime = p->description.pkt_len;

	if ((p->description.byte_per_pckt != 0)
	    && (p->description.byte_per_pckt < 261)) {
		printf
		    ("Warning: the max size for MPEG Video packet is smaller than 261 bytes and if a video header\n");
		printf("is greater the max size would be ignored \n");
		printf("Using Default \n");
	}

	return ERR_NOERROR;

}
