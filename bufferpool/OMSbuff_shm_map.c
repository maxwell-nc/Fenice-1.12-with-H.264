
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/mman.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

OMSBuffer *OMSbuff_shm_map(char *shm_name)
{
	OMSBuffer *buffer;
	OMSSlot *slots;
	OMSControl *control;
//      uint32 index;
	char *shm_file_name;
	int fd;
	struct stat fdstat;

	// *** control struct mapping in shared memory ***
	if (!(shm_file_name = fnc_ipc_name(shm_name, OMSBUFF_SHM_CTRLNAME)))
		return NULL;

	fd = shm_open(shm_file_name, O_RDWR, 0);
	free(shm_file_name);
	if ((fd < 0)) {
		ERRORLOGG("Could not open POSIX shared memory (OMSControl): is Felix running?");
		return NULL;
	}
	if ((fstat(fd, &fdstat) < 0)) {
		ERRORLOGG("Could not stat %s", OMSBUFF_SHM_CTRLNAME);
		close(fd);
		return NULL;
	}

	if (((size_t) fdstat.st_size != sizeof(OMSControl))) {
		ERRORLOGG("Strange size for OMSControl shared memory! (not an integer number of slots)");
		close(fd);
		return NULL;
	}
	control = mmap(NULL, fdstat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (control == MAP_FAILED) {
		FATALLOGG("SHM: error in mmap");
		return NULL;
	}

	// *** XXX inizialization of control struct made by creator!
	pthread_mutex_lock(&control->syn);
	pthread_mutex_unlock(&control->syn);

	// *** slots mapping in shared memory ***
	if (!(shm_file_name = fnc_ipc_name(shm_name, OMSBUFF_SHM_SLOTSNAME)))
		return NULL;

	fd = shm_open(shm_file_name, O_RDWR, 0);
	free(shm_file_name);
	if ((fd < 0)) {
		ERRORLOGG("Could not open POSIX shared memory (OMSSlots): is Felix running?");
		munmap(control, sizeof(OMSControl));
		return NULL;
	}
	if ((fstat(fd, &fdstat) < 0)) {
		ERRORLOGG("Could not stat %s", OMSBUFF_SHM_SLOTSNAME);
		close(fd);
		munmap(control, sizeof(OMSControl));
		return NULL;
	}

	if (((size_t) fdstat.st_size != control->nslots * sizeof(OMSSlot))) {
		ERRORLOGG("Strange size for shared memory! (not an integer number of slots)");
		close(fd);
		munmap(control, sizeof(OMSControl));
		return NULL;
	}
	slots =
	    mmap(NULL, fdstat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
		 0);
	close(fd);
	if (slots == MAP_FAILED) {
		FATALLOGG("SHM: error in mmap.");
		munmap(control, sizeof(OMSControl));
		return NULL;
	}

	if (!(buffer = (OMSBuffer *) malloc(sizeof(OMSBuffer)))) {
		munmap(slots, control->nslots * sizeof(OMSSlot));
		munmap(control, sizeof(OMSControl));
		return NULL;
	}

	buffer->type = buff_shm;
	buffer->slots = slots;
	buffer->known_slots = control->nslots;
	strncpy(buffer->filename, shm_name, sizeof(buffer->filename) - 1);
	// buffer->fd = -1;
	// buffer->fd = NULL;
	buffer->slots = slots;
	buffer->control = control;

	return buffer;
}
