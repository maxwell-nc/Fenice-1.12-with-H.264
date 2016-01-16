
#include <fenice/utils.h>
#include <fenice/log.h>

int get_frame(media_entry * me, double *mtime)
{
	int recallme = 0;
	OMSSlot *slot;
	int res = ERR_EOF;
	uint8 marker = 0;
	int32 err;

	if (!me->media_handler->read_media)
		return ERR_EOF;

	do {
		recallme = 0;
		res = ERR_EOF;
		slot = OMSbuff_getslot(me->pkt_buffer);
		res = me->media_handler->read_media(me, slot->data,
						  &slot->data_size, mtime,
						  &recallme, &marker);
		if (res == ERR_NOERROR && slot->data_size != 0) {	// commit of buffer slot.
			err = OMSbuff_write(me->pkt_buffer, 0,
								*mtime, marker, slot->data,
								slot->data_size);
			if (err)
				ERRORLOGG("Error in bufferpool writing.");
		}
		// slot->timestamp=*mtime;
/*
#ifdef VERBOSE 
		if(res==ERR_NOERROR)
			dump_payload(slot->data+4,slot->data_size-4,"fenice_dump");
#endif
*/
	} while (recallme && res == ERR_NOERROR);

	// fnc_log(FNC_LOG_VERBOSE,"TYPE: %s *mtime=%f\n",me->description.encoding_name,*mtime);

	return res;
}
