/*
 * Zadanie programistyczne numer 2 z sieci komputerowych
 * PROGRAM `CLIENT-UDP`
 *
 * Łukasz Hanuszczak
 * ostatnia aktualizacja: 11 maja 2013
 */

/* Biblioteka standardowa. */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Biblioteki sieciowe. */
#include <sys/types.h>
#include <sys/socket.h>

#include "networking.h"
#include "utils.h"

/**
 * Serce programu: pobiera plik zgodnie z zadanymi parametrami.
 *
 * @param sock Gnizado przez które będzie odbywałą się komunikacja z serwerem.
 * @param prms Parametry programu (parsowane przy pomocy `parseargs`). 
 */
void dload(const int sock, const struct params prms)
{
	/* Ilość części na które zosanie podzielony plik. */
	const size_t pcount =
		prms.fsize / prms.psize + (prms.fsize % prms.psize != 0 ? 1 : 0);

	/* Bufor pobieranego pliku. */
	char *buffer = malloc(pcount * prms.psize);
	
	/* Czy dana część została już pobrana? */
	bool *recvmap = malloc(pcount * sizeof(bool));
	memset(recvmap, false, pcount * sizeof(bool));

	printf("Downloading...\n");
	fflush(stdout);

	/* Dopóki plik nie jest kompletny, wyślij zapytania i odbierz części. */
	while (reqfile(sock, prms.addr, recvmap, pcount, prms.psize) > 0) {
		recvfile(
			sock, prms.addr,
			recvmap, pcount, prms.psize, prms.ptime,
			buffer
		);
		prgbar(recvmap, pcount);
	}
	
	/* Pozostało tylko zapisać ściągnięty plik. */
	FILE *file = fopen(prms.fname, "w");	
	if (file != NULL) {
		fwrite(buffer, sizeof(char), prms.fsize, file);
		fclose(file);
		printf("\nFile successfully saved to %s.\n", prms.fname);
	} else {
		printf("\nSaving to %s has failed.\n", prms.fname);
	}

	free(recvmap);
	free(buffer);
}

int main(int argc, char **argv)
{
	int sock = handle(
		"create socket",
		socket(AF_INET, SOCK_DGRAM, 0)
	);

	struct params prms = parseargs(argv + 1, argc - 1);
	dload(sock, prms);

	return 0;
}
