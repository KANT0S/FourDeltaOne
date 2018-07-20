#include "StdInc.h"
#include "dw.h"
#include <ws2tcpip.h>

static SOCKET dwSocket = -1;
static ULONG masterAddr;

int WINAPI dw_connect(SOCKET socket, const sockaddr* name, int namelen)
{
	if (!masterAddr)
	{
		hostent* host = gethostbyname(MASTER_SERVER);

		if (host)
		{
			masterAddr = *(ULONG*)host->h_addr_list[0];
		}
		else
		{
			masterAddr = -1;
		}
	}

	if (namelen == sizeof(sockaddr_in))
	{
		sockaddr_in* addr = (sockaddr_in*)name;

		if (addr->sin_addr.S_un.S_addr == masterAddr && addr->sin_port == htons(3074))
		{
			dwSocket = socket;
			return 0;
		}
	}

	return connect(socket, name, namelen);
}

int WINAPI dw_send(SOCKET socket, const char* buf, int len, int flags)
{
	if (socket == dwSocket)
	{
		dw_handle_packet(buf, len);
		return len;
	}

	return send(socket, buf, len, flags);
}

int WINAPI dw_sendto(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int tolen)
{
	if (socket == dwSocket)
	{
		dw_handle_packet(buf, len);
		return len;
	}

	return sendto(socket, buf, len, flags, to, tolen);
}

int WINAPI dw_recv(SOCKET socket, char* buf, int len, int flags)
{
	if (socket != dwSocket)
	{
		return recv(socket, buf, len, flags);
	}

	if (dw_packet_available())
	{
		return dw_dequeue_packet(buf, len);
	}

	WSASetLastError(WSAEWOULDBLOCK);
	return SOCKET_ERROR;
}

int WINAPI dw_recvfrom(SOCKET socket, char* buf, int len, int flags, sockaddr* from, int* fromlen)
{
	if (socket != dwSocket)
	{
		return recvfrom(socket, buf, len, flags, from, fromlen);
	}

	if (dw_packet_available())
	{
		return dw_dequeue_packet(buf, len);
	}

	WSASetLastError(WSAEWOULDBLOCK);
	return SOCKET_ERROR;
}

int	WINAPI dw_select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const timeval* timeout)
{
	bool containsDWSocket = false;
	bool containsDWSocketRead = false;

	if (writefds && (dwSocket != -1))
	{
		if (FD_ISSET(dwSocket, writefds))
		{
			containsDWSocket = true;

			Trace("dwentry", "removed DW socket from select() call (write)\n");
		}
	}

	if (readfds && (dwSocket == -1))
	{
		if (FD_ISSET(dwSocket, readfds))
		{
			containsDWSocketRead = true;
			FD_CLR(dwSocket, readfds);

			Trace("dwentry", "removed DW socket from select() call (read)\n");
		}
	}

	if (exceptfds && (dwSocket == -1))
	{
		if (FD_ISSET(dwSocket, exceptfds))
		{
			FD_CLR(dwSocket, exceptfds);

			Trace("dwentry", "removed DW socket from select() call (except)\n");
		}
	}

	int retval = select(nfds, readfds, writefds, exceptfds, timeout);

	if (containsDWSocket)
	{
		FD_SET(dwSocket, writefds);
		retval++;
	}

	if (containsDWSocketRead)
	{
		if (dw_packet_available())
		{
			FD_SET(dwSocket, readfds);
			retval++;
		}
	}

	return retval;
}