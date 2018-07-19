#include "pulse.h"
#include <pigpio.h>
#include <stdlib.h>
#include <stdio.h>
/*Send the start signal*/

int			rec_one_bit(int gpioclk, int gpiodata)
{
	int		tmp;

	gpioWrite(gpioclk, LOW);
	tmp = gpioRead(gpiodata);
	gpioDelay(4);
	gpioWrite(gpioclk, HIGH);
	gpioDelay(4);
	return(tmp);
}

void		send_one_bit(int gpioclk, int gpiodata, int bit)
{
	gpioWrite(gpioclk, LOW);
	gpioWrite(gpiodata, bit);
	gpioDelay(4);
	gpioWrite(gpioclk, HIGH);
	gpioDelay(4);
}

void		send_stop(int gpioclk, int gpiodata)
{
	gpioSetMode(gpiodata, PI_OUTPUT);
	gpioWrite(gpiodata, HIGH);
	gpioDelay(4);
	gpioWrite(gpioclk, LOW);
	gpioDelay(4);
	gpioWrite(gpiodata, LOW);
	gpioWrite(gpioclk, LOW);
}

void		send_start(int gpioclk, int gpiodata)
{
	gpioSetMode(gpiodata, PI_OUTPUT);
	gpioWrite(gpioclk, HIGH);
	gpioWrite(gpiodata, HIGH);
	gpioWrite(gpiodata, LOW);
	gpioDelay(4);
	gpioWrite(gpioclk, LOW);
	gpioDelay(4);
}
/*send nbits to the gpio , *octets must contain adresses for all the required
 * bits*/
void		send_bits(int nbits, char *octets, int gpiodata, int gpioclk)
{
	int		tmp;
	int		tmp2;

	tmp2 = 128;
	gpioSetMode(gpiodata, PI_OUTPUT);
	nbits--;
	while (nbits >= 0)
	{//set tmp2 back to 128 after each octets
		tmp2 = (nbits % 8 == 0 && nbits > 0 ? 128 : tmp2);
		tmp = octets[nbits / 8];
		tmp = tmp & tmp2;
		tmp = tmp >> nbits % 8;
//		printf("%d", tmp);
		send_one_bit(gpioclk, gpiodata, tmp);
		nbits--;
		tmp2 = tmp2 >> 1;
	}
//	printf("\n");
	send_one_bit(gpioclk, gpiodata, 0);
}

char		*rec_bits(int nbits, int gpiodata, int gpioclk)
{
	char		*res;
	int			noctets;
	int			tmp;
	int			i;
	int			j;
//if nbits / 8 is less than one , then alloc one.
	noctets = (nbits / 8 >= 1 ? nbits / 8 : 1);
	gpioSetMode(gpiodata, PI_INPUT);
	res = (char *)malloc(sizeof(char) * noctets);
	i = 0;
	while (i < noctets)
	{
		j = 0;
		while (j < 8)
		{
			tmp = rec_one_bit(gpioclk, gpiodata);
			res[i] = res[i] + tmp;
//			printf("%d", tmp);
			res[i] = res[i] << 1;
			nbits--;
			j++;
		}
		i++;
//		printf(" and j = %d\n", j);
	}
	rec_one_bit(gpioclk, gpiodata);
	return (res);
}
