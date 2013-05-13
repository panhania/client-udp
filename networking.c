/*
 * Zadanie programistyczne numer 2 z sieci komputerowych
 * PROGRAM `CLIENT-UDP`
 *
 * Łukasz Hanuszczak
 * ostatnia aktualizacja: 11 maja 2013
 */

#include "networking.h"

static char tmpbuff[MAX_DGRAM_LEN]; /* Bufor pomocniczy. */

/**
 * Wysyła pojedyncze zapytanie GET o zadanych parametrach.
 *
 * @param sock Gniazdo przez które będzie słane zapytanie.
 * @param addr Adres serwera z którego pobierany jest plik.
 * @param poffset Początek pobieranego fragmentu pliku.
 * @param plen Długość pobieranego fragmentu pliku.
 */
void reqpart(
	const int sock,
	const struct sockaddr_in addr,
	const size_t poffset,
	const size_t plen
)
{
	sprintf(tmpbuff, "GET %zu %zu\n", poffset, plen);
	const struct sockaddr *ptr = (struct sockaddr *)&addr;
	handle(
		"send GET request to the server",
		sendto(sock, tmpbuff, strlen(tmpbuff), 0, ptr, sizeof(addr))
	);
}

/**
 * Wysyła zapytania o wszystkie jeszcze niepobrane fragmenty pliku.
 *
 * @param sock Gnizado przez które słane będą zapytania.
 * @param addr Adres serwera z którego pobierany jest plik.
 * @param recvmap Mapa otrzymanych już fragmentów pliku.
 * @param pcount Ilość części pliku.
 * @param psize Rozmiar pojedynczej części pliku.
 * @return Ilość wysłanych zapytań (ilość jeszcze niepobranych części).
 */
size_t reqfile(
	const int sock,
	const struct sockaddr_in addr,
	const bool *recvmap,
	const size_t pcount, const size_t psize
)
{
	size_t pleft = 0;

	for (uint i = 0; i < pcount; i++) {
		if (!recvmap[i]) {
			pleft++;
			reqpart(sock, addr, i * psize, psize);
		}
	}

	return pleft;
}

/**
 * Ściąga z gniazda pojedynczy pakiet. Zwraca jedną z trzech wartości:
 * - PACK_NONE jeżeli na wskazane gniazdo nie przyszedł żaden pakiet.
 * - PACK_UNKNOWN jażeli odebrany pakiet nie był odpowiedzią na zapytanie.
 * - PACK_OK jeżeli udało się odebrać fragment pliku.
 * Jeżeli odbieranie się powiodło, parametry `poffset` i `plen` dostają
 * wartości odpowiadające początkowi i długości odebranego fragmentu pliku.
 *
 * @param sock Gnizado z którego ściągane są odpowiedzi.
 * @param addr Adres serwera z którego oczekiwana jest odpowiedź.
 * @param buffer Bufor pliku do którego zapisany zostanie fragment.
 * @param poffset Początek odebranej części pliku.
 * @param plen Długość odebranej części pliku.
 * @return Jedna z opisanych wyżej wartości.
 */
int recvpart(
	const int sock,
	const struct sockaddr_in addr,
	char *buffer,
	size_t *poffset, size_t *plen	
)
{
	/*
	 * Adres z którego przyszła odpowiedź. `recvfrom` chce jeszcze długość
	 * tego adresu, którą będzie "przycinać" jeżeli adres jest krótszy. Nie do
	 * końca rozumiem w sumie czemu, ale niech im będzie.
	 */
	struct sockaddr_in raddr;
	socklen_t rlen = sizeof(raddr);

	int retv = recvfrom(
		sock,
		tmpbuff, MAX_DGRAM_LEN,
		MSG_DONTWAIT,
		(struct sockaddr *)&raddr, &rlen
	);
	if (retv < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
		return PACK_NONE;
	} else {
		handle("recive requested packet from the server", retv);
	}

	/* Adres i numer portu odpowiedzi muszą się zgadzać z tym na co ślemy. */
	if (
		raddr.sin_port != addr.sin_port
		||
		raddr.sin_addr.s_addr != addr.sin_addr.s_addr
	) {
		return PACK_UNKNOWN;
	}

	int hlen; /* Długość nagłówka (żeby wiedzieć gdzie właściwy bufor). */
	sscanf(tmpbuff, "DATA %zu %zu%n", poffset, plen, &hlen);

	memcpy(buffer + *poffset, tmpbuff + hlen + 1, *plen);

	return PACK_OK;
}

/**
 * Odbiera fragmenty pliku oczekując maksymalnie `ptime` milisekund.
 *
 * @param sock Gnizado z którego ściągane są odpowiedzi.
 * @param addr Adres serwera z którego oczekiwana jest odpowiedź.
 * @param recvmap Mapa otrzymanych już fragmentów pliku.
 * @param pcount Ilość części pliku.
 * @param psize Rozmiar pojedynczej części pliku.
 * @param ptime Maksymalny czas oczekiwania na pakiet.
 * @param buffer Bufor pliku do którego zapisany zostaną fragmenty.
 */
void recvfile(
	const int sock,
	const struct sockaddr_in addr,
	bool *recvmap,
	const size_t pcount, const size_t psize, const time_t ptime,	
	char *buffer
)
{
	struct timespec st, ct;
	clock_gettime(CLOCK_MONOTONIC, &st);

	time_t dtms = 0;
	while (dtms < ptime) {
		size_t poffset, plen;

		if (recvpart(sock, addr, buffer, &poffset, &plen) == PACK_OK) {
			recvmap[poffset / psize] = true;
		}
		
		clock_gettime(CLOCK_MONOTONIC, &ct);
		struct timespec dt;
		dt.tv_sec = ct.tv_sec - st.tv_sec;
		dt.tv_nsec = ct.tv_nsec - st.tv_nsec;
		dtms = dt.tv_sec * 1000 + dt.tv_nsec / 100000;
	}
}
