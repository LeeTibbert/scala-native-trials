#include "sys/socket_conversions.h"
#include <stdio.h> // DEBUG FIXME

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "netdb.h"
#include "netinet/in.h"

#if 0
int scalanative_getnameinfo(struct scalanative_sockaddr *addr,
                            socklen_t addrlen, char *host, socklen_t hostlen,
                            char *serv, socklen_t servlen, int flags) {

  // This OS interop section is not for the pure of heart or young
  // impressionable children.

  struct scalanative_sockaddr bsdAddr;
  memcpy(&bsdAddr, addr, sizeof(struct scalanative_sockaddr));
  addrlen = (addr->sa_family == AF_INET) ? 4 : 16; // since the dawn of time
  bsdAddr.sa_family = (addr->sa_family << 8) | addrlen;

  struct sockaddr *addrPtr = (struct sockaddr *) &bsdAddr;

    return getnameinfo(addrPtr, addrlen, host, hostlen, serv,
                             servlen, flags);
}

#else
 // Works on linux, REMEMBER FIX MADE Fri May 28, circa 10:30
int scalanative_getnameinfo(struct scalanative_sockaddr *addr,
                            socklen_t addrlen, char *host, socklen_t hostlen,
                            char *serv, socklen_t servlen, int flags) {

  // This OS interop section is not for the pure of heart or young,
  // impressionable children.

#if defined(__linux__) || defined(_WIN32)
  struct sockaddr *addrPtr = (struct sockaddr *) addr;
#elif defined(__APPLE__) || defined(__FreeBSD__)
  struct scalanative_sockaddr bsdAddr;
  memcpy(&bsdAddr, addr, sizeof(struct scalanative_sockaddr));
  addrlen = (addr->sa_family == AF_INET) ? 4 : 16; // since the dawn of time
  bsdAddr.sa_family = (addr->sa_family << 8) | addrlen;

  struct sockaddr *addrPtr = (struct sockaddr *) &bsdAddr;
#else
#error "Unsupported operating system."
#endif

    return getnameinfo(addrPtr, addrlen, host, hostlen, serv,
                             servlen, flags);
}
#endif

/*
int scalanative_getnameinfo(struct scalanative_sockaddr *addr,
                            socklen_t addrlen, char *host, socklen_t hostlen,
                            char *serv, socklen_t servlen, int flags) {
    struct sockaddr *converted_addr;
    scalanative_convert_sockaddr(addr, &converted_addr, &addrlen);
    int status = getnameinfo(converted_addr, addrlen, host, hostlen, serv,
                             servlen, flags);
    free(converted_addr);
    return status;
}
*/

void scalanative_convert_scalanative_addrinfo(struct scalanative_addrinfo *in,
                                              struct addrinfo *out) {
    // ai_addr and ai_next fields are set to NULL because this function is only
    // used for converting hints parameter for the getaddrinfo function, which
    // doesn't
    // care about them
    out->ai_flags = in->ai_flags;
    out->ai_family = in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    out->ai_addrlen = in->ai_addrlen;
    if (in->ai_canonname == NULL) {
        out->ai_canonname = NULL;
    } else {
        out->ai_canonname = strdup(in->ai_canonname);
    }
    out->ai_addr = NULL;
    out->ai_next = NULL;
}

/// 2021-05-28 11:55 -0400 LeeT FIXME -- Once I am finding the
///    macOS with wierd IPv6 address for localhost bug I am trying
///    to track down.
// 2021-05-28 11:55 -0400 LeeT FIXME -- I believe that levels of
//    simplifications are possible on known linux (perhaps not Windows)>
//    Level 1) one memcpy in one move.
//    On BSD/macOS, may not have to malloc, with __careful__ bookkeeping
//    may be able to use original, being expecially careful in
//    freeaddrinfo.  Perhaps another PR?

void scalanative_convert_addrinfo(struct addrinfo *in,
                                  struct scalanative_addrinfo *out);

void scalanative_convert_addrinfo_X2(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
    out->ai_flags = in->ai_flags;
    out->ai_family = in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    out->ai_addrlen = in->ai_addrlen;
    if (in->ai_addr == NULL) {
        out->ai_addr = NULL;
    } else {
        socklen_t size = in->ai_addrlen;

        void *addr = malloc(size);
	        memcpy(addr, in->ai_addr, size);
		//    scalanative_convert_scalanative_in_addr(in->ai_addr, addr);
        out->ai_addr = (struct scalanative_sockaddr *) addr;
    }
    if (in->ai_canonname == NULL) {
        out->ai_canonname = NULL;
    } else {
        out->ai_canonname = strdup(in->ai_canonname);
    }
    if (in->ai_next == NULL) {
        out->ai_next = NULL;
    } else {
        struct scalanative_addrinfo *next_native =
            malloc(sizeof(struct scalanative_addrinfo));
        scalanative_convert_addrinfo(in->ai_next, next_native);
        out->ai_next = next_native;
    }
}

void scalanative_convert_addrinfo_X1(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
    out->ai_flags = in->ai_flags;
    out->ai_family = in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    if (in->ai_addr == NULL) {
        out->ai_addr = NULL;
        out->ai_addrlen = in->ai_addrlen;
    } else {
        socklen_t size;
        if (in->ai_addr->sa_family == AF_INET) {
            struct scalanative_sockaddr_in *addr =
                malloc(sizeof(struct scalanative_sockaddr_in));
            scalanative_convert_scalanative_sockaddr_in(
                (struct sockaddr_in *)in->ai_addr, addr, &size);
            out->ai_addr = (struct scalanative_sockaddr *)addr;
        } else {
            struct scalanative_sockaddr_in6 *addr =
                malloc(sizeof(struct scalanative_sockaddr_in6));
            scalanative_convert_scalanative_sockaddr_in6(
                (struct sockaddr_in6 *)in->ai_addr, addr, &size);
            out->ai_addr = (struct scalanative_sockaddr *)addr;
        }
        out->ai_addrlen = size;
    }
    if (in->ai_canonname == NULL) {
        out->ai_canonname = NULL;
    } else {
        out->ai_canonname = strdup(in->ai_canonname);
    }
    if (in->ai_next == NULL) {
        out->ai_next = NULL;
    } else {
        struct scalanative_addrinfo *next_native =
            malloc(sizeof(struct scalanative_addrinfo));
        scalanative_convert_addrinfo(in->ai_next, next_native);
        out->ai_next = next_native;
    }
}

void scalanative_convert_addrinfo(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
  printf("\n------------- scalanative_convert_addrinfo: Begin\n");

  int size = sizeof(struct scalanative_addrinfo);

  struct scalanative_addrinfo *outX0 = malloc(size);
    scalanative_convert_addrinfo_X1(in, outX0);

  //  scalanative_convert_addrinfo_X1(in, out);


  struct scalanative_addrinfo *outX2 = malloc(size);

  //  scalanative_convert_addrinfo_X2(in, outX2);
  scalanative_convert_addrinfo_X2(in, out);


  printf("\n------------- sn_convert_addrinfo: out->ai_family |%d|\n",
	 (int) out->ai_family);
  printf("\n------------- sn_convert_addrinfo: outX0->ai_family |%d|\n",
	 (int) outX0->ai_family);

  //  out->ai_addr = outX0->ai_addr;

  printf("\n------------- sn_convert_addrinfo: out->ai_addrlen |%d|\n",
	 out->ai_addrlen);

  printf("\n------------- sn_convert_addrinfo: outX0->ai_addrlen |%d|\n",
	 outX0->ai_addrlen);

  //  out->ai_addrlen = outX0->ai_addrlen;


  // 2021-05-28 22:27 -0400 

  if (out->ai_flags != outX2->ai_flags) 
    printf("\nMismatch field %s: x1: |%d| x2: |%d|\n", "ai_flags",
	   out->ai_flags, outX2->ai_flags);

  if (out->ai_family != outX2->ai_family) 
    printf("\nMismatch field %s: x1: |%d| x2: |%d|\n", "ai_family",
	   out->ai_family, outX2->ai_family);
  
  if (out->ai_socktype != outX2->ai_socktype) 
    printf("\nMismatch field %s: x1: |%d| x2: |%d|\n", "ai_socktype",
	   out->ai_socktype, outX2->ai_socktype);

  if (out->ai_protocol != outX2->ai_protocol) 
    printf("\nMismatch field %s: x1: |%d| x2: |%d|\n", "ai_protocol",
	   out->ai_protocol, outX2->ai_protocol);

  if (out->ai_protocol != outX2->ai_protocol) 
    printf("\nMismatch field %s: x1: |%d| x2: |%d|\n", "ai_protocol",
	   out->ai_protocol, outX2->ai_protocol);

  if (out->ai_addrlen != outX2->ai_addrlen) 
    printf("\nMismatch field %s: x1: |%d| x2: |%d|\n", "ai_addrlen",
	   (int) out->ai_addrlen, (int) outX2->ai_addrlen);

  /* Since these are malloc'd output fields, they will alway mismatch.
  if (out->ai_addr != outX2->ai_addr) 
    printf("\nMismatch field %s: x1: |%p| x2: |%p|\n", "ai_addr",
	   out->ai_addr, outX2->ai_addr);
  */

  //  printf("\n???out->ai_canonname |%s|\n", out->ai_canonname);
  //  printf("\n???outX2->ai_canonname |%s|\n", outX2->ai_canonname);

  if (out->ai_canonname != outX2->ai_canonname) 
    printf("\nMismatch field %s: x1: |%s| x2: |%s|\n", "ai_canonname",
	   out->ai_canonname, outX2->ai_canonname);

  if (out->ai_next != outX2->ai_next) 
    printf("\nMismatch field %s: x1: |%p| x2: |%p|\n", "ai_next",
	   out->ai_next, outX2->ai_next);

  printf("\n------------- scalanative_convert_addrinfo: End\n");
}

#if 0
void scalanative_convert_addrinfo(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
    out->ai_flags = in->ai_flags;
    out->ai_family = in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    if (in->ai_addr == NULL) {
        out->ai_addr = NULL;
        out->ai_addrlen = in->ai_addrlen;
    } else {
        socklen_t size;
        if (in->ai_addr->sa_family == AF_INET) {
            struct scalanative_sockaddr_in *addr =
                malloc(sizeof(struct scalanative_sockaddr_in));
            scalanative_convert_scalanative_sockaddr_in(
                (struct sockaddr_in *)in->ai_addr, addr, &size);
            out->ai_addr = (struct scalanative_sockaddr *)addr;
        } else {
            struct scalanative_sockaddr_in6 *addr =
                malloc(sizeof(struct scalanative_sockaddr_in6));
            scalanative_convert_scalanative_sockaddr_in6(
                (struct sockaddr_in6 *)in->ai_addr, addr, &size);
            out->ai_addr = (struct scalanative_sockaddr *)addr;
        }
        out->ai_addrlen = size;
    }
    if (in->ai_canonname == NULL) {
        out->ai_canonname = NULL;
    } else {
        out->ai_canonname = strdup(in->ai_canonname);
    }
    if (in->ai_next == NULL) {
        out->ai_next = NULL;
    } else {
        struct scalanative_addrinfo *next_native =
            malloc(sizeof(struct scalanative_addrinfo));
        scalanative_convert_addrinfo(in->ai_next, next_native);
        out->ai_next = next_native;
    }
}

#elif 0

void scalanative_convert_addrinfo(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
    out->ai_flags = in->ai_flags;
    out->ai_family = in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    if (in->ai_addr == NULL) {
        out->ai_addr = NULL;
        out->ai_addrlen = in->ai_addrlen;
    } else {
        socklen_t size = (in->ai_addr->sa_family == AF_INET)
                             ? sizeof(struct scalanative_sockaddr_in)
                             : sizeof(struct scalanative_sockaddr_in6);

	/*
        socklen_t FIXME = (in->ai_addr->sa_family == AF_INET)
                             ? 4
                             : 16;
	*/

        void *addr = malloc(size);
        memcpy(addr, in->ai_addr, size);
        out->ai_addr = (struct scalanative_sockaddr *)addr;

	out->ai_addrlen = size;

	//        out->ai_addrlen = FIXME; // Broken

    }
    if (in->ai_canonname == NULL) {
        out->ai_canonname = NULL;
    } else {
        out->ai_canonname = strdup(in->ai_canonname);
    }
    if (in->ai_next == NULL) {
        out->ai_next = NULL;
    } else {
        struct scalanative_addrinfo *next_native =
            malloc(sizeof(struct scalanative_addrinfo));
        scalanative_convert_addrinfo(in->ai_next, next_native);
        out->ai_next = next_native;
    }
}

#elif 0
void scalanative_convert_addrinfo(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
    out->ai_flags = in->ai_flags;
    out->ai_family = in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    out->ai_addrlen = in->ai_addrlen;
    if (in->ai_addr == NULL) {
        out->ai_addr = NULL;
    } else {
        socklen_t size = in->ai_addrlen;

        void *addr = malloc(size);
        memcpy(addr, in->ai_addr, size);
        out->ai_addr = (struct scalanative_sockaddr *) addr;
    }
    if (in->ai_canonname == NULL) {
        out->ai_canonname = NULL;
    } else {
        out->ai_canonname = strdup(in->ai_canonname);
    }
    if (in->ai_next == NULL) {
        out->ai_next = NULL;
    } else {
        struct scalanative_addrinfo *next_native =
            malloc(sizeof(struct scalanative_addrinfo));
        scalanative_convert_addrinfo(in->ai_next, next_native);
        out->ai_next = next_native;
    }
}
#else
#endif

void scalanative_freeaddrinfo(struct scalanative_addrinfo *addr) {
    if (addr != NULL) {
        free(addr->ai_canonname);
        free(addr->ai_addr);
        scalanative_freeaddrinfo((struct scalanative_addrinfo *)addr->ai_next);
        free(addr);
    }
}

int scalanative_getaddrinfo(char *name, char *service,
                            struct scalanative_addrinfo *hints,
                            struct scalanative_addrinfo **res) {
    struct addrinfo hints_c;
    struct addrinfo *res_c;
    scalanative_convert_scalanative_addrinfo(hints, &hints_c);
    int status = getaddrinfo(name, service, &hints_c, &res_c);
    free(hints_c.ai_canonname);
    if (status != 0) {
        return status;
    }
    struct scalanative_addrinfo *res_native =
        malloc(sizeof(struct scalanative_addrinfo));
    scalanative_convert_addrinfo(res_c, res_native);
    freeaddrinfo(res_c);
    *res = res_native;
    return status;
}

int scalanative_ai_numerichost() { return AI_NUMERICHOST; }

int scalanative_ai_passive() { return AI_PASSIVE; }

int scalanative_ai_numericserv() { return AI_NUMERICSERV; }

int scalanative_ai_addrconfig() { return AI_ADDRCONFIG; }

int scalanative_ai_v4mapped() { return AI_V4MAPPED; }

int scalanative_ai_canonname() { return AI_CANONNAME; }
