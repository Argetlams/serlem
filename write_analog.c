#include "pulse.h"
#include <string.h>
#include <stdio.h>
#include <pigpio.h>
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

void		write_analog(char pin, int gpioclk, int gpiodata, char value)
{
	static int	first_launch = -1;
	static int	tmp = -1;
	char		addr_octet;

	addr_octet = make_addr(WRITEGPIO);
	if (first_launch == -1 || first_launch != pin)
	{
		send_start(gpioclk, gpiodata);
		send_bits(ADDRBITS, &addr_octet, gpiodata, gpioclk);
		send_bits(REGBITS, &pin, gpiodata, gpioclk);
		first_launch = pin;
	}
	send_start(gpioclk, gpiodata);
	send_bits(ADDRBITS, &addr_octet, gpiodata, gpioclk);
	send_bits(8, &value, gpiodata, gpioclk);
	send_stop(gpioclk, gpiodata);
}
