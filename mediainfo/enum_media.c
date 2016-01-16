
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fenice/utils.h>
#include <fenice/en_xmalloc.h>

int enum_media(char *object, SD_descr ** d)
{
	static SD_descr *SD_global_list = NULL;
	SD_descr *matching_descr = NULL, *descr_curr, *last_descr = NULL;
	int res;

	//test about the loading of current SD (is it done?)
	for (descr_curr = SD_global_list; descr_curr && !matching_descr;
	     descr_curr = descr_curr->next) {
		if (strcmp(descr_curr->filename, object) == 0)
			matching_descr = descr_curr;
		else
			last_descr = descr_curr;
	}

	if (!matching_descr) {
		//.SD not found: update list
		//the first time SD_global_list must be initialized
		if (!SD_global_list) {
			SD_global_list = xnew0(SD_descr);
			matching_descr = SD_global_list;
		} else {
			last_descr->next = xnew0(SD_descr);
			matching_descr = last_descr->next;
		}
		strcpy(matching_descr->filename, object);
	}
	res = parse_SD_file(object, matching_descr);
	(*d) = matching_descr;
	if (res != ERR_NOERROR) {
		if (!last_descr)	//matching is the first
			SD_global_list = SD_global_list->next;
		else {
			last_descr->next = matching_descr->next;
		}
		xfree(matching_descr);
		return res;
	}
	return ERR_NOERROR;
}
