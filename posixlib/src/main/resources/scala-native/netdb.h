#ifdef _WIN32
#include <WinSock2.h>
#define strdup(arg1) _strdup(arg1);
#else
#include <netdb.h>
#endif
// #include "sys/socket_conversions.h"

#include "sys/socket.h"

// 2021-05-28 11:30 -0400 LeeT FIXME -- add comment about socklen_t
// linux vs BSD

struct scalanative_addrinfo {
    int ai_flags;                         /* Input flags.  */
    int ai_family;                        /* Protocol family for socket.  */
    int ai_socktype;                      /* Socket type.  */
    int ai_protocol;                      /* Protocol for socket.  */
    socklen_t ai_addrlen;                 /* Length of socket address.  */
    struct scalanative_sockaddr *ai_addr; /* Socket address for socket.  */
    char *ai_canonname; /* Canonical name for service location.  */
    void *ai_next;      /* Pointer to next in list.  */
};
