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

#include <stdio.h>
#include <string.h>
#if defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include <unistd.h>
#endif

#include <ti/drivers/net/wifi/wifi_host_driver/inc_adapt/wlan_if.h>

extern int Report(const char *pcFormat, ...);
extern int8_t network_stack_get_if_ip(WlanRole_e role, uint32_t *ip, uint32_t *netmask, uint32_t *gw, uint32_t *dhcp);

#define TKL_TEST_AP_BROADCAST_ADDR ((TUYA_IP_ADDR_T)0xffb0a8c0UL)

static TKL_NET_DIAG_T g_tkl_net_diag = {
    .last_socket_fd = -1,
    .last_socket_ret = -1,
    .last_bind_fd = -1,
    .last_bind_ret = -1,
    .last_listen_fd = -1,
    .last_listen_ret = -1,
    .last_select_ret = -1,
    .last_accept_listen_fd = -1,
    .last_accept_ret = -1,
    .last_recv_fd = -1,
    .last_recv_ret = -1,
    .last_send_fd = -1,
    .last_send_ret = -1,
    .last_sendto_fd = -1,
    .last_sendto_ret = -1,
    .last_close_fd = -1,
    .last_close_ret = -1,
    .ap_tcp_server_fd = -1,
};

static TUYA_FD_SET_T g_tkl_nonblock_fds = {0};

static uint32_t _tuya_fdset_mask32(const TUYA_FD_SET_T *fds)
{
    uint32_t mask = 0;
    size_t copy_len;

    if (fds == NULL) {
        return 0;
    }

    copy_len = sizeof(mask);
    if (copy_len > sizeof(fds->placeholder)) {
        copy_len = sizeof(fds->placeholder);
    }

    (void)memcpy(&mask, fds->placeholder, copy_len);
    return mask;
}

static void _tkl_format_ipv4(TUYA_IP_ADDR_T addr, char *buf, size_t buf_len)
{
    uint32_t host_addr = ntohl(addr);

    if (buf == NULL || buf_len == 0U) {
        return;
    }

    (void)snprintf(buf, buf_len, "%lu.%lu.%lu.%lu",
                   (unsigned long)((host_addr >> 24) & 0xffU),
                   (unsigned long)((host_addr >> 16) & 0xffU),
                   (unsigned long)((host_addr >> 8) & 0xffU),
                   (unsigned long)(host_addr & 0xffU));
}

static BOOL_T _tkl_ap_ipv4_ready(void)
{
    uint32_t ap_ip = 0;
    uint32_t ap_netmask = 0;
    uint32_t ap_gw = 0;
    uint32_t ap_dhcp = 0;

    return (network_stack_get_if_ip(WLAN_ROLE_AP, &ap_ip, &ap_netmask, &ap_gw, &ap_dhcp) == 0) ? TRUE : FALSE;
}

OPERATE_RET tkl_net_diag_get(TKL_NET_DIAG_T *diag)
{
    if (diag == NULL) {
        return OPRT_INVALID_PARM;
    }

    *diag = g_tkl_net_diag;
    return OPRT_OK;
}

static int _tuya_fd_index(const int fd)
{
    int idx = fd - LWIP_SOCKET_OFFSET;
    if (idx < 0 || idx >= (int)(sizeof(((TUYA_FD_SET_T *)0)->placeholder) * 8U)) {
        return -1;
    }
    return idx;
}

static void _tkl_track_nonblock_fd(const int fd, const BOOL_T nonblock)
{
    int idx = _tuya_fd_index(fd);

    if (idx < 0) {
        return;
    }

    if (nonblock) {
        g_tkl_nonblock_fds.placeholder[idx / 8] |= (uint8_t)(1U << (idx & 7));
    } else {
        g_tkl_nonblock_fds.placeholder[idx / 8] &= (uint8_t)~(1U << (idx & 7));
    }
}

static void _tuya_fdset_to_lwip(const TUYA_FD_SET_T *src, fd_set *dst, int maxfd)
{
    int fd;

    FD_ZERO(dst);
    if (src == NULL) {
        return;
    }

    for (fd = LWIP_SOCKET_OFFSET; fd < maxfd; fd++) {
        int idx = _tuya_fd_index(fd);
        if (idx < 0) {
            continue;
        }
        if (src->placeholder[idx / 8] & (uint8_t)(1U << (idx & 7))) {
            FD_SET(fd, dst);
        }
    }
}

static void _lwip_fdset_to_tuya(const fd_set *src, TUYA_FD_SET_T *dst, int maxfd)
{
    int fd;

    if (dst == NULL) {
        return;
    }

    memset(dst->placeholder, 0, sizeof(dst->placeholder));

    if (src == NULL) {
        return;
    }

    for (fd = LWIP_SOCKET_OFFSET; fd < maxfd; fd++) {
        int idx = _tuya_fd_index(fd);
        if (idx < 0) {
            continue;
        }
        if (FD_ISSET(fd, src)) {
            dst->placeholder[idx / 8] |= (uint8_t)(1U << (idx & 7));
        }
    }
}

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
    int idx;

    if (fds == NULL) return OPRT_INVALID_PARM;
    idx = _tuya_fd_index(fd);
    if (idx < 0) return OPRT_INVALID_PARM;
    fds->placeholder[idx / 8] |= (uint8_t)(1U << (idx & 7));
    return OPRT_OK;
}

/**
 * @brief Clear file descriptor from set
 */
OPERATE_RET tkl_net_fd_clear(const int fd, TUYA_FD_SET_T *fds)
{
    int idx;

    if (fds == NULL) return OPRT_INVALID_PARM;
    idx = _tuya_fd_index(fd);
    if (idx < 0) return OPRT_INVALID_PARM;
    fds->placeholder[idx / 8] &= (uint8_t)~(1U << (idx & 7));
    return OPRT_OK;
}

/**
 * @brief Check file descriptor is in set
 */
OPERATE_RET tkl_net_fd_isset(const int fd, TUYA_FD_SET_T *fds)
{
    int idx;

    if (fds == NULL) return 0;
    idx = _tuya_fd_index(fd);
    if (idx < 0) return 0;
    return (fds->placeholder[idx / 8] & (uint8_t)(1U << (idx & 7))) ? 1 : 0;
}

/**
 * @brief Clear all file descriptor in set
 */
OPERATE_RET tkl_net_fd_zero(TUYA_FD_SET_T *fds)
{
    if (fds == NULL) return OPRT_INVALID_PARM;
    memset(fds->placeholder, 0, sizeof(fds->placeholder));
    return OPRT_OK;
}

/**
 * @brief Get available file descriptors
 */
int tkl_net_select(const int maxfd, TUYA_FD_SET_T *readfds, TUYA_FD_SET_T *writefds, TUYA_FD_SET_T *errorfds,
                   const uint32_t ms_timeout)
{
    fd_set lwip_readfds;
    fd_set lwip_writefds;
    fd_set lwip_errorfds;
    fd_set *pread = NULL;
    fd_set *pwrite = NULL;
    fd_set *perr = NULL;
    struct timeval timeout;
    struct timeval *pto = NULL;
    int ret;

    g_tkl_net_diag.last_select_maxfd = maxfd;
    g_tkl_net_diag.last_select_timeout_ms = ms_timeout;
    g_tkl_net_diag.last_select_read_mask_before = _tuya_fdset_mask32(readfds);
    g_tkl_net_diag.last_select_error_mask_before = _tuya_fdset_mask32(errorfds);
    g_tkl_net_diag.select_call_count++;

    if (maxfd > LWIP_SELECT_MAXNFDS) {
        Report("[TKL_NET] select warn maxfd=%d exceeds lwip_max=%d\r\n", maxfd, LWIP_SELECT_MAXNFDS);
    }

    if (readfds != NULL) {
        _tuya_fdset_to_lwip(readfds, &lwip_readfds, maxfd);
        pread = &lwip_readfds;
    }
    if (writefds != NULL) {
        _tuya_fdset_to_lwip(writefds, &lwip_writefds, maxfd);
        pwrite = &lwip_writefds;
    }
    if (errorfds != NULL) {
        _tuya_fdset_to_lwip(errorfds, &lwip_errorfds, maxfd);
        perr = &lwip_errorfds;
    }

    if (ms_timeout != 0xFFFFFFFF) { // Check for infinite wait
        timeout.tv_sec = ms_timeout / 1000;
        timeout.tv_usec = (ms_timeout % 1000) * 1000;
        pto = &timeout;
    }

    ret = select(maxfd, pread, pwrite, perr, pto);
    if (readfds != NULL) {
        _lwip_fdset_to_tuya((ret >= 0) ? pread : NULL, readfds, maxfd);
    }
    if (writefds != NULL) {
        _lwip_fdset_to_tuya((ret >= 0) ? pwrite : NULL, writefds, maxfd);
    }
    if (errorfds != NULL) {
        _lwip_fdset_to_tuya((ret >= 0) ? perr : NULL, errorfds, maxfd);
    }
    g_tkl_net_diag.last_select_ret = ret;
    g_tkl_net_diag.last_select_read_mask_after = _tuya_fdset_mask32(readfds);
    g_tkl_net_diag.last_select_error_mask_after = _tuya_fdset_mask32(errorfds);
    if (ret < 0) {
        g_tkl_net_diag.select_error_count++;
        Report("[TKL_NET] select fail maxfd=%d timeout=%lu errno=%d\r\n", maxfd, (unsigned long)ms_timeout, errno);
    } else if (ret > 0) {
        g_tkl_net_diag.select_ready_count++;
        Report("[TKL_NET] select ready maxfd=%d timeout=%lu ret=%d read_before=0x%08lx read_after=0x%08lx err_after=0x%08lx\r\n",
               maxfd,
               (unsigned long)ms_timeout,
               ret,
               (unsigned long)g_tkl_net_diag.last_select_read_mask_before,
               (unsigned long)g_tkl_net_diag.last_select_read_mask_after,
               (unsigned long)g_tkl_net_diag.last_select_error_mask_after);
    } else {
        g_tkl_net_diag.select_timeout_count++;
    }
    return ret;
}

/**
 * @brief Get no block file descriptors
 */
int tkl_net_get_nonblock(const int fd)
{
    // --- BEGIN: user implements ---
    int idx = _tuya_fd_index(fd);

    if (idx < 0) {
        return -1;
    }

    return (g_tkl_nonblock_fds.placeholder[idx / 8] & (uint8_t)(1U << (idx & 7))) ? 1 : 0;
    // --- END: user implements ---
}

/**
 * @brief Set block flag for file descriptors
 */
OPERATE_RET tkl_net_set_block(const int fd, const BOOL_T block)
{
    // --- BEGIN: user implements ---
    unsigned long nonblock = block ? 0UL : 1UL;

    if (ioctlsocket(fd, FIONBIO, &nonblock) != 0) {
        return OPRT_COM_ERROR;
    }

    _tkl_track_nonblock_fd(fd, block ? FALSE : TRUE);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief Close file descriptors
 */
TUYA_ERRNO tkl_net_close(const int fd)
{
    // --- BEGIN: user implements ---
    int ret = close(fd);

    g_tkl_net_diag.last_close_fd = fd;
    g_tkl_net_diag.last_close_ret = ret;

    if (ret == 0) {
        _tkl_track_nonblock_fd(fd, FALSE);
        if (fd == g_tkl_net_diag.ap_tcp_server_fd) {
            g_tkl_net_diag.ap_tcp_server_fd = -1;
            g_tkl_net_diag.ap_tcp_server_port = 0;
            g_tkl_net_diag.ap_tcp_server_bind_ok = FALSE;
            g_tkl_net_diag.ap_tcp_server_listen_ok = FALSE;
        }
        Report("[TKL_NET] close fd=%d\r\n", fd);
        return 0;
    }
    Report("[TKL_NET] close fail fd=%d errno=%d\r\n", fd, errno);
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

    g_tkl_net_diag.last_socket_type = type;
    g_tkl_net_diag.last_socket_fd = fd;
    g_tkl_net_diag.last_socket_ret = fd;

    if (fd >= 0) {
        Report("[TKL_NET] socket type=%d fd=%d\r\n", type, fd);
    } else {
        Report("[TKL_NET] socket fail type=%d errno=%d\r\n", type, errno);
    }

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
    struct in_addr bind_ip;
    const char *bind_ip_str = NULL;
    int ret;

    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port);
    bind_addr.sin_addr.s_addr = addr;
    bind_ip.s_addr = addr;
    bind_ip_str = inet_ntoa(bind_ip);

    Report("[TKL_NET] bind fd=%d ip=%s raw=0x%08lx port=%u%s%s\r\n",
           fd,
           bind_ip_str ? bind_ip_str : "<null>",
           (unsigned long)addr,
           (unsigned)port,
           (addr == INADDR_ANY) ? " any=1" : "",
           (addr == htonl(INADDR_LOOPBACK)) ? " loopback=1" : "");

    ret = bind(fd, (struct sockaddr *)&bind_addr, sizeof(bind_addr));
    g_tkl_net_diag.last_bind_fd = fd;
    g_tkl_net_diag.last_bind_addr = addr;
    g_tkl_net_diag.last_bind_port = port;
    g_tkl_net_diag.last_bind_ret = ret;
    if (ret == 0) {
        Report("[TKL_NET] bind ok fd=%d ip=%s port=%u\r\n",
               fd, bind_ip_str ? bind_ip_str : "<null>", (unsigned)port);
        if (port == 6668U) {
            g_tkl_net_diag.ap_tcp_server_fd = fd;
            g_tkl_net_diag.ap_tcp_server_port = port;
            g_tkl_net_diag.ap_tcp_server_bind_ok = TRUE;
        }
        return 0;
    }
    Report("[TKL_NET] bind fail fd=%d ip=%s port=%u errno=%d\r\n",
           fd, bind_ip_str ? bind_ip_str : "<null>", (unsigned)port, errno);
    return errno;
    // --- END: user implements ---
}

/**
 * @brief Listen to network
 */
TUYA_ERRNO tkl_net_listen(const int fd, const int backlog)
{
    // --- BEGIN: user implements ---
    int ret = listen(fd, backlog);

    g_tkl_net_diag.last_listen_fd = fd;
    g_tkl_net_diag.last_listen_backlog = backlog;
    g_tkl_net_diag.last_listen_ret = ret;

    if (ret == 0) {
        Report("[TKL_NET] listen ok fd=%d backlog=%d\r\n", fd, backlog);
        if (fd == g_tkl_net_diag.ap_tcp_server_fd && g_tkl_net_diag.ap_tcp_server_port == 6668U) {
            g_tkl_net_diag.ap_tcp_server_listen_ok = TRUE;
        }
        return 0;
    }
    Report("[TKL_NET] listen fail fd=%d backlog=%d errno=%d\r\n", fd, backlog, errno);
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
    char peer_ip[20] = {0};
    int client_fd;

    Report("[TKL_NET] accept wait listen_fd=%d\r\n", fd);
    client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
    g_tkl_net_diag.last_accept_listen_fd = fd;
    g_tkl_net_diag.last_accept_ret = client_fd;

    if (client_fd >= 0) {
        if (addr) *addr = client_addr.sin_addr.s_addr;
        if (port) *port = ntohs(client_addr.sin_port);
        g_tkl_net_diag.last_accept_addr = client_addr.sin_addr.s_addr;
        g_tkl_net_diag.last_accept_port = ntohs(client_addr.sin_port);
        _tkl_format_ipv4(client_addr.sin_addr.s_addr, peer_ip, sizeof(peer_ip));
        Report("[TKL_NET] accept ok listen_fd=%d client_fd=%d ip=%s port=%u\r\n",
               fd, client_fd, peer_ip, (unsigned)ntohs(client_addr.sin_port));
        return client_fd; // Return the new FD
    }
    Report("[TKL_NET] accept fail listen_fd=%d errno=%d\r\n", fd, errno);
    return -1; // Standard error indication
    // --- END: user implements ---
}

/**
 * @brief Send data to network
 */
TUYA_ERRNO tkl_net_send(const int fd, const void *buf, const uint32_t nbytes)
{
    // --- BEGIN: user implements ---
    int ret = send(fd, buf, nbytes, 0);

    g_tkl_net_diag.last_send_fd = fd;
    g_tkl_net_diag.last_send_len = nbytes;
    g_tkl_net_diag.last_send_ret = ret;

    if (ret < 0) {
        Report("[TKL_NET] send fd=%d len=%lu ret=%d errno=%d\r\n",
               fd, (unsigned long)nbytes, ret, errno);
    } else {
        Report("[TKL_NET] send fd=%d len=%lu ret=%d\r\n",
               fd, (unsigned long)nbytes, ret);
    }

    return ret;
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
    TUYA_IP_ADDR_T send_addr = addr;
    char dst_ip[20] = {0};
    int ret;

    if ((addr == (TUYA_IP_ADDR_T)0xffffffffUL) && _tkl_ap_ipv4_ready()) {
        send_addr = TKL_TEST_AP_BROADCAST_ADDR;
        g_tkl_net_diag.ap_broadcast_translate_active = TRUE;
        _tkl_format_ipv4(addr, dst_ip, sizeof(dst_ip));
        Report("[TKL_NET] sendto translate fd=%d dst=%s -> 192.168.176.255 port=%u\r\n", fd, dst_ip, port);
    } else {
        g_tkl_net_diag.ap_broadcast_translate_active = FALSE;
    }

    memset(&to_addr, 0, sizeof(to_addr));
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(port);
    to_addr.sin_addr.s_addr = send_addr;

    ret = sendto(fd, buf, nbytes, 0, (struct sockaddr *)&to_addr, sizeof(to_addr));
    g_tkl_net_diag.last_sendto_fd = fd;
    g_tkl_net_diag.last_sendto_addr = send_addr;
    g_tkl_net_diag.last_sendto_port = port;
    g_tkl_net_diag.last_sendto_len = nbytes;
    g_tkl_net_diag.last_sendto_ret = ret;
    _tkl_format_ipv4(send_addr, dst_ip, sizeof(dst_ip));
    if (ret < 0) {
        Report("[TKL_NET] sendto fd=%d ip=%s raw=0x%08lx port=%u len=%lu ret=%d errno=%d\r\n",
               fd, dst_ip, (unsigned long)send_addr, port, (unsigned long)nbytes, ret, errno);
    } else {
        Report("[TKL_NET] sendto fd=%d ip=%s raw=0x%08lx port=%u len=%lu ret=%d\r\n",
               fd, dst_ip, (unsigned long)send_addr, port, (unsigned long)nbytes, ret);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Receive data from network
 */
TUYA_ERRNO tkl_net_recv(const int fd, void *buf, const uint32_t nbytes)
{
    // --- BEGIN: user implements ---
    int ret = recv(fd, buf, nbytes, 0);

    g_tkl_net_diag.last_recv_fd = fd;
    g_tkl_net_diag.last_recv_len = nbytes;
    g_tkl_net_diag.last_recv_ret = ret;

    if (ret < 0) {
        Report("[TKL_NET] recv fd=%d len=%lu ret=%d errno=%d\r\n",
               fd, (unsigned long)nbytes, ret, errno);
    } else {
        Report("[TKL_NET] recv fd=%d len=%lu ret=%d\r\n",
               fd, (unsigned long)nbytes, ret);
    }

    return ret;
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
    /*
     * The TI/LwIP socket layer may reject SO_REUSEADDR for listening sockets
     * used by Tuya AP provisioning. Reuse is an optimization here, not a hard
     * requirement, so keep the socket usable even when the option is unsupported.
     */
    return OPRT_OK;
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
        Report("[TKL_NET] set_broadcast ok fd=%d\r\n", fd);
        return OPRT_OK;
    }
    Report("[TKL_NET] set_broadcast fail fd=%d errno=%d\r\n", fd, errno);
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
