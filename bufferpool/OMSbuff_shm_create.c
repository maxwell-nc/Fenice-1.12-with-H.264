
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/mman.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*! \brief This function creates che shared memory object on the file system.
 *  In order to let different, and independent, processes to access it.
 * WARNING: this function is not used by fenice directly but it is implemented here for completeness.
 * Fenice is just the "consumer" of a shared memory object, while the producer is some other object like felix.
 * */
 // TODO: unlink on error
OMSBuffer *OMSbuff_shm_create(char *shm_name, uint32 buffer_size)
{
	OMSBuffer *buffer;
	OMSSlot *slots;
	OMSControl *control;
	int fd, shm_open_errno;
	uint32 index;
	char *shm_file_name;
	pthread_mutexattr_t mutex_attr;

	if (!buffer_size)
		return NULL;

	// *** control struct shared memory object ***
	if (!(shm_file_name = fnc_ipc_name(shm_name, OMSBUFF_SHM_CTRLNAME)))
		return NULL;

	fd = shm_open(shm_file_name, O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	shm_open_errno = errno;
	if ((fd < 0)) {
		switch (shm_open_errno) {
		case EEXIST:
			ERRORLOGG("SHM object \"%s\" already exists! Perhaps some other apps are using it\n",
				shm_file_name);
			ERRORLOGG("TIP: If you are sure none is using it try deleting it manually.\n");
			break;
		case EINVAL:
			ERRORLOGG("Invalid name (%s) was given for shared memory object\n",
				shm_file_name);
			break;
		case EACCES:
			ERRORLOGG("Permission denied for shared memory object\n",
				shm_file_name);
			break;
		case ENOENT:
			ERRORLOGG( "Could not create %d", shm_file_name);
			break;
		default:
			break;
		}
		ERRORLOGG("Could not open/create POSIX shared memory %s (OMSControl)\n",
			shm_file_name);
		free(shm_file_name);
		return NULL;
	}
	free(shm_file_name);
	if (ftruncate(fd, sizeof(OMSControl))) {
		ERRORLOGG("Could not set correct size for shared memory object (OMSControl)\n");
		close(fd);
		return NULL;
	}
	control =
	    mmap(NULL, sizeof(OMSControl), PROT_READ | PROT_WRITE, MAP_SHARED,
		 fd, 0);
	close(fd);
	if (control == MAP_FAILED) {
		FATALLOGG("SHM: error in mmap.");
		return NULL;
	}
	// inizialization of OMSControl
	if (pthread_mutexattr_init(&mutex_attr)
	    || pthread_mutex_init(&control->syn, &mutex_attr)) {
		munmap(control, sizeof(OMSControl));
		return NULL;
	}

	pthread_mutex_lock(&control->syn);
	control->refs = 0;
	control->nslots = buffer_size;

	// *** slots mapping in shared memory ***
	if (!(shm_file_name = fnc_ipc_name(shm_name, OMSBUFF_SHM_SLOTSNAME)))
		return NULL;

	fd = shm_open(shm_file_name, O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	shm_open_errno = errno;
	free(shm_file_name);
	if ((fd < 0)) {
		switch (shm_open_errno) {
		case EEXIST:
			ERRORLOGG("SHM object \"%s\" already exists! Perhaps some other apps are using it\n",
				shm_file_name);
				ERRORLOGG("TIP: If you are sure none is using it try deleting it manually.\n");
			break;
		case EINVAL:
			ERRORLOGG("Invalid name (%s) was given for shared memory object\n",
				shm_file_name);
			break;
		default:
			break;
		}
		ERRORLOGG("Could not open/create POSIX shared memory (OMSSlots)\n");
		munmap(control, sizeof(OMSControl));
		return NULL;
	}
	if (ftruncate(fd, buffer_size * sizeof(OMSSlot))) {
		ERRORLOGG("Could not set correct size for shared memory object (OMSControl)\n");
		close(fd);
		munmap(control, sizeof(OMSControl));
		return NULL;
	}
	slots =
	    mmap(NULL, buffer_size * sizeof(OMSSlot), PROT_READ | PROT_WRITE,
		 MAP_SHARED, fd, 0);
	close(fd);
	if (slots == MAP_FAILED) {
		FATALLOGG("SHM: error in mmap.");
		munmap(control, sizeof(OMSControl));
		return NULL;
	}
	// inizialization of OMSSlots
	for (index = 0; index < buffer_size - 1; index++) {
		slots[index].refs = 0;
		slots[index].slot_seq = 0;
		slots[index].next = index + 1;	// &(slots[index+1]) - slots;
	}

	slots[index].next = 0;	// slots - slots; /*end of the list back to the head*/
	slots[index].slot_seq = 0;
	slots[index].refs = 0;	// last slot not yet initialized if for cycle.

	control->write_pos = buffer_size - 1;
	control->valid_read_pos = 0;	// buffer_size-1;

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

	pthread_mutex_unlock(&control->syn);

	return buffer;
}
