
#include <string.h>

#include <fenice/intnet.h>

int upgrade_MP3(RTP_session * changing_session)
{
	int speed;
	media_entry req, *list, *p;
	SD_descr *matching_descr;

	memset(&req, 0, sizeof(req));

	req.description.flags |= MED_BITRATE;
	req.description.flags |= MED_ENCODING_NAME;
	strcpy(req.description.encoding_name, "MPA");

	enum_media(changing_session->sd_filename, &matching_descr);
	list = matching_descr->me_list;

	speed = changing_session->current_media->description.bitrate;
	p = NULL;
	switch (speed) {
	case 8000:
		speed = 16000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 16000:
		speed = 24000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 24000:
		speed = 32000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 32000:
		speed = 40000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 40000:
		speed = 48000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 48000:
		speed = 56000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 56000:
		speed = 64000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 64000:
		speed = 80000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 80000:
		speed = 96000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 96000:
		speed = 112000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 112000:
		speed = 128000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 128000:
		speed = 144000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 144000:
		speed = 160000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 160000:
		speed = 176000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 176000:
		speed = 192000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 192000:
		speed = 224000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 224000:
		speed = 256000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 256000:
		speed = 288000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 288000:
		speed = 320000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 320000:
		speed = 352000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 352000:
		speed = 384000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 384000:
		speed = 416000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		if (p != NULL)
			break;
	case 416000:
		speed = 448000;
		req.description.bitrate = speed;
		p = search_media(&req, list);
		break;
	}
	if (p != NULL)
		return stream_switch(changing_session, p);
	else
		return priority_increase(changing_session);
}
