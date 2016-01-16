
#include <stdlib.h>
#include <fenice/utils.h>


int get_media_entry(
		media_entry * req,
		media_entry * list,
		media_entry ** result)
// return a media with the requested characteristics
{
	if (media_is_empty(req)) {
		*result = default_selection_criterion(list);
		if (*result == NULL) {
			return ERR_NOT_FOUND;
		}
	} else {
		*result = search_media(req, list);
		if (*result == NULL) {
			return ERR_NOT_FOUND;
		}
	}
	return ERR_NOERROR;
}
