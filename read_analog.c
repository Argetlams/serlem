#include "pulse.h"
#include <string.h>
#include <stdio.h>

static char	make_reg(char ana_out, char chan)
{
	char	res;
	char	tmp;

	tmp = ana_out;
	tmp = tmp << 8;
	res = chan + tmp;
	return (res);
}

static char	make_addr(int rw)
{
	char	res;

	res = ADDR; 
	res = res << 1;
	res = res | rw;
	return (res);
}

int			read_analog(char pin, int gpioclk, int gpiodata, int nbits)
{
	static int	first_launch = -1;
	static int	tmp = -1;
	char		addr_octet;
	int			res;

	if (first_launch == -1 || first_launch != pin)
	{
		addr_octet = make_addr(WRITEGPIO);
		send_start(gpioclk, gpiodata);
		send_bits(ADDRBITS, &addr_octet, gpiodata, gpioclk);
		send_bits(REGBITS, &pin, gpiodata, gpioclk);
		first_launch = pin;
	}
	addr_octet = make_addr(READGPIO);
	send_start(gpioclk, gpiodata);
	send_bits(ADDRBITS, &addr_octet, gpiodata, gpioclk);
	res = rec_bits(nbits, gpiodata, gpioclk);
	if (tmp == -1)
		tmp = res;
	else if (res < tmp - 10 || res > tmp + 10)
	{
		tmp = res;
		return (tmp);
	}
	else
	{
		tmp = res;
		return (res);
	}
	return (res);
}
