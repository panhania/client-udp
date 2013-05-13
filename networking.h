/*
 * Zadanie programistyczne numer 2 z sieci komputerowych
 * PROGRAM `CLIENT-UDP`
 *
 * Łukasz Hanuszczak
 * ostatnia aktualizacja: 11 maja 2013
 */

#ifndef __NETWORKING_H
#define __NETWORKING_H

#define _GNU_SOURCE

/* Biblioteka standardowa. */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* Biblioteka systemowa. */
#include <sys/types.h>
#include <sys/socket.h>

/* Biblioteki sieciowe. */
#include <arpa/inet.h>

#include "utils.h"

#define MAX_DGRAM_LEN 65535

#define PACK_NONE 0
#define PACK_UNKNOWN 1
#define PACK_OK 2

void reqpart(
	const int sock,
	const struct sockaddr_in addr,
	const size_t poffset,
	const size_t plen
);

size_t reqfile(
	const int sock,
	const struct sockaddr_in addr,
	const bool *recvmap,
	const size_t pcount, const size_t psize
);

int recvpart(
	const int sock,
	const struct sockaddr_in addr,
	char *buffer,
	size_t *poffset, size_t *plen	
);

void recvfile(
	const int sock,
	const struct sockaddr_in addr,
	bool *recvmap,
	const size_t pcount, const size_t psize, const time_t ptime,	
	char *buffer
);

#endif
