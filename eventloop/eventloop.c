
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <fenice/socket.h>
#include <fenice/eventloop.h>
#include <fenice/log.h>

int num_conn = 0;

void eventloop(tsocket main_fd, tsocket main_sctp_fd)
{
	static uint32 child_count = 0;
	static int conn_count = 0;
	static RTSP_buffer *rtsp_list = NULL;
	tsocket max_fd, fd = -1;

	RTSP_buffer *p = NULL;
	RTSP_proto rtsp_proto;
	uint32 fd_found;
	fd_set rset,wset;

	//Init of scheduler
	FD_ZERO(&rset);
	FD_ZERO(&wset);

	if (conn_count != -1) {
		/* This is the process allowed for accepting new clients */
		FD_SET(main_fd, &rset);
		max_fd = main_fd;
	}

	/* Add all sockets of all sessions to fd_sets */
	for (p = rtsp_list; p; p = p->next) {
		rtsp_set_fdsets(p, &max_fd, &rset, &wset, NULL);
	}
	/* Stay here and wait for something happens */
	if (select(max_fd+1, &rset, &wset, NULL, NULL) < 0) {
		ERRORLOGG("select error in eventloop()");
		/* Maybe we have to force exit here*/
		return;
	}
	/* transfer data for any RTSP sessions */
	schedule_connections(&rtsp_list, &conn_count, &rset, &wset, NULL);
	/* handle new connections */
	if (conn_count != -1) {
		if (FD_ISSET(main_fd, &rset)) {
			fd = tcp_accept(main_fd);
			rtsp_proto = TCP;
		}
		// Handle a new connection
		if (fd >= 0) {
			for (fd_found = 0, p = rtsp_list; p != NULL; p = p->next) {
				if (p->fd == fd) {
					fd_found = 1;
					break;
				}
			}

			if (!fd_found) {
				if (conn_count < ONE_FORK_MAX_CONNECTION) {
					++conn_count;
					// ADD A CLIENT
					add_client(&rtsp_list, fd, rtsp_proto);
				} else {
					if (fork() == 0) {
						// I'm the child
						++child_count;
						RTP_port_pool_init
						    (ONE_FORK_MAX_CONNECTION *
						     child_count * 2 +
						     RTP_DEFAULT_PORT);
						if (schedule_init() == ERR_FATAL) {
							FATALLOGG("Can't start schedule. Server is aborting");
							return;
						}
						conn_count = 1;
						rtsp_list = NULL;
						add_client(&rtsp_list, fd, rtsp_proto);
					} else {
						// I'm the father
						fd = -1;
						conn_count = -1;
						tcp_close(main_fd);
					}
				}
				num_conn++;
				INFOLOGG("Connection reached: %d", num_conn);
			}
		}
	} // shawill: and... if not?  END OF "HANDLE NEW CONNECTIONS"
}
