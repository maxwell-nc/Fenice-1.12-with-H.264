
#include <unistd.h>

#include <fenice/utils.h>
#include <fenice/gsm.h>

int load_GSM(media_entry * p)
{

	unsigned char byte1;
	int ret;

	if (!(p->description.flags & MED_PKT_LEN)) {
		p->description.pkt_len = 20;	/* By default for GSM */
		p->description.delta_mtime = p->description.pkt_len;
		p->description.flags |= MED_PKT_LEN;
	}
	if ((ret = mediaopen(p)) < 0)
		return ret;
	if (read(p->fd, &byte1, 1) != 1)
		return ERR_PARSE;
	mediaclose(p);

	switch (byte1 & 0x07) {
	case 0:
		p->description.bitrate = 4750;
		break;
	case 1:
		p->description.bitrate = 5150;
		break;
	case 2:
		p->description.bitrate = 5900;
		break;
	case 3:
		p->description.bitrate = 6700;
		break;
	case 4:
		p->description.bitrate = 7400;
		break;
	case 5:
		p->description.bitrate = 7950;
		break;
	case 6:
		p->description.bitrate = 10200;
		break;
	case 7:
		p->description.bitrate = 12200;
		break;
	}
	p->description.flags |= MED_BITRATE;

	return ERR_NOERROR;
}
