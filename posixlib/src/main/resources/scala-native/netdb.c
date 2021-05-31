#include <stdio.h> // DEBUG FIXME

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "netdb.h"
#include "netinet/in.h"

  // 2021-05-30 16:13 -0400 LeeT Design decision - Punt _WIN32, not knowing.
  // #if defined(__linux__) || defined(_WIN32)

#if defined(NO__linux__) // FIXME - comment so I can smoke test bsd on linux

// #if defined(__linux__)

// 2021-05-30 19:47 -0400 LeeT FIXME - add gai_strerror, I have it somewhere
//    in my mess (of debugging). Also add to netdb.scala.

void scalanative_freeaddrinfo(struct scalanative_addrinfo *addr) {
  printf("=== My scalanative_freeaddrinfo: Direct Call\n");

  freeaddrinfo((struct addrinfo *) addr);
}

int scalanative_getaddrinfo(char *name, char *service,
                            struct scalanative_addrinfo *hints,
                            struct scalanative_addrinfo **res) {

  printf("=== My scalanative_getaddrinfo: Direct Call\n");

  return getaddrinfo(name, service, (struct addrinfo *) hints,
				     (struct addrinfo **) res);
  }

int scalanative_getnameinfo(struct scalanative_sockaddr *addr,
                            socklen_t addrlen, char *host, socklen_t hostlen,
                            char *serv, socklen_t servlen, int flags) {

  printf("=== My scalanative_nameinfo: Direct Call\n");

  return getnameinfo((struct sockaddr *) addr, addrlen, host, hostlen, serv,
                             servlen, flags);
}

// #elif defined(no__linux__)
#elif defined(__linux__)

// #elif defined(__APPLE__) || defined(__FreeBSD__)

// 2021-05-30 13:11 -0400 LeeT FIXME!!! Tidy for GitHub submission.
 // Works on linux, REMEMBER *DOCUMENT * FIX MADE Fri May 28, circa 10:30
int scalanative_getnameinfo(struct scalanative_sockaddr *addr,
                            socklen_t addrlen, char *host, socklen_t hostlen,
                            char *serv, socklen_t servlen, int flags) {

  struct scalanative_sockaddr bsdAddr;
  memcpy(&bsdAddr, addr, sizeof(struct scalanative_sockaddr));
  addrlen = (addr->sa_family == AF_INET) ? 4 : 16; // since the dawn of time
  bsdAddr.sa_family = (addr->sa_family << 8) | addrlen;

  return getnameinfo((struct sockaddr *) &bsdAddr, addrlen,
		     host, hostlen,
		     serv, servlen,
		     flags);
}

void scalanative_convert_scalanative_addrinfo(struct scalanative_addrinfo *in,
                                              struct addrinfo *out) {
    // ai_addr and ai_next fields are set to NULL because this function is only
    // used for converting hints parameter for the getaddrinfo function, which
    // doesn't care about them
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

// Perhaps a better name
// static void fill_sn_sockaddr_in(

static void sn_convert_sn_sockaddr_in(
    struct sockaddr_in *in, struct scalanative_sockaddr_in *out) {
  //    out->sin_port = in->sin_port;

    //    sn_convert_sn_in_addr(&(in->sin_addr), &(out->sin_addr));
    // 2021-05-30 14:53 -0400 FIXME netinet/in.h defines so_adder which
    //    should be s_addr.

  //    out->sin_addr.so_addr = in->sin_addr.s_addr;

    // 2021-05-30 15:08 -0400 LeeT FIXME -- make sure there is
    // a/an _Static_assert() in netinet/in.h to enforce that two
    // sizes are the same.
    memcpy(out, in, sizeof(struct scalanative_sockaddr_in));

    out->sin_family = in->sin_family; // also zeros _sa_len, where present.
}

// 2021-05-30 20:37 -0400 LeeT FIXME -- this is a quick hack, fix it
static void sn_convert_sn_sockaddr_in6(
    struct sockaddr_in6 *in, struct scalanative_sockaddr_in6 *out) {
  printf(",,, sn_convert_sn_sockaddr_in6 memcpy\n");
    memcpy(out, in, sizeof(struct scalanative_sockaddr_in6));

    out->sin6_family = in->sin6_family; // also zeros _sa_len, where present.
}

// 2021-05-30 20:41 -0400 LeeT FIXME -- Delete this forward declaration
//     when renames are done

void scalanative_convert_addrinfo(struct addrinfo *in,
                                  struct scalanative_addrinfo *out);

void scalanative_convert_addrinfo_X3(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
    out->ai_flags = in->ai_flags;
    out->ai_family =  in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    if (in->ai_addr == NULL) {
        out->ai_addr = NULL;
    } else {
        if (in->ai_addr->sa_family == AF_INET) {
	  //2021-05-30 14:03 -0400 LeeT fixme -- see if can zero only _sin_zero
            struct scalanative_sockaddr_in *addr =
	      malloc(sizeof(struct scalanative_sockaddr_in));

	    // scalanative_convert_scalanative_sockaddr_in(
	    //   (struct sockaddr_in *)in->ai_addr, addr, &size);

	    sn_convert_sn_sockaddr_in(
	                    (struct sockaddr_in *)in->ai_addr, addr);
            out->ai_addr = (struct scalanative_sockaddr *) addr;
        } else {
	  //	  socklen_t UNUSEDsize; // LeeT FIXME Once I have IPv4 working.

            struct scalanative_sockaddr_in6 *addr =
	      malloc(sizeof(struct scalanative_sockaddr_in6));
	    //            scalanative_convert_scalanative_sockaddr_in6(
	    //                (struct sockaddr_in6 *)in->ai_addr, addr, &UNUSEDsize);
	    sn_convert_sn_sockaddr_in6(
	                    (struct sockaddr_in6 *)in->ai_addr, addr);
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

  printf(
    "---- My scalanative_convert_addrinfo: X3 using Magic \n");
  scalanative_convert_addrinfo_X3(in, out);

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
#else
#error "Unsupported operating system."
#endif

int scalanative_ai_numerichost() { return AI_NUMERICHOST; }

int scalanative_ai_passive() { return AI_PASSIVE; }

int scalanative_ai_numericserv() { return AI_NUMERICSERV; }

int scalanative_ai_addrconfig() { return AI_ADDRCONFIG; }

int scalanative_ai_v4mapped() { return AI_V4MAPPED; }

int scalanative_ai_canonname() { return AI_CANONNAME; }
