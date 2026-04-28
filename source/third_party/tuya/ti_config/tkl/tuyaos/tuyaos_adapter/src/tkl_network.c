/**
 * @file tkl_network.c
 * @brief Network driver implementation for TI CC35xx using LwIP
 * @version 1.0
 */

// --- BEGIN: user defines and implements ---
#include "tkl_network.h"
#include "tuya_error_code.h"

/* --- LwIP & BSD Socket Includes --- */
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/errno.h>
#include <lwip/inet.h>
#include <lwip/tcp.h>

#include <string.h>

/* Helper to convert Tuya Protocol type to BSD Socket type */
static int _get_socket_type(TUYA_PROTOCOL_TYPE_E type) {
    if (type == PROTOCOL_TCP) return SOCK_STREAM;
    if (type == PROTOCOL_UDP) return SOCK_DGRAM;
    return SOCK_STREAM; // Default
}
// --- END: user defines and implements ---

/**
 * @brief Get error code of network
 */
TUYA_ERRNO tkl_net_get_errno(void)
{
    // --- BEGIN: user implements ---
    /* LwIP maps errno correctly, so we just return the global errno */
    return errno;
    // --- END: user implements ---
}

/**
 * @brief Add file descriptor to set
 */
OPERATE_RET tkl_net_fd_set(const int fd, TUYA_FD_SET_T *fds)
{
    // --- BEGIN: user implements ---
    if (fds == NULL) return OPRT_INVALID_PARM;
    FD_SET(fd, (fd_set *)fds);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief Clear file descriptor from set
 */
OPERATE_RET tkl_net_fd_clear(const int fd, TUYA_FD_SET_T *fds)
{
    // --- BEGIN: user implements ---
    if (fds == NULL) return OPRT_INVALID_PARM;
    FD_CLR(fd, (fd_set *)fds);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief Check file descriptor is in set
 */
OPERATE_RET tkl_net_fd_isset(const int fd, TUYA_FD_SET_T *fds)
{
    // --- BEGIN: user implements ---
    if (fds == NULL) return 0;
    return FD_ISSET(fd, (fd_set *)fds) ? 1 : 0;
    // --- END: user implements ---
}

/**
 * @brief Clear all file descriptor in set
 */
OPERATE_RET tkl_net_fd_zero(TUYA_FD_SET_T *fds)
{
    // --- BEGIN: user implements ---
    if (fds == NULL) return OPRT_INVALID_PARM;
    FD_ZERO((fd_set *)fds);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief Get available file descriptors
 */
int tkl_net_select(const int maxfd, TUYA_FD_SET_T *readfds, TUYA_FD_SET_T *writefds, TUYA_FD_SET_T *errorfds,
                   const uint32_t ms_timeout)
{
    // --- BEGIN: user implements ---
    struct timeval timeout;
    struct timeval *pto = NULL;

    if (ms_timeout != 0xFFFFFFFF) { // Check for infinite wait
        timeout.tv_sec = ms_timeout / 1000;
        timeout.tv_usec = (ms_timeout % 1000) * 1000;
        pto = &timeout;
    }

    return select(maxfd, (fd_set *)readfds, (fd_set *)writefds, (fd_set *)errorfds, pto);
    // --- END: user implements ---
}

/**
 * @brief Get no block file descriptors
 */
int tkl_net_get_nonblock(const int fd)
{
    // --- BEGIN: user implements ---
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return (flags & O_NONBLOCK) ? 1 : 0;
    // --- END: user implements ---
}

/**
 * @brief Set block flag for file descriptors
 */
OPERATE_RET tkl_net_set_block(const int fd, const BOOL_T block)
{
    // --- BEGIN: user implements ---
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return OPRT_COM_ERROR;

    if (block) {
        flags &= ~O_NONBLOCK; // Clear Non-Block (Enable Block)
    } else {
        flags |= O_NONBLOCK;  // Set Non-Block
    }

    if (fcntl(fd, F_SETFL, flags) == -1) {
        return OPRT_COM_ERROR;
    }
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief Close file descriptors
 */
TUYA_ERRNO tkl_net_close(const int fd)
{
    // --- BEGIN: user implements ---
    if (close(fd) == 0) {
        return 0;
    }
    return errno;
    // --- END: user implements ---
}

/**
 * @brief Shutdown file descriptors
 */
TUYA_ERRNO tkl_net_shutdown(const int fd, const int how)
{
    // --- BEGIN: user implements ---
    if (shutdown(fd, how) == 0) {
        return 0;
    }
    return errno;
    // --- END: user implements ---
}

/**
 * @brief Create a tcp/udp socket
 */
int tkl_net_socket_create(const TUYA_PROTOCOL_TYPE_E type)
{
    // --- BEGIN: user implements ---
    int sock_type = _get_socket_type(type);
    int fd = socket(AF_INET, sock_type, 0);
    return fd; // Return the FD or -1 on error (standard behavior)
    // --- END: user implements ---
}

/**
 * @brief Connect to network
 */
TUYA_ERRNO tkl_net_connect(const int fd, const TUYA_IP_ADDR_T addr, const uint16_t port)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = addr; // Tuya passes IP in Network Byte Order usually

    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
        return 0;
    }
    return errno;
    // --- END: user implements ---
}

/**
 * @brief Connect to network with raw data
 */
TUYA_ERRNO tkl_net_connect_raw(const int fd, void *p_socket_addr, const int len)
{
    // --- BEGIN: user implements ---
    if (connect(fd, (struct sockaddr *)p_socket_addr, len) == 0) {
        return 0;
    }
    return errno;
    // --- END: user implements ---
}

/**
 * @brief Bind to network
 */
TUYA_ERRNO tkl_net_bind(const int fd, const TUYA_IP_ADDR_T addr, const uint16_t port)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in bind_addr;

    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port);
    bind_addr.sin_addr.s_addr = addr;

    if (bind(fd, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) == 0) {
        return 0;
    }
    return errno;
    // --- END: user implements ---
}

/**
 * @brief Listen to network
 */
TUYA_ERRNO tkl_net_listen(const int fd, const int backlog)
{
    // --- BEGIN: user implements ---
    if (listen(fd, backlog) == 0) {
        return 0;
    }
    return errno;
    // --- END: user implements ---
}

/**
 * @brief Accept network connection
 */
TUYA_ERRNO tkl_net_accept(const int fd, TUYA_IP_ADDR_T *addr, uint16_t *port)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);

    if (client_fd >= 0) {
        if (addr) *addr = client_addr.sin_addr.s_addr;
        if (port) *port = ntohs(client_addr.sin_port);
        return client_fd; // Return the new FD
    }
    return -1; // Standard error indication
    // --- END: user implements ---
}

/**
 * @brief Send data to network
 */
TUYA_ERRNO tkl_net_send(const int fd, const void *buf, const uint32_t nbytes)
{
    // --- BEGIN: user implements ---
    /* Return number of bytes sent or -1 on error */
    return send(fd, buf, nbytes, 0); 
    // --- END: user implements ---
}

/**
 * @brief Send data to specified server
 */
TUYA_ERRNO tkl_net_send_to(const int fd, const void *buf, const uint32_t nbytes, const TUYA_IP_ADDR_T addr,
                           const uint16_t port)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in to_addr;
    memset(&to_addr, 0, sizeof(to_addr));
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(port);
    to_addr.sin_addr.s_addr = addr;

    return sendto(fd, buf, nbytes, 0, (struct sockaddr *)&to_addr, sizeof(to_addr));
    // --- END: user implements ---
}

/**
 * @brief Receive data from network
 */
TUYA_ERRNO tkl_net_recv(const int fd, void *buf, const uint32_t nbytes)
{
    // --- BEGIN: user implements ---
    return recv(fd, buf, nbytes, 0);
    // --- END: user implements ---
}

/**
 * @brief Receive data from network with need size (Wait all)
 */
int tkl_net_recv_nd_size(const int fd, void *buf, const uint32_t buf_size, const uint32_t nd_size)
{
    // --- BEGIN: user implements ---
    uint32_t total = 0;
    char *dst = (char *)buf;

    if ((buf == NULL) || (nd_size > buf_size)) {
        errno = EINVAL;
        return -1;
    }

    while (total < nd_size) {
        int ret = recv(fd, dst + total, nd_size - total, 0);
        if (ret <= 0) {
            if (total > 0U) {
                return (int)total;
            }
            return ret;
        }
        total += (uint32_t)ret;
    }

    return (int)total;
    // --- END: user implements ---
}

/**
 * @brief Receive data from specified server
 */
TUYA_ERRNO tkl_net_recvfrom(const int fd, void *buf, const uint32_t nbytes, TUYA_IP_ADDR_T *addr, uint16_t *port)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    int ret = recvfrom(fd, buf, nbytes, 0, (struct sockaddr *)&from_addr, &addr_len);

    if (ret >= 0) {
        if (addr) *addr = from_addr.sin_addr.s_addr;
        if (port) *port = ntohs(from_addr.sin_port);
    }
    return ret;
    // --- END: user implements ---
}

/**
 * @brief Get address information by domain (DNS)
 */
OPERATE_RET tkl_net_gethostbyname(const char *domain, TUYA_IP_ADDR_T *addr)
{
    // --- BEGIN: user implements ---
    struct hostent *h;
    
    if (domain == NULL || addr == NULL) return OPRT_INVALID_PARM;

    h = gethostbyname(domain);
    if (h != NULL) {
        *addr = ((struct in_addr *)(h->h_addr))->s_addr;
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Bind to network with specified ip
 */
OPERATE_RET tkl_net_socket_bind(const int fd, const char *ip)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = inet_addr(ip);
    bind_addr.sin_port = 0; // Random port

    if (bind(fd, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) == 0) {
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Set socket fd close mode
 */
OPERATE_RET tkl_net_set_cloexec(const int fd)
{
    // --- BEGIN: user implements ---
    // LwIP doesn't typically support FD_CLOEXEC logic like Linux, 
    // but usually it's not needed for embedded single-process systems.
    return OPRT_OK; 
    // --- END: user implements ---
}

/**
 * @brief Get ip address by socket fd
 */
OPERATE_RET tkl_net_get_socket_ip(const int fd, TUYA_IP_ADDR_T *addr)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);

    if (getsockname(fd, (struct sockaddr *)&local_addr, &addr_len) == 0) {
        *addr = local_addr.sin_addr.s_addr;
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Change ip string to address
 */
TUYA_IP_ADDR_T tkl_net_str2addr(const char *ip_str)
{
    // --- BEGIN: user implements ---
    return inet_addr(ip_str);
    // --- END: user implements ---
}

/**
 * @brief Change ip address to string
 */
char *tkl_net_addr2str(const TUYA_IP_ADDR_T ipaddr)
{
    // --- BEGIN: user implements ---
    struct in_addr ia;
    ia.s_addr = ipaddr;
    return inet_ntoa(ia);
    // --- END: user implements ---
}

/**
 * @brief Set socket options
 */
OPERATE_RET tkl_net_setsockopt(const int fd, const TUYA_OPT_LEVEL level, const TUYA_OPT_NAME optname,
                               const void *optval, const int optlen)
{
    // --- BEGIN: user implements ---
    if (setsockopt(fd, level, optname, optval, (socklen_t)optlen) == 0) {
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Get socket options
 */
OPERATE_RET tkl_net_getsockopt(const int fd, const TUYA_OPT_LEVEL level, const TUYA_OPT_NAME optname, void *optval,
                               int *optlen)
{
    // --- BEGIN: user implements ---
    if (getsockopt(fd, level, optname, optval, (socklen_t *)optlen) == 0) {
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Set timeout option of socket fd
 */
OPERATE_RET tkl_net_set_timeout(const int fd, const int ms_timeout, const TUYA_TRANS_TYPE_E type)
{
    // --- BEGIN: user implements ---
    struct timeval tv;
    tv.tv_sec = ms_timeout / 1000;
    tv.tv_usec = (ms_timeout % 1000) * 1000;
    int optname;

    if (type == TRANS_RECV) {
        optname = SO_RCVTIMEO;
    } else {
        optname = SO_SNDTIMEO;
    }

    if (setsockopt(fd, SOL_SOCKET, optname, &tv, sizeof(tv)) == 0) {
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Set buffer_size option of socket fd
 */
OPERATE_RET tkl_net_set_bufsize(const int fd, const int buf_size, const TUYA_TRANS_TYPE_E type)
{
    // --- BEGIN: user implements ---
    int optname = (type == TRANS_RECV) ? SO_RCVBUF : SO_SNDBUF;
    if (setsockopt(fd, SOL_SOCKET, optname, &buf_size, sizeof(buf_size)) == 0) {
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Enable reuse option of socket fd
 */
OPERATE_RET tkl_net_set_reuse(const int fd)
{
    // --- BEGIN: user implements ---
    int flag = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == 0) {
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Disable nagle option of socket fd
 */
OPERATE_RET tkl_net_disable_nagle(const int fd)
{
    // --- BEGIN: user implements ---
    int flag = 1;
    // TCP_NODELAY is usually defined in netinet/tcp.h or lwip/sockets.h
    // It maps to IPPROTO_TCP level
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == 0) {
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Enable broadcast option of socket fd
 */
OPERATE_RET tkl_net_set_broadcast(const int fd)
{
    // --- BEGIN: user implements ---
    int flag = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) == 0) {
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Set keepalive option
 */
OPERATE_RET tkl_net_set_keepalive(int fd, const BOOL_T alive, const uint32_t idle, const uint32_t intr,
                                  const uint32_t cnt)
{
    // --- BEGIN: user implements ---
    int keepalive = alive ? 1 : 0;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(int)) != 0) return OPRT_COM_ERROR;
    
    // Note: LwIP may not support setting IDLE/INTVL/CNT per socket on all versions,
    // but basic Keepalive enable/disable is standard.
    // If supported by LwIP config:
    #ifdef TCP_KEEPIDLE
        setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));
    #endif
    #ifdef TCP_KEEPINTVL
        setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &intr, sizeof(int));
    #endif
    #ifdef TCP_KEEPCNT
        setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &cnt, sizeof(int));
    #endif

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief Get socket name
 */
OPERATE_RET tkl_net_getsockname(int fd, TUYA_IP_ADDR_T *addr, uint16_t *port)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    if (getsockname(fd, (struct sockaddr*)&sa, &len) == 0) {
        *addr = sa.sin_addr.s_addr;
        *port = ntohs(sa.sin_port);
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Get name of connected peer socket
 */
OPERATE_RET tkl_net_getpeername(int fd, TUYA_IP_ADDR_T *addr, uint16_t *port)
{
    // --- BEGIN: user implements ---
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    if (getpeername(fd, (struct sockaddr*)&sa, &len) == 0) {
        *addr = sa.sin_addr.s_addr;
        *port = ntohs(sa.sin_port);
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief Set the system hostname
 */
OPERATE_RET tkl_net_sethostname(const char *hostname)
{
    // --- BEGIN: user implements ---
    // Normally not supported in standard LwIP Socket API, handled by netif directly
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}
