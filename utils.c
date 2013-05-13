/*
 * Zadanie programistyczne numer 2 z sieci komputerowych
 * PROGRAM `CLIENT-UDP`
 *
 * Łukasz Hanuszczak
 * ostatnia aktualizacja: 12 maja 2013
 */

#include "utils.h"

/**
 * Obsługuje błędy wywalane przez gniazda i inne funkcje systemowe
 * (taka obsługa wyjątkół dla ubogich).
 *
 * @param desc Opis miejsca/sytuacji, która jest obsługiwana.
 * @param retv Wartość którą wyrzuciła funkcja.
 * @return Wartość którą wyrzuciła funkcja (do ewentualnej dalszej obsługi).
 */
int handle(const char *desc, const int retv)
{
	if (retv < 0) {
		fprintf(stderr, "%3d - %s (%s)\n", errno, strerror(errno), desc);
		exit(1);
	}

	return retv;
}

static const char sym[] = { ' ', '.', 'o', '#' }; /* Postęp kawałka paska. */
static uint cnt[BAR_WIDTH]; /* Ilość fragmentów w kawałku paska. */

/**
 * Wyświetla ładny pasek postępu.
 *
 * @param recvmap Mapa otrzymanych już fragmentów pliku.
 * @param pcount Łączna liczba fragmentów pliku.
 */
void prgbar(const bool *recvmap, const size_t pcount)
{
	/* Traktowane jak specjalny przypadek aby się nie babrać. */
	if (pcount < BAR_WIDTH) {
		size_t ccount = 0; /* Ilość odebranych fragmentów. */
		printf("[");
		for (uint i = 0; i < pcount; i++) {
			printf("%c", sym[recvmap[i] ? (sizeof(sym) - 1) : 0]);
			ccount += recvmap[i] ? 1 : 0;
		}
		printf("] %6.2f%%\r", 100.0f * (float)ccount/(float)pcount);	
		fflush(stdout);

		return;
	}

	const float ppf = (float)pcount / (float)BAR_WIDTH; /* Ile w kawałku? */

	memset(cnt, 0, BAR_WIDTH * sizeof(uint));	
	size_t ccount = 0; /* Ilość odebranych fragmentów. */
	for (uint i = 0; i < pcount; i++) {
		if (recvmap[i]) {
			cnt[(uint)(i / ppf)]++;
			ccount++;
		}
	}

	/* Drukujemy paskek. */
	printf("[");
	for (uint i = 0; i < BAR_WIDTH; i++) {
		printf("%c", sym[(uint)(cnt[i] * (sizeof(sym) - 1) / ppf)]);
	}
	printf("] %6.2f%%\r", 100.0f * (float)ccount/(float)pcount);
	fflush(stdout);
}

static void phelp()
{
	printf(
		"Usage:\n"
		"  client-udp [-t timeout ] [ -p psize ] [ -a addr ] port file size\n"
		"Options:\n"
		"  -t timeout  | --timeout timeout\n"
		"    Set maximum timeout for response in miliseconds (default: 100).\n"
		"  -p psize    | --psize pize\n"
		"    Set file part (fragment) size (default: 1000).\n"
		"  -a addr     | --address addr\n"
		"    Set server address.\n"
	);
}

#define OPTARG(v, short, long) (strcmp(v, short) == 0 || strcmp(v, long) == 0)

/* Parsuje pojedyńczy opcjonalny argument, zwraca wartość `scanf`.*/
static int parseopt(
	const char *arg,
	const char *val,
	struct params *result
)
{
	int retv = 0;
	if (OPTARG(arg, "-t", "--timeout")) {
		uint tmp;
		retv = sscanf(val, "%u", &tmp);
		result->ptime = tmp;
	} else if (OPTARG(arg, "-p", "--psize")) {
		uint tmp;
		retv = sscanf(val, "%u", &tmp);

		/* Truncate fragment length if it is too big. */
		if (tmp >= MAX_FRAG_LEN) {
			result->psize = MAX_FRAG_LEN;
		} else {
			result->psize = tmp;
		}
	} else if (OPTARG(arg, "-a", "--address")) {
		retv = handle(
			"parse ip adress",
			inet_pton(AF_INET, val, &result->addr.sin_addr)
		);
	}

	return retv;
}

/**
 * Parsuje i zwraca argumenty wywołania,
 * w przypadku braku któregokolwiek zwraca wartości domyślne.
 *
 * @param argv Argumenty wywołania (bez nazwy programu!).
 * @param argc Ilość argumentów.
 * @return Ładna struktura z zapakowanymi wszystkimi potrzebnymi danymi.
 */
struct params parseargs(char **argv, const size_t argc)
{
	struct params result = { NULL, 1000, { 0 }, 100, 1000 };
	
	/* Wartości domyślne adresu. */
	memset(&result.addr, 0, sizeof(result.addr));
	result.addr.sin_family = AF_INET;
	inet_pton(AF_INET, "156.17.4.30", &result.addr.sin_addr);

	/* Parsowanie parametrów opcjonalnych. */
	uint i = 0;
	for (; i < argc; i++) {
		int retv = 0;

		if (OPTARG(argv[i], "-h", "--help")) {
			phelp();
			exit(1);
		} else if (argv[i][0] == '-' && i < argc - 1) {
			retv = parseopt(argv[i], argv[i + 1], &result);
			i++;
		} else {
			break;
		}

		if (retv <= 0) {
			fprintf(stderr, "Incorrect argument: %s.\n", argv[i]);
			fprintf(stderr, "Try with `--help` for more information.\n");
			exit(1);
		}
	}

	/* Parsowanie numeru portu. */
	uint16_t port;
	if (i < argc && sscanf(argv[i], "%hu", &port) > 0) {
		result.addr.sin_port = htons(port);
		i++;
	} else {
		fprintf(stderr, "Missing or incorrect port number.\n");
		exit(1);
	}

	/* Parsowanie nazwy pliku wyjściowego. */
	if (i < argc) {
		result.fname = argv[i];
		i++;
	} else {
		fprintf(stderr, "Missing output filename.\n");
		exit(1);
	}

	/* Parsowanie rozmiaru pliku do ściągnięcia. */
	if (!(i < argc && sscanf(argv[i], "%zu", &result.fsize))) {
		fprintf(stderr, "Missing or incorrect file size.\n");
		exit(1);
	}

	return result;	
}
