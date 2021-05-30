#include "sys/socket_conversions.h"
#include <stdio.h> // DEBUG FIXME

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "netdb.h"
#include "netinet/in.h"

// 2021-05-30 13:11 -0400 LeeT FIXME!!! Tidy for GitHub submission.
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
#endif // original getaddrinfo

// Forward declaration for original code (perhaps others)
void scalanative_convert_addrinfo(struct addrinfo *in,
                                  struct scalanative_addrinfo *out);

// The original code

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

static void sn_convert_sn_sockaddr_in(
    struct sockaddr_in *in, struct scalanative_sockaddr_in *out) {
    out->sin_family = in->sin_family;
    out->sin_port = in->sin_port;
    scalanative_convert_scalanative_in_addr(&(in->sin_addr), &(out->sin_addr));
}

// Testbed for alternate conversion implementations.  Where is the glitch?
void scalanative_convert_addrinfo_X2(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
    out->ai_flags = in->ai_flags;
    out->ai_family = in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    if (in->ai_addr == NULL) {
        out->ai_addr = NULL;
	//        out->ai_addrlen = in->ai_addrlen;
    } else {
        if (in->ai_addr->sa_family == AF_INET) {
	  //2021-05-30 14:03 -0400 LeeT fixme -- see if can zero only _sin_zero
            struct scalanative_sockaddr_in *addr =
	      calloc(1, sizeof(struct scalanative_sockaddr_in));

	    // scalanative_convert_scalanative_sockaddr_in(
	    //   (struct sockaddr_in *)in->ai_addr, addr, &size);

	    sn_convert_sn_sockaddr_in(
	                    (struct sockaddr_in *)in->ai_addr, addr);
            out->ai_addr = (struct scalanative_sockaddr *)addr;
        } else {
	  socklen_t UNUSEDsize; // LeeT FIXME Once I have IPv4 working.

            struct scalanative_sockaddr_in6 *addr =
	      malloc(sizeof(struct scalanative_sockaddr_in6));
            scalanative_convert_scalanative_sockaddr_in6(
                (struct sockaddr_in6 *)in->ai_addr, addr, &UNUSEDsize);
            out->ai_addr = (struct scalanative_sockaddr *)addr;
        }
    }

    out->ai_addrlen = in->ai_addrlen;

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
  printf("---= My scalanative_convert_addrinfo: Begin\n");

  //  printf("\n--- My scalanative_convert_addrinfo: X1 Using original code\n");
  //  scalanative_convert_addrinfo_X1(in, out);

  printf(
    "---- My scalanative_convert_addrinfo: X2 Using infile original code \n");
  scalanative_convert_addrinfo_X2(in, out);

  printf("---= MY scalanative_convert_addrinfo: End\n\n");
}

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
