
#include <fenice/mediainfo.h>

int calculate_skip(int byte_value, double *skip, int peso)
{
	int pos, i, resto = 1;
	double partial, potenza;
	int quoziente;
	int numero;
	int flag;
	if (peso == 0) {
		*skip += byte_value;
		return 0;
	} else {
		pos = 0;
		partial = 0.0;
		quoziente = byte_value / 2;

		resto = byte_value % 2;
		flag = 0;
		numero = quoziente;
		potenza = 1;
		while (quoziente != 0) {
			flag = 1;
			if (resto != 0) {
				for (i = 1; i <= pos + peso; i++)
					potenza = 2 * potenza;
			}
			partial += (resto) * potenza;
			quoziente = numero / 2;
			resto = numero % 2;
			numero = quoziente;
			pos++;
		}
		if (resto != 0) {
			for (i = 1; i <= pos + peso; i++)
				potenza = 2 * potenza;
		}
		partial += (resto) * potenza;
		*skip += partial;
		return 0;
	}
}
