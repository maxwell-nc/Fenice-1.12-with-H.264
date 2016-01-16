

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

/*!\brief This function adds shared memory page of slots.
 * The function remaps the shared memory according to the new size, but
 * it does NOT link new page to previous existing queue:
 * This is done in OMSbuff_slotadd.
 * WARNING: the function assumes that the caller (OMSbuff_write or OMSbuff_getslot) locked the buffer mutex
 * \return the first OMSSlot of new added page of slots.
 * */

OMSSlot *OMSbuff_shm_addpage(OMSBuffer * buffer)
{
	OMSSlot *added;
	OMSSlot *slots;
	unsigned int i;
	char *shm_file_name;
	int fd;
	struct stat fdstat;

	// *** slots mapping in shared memory ***
	if (!
	    (shm_file_name =
	     fnc_ipc_name(buffer->filename, OMSBUFF_SHM_SLOTSNAME)))
		return NULL;

	fd = shm_open(shm_file_name, O_RDWR, 0);
	free(shm_file_name);
	if ((fd < 0)) {
		ERRORLOGG("Could not open POSIX shared memory (OMSSlots): is Felix running?\n");
		return NULL;
	}
	if ((fstat(fd, &fdstat) < 0)) {
		ERRORLOGG("Could not stat %s", OMSBUFF_SHM_SLOTSNAME);
		close(fd);
		return NULL;
	}

	if (((size_t) fdstat.st_size !=
	     buffer->control->nslots * sizeof(OMSSlot))) {
		ERRORLOGG("Strange size for shared memory! (not the number of slots reported in control struct)\n");
		close(fd);
		return NULL;
	}
	if (ftruncate(fd, (buffer->control->nslots + OMSBUFF_SHM_PAGE) * sizeof(OMSSlot))) {
		ERRORLOGG("Could not set correct size for shared memory object (OMSControl)");
		close(fd);
		return NULL;
	}
	if (munmap(buffer->slots, buffer->known_slots * sizeof(OMSSlot))) {
		ERRORLOGG("Could not unmap previous slots!!!");
		close(fd);
		return NULL;
	}
	slots = mmap(NULL, (buffer->control->nslots + OMSBUFF_SHM_PAGE) * sizeof(OMSSlot),
		 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (slots == MAP_FAILED) {
		FATALLOGG("SHM: error in mmap");
		return NULL;
	}
	// inizialization of OMSSlots added
	for (i = buffer->control->nslots;
	     i < buffer->control->nslots + OMSBUFF_SHM_PAGE - 1; i++) {
		slots[i].refs = 0;
		slots[i].slot_seq = 0;
		slots[i].next = i + 1;
	}
	slots[i].refs = 0;
	slots[i].slot_seq = 0;
	slots[i].next = -1;	// last added slot in shm new page has next slot to NULL: OMSbuff_slotadd will link it correctly.

	added = &slots[buffer->control->nslots];
	buffer->slots = slots;
	buffer->control->nslots += OMSBUFF_SHM_PAGE;
	buffer->known_slots = buffer->control->nslots;

	return added;
}
