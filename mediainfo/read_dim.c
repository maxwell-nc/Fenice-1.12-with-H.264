
#include <unistd.h>

#include <fenice/utils.h>
#include <fenice/log.h>

int read_dim(int file, long int *dim)
{
	unsigned char byte1, byte2, byte3, byte4;
	int n, res;
	double skip = 0.0;

	if ((n = read(file, &byte1, 1)) != 1) {
		ERRORLOGG("Errore durante la lettura della dimensione del tag ID3  al byte1");
		return ERR_PARSE;
	}
	res = calculate_skip((int) byte1, &skip, 24);
	if ((n = read(file, &byte2, 1)) != 1) {
		ERRORLOGG("Errore durante la lettura della dimensione del tag ID3 al byte2");
		return ERR_PARSE;
	}
	res = calculate_skip((int) byte2, &skip, 16);
	if ((n = read(file, &byte3, 1)) != 1) {
		ERRORLOGG("Errore durante la lettura della dimensione del tag ID3 al byte3");
		return ERR_PARSE;
	}
	res = calculate_skip((int) byte3, &skip, 8);

	if ((n = read(file, &byte4, 1)) != 1) {
		ERRORLOGG("Errore durante la lettura della dimensione del tag ID3 al byte4");
		return ERR_PARSE;
	}
	res = calculate_skip((int) byte4, &skip, 0);
	*dim = (long int) skip;

	return ERR_NOERROR;
}
