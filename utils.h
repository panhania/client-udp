/*
 * Zadanie programistyczne numer 2 z sieci komputerowych
 * PROGRAM `CLIENT-UDP`
 *
 * Łukasz Hanuszczak
 * ostatnia aktualizacja: 11 maja 2013
 */

#ifndef __UTILS_H
#define __UTILS_H

/* Biblioteka standardowa. */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/* Biblioteka systemowa. */
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <getopt.h>

/* Biblioteki sieciowe. */
#include <arpa/inet.h>

#define MAX_FRAG_LEN 1000
#define BAR_WIDTH 50

typedef unsigned int uint;

int handle(
	const char *desc,
	const int retv
);

void prgbar(
	const bool *recvmap,
	const size_t pcount
);

struct params {
	/* Parametry obowiązkowe. */
	char *fname;
	size_t fsize;
	/* Parametry opcjonalne. */
	struct sockaddr_in addr; /* Adres serwera (domyślnie: aisd.ii.uni.wroc.pl). */
	time_t ptime; /* Czas oczekiwania na przyjście części. */
	size_t psize; /* Rozmiar pojedynczej części. */
};

struct params parseargs(
	char **argv,
	const size_t argc
);

#endif
