#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "netdb.h"
#include "netinet/in.h"

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

void scalanative_convert_addrinfo(struct addrinfo *in,
                                  struct scalanative_addrinfo *out) {
    out->ai_flags = in->ai_flags;
    out->ai_family = in->ai_family;
    out->ai_socktype = in->ai_socktype;
    out->ai_protocol = in->ai_protocol;
    if (in->ai_addr == NULL) {
        out->ai_addr = NULL;
        out->ai_addrlen = 0;
    } else {
        socklen_t size = (in->ai_addr->sa_family == AF_INET)
                             ? sizeof(struct scalanative_sockaddr_in)
                             : sizeof(struct scalanative_sockaddr_in6);

        void *addr = malloc(size);
        memcpy(addr, in->ai_addr, size);
        out->ai_addr = (struct scalanative_sockaddr *)addr;

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
