
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <fenice/mediainfo.h>
#include <fenice/utils.h>
#include <fenice/prefs.h>
#include <fenice/log.h>

int mediaopen(media_entry * me)
{
	char thefile[256];
	struct stat filestat;
	int oflag = O_RDONLY;

	if (!(me->flags & ME_FILENAME))
		return ERR_INPUT_PARAM;

	snprintf(thefile, sizeof(thefile) - 1, "%s%s%s", prefs_get_serv_root(),
		 (prefs_get_serv_root()[strlen(prefs_get_serv_root()) - 1] ==
		  '/') ? "" : "/", me->filename);

	DEBUGLOGG("opening file %s...", thefile);

	if (me->description.msource == live) {
		DEBUGLOGG(" Live stream... ");
		stat(thefile, &filestat);
		if (S_ISFIFO(filestat.st_mode)) {
			DEBUGLOGG(" IS_FIFO... ");
			oflag |= O_NONBLOCK;
		}
	}

	me->fd = open(thefile, oflag);

	if (me->fd == -1) {
		ERRORLOGG("Could not open %s", thefile);
		return ERR_NOT_FOUND;
	}

	me->flags |= ME_FD;

	return me->fd;
}
