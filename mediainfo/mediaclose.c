
#include <unistd.h>
																							      /*#include <stdlib.h> *//*free */
#include <fenice/mediainfo.h>


int mediaclose(media_entry * me)
{
	int ret = 0;

	if (me->fd > 0)
		ret = close(me->fd);
	me->fd = -1;
	me->flags &= ~ME_FD;
	me->buff_size = 0;
	me->media_handler->free_media((void *) me->stat);
	me->stat = NULL;

	// me->media_handler->free_media((void*) me->stat);
	/*do not release the media handler, because load_X is recalled only if .sd change */
	/*free(me->media_handler); */

	return ret;
}
