#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pigpio.h>
#include "pulse.h"
#include <stdlib.h>
#include <pthread.h>

void sigintHandler(int sig);

static void		close_all(t_data *data)
{
	printf("Terminating gpio operations.\n");
	gpioWrite(data->gpioclk, 0);
	gpioWrite(data->gpiodata, 0);
	gpioTerminate();
	printf("Unallocating memory slots.\n");
	printf("Operation done, have a good day.\n");
}

static void		make_bytes(t_data *data, int chan)
{
	data->bytes = (char *)malloc(sizeof(char) * 5);
	data->bytes[5] = '\0';
	data->bytes[0] = 0x48;
	data->bytes[0] = data->bytes[0] << 1;
	data->bytes[0] = data->bytes[0] | 1;
	data->bytes[1] = 0x40 + chan;
}

static void		send_bit(int gpioclk, int gpiodata, int tmp)
{
	gpioWrite(gpioclk, LOW);
	gpioWrite(gpiodata, tmp); //start bit
	gpioDelay(4);
	gpioWrite(gpioclk, HIGH);
	gpioDelay(4);
//	gpioWrite(gpiodata, HIGH); //start bit
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

static int		wait_clk(int gpioclk, int gpiodata)
{
	int		i;

	i = 0;
	while (i < 1000)
	{
		if (receive_bit(gpioclk, gpiodata))
			return (1);
		i++;
		gpioDelay(1);
	}
	return (0);
}

static int		read_gpio(int gpioclk, int gpiodata, char *octet)
{
	int		nbit;
	int		noctet;
	char	tmp;
	char	tmp2;
	char	res;

	noctet = 0;
	gpioSetMode(gpiodata, PI_OUTPUT);
	gpioWrite(gpioclk, HIGH);
	gpioWrite(gpiodata, HIGH);
	gpioWrite(gpiodata, LOW); //send start signal
	gpioDelay(4);
	gpioWrite(gpioclk, LOW);
	gpioDelay(4);
	while (noctet < 1)
	{
		nbit = 7;
		tmp2 = 128;
		while (nbit >= 0)
		{
			tmp = octet[noctet];
			tmp = tmp & tmp2;
			tmp = tmp >> nbit;
			send_bit(gpioclk, gpiodata, tmp);
			nbit--;
			tmp2 = tmp2 >> 1;
		}
		noctet++;
	}
	gpioSetMode(gpiodata, PI_INPUT);
	if (!wait_clk(gpioclk, gpiodata))
	{
		printf("gpioclk timed out\n");
		return (-1);
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
	//		printf("%d", tmp);
		}
		nbit++;
	}
	gpioWrite(gpiodata, HIGH);
	printf("res = %d\n", res);
	return (0);
}

static int		initialise(t_data *data)
{
	ctrl_c_pressed = 0;
	if ((data = (t_data *)malloc(sizeof(t_data))) == NULL)
	{
		printf("Allocation failed\n");
		exit(-1);
	}
	data->addr = ADDR;
	data->n = 99;
	make_bytes(data, 0);
	return (0);
}

int				main()
{
	t_data	data;
	int		err;

	initialise(&data);
	if (gpioInitialise() < 0)
	{
		fprintf(stderr, "pigpio initialisation failed\n");
		return (-1);
	}
	data.gpioclk = 27;
	data.gpiodata = 17;
	gpioSetMode(data.gpioclk, PI_OUTPUT);
	signal(SIGINT, sigintHandler);
//	make_bytes(&data, 1);
	while (1)
	{
		if ((err = read_gpio(data.gpioclk, data.gpiodata, data.bytes)) < 0)
		{
			printf("Error (%d)\n", err);
			ctrl_c_pressed = 1;
		}
		gpioDelay(500);
		if (ctrl_c_pressed)
		{
			close_all(&data);
			break;
		}
	}
   return (0);
}

void sigintHandler(int sig)
{
	signal(SIGINT, sigintHandler);
	ctrl_c_pressed = 1;
	(void)sig;
}
