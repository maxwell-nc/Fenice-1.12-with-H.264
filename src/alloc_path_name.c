
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fenice/utils.h>
#include <fenice/log.h>

char *alloc_path_name(char *base_path, char *file_path)
{
	char *PathName;
	char *p;
	int len;

	len = strlen(base_path);

	PathName = malloc(len + strlen(file_path) + 2);
	if (PathName == NULL) {
		FATALLOGG("Out of memory in alloc_path_name()");
		exit(-1);
	}

	p = base_path + len;
	if (len)
		p--;
	if ((*p == '/') && (*file_path == '/'))
		sprintf(PathName, "%s%s", base_path, file_path + 1);
	else if ((*p != '/') && (*file_path != '/'))
		sprintf(PathName, "%s/%s", base_path, file_path);
	else
		sprintf(PathName, "%s%s", base_path, file_path);

	return (PathName);
}
