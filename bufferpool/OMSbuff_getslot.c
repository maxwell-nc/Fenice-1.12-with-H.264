

#include <fenice/bufferpool.h>
#include <fenice/log.h>

/*! \brief get a new empty slot from bufferpool.
 *
 * It do NOT mark the slot as busy, this action will be performed by
 * OMSbuff_write that MUST be called at the end of operations with the obtained
 * slot.
 *
 * \return empty slot.
 */
OMSSlot *OMSbuff_getslot(OMSBuffer * buffer)
{
//	TRACE_FUNC();

	OMSSlot *slot = &buffer->slots[buffer->control->write_pos];	// lock not needed here
	// uint64 curr_seq = slot->slot_seq;
	OMSSlot *valid_read_pos = &buffer->slots[buffer->control->valid_read_pos];
	double ts;

	OMSbuff_lock(buffer);

	if (OMSbuff_shm_refresh(buffer))
		return NULL;

	if (buffer->slots[slot->next].refs > 0) {
		// printf("must add slot\n");
		if (!(slot = OMSbuff_addpage(buffer, slot))) {
			OMSbuff_unlock(buffer);
			return NULL;
		}
	} else {
		slot = &buffer->slots[slot->next];
		// write_pos reaches valid_read_pos, we "push" it
		if ((valid_read_pos->slot_seq) && (valid_read_pos == slot)) {
			for (ts = valid_read_pos->timestamp;
			     /*(buffer->slots[valid_read_pos->next].slot_seq) && \ */
			     (valid_read_pos->slot_seq <
			      buffer->slots[valid_read_pos->next].slot_seq)
			     && (ts == valid_read_pos->timestamp);
			     ts = valid_read_pos->timestamp, valid_read_pos =
			     &buffer->slots[valid_read_pos->next]);
			buffer->control->valid_read_pos = OMStoSlotPtr(buffer, valid_read_pos);
		}
	}

	OMSbuff_unlock(buffer);

	return slot;
}
