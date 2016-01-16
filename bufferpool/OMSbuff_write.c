
#include <sys/mman.h>

#include <stdio.h>
#include <string.h>
#include <fenice/utils.h>
#include <fenice/log.h>

/*! \brief Write a new slot in the buffer using the input parameters.
 *  
 *  The function writes a new slot according with input parameters. If the
 *  <tt>data</tt> belongs to a slot that was prevoiusly requested with
 *  <tt>OMSbuff_getslot</tt> then there is not the copy of data, just the
 *  parameter setting like a commit of changes done.
 * \param seq impose explicitly the sequence number of pkt(slot). If this parameter is 0 the
 * it's set calculating 'prev_seq+1'.
 *  \return ERR_NOERROR or ERR_ALLOC
 *  */
int32 OMSbuff_write(
		OMSBuffer * buffer,
		uint64 seq,
		uint32 timestamp,
		uint8 marker,
		uint8 * data,
		uint32 data_size)
{
	OMSSlot *slot = &buffer->slots[buffer->control->write_pos];
	uint64 curr_seq = slot->slot_seq;
	OMSSlot *valid_read_pos = &buffer->slots[buffer->control->valid_read_pos];
	double ts;

//	TRACE_FUNC();

	OMSbuff_lock(buffer);

	if (OMSbuff_shm_refresh(buffer))
		return ERR_ALLOC;

//	DEBUGLOGG("slot->next = %d", slot->next);
//	DEBUGLOGG("buffer->slots[slot->next].data = %p",
//			  buffer->slots[slot->next].data);
//	DEBUGLOGG("data                           = %p", data);

	if (buffer->slots[slot->next].data == data) {
		slot = &buffer->slots[slot->next];
	} else {
		if (buffer->slots[slot->next].refs > 0) {
			if ((slot = OMSbuff_addpage(buffer, slot)) == NULL) {
				OMSbuff_unlock(buffer);
				return ERR_ALLOC;
			}
		} else {
			slot = &buffer->slots[slot->next];
			// write_pos reaches valid_read_pos, we "push" it
			if ((valid_read_pos->slot_seq) && (valid_read_pos == slot)) {
				for (ts = valid_read_pos->timestamp;
				     (buffer->slots[valid_read_pos->next].slot_seq)
				     && (ts == valid_read_pos->timestamp);
				     ts = valid_read_pos->timestamp, valid_read_pos =
				     &buffer->slots[valid_read_pos->next]);
				buffer->control->valid_read_pos =
				    OMStoSlotPtr(buffer, valid_read_pos);
			}
		}

		memcpy(slot->data, data, data_size);
	}

	slot->timestamp = timestamp;
	slot->marker = marker;
	slot->data_size = data_size;

	slot->refs = buffer->control->refs;
	slot->slot_seq = seq ? seq : curr_seq + 1;

	buffer->control->write_pos = OMStoSlotPtr(buffer, slot);

//	DEBUGLOGG("write_pos = %d", buffer->control->write_pos);

//      if ( msync(slot, sizeof(OMSSlot), MS_ASYNC) )
//      if ( msync(buffer->slots, buffer->known_slots*sizeof(OMSSlot), MS_ASYNC) )
//              printf("*** slot msync error\n");
//      if ( msync(buffer->control, sizeof(OMSControl), MS_ASYNC) )
//              printf("*** control msync error\n");
	OMSbuff_dump(NULL, buffer);

	OMSbuff_unlock(buffer);

	return ERR_NOERROR;
}
