
#ifndef RTP_SHM_H_
#define RTP_SHM_H_

int load_RTP_SHM(media_entry *);
int read_RTP_SHM(media_entry *, uint8 *, uint32 *, double *, int *, uint8 *);
int free_RTP_SHM(void *);

#endif				/*RTP_SHM_H_ */
