

#include <string.h>
#include <errno.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>
 
ssize_t RTSP_send(RTSP_buffer * rtsp)
{
	int n = 0;
	size_t to_send;
	// char *buffer;

 	if (!rtsp->out_size) {
 		WARNLOGG("RTSP_send called, but no data to be sent");
 		return 0;
 	}

	to_send = rtsp->out_size - rtsp->out_sent;
		
	// TODO: find che interleaved channel to be used and, in the TCP case, build the pkt.
		
	switch (rtsp->proto) {
	case TCP:
		if ( (n = send(rtsp->fd, rtsp->out_buffer + rtsp->out_sent, to_send, MSG_DONTWAIT | MSG_NOSIGNAL)) < 0) {
			switch (errno) {
				case EACCES:
					ERRORLOGG("EACCES error");
					break;
				case EAGAIN:
					ERRORLOGG("EAGAIN error");
					break;
				case EBADF:
					ERRORLOGG("EBADF error");
					break;
				case ECONNRESET:
					ERRORLOGG("ECONNRESET error");
					break;
				case EDESTADDRREQ:
					ERRORLOGG("EDESTADDRREQ error");
					break;
				case EFAULT:
					ERRORLOGG("EFAULT error");
					break;
				case EINTR:
					ERRORLOGG("EINTR error");
					break;
				case EINVAL:
					ERRORLOGG("EINVAL error");
					break;
				case EISCONN:
					ERRORLOGG("EISCONN error");
					break;
				case EMSGSIZE:
					ERRORLOGG("EMSGSIZE error");
					break;
				case ENOBUFS:
					ERRORLOGG("ENOBUFS error");
					break;
				case ENOMEM:
					ERRORLOGG("ENOMEM error");
					break;
				case ENOTCONN:
					ERRORLOGG("ENOTCONN error");
					break;
				case ENOTSOCK:
					ERRORLOGG("ENOTSOCK error");
					break;
				case EOPNOTSUPP:
					ERRORLOGG("EOPNOTSUPP error");
					break;
				case EPIPE:
					ERRORLOGG("EPIPE error");
					break;
				default:
					break;
			}
			ERRORLOGG("send() error in RTSP_send()");
			return n;
		}
		break;
	default:
		return ERR_GENERIC;
		break;
	}

	if ( (rtsp->out_sent += n) == rtsp->out_size )
		rtsp->out_sent = rtsp->out_size = 0;
	return n;
}
