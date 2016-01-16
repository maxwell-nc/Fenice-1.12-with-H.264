
#ifndef _EVENTLOOPH
#define _EVENTLOOPH

#include <fenice/socket.h>
#include <fenice/rtsp.h>

#define MAX_FDS 800

typedef int (*event_function) (void *data);

void eventloop(tsocket main_fd, tsocket main_sctp_fd);
int rtsp_server(RTSP_buffer * rtsp, fd_set * rset, fd_set * wset, fd_set * xset);
void add_client(RTSP_buffer ** rtsp_list, tsocket fd, RTSP_proto proto);
void schedule_connections(RTSP_buffer ** rtsp_list, int *conn_count, fd_set * rset, fd_set * wset, fd_set * xset);
void rtsp_set_fdsets(RTSP_buffer * rtsp, int * max_fd , fd_set * rset, fd_set * wset, fd_set * xset);

#endif
