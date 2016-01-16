
#include <string.h>

#include <fenice/intnet.h>

int downgrade_GSM(RTP_session * changing_session)
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
	speed = changing_session->current_media->description.bitrate;
	p = NULL;
	switch (speed) {
	case 12200:
		speed = 10200;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 10200:
		speed = 7950;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 7950:
		speed = 7400;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 7400:
		speed = 6700;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 6700:
		speed = 5900;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 5900:
		speed = 5150;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 5150:
		speed = 4750;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		break;
	}
	if (p != NULL)
		return stream_switch(changing_session, p);
	else
		return priority_decrease(changing_session);
}
