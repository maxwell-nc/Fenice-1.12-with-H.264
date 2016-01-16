
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

/*!\brief This function remaps shared memory slots in case that the slots queue size is changed.
 * The function only remap the shared memory according to the new size, but it does not do anything
 * on new shm slots page: if it must be initialized you must do it by yourself.
 * WARNING: the function assumes that the caller locked the buffer mutex.
 * \return 0 on succes, 1 otherwis.
 * */
int OMSbuff_shm_remap(OMSBuffer * buffer)
{
	OMSSlot *slots;
	char *shm_file_name;
	int fd;
	struct stat fdstat;

	// *** slots mapping in shared memory ***
	if (!(shm_file_name = fnc_ipc_name(buffer->filename, OMSBUFF_SHM_SLOTSNAME)))
		return 1;

	fd = shm_open(shm_file_name, O_RDWR, 0);
	free(shm_file_name);
	if ((fd < 0)) {
		ERRORLOGG("Could not open POSIX shared memory (OMSSlots): is Felix running?\n");
		return 1;
	}
	if ((fstat(fd, &fdstat) < 0)) {
		ERRORLOGG("Could not stat %s", OMSBUFF_SHM_SLOTSNAME);
		close(fd);
		return 1;
	}

	if (((size_t) fdstat.st_size !=
	     buffer->control->nslots * sizeof(OMSSlot))) {
		ERRORLOGG("Strange size for shared memory! (not the number of slots reported in control struct)\n");
		close(fd);
		return 1;
	}

	if (munmap(buffer->slots, buffer->known_slots * sizeof(OMSSlot))) {
		ERRORLOGG("Could not unmap previous slots!!!");
		close(fd);
		return 1;
	}
	slots = mmap(NULL, fdstat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (slots == MAP_FAILED) {
		FATALLOGG("SHM: error in mmap");
		return 1;
	}

	buffer->slots = slots;
	buffer->known_slots = buffer->control->nslots;

	DEBUGLOGG("SHM memory remapped (known slots %d)", buffer->known_slots);

	return 0;
}
