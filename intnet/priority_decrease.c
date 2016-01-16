
#include <string.h>

#include <fenice/intnet.h>
#include <fenice/utils.h>

int priority_decrease(RTP_session * changing_session)
{
	int priority;
	media_entry req, *list, *p;
	SD_descr *matching_descr;

	memset(&req, 0, sizeof(req));

	req.description.flags |= MED_PRIORITY;
	enum_media(changing_session->sd_filename, &matching_descr);
	list = matching_descr->me_list;
	priority = changing_session->current_media->description.priority;
	priority += 1;
	req.description.priority = priority;
	p = search_media(&req, list);
	if (p != NULL)
		return stream_switch(changing_session, p);
	else
		changing_session->MinimumReached = 1;

	return ERR_NOERROR;
}
