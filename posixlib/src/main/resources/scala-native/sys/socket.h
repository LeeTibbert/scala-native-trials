#ifndef __SYS_SOCKET_H
#define __SYS_SOCKET_H

// 2021-05-31 09:36 -0400 LeeT FIXME -- This needs _Static_assert/s
//   to ensure sn_sockaddr == sockaddr cndition used in netdb.c.


typedef unsigned short scalanative_sa_family_t;

struct scalanative_sockaddr {
    scalanative_sa_family_t sa_family;
    char sa_data[14];
};

// Not Posix compliant. BEWARE using with memcpy() & friends.
struct scalanative_sockaddr_storage {
    scalanative_sa_family_t ss_family;
};

struct scalanative_linger {
    int l_onoff;
    int l_linger;
};
#endif // __SYS_SOCKET_H
