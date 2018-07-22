#include <stdio.h>

int		average(int *mesure, unsigned int size)
{
	int		i;
	int		res;

	i = 0;
	res = 0;
	while (i < size)
		res = res + mesure[i++];
	res = res / size;
	return (res);
}
