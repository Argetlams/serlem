#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pigpio.h>
#include "pulse.h"
#include <stdlib.h>
#include <pthread.h>

static void		close_all(t_data *data)
{
	printf("Terminating gpio operations.\n");
	gpioWrite(data->gpioclk, 0);
	gpioWrite(data->gpiodata, 0);
	gpioTerminate();
	printf("Unallocating memory slots.\n");
	printf("Operation done, have a good day.\n");
}

static char		make_addr(int addr, int rw) //return first octet
{
	char		res;
	printf("addr = %d, rw = %d\n", addr, rw);
	res = addr;			//res is addr + rw
 	res = res << 1;
	res = res | rw;	//byte[4] will be the received data;
	return (res);
}

static char		make_register(char ana_out, char chan)
{
	char	res;
	char	tmp;

	tmp = ana_out;
	tmp = tmp << 8;
	res = chan + tmp;
	return (res);
}

static void		send_bit(int gpioclk, int gpiodata, int tmp)
{
	gpioWrite(gpioclk, LOW);
	gpioWrite(gpiodata, tmp); //start bit
	gpioDelay(4);
	gpioWrite(gpioclk, HIGH);
	gpioDelay(4);
}

static int		receive_bit(int gpioclk, int gpiodata)
{
	int		tmp;

	gpioWrite(gpioclk, LOW);
	tmp = gpioRead(gpiodata); //start bit
	gpioDelay(4);
	gpioWrite(gpioclk, HIGH);
	gpioDelay(4);
	return (tmp);
}

/*static int		wait_clk(int gpioclk, int gpiodata)
{
	int		i;

	i = 0;
	while (i <= 500)
	{
		if (receive_bit(gpioclk, gpiodata))
			return (1);
		i++;
		gpioDelay(1);
	}
	return (0);
}*/

void			*f_input(void *dta)
{
	char		c;

	(void)dta;
	while (1)
	{
		read(STDIN_FILENO, &c, 1);
		if (c == 27)
		{
			ctrl_c_pressed = 1;
			break;
		}
	}
	return (NULL);
}

/*static char		read_gpio(int gpioclk, int gpiodata, char addr)
{
	int			nbit;
	char		tmp;
	char		tmp2;
	char		res;

	gpioSetMode(gpiodata, PI_OUTPUT);
	gpioWrite(gpioclk, HIGH);
	gpioWrite(gpiodata, HIGH);
	gpioWrite(gpiodata, LOW);
	gpioDelay(4);
	gpioWrite(gpioclk, LOW);
	gpioDelay(4);
	nbit = 7;
	tmp2 = 128;
	while (nbit >= 0)
	{
		tmp = addr;
		tmp = tmp & tmp2;
		tmp = tmp >> nbit;
		send_bit(gpioclk, gpiodata, tmp);
		nbit--;
		tmp2 = tmp2 >> 1;
	}
	gpioSetMode(gpiodata, PI_INPUT);
	if (!(wait_clk(gpioclk, gpiodata)))
	{
		printf("Clock timed out\n");
		ctrl_c_pressed = 1;
		return (0);
	}
	nbit = 0;
	res = 0;
	while (nbit < 10)
	{
		tmp = receive_bit(gpioclk, gpiodata);
		if (nbit < 8)
		{
			res = res + tmp;
			res = res << 1;
		}
		nbit++;
	}
	gpioWrite(gpiodata, HIGH);
	return (res);
}*/

static void		rec_gpio(t_data *data, int nbit)
{
	int		cbit;
	int		res;
	int		xbit;
	int		tmp;
	int		noctet;

	cbit = 0;
	noctet = 0;
	xbit = 0;
	gpioSetMode(data->gpiodata, PI_INPUT);
	while (cbit < nbit)
	{
		xbit++;
		tmp = receive_bit(data->gpioclk, data->gpiodata);
		if (xbit <= 8)
		{
			res = res + tmp;
			res = res << 1;
		}
		else
		{
			xbit = 0;
			data->rec[noctet] = res;
			noctet++;
			res = 0;
		}
		cbit++;
	}
}

static int		send_gpio(int gpiodata, int gpioclk, char *bytes, int noctet, t_data *data)
{
	int			nbit;
	char		tmp;
	char		mask;
	char		res;
	int			ioctet;

	ioctet = 0;
	res = 0;
	gpioSetMode(gpiodata, PI_OUTPUT);
	gpioWrite(gpioclk, HIGH);
	gpioWrite(gpiodata, HIGH);
	gpioWrite(gpiodata, LOW);
	gpioDelay(4);
	gpioWrite(gpioclk, LOW);
	gpioDelay(4);
	while (ioctet < noctet)
	{
		nbit = 7;
		mask = 128;
		while (nbit >= 0)
		{
			tmp = bytes[ioctet];
			tmp = tmp & mask;
			tmp = tmp >> nbit;
			send_bit(gpioclk, gpiodata, tmp);
			nbit--;
			mask = mask >> 1;
		}
		gpioWrite(gpioclk, LOW);
		gpioSetMode(data->gpiodata, PI_INPUT);
		while (gpioRead(gpiodata) == 0)
		{
		}
/*		if (!(wait_clk(gpioclk, gpiodata)))
		{
			printf("Clock timed out\n");
			ctrl_c_pressed = 1;
			return (0);
		}*/
		gpioSetMode(gpiodata, PI_OUTPUT);
		ioctet++;
	}
	return (res);
}

static int		initialise(t_data *data)
{
	ctrl_c_pressed = 0;
	if ((data = (t_data *)malloc(sizeof(t_data))) == NULL)
	{
		printf("Allocation failed\n");
		exit(-1);
	}
	set_input_mode();
	return (0);
}

int				main()
{
	t_data	data;
	pthread_t	input_thread;

	initialise(&data);
	pthread_create(&input_thread, NULL, &f_input, &data);
	if (gpioInitialise() < 0)
	{
		fprintf(stderr, "pigpio initialisation failed\n");
		return (-1);
	}
	data.gpioclk = 27;
	data.gpiodata = 17;
	gpioSetMode(data.gpioclk, PI_OUTPUT);
	data.bytes[0] = make_addr(0x48, READGPIO);
	data.bytes[1] = make_register(0, 0);
	while (1)
	{
		send_gpio(data.gpiodata, data.gpioclk, data.bytes, 1, &data);
		rec_gpio(&data, 20);
		printf("rec = %d\n", data.rec[0]);
		gpioDelay(50);
		if (ctrl_c_pressed)
		{
			close_all(&data);
			break;
		}
	}
   return (0);
}

