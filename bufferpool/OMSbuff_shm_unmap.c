
#include <sys/mman.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

int OMSbuff_shm_unmap(OMSBuffer * buffer)
{
	int slots_err;
	int control_err;

	if (buffer->type != buff_shm) {
		ERRORLOGG("Bufferpool desn't seems to be a Shared Memory object");
		return 1;
	}

	if ((slots_err = munmap(buffer->slots, buffer->control->nslots * sizeof(OMSSlot))))
	ERRORLOGG("Error unmapping OMSSlots SHM object");
	if ((control_err = munmap(buffer->control, sizeof(OMSControl))))
	ERRORLOGG("Error unmapping OMSControl SHM object");

	return slots_err ? slots_err : control_err;
}
