#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <fenice/types.h>
#include <fenice/log.h>

#define MAX_FILE_DUMP 6

int dump_payload(uint8 * data_slot, uint32 data_size, uint8 fname[255])
{
	static int fin[MAX_FILE_DUMP];
	static char filename[MAX_FILE_DUMP][255];
	static int idx = 0;
	int i = 0, found = 0;


	if (idx == 0) {
		for (i = 0; i < MAX_FILE_DUMP; i++)
			memset(filename[i], 0, sizeof(filename[i]));
	}

	for (i = 0; i < idx; i++) {
		if ((strcmp((const char *)fname, filename[i])) == 0) {
			found = 1;
			break;
		}
	}

	if (!found) {
		if (idx >= MAX_FILE_DUMP)
			return -1;
		strcpy(filename[idx], (const char *)fname);
		i = idx;
		idx++;
	}

	if (fin[i] <= 0) {
		int oflag = O_RDWR;

		oflag |= O_CREAT;
		oflag |= O_TRUNC;
		fin[i] = open((const char *)fname, oflag, 644);
	}
	if (fin[i] < 0) {
		ERRORLOGG("Error to open file for dumping.");
	} else {
		write(fin[i], (void *) data_slot, data_size);
	}

	return fin[i];
}
