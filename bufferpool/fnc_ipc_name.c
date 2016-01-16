
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

char *fnc_ipc_name(const char *firstname, const char *lastname)
{
	char *dir, *dst, *slash;

	if ((dst = malloc(PATH_MAX)) == NULL)
		return (NULL);

	/* 4can override default directory with environment variable */
	if ((dir = getenv("PX_IPC_NAME")) == NULL) {
#ifdef	POSIX_IPC_PREFIX
		dir = POSIX_IPC_PREFIX;	/* from "config.h" */
#else
		dir = "";	// "/tmp/";   /* default */
#endif
	}
	/* 4dir must end in a slash */
	slash = (strlen(dir) && (dir[strlen(dir) - 1] == '/')) ? "" : "/";
	snprintf(dst, PATH_MAX, "%s%s%s.%s", dir, slash, firstname, lastname);

	return (dst);		/* caller can free() this pointer */
}
