

#include <sys/mman.h>
#include <errno.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

/*! \brief This function destroys the shared memory object.
 * Firstly it unlinks the shm objects referenced in buffer and
 * then unlink them thus removing them from system.
 * It doesn't stop on the first error, but completes all operations
 * and returns the code of first error.
 * \return 0 on succes or the number of the first error occurred.
 * */
int OMSbuff_shm_destroy(OMSBuffer * buffer)
{
	int unmap_err, shm_unlink_err = 0;
	char *shm_file_name;

	unmap_err = OMSbuff_shm_unmap(buffer);

	if (!
	    (shm_file_name =
	     fnc_ipc_name(buffer->filename, OMSBUFF_SHM_CTRLNAME)))
		return 1;
	if (shm_unlink(shm_file_name)) {
		switch (errno) {
		case ENOENT:
			ERRORLOGG("SHM Object %s doesn't exists");
			break;
		case EACCES:
			ERRORLOGG("Permission denied on SHM Object %s");
			break;
		default:
			ERRORLOGG("Could not unlink SHM Object %s");
			break;
		}
		shm_unlink_err = errno;
	}
	free(shm_file_name);

	if (!
	    (shm_file_name =
	     fnc_ipc_name(buffer->filename, OMSBUFF_SHM_SLOTSNAME)))
		return 1;
	if (shm_unlink(shm_file_name)) {
		switch (errno) {
		case ENOENT:
			ERRORLOGG("SHM Object %s doesn't exists");
			break;
		case EACCES:
			ERRORLOGG("Permission denied on SHM Object");
			break;
		default:
			ERRORLOGG("Could not unlink SHM Object %s");
			break;
		}
		shm_unlink_err = shm_unlink_err ? shm_unlink_err : errno;
	}
	free(shm_file_name);

	return unmap_err ? unmap_err : shm_unlink_err;
}
