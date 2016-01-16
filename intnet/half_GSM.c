
#include <string.h>

#include <fenice/intnet.h>

int half_GSM(RTP_session * changing_session)
{
	int speed;
	media_entry req, *list, *p;
	SD_descr *matching_descr;

	memset(&req, 0, sizeof(req));

	req.description.flags |= MED_BITRATE;
	req.description.flags |= MED_ENCODING_NAME;
	strcpy(req.description.encoding_name, "GSM");

	enum_media(changing_session->sd_filename, &matching_descr);
	list = matching_descr->me_list;

	speed = changing_session->current_media->description.bitrate / 2;
	p = NULL;
	if (changing_session->current_media->description.bitrate != 4750) {
		if (speed == 6100)
			speed = 6700;
		else if (speed == 5100)
			speed = 5150;
		else
			speed = 4750;
		switch (speed) {
		case 6700:
			req.description.bitrate = speed;
			p = search_media(&req, list);
			if (p != NULL)
				break;
			speed = 5900;
		case 5900:
			req.description.bitrate = speed;
			p = search_media(&req, list);
			if (p != NULL)
				break;
			speed = 5150;
		case 5150:
			req.description.bitrate = speed;
			p = search_media(&req, list);
			if (p != NULL)
				break;
			speed = 4750;
		case 4750:
			req.description.bitrate = speed;
			p = search_media(&req, list);
			break;
		}
	}
	if (p != NULL)
		return stream_switch(changing_session, p);
	else
		return priority_decrease(changing_session);
}
