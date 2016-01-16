
#include <fenice/utils.h>
#include <fenice/pcm.h>

int load_L16(media_entry * p)
{

	if (!(p->description.flags & MED_AUDIO_CHANNELS)) {
		return ERR_PARSE;
	}
	if (!(p->description.flags & MED_PKT_LEN)) {
		p->description.pkt_len = 20;	/* By default for L16 */
		p->description.delta_mtime = p->description.pkt_len;
		p->description.flags |= MED_PKT_LEN;
	}
	p->description.bit_per_sample = 16;
	p->description.flags |= MED_BIT_PER_SAMPLE;

	p->description.bitrate =
	    p->description.clock_rate * p->description.bit_per_sample *
	    p->description.audio_channels;
	p->description.flags |= MED_BITRATE;

	return ERR_NOERROR;
}
